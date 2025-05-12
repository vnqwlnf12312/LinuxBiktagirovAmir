obj-m := vma_module.o

vma_module-y := vma.o

all:
	make -C ../../root/lib/modules/6.14.6/build M=$(PWD) modules
clean:
	make -C ../../root/lib/modules/6.14.6/build M=$(PWD) clean
install:
	INSTALL_MOD_PATH=../../root/ make -C ../../root/lib/modules/6.14.6/build M=$(PWD) modules_install