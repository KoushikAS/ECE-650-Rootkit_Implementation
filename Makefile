ifeq ($(KERNELRELEASE),)  

KERNELDIR ?= /lib/modules/$(shell uname -r)/build 
PWD := $(shell pwd)  

.PHONY: build clean  

build:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules  

clean:
	rm -rf *.o *~ core .depend .*.cmd *.order *.symvers *.ko *.mod.c sneaky_process
else  

$(info Building with KERNELRELEASE = ${KERNELRELEASE}) 
obj-m :=    sneaky_mod.o  

endif

CC=gcc
CFLAGS= -ggdb3 -fPIC
WDIR=..

all: sneaky_process

sneaky_process: sneaky_process.c
	$(CC) $(CFLAGS) -I$(WDIR) -L$(WDIR) -Wl,-rpath=$(WDIR) -o $@ sneaky_process.c

