# SPDX-License-Identifier: GPL-2.0
# Makefile for the Virtual Character Device Driver

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

obj-m := virtchar.o
virtchar-objs := src/virtchar.o src/virtchar_ops.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

