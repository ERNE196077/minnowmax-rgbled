obj-m += ws281x_dma.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd)	modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd)	clean
