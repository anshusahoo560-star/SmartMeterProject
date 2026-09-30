/*
 * Smart Energy Smart Meter - Pulse Counter Kernel Module (Character Device Driver Concept)
 * Demonstrates hardware pulse interception and Linux kernel-to-userspace interface.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Smart Meter Project Team");
MODULE_DESCRIPTION("Pulse Counter Character Device Driver for Smart Meter");
MODULE_VERSION("1.0");

#define DEVICE_NAME "smart_meter_pulse"
#define CLASS_NAME "smart_meter"

static int major_number;
static dev_t dev_num;
static struct cdev pulse_cdev;
static struct class *pulse_class = NULL;
static struct device *pulse_device = NULL;

static unsigned long pulse_count = 0;

static int dev_open(struct inode *inodep, struct file *filep) {
    pr_info("SmartMeter: Device opened\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    pr_info("SmartMeter: Device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    char msg[64];
    int msg_len;
    int error_count;

    if (*offset > 0)
        return 0;

    msg_len = snprintf(msg, sizeof(msg), "%lu\n", pulse_count);

    if (len < msg_len)
        return -EINVAL;

    error_count = copy_to_user(buffer, msg, msg_len);
    if (error_count == 0) {
        *offset += msg_len;
        return msg_len;
    } else {
        return -EFAULT;
    }
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    // Writing to device simulates hardware pulse triggers or resets
    pulse_count++;
    pr_info("SmartMeter: Pulse received by driver! Total: %lu\n", pulse_count);
    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int __init pulse_driver_init(void) {
    int ret;
    pr_info("SmartMeter: Initializing Pulse Driver...\n");

    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("SmartMeter: Failed to allocate major number\n");
        return ret;
    }
    major_number = MAJOR(dev_num);

    cdev_init(&pulse_cdev, &fops);
    pulse_cdev.owner = THIS_MODULE;
    ret = cdev_add(&pulse_cdev, dev_num, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev_num, 1);
        pr_err("SmartMeter: Failed to add cdev\n");
        return ret;
    }

    pulse_class = class_create(CLASS_NAME);
    if (IS_ERR(pulse_class)) {
        cdev_del(&pulse_cdev);
        unregister_chrdev_region(dev_num, 1);
        pr_err("SmartMeter: Failed to create device class\n");
        return PTR_ERR(pulse_class);
    }

    pulse_device = device_create(pulse_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(pulse_device)) {
        class_destroy(pulse_class);
        cdev_del(&pulse_cdev);
        unregister_chrdev_region(dev_num, 1);
        pr_err("SmartMeter: Failed to create device\n");
        return PTR_ERR(pulse_device);
    }

    pr_info("SmartMeter: Driver loaded successfully with major number %d\n", major_number);
    return 0;
}

static void __exit pulse_driver_exit(void) {
    device_destroy(pulse_class, dev_num);
    class_destroy(pulse_class);
    cdev_del(&pulse_cdev);
    unregister_chrdev_region(dev_num, 1);
    pr_info("SmartMeter: Driver unloaded\n");
}

module_init(pulse_driver_init);
module_exit(pulse_driver_exit);
