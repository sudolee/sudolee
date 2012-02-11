ifeq ($(KERNELRELEASE),)
modules:
	$(MAKE) -C /home/sudolee/workstation/kernel_msm/ M=$(shell pwd) $@

clean:
	rm -f *.o *.cmd *.mod.* modules.order Module.symvers *.ko .*.cmd
	rm -rf .tmp_versions/

else
	obj-m := debugfs.o
endif
.PHONY:modules clean
