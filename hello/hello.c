#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h> 

MODULE_LICENSE("GPL");
MODULE_VERSION("1.0.0");
MODULE_AUTHOR("Kupriyanov Ilya");
MODULE_DESCRIPTION("It is my first kernel module");

static int __init hello_world_init(void)
{
    printk(KERN_INFO "Hello, Kernel module!");

    return 0;
}

void __exit hello_world_exit(void)
{
    printk(KERN_INFO "Goodbye, Kernel module!");
}

module_init (hello_world_init);
module_exit (hello_world_exit);