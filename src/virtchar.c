// SPDX-License-Identifier: GPL-2.0
/***************************************************************************
 * @file virtchar.c
 * @brief Core module for the Virtual Character Device Driver initialization 
 * and exit routines
 *
 * @author Ameed Othman
 * @date 2024-11-18
 **************************************************************************/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#include "../include/virtchar.h"
#include "virtchar_ops.c"

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERS);

static int major_number;
static struct class * virtchar_class = NULL;
static struct device * virtchar_device = NULL;
struct virtchar_dev * virtchar_device_data;

/***********************************************************************
 * @brief Initialize the Virtual Character Device Driver module
 * @return Returns 0 on success or a negative error code on failure
 */
int __init virtchar_init(void) {
    int ret;
    dev_t dev_no;

    /* Allocate a major number dynamically */
    ret = alloc_chrdev_region(&dev_no, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("Failed to allocate a major number\n");
        return ret;
    }
    major_number = MAJOR(dev_no);
    pr_info("Registered with major number %d\n", major_number);

    /* Create device class */
    virtchar_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(virtchar_class)) {
        unregister_chrdev_region(MKDEV(major_number, 0), 1);
        pr_err("Failed to create device class\n");
        return PTR_ERR(virtchar_class);
    }

    /* Allocate and initialize device data */
    virtchar_device_data = kmalloc(sizeof(struct virtchar_dev), GFP_KERNEL);
    if (!virtchar_device_data) {
        class_destroy(virtchar_class);
        unregister_chrdev_region(MKDEV(major_number, 0), 1);
        pr_err("Failed to allocate device data\n");
        return -ENOMEM;
    }
    virtchar_device_data->buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!virtchar_device_data->buffer) {
        kfree(virtchar_device_data);
        class_destroy(virtchar_class);
        unregister_chrdev_region(MKDEV(major_number, 0), 1);
        pr_err("Failed to allocate buffer\n");
        return -ENOMEM;
    }
    virtchar_device_data->open_count = 0;

    /* Initialize the character device */
    cdev_init(&virtchar_device_data->cdev, &fops);
    virtchar_device_data->cdev.owner = THIS_MODULE;

    /* Add the character device to the system */
    ret = cdev_add(&virtchar_device_data->cdev, dev_no, 1);
    if (ret < 0) {
        kfree(virtchar_device_data->buffer);
        kfree(virtchar_device_data);
        class_destroy(virtchar_class);
        unregister_chrdev_region(dev_no, 1);
        pr_err("Failed to add cdev\n");
        return ret;
    }

    /* Create the device node in /dev */
    virtchar_device = device_create(virtchar_class, NULL, dev_no, NULL, DEVICE_NAME);
    if (IS_ERR(virtchar_device)) {
        cdev_del(&virtchar_device_data->cdev);
        kfree(virtchar_device_data->buffer);
        kfree(virtchar_device_data);
        class_destroy(virtchar_class);
        unregister_chrdev_region(dev_no, 1);
        pr_err("Failed to create device\n");
        return PTR_ERR(virtchar_device);
    }

    pr_info("Device initialized successfully\n");
    return 0;
}

/***********************************************************************************
 * @brief Exit routine for the Virtual Character Device Driver module
 **********************************************************************************/
void __exit virtchar_exit(void) {
    device_destroy(virtchar_class, MKDEV(major_number, 0));
    class_destroy(virtchar_class);
    cdev_del(&virtchar_device_data->cdev);
    kfree(virtchar_device_data->buffer);
    kfree(virtchar_device_data);
    unregister_chrdev_region(MKDEV(major_number, 0), 1);
    pr_info("Device unregistered and module unloaded\n");
}

module_init(virtchar_init);
module_exit(virtchar_exit);

