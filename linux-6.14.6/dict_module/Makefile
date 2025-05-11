obj-m := dict_module.o

dict_module-y := dict.o dict_mod.o

obj-y := dict_sys.o

dict_sys-y := dict.o dict_syscalls.o

all:
	make -C ../../root/lib/modules/6.14.6/build M=$(PWD) modules
clean:
	make -C ../../root/lib/modules/6.14.6/build M=$(PWD) clean
install:
	INSTALL_MOD_PATH=../../root/ make -C ../../root/lib/modules/6.14.6/build M=$(PWD) modules_install