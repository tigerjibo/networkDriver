obj-m:= mod.o
KERNEL_SOURCE="/home/vysakh/test/linux-3.17.2"

default:
	make -C $(KERNEL_SOURCE) SUBDIRS=$(PWD) modules
clean:
	make -C $(KERNEL_SOURCE) SUBDIRS=$(PWD) clean
