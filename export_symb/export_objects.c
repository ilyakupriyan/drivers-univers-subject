#include <linux/init.h>
#include <linux/module.h>

void export_function(void)
{
    printk (KERN_INFO "export_function is launched successful");
}
EXPORT_SYMBOL(export_function);

int export_var = 345;
EXPORT_SYMBOL(export_var);

static int __init export_object_init(void) 
{
    printk(KERN_INFO "HELLO from exported module!");
    return 0;
}

static void __exit export_object_exit(void)
{
    printk(KERN_INFO "Bye from exported module");
}

module_init(export_object_init);
module_exit(export_object_exit);

//Data about the module
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ilya Kupriyanov");
MODULE_DESCRIPTION("Module to demonstrate the EXPORT_SYMBOL functionality");
MODULE_VERSION("0.1.0");