#include <linux/module.h>
#include <linux/init.h>

extern void export_function(void);
extern int export_var;

static int __init import_object_init(void)
{
    printk(KERN_INFO "Hello from imported module");
    export_function();
    printk(KERN_INFO "Value of imported variable: %d", export_var);
    return 0;
}
module_init(import_object_init);

static void __exit import_object_exit(void)
{
    printk(KERN_INFO "Goodbye from imported module");
}
module_exit(import_object_exit);

//Data about the module
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ilya Kupriyanov");
MODULE_DESCRIPTION("Module to demonstrate the EXPORT_SYMBOL functionality");
MODULE_VERSION("0.1.0");