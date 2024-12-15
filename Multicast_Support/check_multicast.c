#include <stdio.h>
#include <infiniband/verbs.h>

int main() {
    struct ibv_device **dev_list;
    struct ibv_device *ib_dev;
    struct ibv_context *context;
    struct ibv_device_attr dev_attr;
    int num_devices, i;

    // Get the list of RDMA devices
    dev_list = ibv_get_device_list(&num_devices);
    if (!dev_list) {
        perror("Failed to get RDMA device list");
        return 1;
    }

    if (num_devices == 0) {
        printf("No RDMA devices found.\n");
        return 1;
    }

    for (i = 0; i < num_devices; i++) {
        ib_dev = dev_list[i];
        printf("Checking device: %s\n", ibv_get_device_name(ib_dev));

        // Open the device
        context = ibv_open_device(ib_dev);
        if (!context) {
            perror("Failed to open device");
            continue;
        }

        // Query device attributes
        if (ibv_query_device(context, &dev_attr)) {
            perror("Failed to query device attributes");
            ibv_close_device(context);
            continue;
        }

        // Check multicast support
        if (dev_attr.device_cap_flags & IBV_DEVICE_MULTICAST) {
            printf("Device %s supports multicast.\n", ibv_get_device_name(ib_dev));
        } else {
            printf("Device %s does NOT support multicast.\n", ibv_get_device_name(ib_dev));
        }

        // Close the device
        ibv_close_device(context);
    }

    // Free the device list
    ibv_free_device_list(dev_list);
    return 0;
}