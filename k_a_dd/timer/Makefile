ifeq ($(KERNELRELEASE),)
modules:
	$(MAKE) -C /home/sudolee/workspace/linux-source-2.6.35/ M=$(shell pwd) $@
else
	obj-m := my_timer.o
endif

clean:
	rm -f *.o *.cmd *.mod.* modules.order Module.symvers *.ko .*.cmd
	rm -rf .tmp_versions/
.PHONY:clean modules
