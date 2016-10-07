obj-m := main_rgbled.o
main_rgbled-objs := dma.o gpio.o rgbled_driver.o rgbled_devices.o 

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd)	modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd)	clean
