// vi:	sw=4 ts=4 noet:
/*
	Mnemonic:	vfd_dcb.c
	Abstract: 	DCB support functions
	Date:		28 October 2016
	Author:		E. Scott Daniels

	Mods:

	useful doc:
		http://dpdk.org/doc/api/vmdq_dcb_2main_8c-example.html

	Useful files:
		./drivers/net/ixgbe/base/ixgbe_dcb.h

*/

#include <vfdlib.h>		// if vfdlib.h needs an include it must be included there, can't be include prior
#include "sriov.h"
#include "vfd_dcb.h"
#include "vfdlib.h"

/* 
	Default dcb settings.
static const struct rte_eth_conf eth_dcb_default = {
	.rxmode = {
		.mq_mode        = ETH_MQ_RX_VMDQ_DCB,
		.split_hdr_size = 0,
		.header_split   = 0, 
		.hw_ip_checksum = 0, 
		.hw_vlan_filter = 0, 
		.jumbo_frame    = 0, 
	},
	.txmode = {
		.mq_mode = ETH_MQ_TX_DCB,
	},
	.rx_adv_conf = {
		.vmdq_dcb_conf = {
			.nb_queue_pools = ETH_32_POOLS,
			.enable_default_pool = 0,
			.default_pool = 0,
			.nb_pool_maps = 0,					// up to 64
			.pool_map = {{0, 0},},				// {vlanID,pools}  pools is a bit mask(64) of which pool(s) the vlan id maps to
			.dcb_tc = {0},						// up to num-user-priorities; 'selects a queue in a pool' what ever thef that means
		},
		.dcb_rx_conf = {
			.nb_tcs = ETH_4_TCS,
			.dcb_tc = {0},
		},
		.vmdq_rx_conf = {
			.nb_queue_pools = ETH_32_POOLS,
			.enable_default_pool = 0,
			.default_pool = 0,
			.nb_pool_maps = 0,
			.pool_map = {{0, 0},},
		},
	},
	.tx_adv_conf = {
		.vmdq_dcb_tx_conf = {
			.nb_queue_pools = ETH_32_POOLS,
		 	.dcb_tc = {0},
		},
	},
};
*/


/*
	Create and initialise a DCB structure for a given port
struct rte_eth_conf *vfd_dcb_init( uint8_t port ) {

	
}
*/

/*
	Configure the given port for DCB.
*/
extern int vfd_dcb_config( uint8_t port ) {
	struct rte_eth_dev *pf_dev = &rte_eth_devices[port];		// this seems to be an array exported by dpdk

	ixgbe_configure_dcb( pf_dev );			// see what happens

	return 0;			// for now constant; but in future it will report an error if needed
}


/*
	Convert rte_eth_dev to ixgbe_hw:
    struct ixgbe_hw     *hw;
    hw = IXGBE_DEV_PRIVATE_TO_HW(dev->data->dev_private);

*/
    
/*
	Initialise a device (port) in dcb mode.
	Return 0 if there were no errors, 1 otherwise.  The calling programme should
	not continue if this function returns anything but 0.

	This funciton is a complete replacement for port_init() which is used when
	not running in dcb (qos) mode.
*/
extern int dcb_port_init(uint8_t port, __attribute__((__unused__)) struct rte_mempool *mbuf_pool)
{
	struct rte_eth_conf port_conf = eth_dcb_default;
	const uint16_t rx_rings = 4; 
	const uint16_t tx_rings = 4;
	int retval;
	uint16_t q;

/*
	TODO: pull in tc count and set pool value
	switch( tc_count ) {
		case 4:
			port_conf.rx_adv_conf.vmdq_dcb_conf.nb_queue_pools = ETH_32_POOLS;
			port_conf.rx_adv_conf.vmdq_rx_conf.nb_queue_pools = ETH_32_POOLS;
			port_conf.tx_adv_conf.vmdq_dcb_tx_conf.nb_queue_pools = ETH_32_POOLS;
			break;

		case 8:
			port_conf.rx_adv_conf.vmdq_dcb_conf.nb_queue_pools = ETH_16_POOLS;
			port_conf.rx_adv_conf.vmdq_rx_conf.nb_queue_pools = ETH_16_POOLS;
			port_conf.tx_adv_conf.vmdq_dcb_tx_conf.nb_queue_pools = ETH_16_POOLS;
			break;

	}
*/


	if (port >= rte_eth_dev_count()) {
		bleat_printf( 0, "CRI: abort: dcb_port_init: port >= rte_eth_dev_count" );
		return 1;
	}


	// Configure the Ethernet device.
	retval = rte_eth_dev_configure(port, rx_rings, tx_rings, &port_conf);
	if (retval != 0) {
		bleat_printf( 0, "CRI: abort: dcb_port_init: cannot configure port %u, err=%d", port, retval);
		return 1;
	}


	rte_eth_dev_callback_register(port, RTE_ETH_EVENT_INTR_LSC, lsi_event_callback, NULL);
	rte_eth_dev_callback_register(port, RTE_ETH_EVENT_VF_MBOX, vf_msb_event_callback, NULL);


	// Allocate and set up RX queues for the port.
	for (q = 0; q < rx_rings; q++) {
		retval = rte_eth_rx_queue_setup(port, q, RX_RING_SIZE, rte_eth_dev_socket_id(port), NULL, mbuf_pool);
		if (retval < 0) {
			bleat_printf( 0, "CRI: abort: dcb_port_init: cannot setup rx queue, port %u err=%d", port, retval );
			return 1;
		}
	}

	// Allocate and set up 1 TX queue per Ethernet port.
	for (q = 0; q < tx_rings; q++) {
		retval = rte_eth_tx_queue_setup(port, q, TX_RING_SIZE, rte_eth_dev_socket_id(port), NULL);
		if (retval < 0) {
			bleat_printf( 0, "CRI: abort: dcb_port_init: cannot setup tx queue, port %u err=%d", port, retval );
			return 1;
		}
	}


	// Start the Ethernet port.
	retval = rte_eth_dev_start(port);
	if (retval < 0) {
		bleat_printf( 0, "CRI: abort: dcb_port_init: cannot start port %u err=%d", port, retval );
		return 1;
	}
	

	// Display the port MAC address.
	struct ether_addr addr;
	rte_eth_macaddr_get(port, &addr);
	bleat_printf( 3,  "port_init: port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "",
			(unsigned)port,
			addr.addr_bytes[0], addr.addr_bytes[1],
			addr.addr_bytes[2], addr.addr_bytes[3],
			addr.addr_bytes[4], addr.addr_bytes[5]);

	// Enable RX in promiscuous mode for the Ethernet device.
	rte_eth_promiscuous_enable(port);

	return 0;
}