// SPDX-License-Identifier: GPL-2.0
/**
 * @file virtchar.h
 * @brief Header file for the Virtual Character Device Driver
 *
 * @author Ameed Othman
 * @date 2024-11-18
 */
#ifndef __VIRTCHAR_H__
#define __VIRTCHAR_H__

#include <linux/cdev.h>

#define DRIVER_AUTHOR   "Ameed Othman <othman.ameed@gmail.com>"
#define DRIVER_DESC     "Virtual Character Device Driver"
#define DRIVER_VERS     "1.0"
#define DEVICE_NAME     "virtchar"
#define BUFFER_SIZE     1024

// Device structure
struct virtchar_dev {
    struct cdev cdev;
    char *buffer;
    int open_count;
};

extern int virtchar_init(void);
extern void virtchar_exit(void);

#endif
