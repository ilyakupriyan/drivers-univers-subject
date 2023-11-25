#include <linux/module.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <asm/io.h>
#include "../header/comm.h"

#define VENDOR_ID 0x8086
#define DEVICE_ID 0x2723

#define DEV_MAC_ADDR 0x24418C95D242

static int my_driver_probe(struct pci_dev *pdev, const struct pci_device_id *ent);
static void my_driver_remove(struct pci_dev *pdev);
static long dev_ioctl(struct file *fops, unsigned int cmd, unsigned long arg);
static int dev_open(struct inode *inode, struct file *fops);
static int dev_release(struct inode *node, struct file *fops);

static long long register_data = 0;

static dev_t cdev = 0;
static struct class *dev_class;

//Device file for IOCTL
struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = dev_ioctl,
    .open = dev_open,
    .release = dev_release
};

//Structure is for storing physical address of some data
struct pci_mem {
    resource_size_t real;
    resource_size_t size;
};

//Table of PCI_device
static struct pci_device_id network_device_id_tbl[] = {
    { PCI_DEVICE(VENDOR_ID, DEVICE_ID) },
    { 0, }
};
MODULE_DEVICE_TABLE(pci, network_device_id_tbl);

//PCI device
static struct pci_driver network_hard = {
    .name = "network_hard",
    .id_table = network_device_id_tbl,
    .probe = my_driver_probe,
    .remove = my_driver_remove
};

/*
 * Functions for device file
 */
static long dev_ioctl(struct file *fops, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
        case RD_MAC:
            if (copy_to_user(&arg, &register_data, sizeof(register_data))) {
                pr_err("MAC: error...\n");
            }
            break;
        default:
            pr_info("Default case in switch.\n");
    }
    
    return 0;
}

static int dev_open(struct inode *node, struct file *fops)
{
    return 0;
}

static int dev_release(struct inode *node, struct file *fops)
{
    return 0;
}

/*
 * Functions for operating PCI device
 */
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

    unsigned int offset;
    for (offset = 0; offset < net_dev.size; offset++) {
        register_data = 0;
        int byte = 0;
        for (byte = 0; byte <= 5; byte++) {
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

    unregister_chrdev_region(cdev, 1);
}


/*
 * Init and exit module functions
 */
int init_module(void)
{
    int err;
    pr_info("Initialization module...\n");
    pci_register_driver(&network_hard);

    /* Allocating major and minor numbers*/
    err = alloc_chrdev_region(&cdev, 0, 1, "PCI_chrdev");
    if (err < 0) {
        pr_info("Failed allocate major number for device\n");
        return -1;
    }
    pr_info("Major = %d Minor = %d\n", MAJOR(cdev), MINOR(cdev));

    /* Creating dev class */
    dev_class = class_create(THIS_MODULE, "My own class");
    if (IS_ERR(dev_class)) {
        pr_err("Cannot create the struct class for device\n");
    }
    if (IS_ERR(device_create(dev_class, NULL, cdev, NULL, "pci_ioctl_device"))) {
        pr_err("Cannot create the Device\n");
    }

    pr_info("Creating the device is successfully.\n");
    return 0;

    r_device:
        class_destroy(dev_class);
    r_class:
        unregister_chrdev_region(cdev, 1);
        return -1;
}

void cleanup_module(void)
{   
    /* */
    device_destroy(dev_class, cdev);
    class_destroy(dev_class);
    unregister_chrdev_region(cdev, 1);

    /* */
    pci_unregister_driver(&network_hard);

    pr_info("Kernel module is removed successfully.\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ilya Kupriyanov");
MODULE_DESCRIPTION("Module prints MAC of network card");
MODULE_VERSION("1.1.0");