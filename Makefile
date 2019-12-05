#KERNELDIR =
#INSTALLDIR =

#H3-V390
KERNELDIR=

PWD := $(shell pwd)

obj-m+= tp2912.o

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

install:
	sudo scp led.ko $(INSTALLDIR)

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order  Module.symvers

.PHONY: modules install clean
