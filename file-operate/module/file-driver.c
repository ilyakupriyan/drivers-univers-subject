#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>

/* function prototype for file_operation structure */
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
static int device_open(struct inode *, struct file *);
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);
static int device_release(struct inode *, struct file *);

#define DEVICE_NAME "dev_md"
#define BUF_LEN 90
#define SUCCESS 0

/* Global variables. Because there are used "static", these variables are visible only this file */
static int major = 0;               // major number of driver
static int is_open_file = 0;        // count how many times file is opened   

static char msg[BUF_LEN];           // the msg the device will give when asked
static char *msg_ptr;

static struct file_operations fops = {
    .owner      = THIS_MODULE,
    .read       = device_read,
    .open       = device_open,
    .write      = device_write,
    .release    = device_release
};

static int __init file_operate_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        pr_err("dev_md: can't get major %d\n", major);
        return major;
    }
    
    pr_info("Kernel module inserted successfully...\n");
    pr_info("To talk to device driver, create a device file with \n");
    pr_info("'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, major);

    return 0;
}

static void __exit file_operate_exit(void)
{
    //int error;
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("Kernel module remoted successfully...\n");
}

module_init(file_operate_init);
module_exit(file_operate_exit);

/*
 * Methods
 */

/*
 * Called when a process tries to open device file, like
 * cat /dev/charFile
 */ 
static int device_open(struct inode *inode, struct file *file) 
{
    static int count = 0;

    if (is_open_file) {
        -EBUSY;
    }

    ++is_open_file;
    sprintf(msg, "Device file has been opened %d", count++);
    msg_ptr = msg;
    try_module_get(THIS_MODULE);

    return SUCCESS;
}

/* 
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file)
{
	is_open_file--;		/* We're now ready for our next caller */

	/* 
	 * Decrement the usage count, or else once you opened the file, you'll
	 * never get get rid of the module. 
	 */
	module_put(THIS_MODULE);

	return 0;
}


/*
 * Called when a process, which already opened the dev file, attempts to read from it.
 */
static ssize_t device_read(struct file *file, char __user *buf, size_t len, loff_t *offs)
{
    int size_msg = strlen(msg);
    if (len > size_msg + 1) {
        len = size_msg + 1;
    }
    copy_to_user(buf, msg, len);

    return len;
}

/*
 *  Called when a process tries write something in device file
 *  Example: echo > /dev/device_file "hello"
 */
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *offs)
{
    printk(KERN_ALERT "Sorry, this operation isn't supported.\n");
	return -EINVAL;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kupriyanov Ilya");
MODULE_VERSION("1.0.0");
MODULE_DESCRIPTION("Simple linux driver to demonstrate communication between application and kernel module");