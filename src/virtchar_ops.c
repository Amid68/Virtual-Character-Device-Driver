// SPDX-License-Identifier: GPL-2.0
/********************************************************************
 * @file virtchar_ops.c                                             *
 * @brief Device operations for the Virtual Character Device Driver *
 *                                                                  *
 * @author Ameed Othman                                             *
 * @date 2024-11-18                                                 *
 ********************************************************************/

#include <linux/fs.h>
#include <linux/uaccess.h> // contains copy_to_user & copy_from_user
#include "../include/virtchar.h"

extern struct virtchar_dev * virtchar_device_data;

/************************************************
 * @brief Open the virtual character device     *
 * @param inodep Pointer to the inode structure *
 * @param filep Pointer to the file structure   *
 * @return Returns 0 on success                 *
 ************************************************/
static int dev_open(struct inode * inodep, struct file * filep) {
    virtchar_device_data->open_count++;
    pr_info("Device opened %d time(s)\n", virtchar_device_data->open_count);
    return 0;
}

/************************************************
 * @brief Release the virtual character device  *
 * @param inodep Pointer to the inode structure *
 * @param filep Pointer to the file structure   *
 * @return Returns 0 on success                 *
 ************************************************/
static int dev_release(struct inode * inodep, struct file * filep) {
    pr_info("Device successfully closed\n");
    return 0;
}

/****************************************************************************
 * @brief Read data from the virtual character device                       *
 * @param filep Pointer to the file structure                               *
 * @param buffer User space buffer to write data to                         *
 * @param len Number of bytes to read                                       *
 * @param offset Offset in the device buffer                                *
 * @return Number of bytes read on success, negative error code on failure  *
 ****************************************************************************/
static ssize_t dev_read(struct file * filep, char __user * buffer, size_t len, loff_t *offset) {
    int bytes_to_read = min(len, (size_t)(BUFFER_SIZE - *offset));
    if (bytes_to_read == 0) {
        return 0;
    }

    if (copy_to_user(buffer, virtchar_device_data->buffer + *offset, bytes_to_read)) {
        return -EFAULT;
    }

    *offset += bytes_to_read;
    pr_info("Read %d bytes from device\n", bytes_to_read);
    return bytes_to_read;
}

/********************************************************************************
 * @brief Write data to the virtual character device                            *
 * @param filep Pointer to the file structure                                   *
 * @param buffer User space buffer containing data to write                     *
 * @param len Number of bytes to write                                          *
 * @param offset offset in the device buffer                                    *
 * @return Number of bytes written on success, negative error code on failure   *
 ********************************************************************************/
static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t * offset) {
    int bytes_to_write = min(len, (size_t)(BUFFER_SIZE - *offset));
    if (bytes_to_write == 0) {
        return -ENOSPC; // No space left in buffer
    }

    if (copy_from_user(virtchar_device_data->buffer + *offset, buffer, bytes_to_write)) {
        return -EFAULT;
    }

    *offset += bytes_to_write;
    pr_info("Wrote %d bytes to device\n", bytes_to_write);
    return bytes_to_write;
}

struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

