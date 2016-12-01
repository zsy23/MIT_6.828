#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include <kern/pci.h>

#define E1000_VENDOR_ID   		0x8086
#define E1000_DEVICE_ID   		0x100e
		
#define E1000_MAC_LOW     		0x12005452
#define E1000_MAC_HIGH    		0x00005634 
		
#define E1000_TX_DESC_CNT 		64
#define E1000_TX_PKT_SIZE 		1518
#define E1000_RX_DESC_CNT 		128
#define E1000_RX_PKT_SIZE 		1024
		
#define E1000_STATUS 	  		(0x00008 >> 2)
		
#define E1000_TDBAL       		(0x03800 >> 2) /* TX Descriptor Base Address Low - RW */
#define E1000_TDBAH       		(0x03804 >> 2) /* TX Descriptor Base Address High - RW */
#define E1000_TDLEN       		(0x03808 >> 2) /* TX Descriptor Length - RW */
#define E1000_TDH 		  		(0x03810 >> 2) /* TX Descriptor Head - RW */
#define E1000_TDT 		  		(0x03818 >> 2) /* TX Descripotr Tail - RW */
#define E1000_TIPG   	  		(0x00410 >> 2) /* TX Inter-packet gap -RW */
		
#define E1000_TCTL 		  		(0x00400 >> 2) /* TX Control - RW */
#define E1000_TCTL_EN     		0x00000002    /* enable tx */
#define E1000_TCTL_PSP    		0x00000008    /* pad short packets */
#define E1000_TCTL_CT     		0x00000ff0    /* collision threshold */
#define E1000_TCTL_COLD   		0x003ff000    /* collision distance */
		
#define E1000_TXD_STAT_DD 		0x01
#define E1000_TXD_CMD_RS  		0x08
#define E1000_TXD_CMD_EOP 		0x01
		
#define E1000_RAL         		(0x05400 >> 2) /* Receive Address Low - RW Array */
#define E1000_RAH         		(0x05404 >> 2) /* Receive Address High - RW Array */
#define E1000_MTA      	  		(0x05200 >> 2) /* Multicast Table Array - RW Array */
#define E1000_RDBAL       		(0x02800 >> 2) /* RX Descriptor Base Address Low - RW */
#define E1000_RDBAH       		(0x02804 >> 2) /* RX Descriptor Base Address High - RW */
#define E1000_RDLEN       		(0x02808 >> 2) /* RX Descriptor Length - RW */
#define E1000_RDH         		(0x02810 >> 2) /* RX Descriptor Head - RW */
#define E1000_RDT         		(0x02818 >> 2) /* RX Descriptor Tail - RW */
		
#define E1000_RAH_AV      		0x80000000     /* Receive Address Address Valid - RW Array */
		
#define E1000_RCTL        		(0x00100 >> 2) /* RX Control - RW */
#define E1000_RCTL_EN     		0x00000002     /* enable */
#define E1000_RCTL_LPE    		0x00000020     /* long packet enable */
#define E1000_RCTL_LBM    		0x000000c0     /* loopback mode */
#define E1000_RCTL_RDMTS  		0x00000300     /* rx min threshold size */
#define E1000_RCTL_MO 	  		0x00003000     /* multicast offset shift */
#define E1000_RCTL_BAM    		0x00008000     /* broadcast enable */
#define E1000_RCTL_BSIZE  		0x00030000     /* rx buffer size */
#define E1000_RCTL_SECRC  		0x04000000     /* Strip Ethernet CRC */
		
#define E1000_RXD_STAT_DD 		0x01
#define E1000_RXD_STAT_EOP		0x02 

struct e1000_tx_desc
{
	uint64_t addr;
	uint16_t length;
	uint8_t cso;
	uint8_t cmd;
	uint8_t status;
	uint8_t css;
	uint16_t special;
} __attribute__((packed));

struct e1000_tx_pkt
{
	uint8_t buf[E1000_TX_PKT_SIZE];
} __attribute__((packed));

struct e1000_rx_desc 
{
    uint64_t addr;       /* Address of the descriptor's data buffer */
    uint16_t length;     /* Length of data DMAed into data buffer */
    uint16_t csum;       /* Packet checksum */
    uint8_t status;      /* Descriptor status */
    uint8_t errors;      /* Descriptor Errors */
    uint16_t special;
} __attribute__((packed));

struct e1000_rx_pkt
{
	uint8_t buf[E1000_RX_PKT_SIZE];
} __attribute__((packed));

int e1000_attachfn(struct pci_func *pcif);
int e1000_transmit(char *data, size_t len);
int e1000_receive(char *data, size_t *len);

#endif	// JOS_KERN_E1000_H
