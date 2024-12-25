// File: check_rdma_multicast.c

#include <stdio.h>
#include <stdlib.h>
#include <infiniband/verbs.h>
#include <rdma/rdma_verbs.h>
#include <string.h>

// Define the multicast capability flag if not already defined
#ifndef IBV_DEVICE_MCAST_WR
#define IBV_DEVICE_MCAST_WR (1 << 5) // Adjust the bit position as per your RDMA library
#endif

int main() {
    struct ibv_device **dev_list;
    int num_devices, i, j;

    // Get the list of IB devices
    dev_list = ibv_get_device_list(&num_devices);
    if (!dev_list) {
        perror("Failed to get IB devices list");
        return EXIT_FAILURE;
    }

    if (num_devices == 0) {
        printf("No IB devices found.\n");
        ibv_free_device_list(dev_list);
        return EXIT_SUCCESS;
    }

    printf("Found %d IB device(s).\n\n", num_devices);

    for (i = 0; i < num_devices; i++) {
        struct ibv_context *ctx = ibv_open_device(dev_list[i]);
        if (!ctx) {
            fprintf(stderr, "Failed to open device %s\n", ibv_get_device_name(dev_list[i]));
            continue;
        }

        struct ibv_device_attr device_attr;
        if (ibv_query_device(ctx, &device_attr)) {
            fprintf(stderr, "Failed to query device %s\n", ibv_get_device_name(dev_list[i]));
            ibv_close_device(ctx);
            continue;
        }

        printf("Device %d: %s\n", i, ibv_get_device_name(dev_list[i]));

        // Iterate over each port
        for (j = 1; j <= device_attr.phys_port_cnt; j++) {
            struct ibv_port_attr port_attr;
            if (ibv_query_port(ctx, j, &port_attr)) {
                fprintf(stderr, "  Port %d: Failed to query port\n", j);
                continue;
            }

            // Check multicast support by inspecting device_cap_flags
            int mcast_supported = (device_attr.device_cap_flags & IBV_DEVICE_MCAST_WR) ? 1 : 0;

            printf("  Port %d:\n", j);
            printf("    State: %s\n", (port_attr.state == IBV_PORT_ACTIVE) ? "ACTIVE" : "INACTIVE");
            printf("    Link Layer: %s\n", (port_attr.link_layer == IBV_LINK_LAYER_INFINIBAND) ? "InfiniBand" : "Ethernet");
            printf("    Multicast Support: %s\n\n", mcast_supported ? "Yes" : "No");
        }

        ibv_close_device(ctx);
    }

    ibv_free_device_list(dev_list);
    return EXIT_SUCCESS;
}
/*
$ gcc -o check_rdma_multicast check_rdma_multicast.c -libverbs -lrdmacm

(op-tf-py3.9) pxg@node1:~/paging-over-rdma/Multicast_Support$ ./check_rdma_multicast
Found 1 IB device(s).

Device 0: rocep23s0f1
  Port 1:
    State: ACTIVE
    Link Layer: Ethernet
    Multicast Support: No
*/