#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <asm/io.h>

#define VENDOR_ID 0x8086
#define DEVICE_ID 0x2723

#define DEV_MAC_ADDR 0x24418C95D242

static int my_driver_probe(struct pci_dev *pdev, const struct pci_device_id *ent);
static void my_driver_remove(struct pci_dev *pdev);

static long long register_data = 0;

struct pci_mem {
    resource_size_t real;
    resource_size_t size;
};

static struct pci_device_id network_device_id_tbl[] = {
    { PCI_DEVICE(VENDOR_ID, DEVICE_ID) },
    { 0, }
};
MODULE_DEVICE_TABLE(pci, network_device_id_tbl);

static struct pci_driver network_hard = {
    .name = "network_hard",
    .id_table = network_device_id_tbl,
    .probe = my_driver_probe,
    .remove = my_driver_remove
};

int init_module(void)
{
    printk (KERN_INFO "Initialization module...\n");
    //printk (KERN_INFO "long long: %d\n", sizeof(long long));
    pci_register_driver(&network_hard);
    return 0;
}

void cleanup_module(void)
{
    pci_unregister_driver(&network_hard);
}

static int my_driver_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
    int err, bar;
    struct pci_mem net_dev;
    void *log_address;

    printk(KERN_INFO "Device probed\n");

    bar = pci_select_bars(pdev, IORESOURCE_MEM);

    err = pci_enable_device(pdev);
    if (err < 0)
        printk(KERN_INFO "Failed while enabling...\n");

    err = pci_request_region(pdev, bar, "my_pci");
    if (err < 0)
        printk(KERN_INFO "Request region failed...\n");
    
    net_dev.real = pci_resource_start(pdev, 0);
    net_dev.size = pci_resource_len(pdev, 0);

    log_address = ioremap(net_dev.real, net_dev.size);

    for (unsigned int offset = 0; offset < net_dev.size; offset++) {
        register_data = 0;
        for (int byte = 0; byte <= 5; byte++) {
            register_data <<= 8;
            register_data |= inb(log_address + offset + byte);
        }
        if (register_data == DEV_MAC_ADDR) {
            printk(KERN_INFO "IO base = %lx\n", net_dev.real);
            printk(KERN_INFO "MAC = %x\n", register_data);
            break;
        }
    }

    return 0;
}

static void my_driver_remove(struct pci_dev *pdev)
{
    pci_release_regions(pdev);
    pci_disable_device(pdev);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ilya Kupriyanov");
MODULE_DESCRIPTION("Module prints MAC of network card");