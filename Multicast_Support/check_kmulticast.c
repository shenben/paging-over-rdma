#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <rdma/ib_verbs.h>

static int __init check_multicast_init(void) {
    struct ib_device *dev;
    struct ib_device_attr dev_attr;
    int ret;

    // Get the IB device by name
    dev = ib_device_get_by_name("mlx5_0");
    if (!dev) {
        pr_err("Failed to get IB device.\n");
        return -ENODEV;
    }

    // Query device attributes
    ret = ib_query_device(dev, &dev_attr);
    if (ret) {
        pr_err("ib_query_device failed (%d)\n", ret);
        ib_device_put(dev);
        return ret;
    }

    // Check multicast support
    if (dev_attr.device_cap_flags & IB_DEVICE_MULTICAST) {
        pr_info("Device %s supports multicast.\n", dev->name);
    } else {
        pr_info("Device %s does NOT support multicast.\n", dev->name);
    }

    // Release the device
    ib_device_put(dev);
    return 0;
}

static void __exit check_multicast_exit(void) {
    pr_info("RDMA multicast check module unloaded.\n");
}

module_init(check_multicast_init);
module_exit(check_multicast_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RDMA Multicast Support Check");
MODULE_AUTHOR("Your Name");