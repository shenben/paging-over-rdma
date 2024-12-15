// File: rdma_multicast_check.c

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <rdma/rdma_verbs.h>
// #include <infiniband/verbs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Kernel Module to Check RDMA Multicast Support");
MODULE_VERSION("1.0");

static int __init rdma_multicast_init(void) {
    struct ib_device **dev_list;
    int num_devices, i, j;

    pr_info("RDMA Multicast Check Module Loaded\n");

    // Get the list of IB devices
    dev_list = rdma_get_devices(&num_devices);
    if (!dev_list) {
        pr_err("Failed to get RDMA devices list\n");
        return -ENODEV;
    }

    if (num_devices == 0) {
        pr_info("No RDMA devices found.\n");
        rdma_put_devices(dev_list, num_devices);
        return 0;
    }

    pr_info("Found %d RDMA device(s).\n", num_devices);

    for (i = 0; i < num_devices; i++) {
        struct ib_device_attr device_attr;
        struct ib_device *dev = dev_list[i];

        if (ib_query_device(dev, &device_attr)) {
            pr_err("Failed to query device %s\n", dev->name);
            continue;
        }

        pr_info("Device %d: %s\n", i, dev->name);

        // Iterate over each port
        for (j = 1; j <= device_attr.phys_port_cnt; j++) {
            struct ib_port_attr port_attr;
            if (ib_query_port(dev, j, &port_attr)) {
                pr_err("  Port %d: Failed to query port\n", j);
                continue;
            }

            // Check multicast support using rdma_cap_ib_mcast
            int mcast_supported = rdma_cap_ib_mcast(dev, j);

            pr_info("  Port %d:\n", j);
            pr_info("    State: %s\n", (port_attr.state == IB_PORT_ACTIVE) ? "ACTIVE" : "INACTIVE");
            pr_info("    Link Layer: %s\n", (port_attr.link_layer == IB_LINK_LAYER_INFINIBAND) ? "InfiniBand" : "Ethernet");
            pr_info("    Multicast Support: %s\n", mcast_supported ? "Yes" : "No");
        }
    }

    rdma_put_devices(dev_list, num_devices);
    return 0;
}

static void __exit rdma_multicast_exit(void) {
    pr_info("RDMA Multicast Check Module Unloaded\n");
}

module_init(rdma_multicast_init);
module_exit(rdma_multicast_exit);
