#include <kern/e1000.h>
#include <kern/pmap.h>
#include <inc/string.h>
#include <inc/error.h>

volatile uint32_t *e1000;

struct e1000_tx_desc tx_desc_arr[E1000_TX_DESC_CNT] __attribute__ ((aligned (16)));
struct e1000_tx_pkt  tx_pkt_buf [E1000_TX_DESC_CNT];

struct e1000_rx_desc rx_desc_arr[E1000_RX_DESC_CNT] __attribute__ ((aligned (16)));
struct e1000_rx_pkt  rx_pkt_buf [E1000_RX_DESC_CNT];

static uint32_t
e1000r(int index)
{
	return e1000[index];
}

static void
e1000w(int index, int value)
{
	e1000[index] = value;
	e1000[0];  // wait for write to finish, by reading
}

int e1000_attachfn(struct pci_func *pcif)
{
	int i;

	pci_func_enable(pcif);

	e1000 = (uint32_t *)mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);
	cprintf("E1000 status: 0x%x\n", e1000r(E1000_STATUS));
	assert(e1000r(E1000_STATUS) == 0x80080783);

	// Transmit Initialization
	memset(tx_desc_arr, 0, sizeof(struct e1000_tx_desc)*E1000_TX_DESC_CNT);
	memset(tx_pkt_buf, 0, sizeof(struct e1000_tx_pkt)*E1000_TX_DESC_CNT);
	for (i = 0; i < E1000_TX_DESC_CNT; i++) {
		tx_desc_arr[i].addr = PADDR(tx_pkt_buf[i].buf);
		tx_desc_arr[i].status = E1000_TXD_STAT_DD;
	}

	e1000w(E1000_TDBAL, PADDR(tx_desc_arr));
	e1000w(E1000_TDBAH, 0);
	e1000w(E1000_TDLEN, sizeof(struct e1000_tx_desc) * E1000_TX_DESC_CNT);
	e1000w(E1000_TDH, 0);
	e1000w(E1000_TDT, 0);
	e1000w(E1000_TCTL, E1000_TCTL_EN | E1000_TCTL_PSP | 
		(E1000_TCTL_CT & (0x10 << 4)) | (E1000_TCTL_COLD & (0x40 << 12)));
	e1000w(E1000_TIPG, 10 | (4 << 10) | (6 << 20));

	// Receive Initialization
	memset(rx_desc_arr, 0, sizeof(struct e1000_rx_desc)*E1000_RX_DESC_CNT);
	memset(rx_pkt_buf, 0, sizeof(struct e1000_rx_pkt)*E1000_RX_DESC_CNT);
	for (i = 0; i < E1000_RX_DESC_CNT; i++)
		rx_desc_arr[i].addr = PADDR(rx_pkt_buf[i].buf);

	e1000w(E1000_RAL, E1000_MAC_LOW);
	e1000w(E1000_RAH, E1000_MAC_HIGH | E1000_RAH_AV);
	e1000w(E1000_MTA, 0);
	e1000w(E1000_RDBAL, PADDR(rx_desc_arr));
	e1000w(E1000_RDBAH, 0);
	e1000w(E1000_RDLEN, sizeof(struct e1000_rx_desc) * E1000_RX_DESC_CNT);
	e1000w(E1000_RDH, 0);
	e1000w(E1000_RDT, E1000_RX_DESC_CNT - 1);
	e1000w(E1000_RCTL, E1000_RCTL_EN | (E1000_RCTL_LPE & 0) | 
		(E1000_RCTL_LBM & 0) | (E1000_RCTL_RDMTS & 0) | (E1000_RCTL_MO & 0) | 
		E1000_RCTL_BAM | (E1000_RCTL_BSIZE & (0x1 << 16)) | E1000_RCTL_SECRC);

	return 0;
}

int e1000_transmit(char *data, size_t len)
{
	int tdt = e1000r(E1000_TDT);
	if (tx_desc_arr[tdt].status & E1000_TXD_STAT_DD) {
		if (len > E1000_TX_PKT_SIZE)
			len = E1000_TX_PKT_SIZE;
		memmove(tx_pkt_buf[tdt].buf, data, len);
		tx_desc_arr[tdt].status &= ~E1000_TXD_STAT_DD;
		tx_desc_arr[tdt].cmd |= E1000_TXD_CMD_RS | E1000_TXD_CMD_EOP;
		tx_desc_arr[tdt].length = len;
		e1000w(E1000_TDT, (tdt + 1) % E1000_TX_DESC_CNT);

		return 0;
	}
	return -E_TX_FULL;
}

int e1000_receive(char *data, size_t *len)
{
	int rdt = (e1000r(E1000_RDT) + 1) % E1000_RX_DESC_CNT;
	if (rx_desc_arr[rdt].status & E1000_RXD_STAT_DD) {
		*len = rx_desc_arr[rdt].length;
		if (*len > E1000_RX_PKT_SIZE)
			*len = E1000_RX_PKT_SIZE;
		memmove(data, rx_pkt_buf[rdt].buf, *len);
		rx_desc_arr[rdt].status &= ~E1000_RXD_STAT_DD;
		rx_desc_arr[rdt].status &= ~E1000_RXD_STAT_EOP;
		e1000w(E1000_RDT, rdt);

		return 0;
	}
	return -E_RX_EMPTY;
}
