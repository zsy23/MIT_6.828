#include "ns.h"

extern union Nsipc nsipcbuf;

void
output(envid_t ns_envid)
{
	binaryname = "ns_output";

	// LAB 6: Your code here:
	// 	- read a packet from the network server
	//	- send the packet to the device driver

	envid_t e;

	while (1)
		if (ipc_recv(&e, &nsipcbuf, 0) == NSREQ_OUTPUT && e == ns_envid)
			while (sys_net_try_send(nsipcbuf.pkt.jp_data, nsipcbuf.pkt.jp_len) < 0);
}
