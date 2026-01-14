obj-m := fb_ili9488.o

SRC := $(shell pwd)

# Add include path for fbtft.h
ccflags-y := -I$(KERNEL_SRC)/drivers/staging/fbtft

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
