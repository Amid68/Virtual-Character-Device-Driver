# Virtual Character Device Driver

## Overview

This project implements a virtual character device driver for Linux, allowing interaction between the kernel and user space via a virtual device file (`/dev/virtchar`). It demonstrates key concepts in Linux driver development, such as registering a device, handling file operations, and managing memory.

## Project Structure

```
virtchar/
├── Makefile
├── README.md
├── include/
│   └── virtchar.h
└── src/
    ├── virtchar.c
    └── virtchar_ops.c
```

## Building the Module

Run the following command in the project root directory:

```bash
make
```

## Installing the Module

Load the module into the kernel:

```bash
sudo insmod virtchar.ko
```

Verify that the module is loaded:

```bash
lsmod | grep virtchar
```

## Creating the Device File

The module now automatically creates the device file `/dev/virtchar`. If not, you can create it manually:

```bash
sudo mknod /dev/virtchar c <major_number> 0
sudo chmod 666 /dev/virtchar
```

Replace `<major_number>` with the major number assigned to the device (check `dmesg` logs).

## Testing the Driver

**Write to the Device:**

```bash
echo "Hello from userspace" > /dev/virtchar
```

**Read from the Device:**

```bash
cat /dev/virtchar
```

**Check Kernel Logs:**

```bash
dmesg | tail -n 20
```

## Uninstalling the Module

To remove the module from the kernel:

```bash
sudo rmmod virtchar
```

Clean the build files:

```bash
make clean
```

