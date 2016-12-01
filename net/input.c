#include "ns.h"

#define RECV_MAX_SIZE 2048

extern union Nsipc nsipcbuf;

void
input(envid_t ns_envid)
{
	binaryname = "ns_input";

	// LAB 6: Your code here:
	// 	- read a packet from the device driver
	//	- send it to the network server
	// Hint: When you IPC a page to the network server, it will be
	// reading from it for a while, so don't immediately receive
	// another packet in to the same physical page.

	char data[RECV_MAX_SIZE];
	size_t len;
	int perm = PTE_P | PTE_U | PTE_W;
	int r;

	while (1)
		if (sys_net_recv(data, &len) == 0) {
			if ((r = sys_page_alloc(0, &nsipcbuf, perm)) < 0)
				panic("input: unable to allocate new page, error %e\n", r);
			memmove(nsipcbuf.pkt.jp_data, data, len);
			nsipcbuf.pkt.jp_len = len;
			ipc_send(ns_envid, NSREQ_INPUT, &nsipcbuf, perm);
		}
}
