# minnowmax-rgbled
WS2812B/APA102 Linux driver for Minnowboard Max
This is an experimental scholar project for the Minnowboard Max platform. The objective is to provide a Linux driver that use the SPI and DMA of the Minnowboard Max to interface with the LED strips of WS2812B and APA102 types. 

In addition, the libraries provide an Ambilight-like function just as the [Boblight](https://www.tweaking4all.com/home-theatre/xbmc/xbmc-boblight-openelec-ws2811-ws2812/) project.

## Requirements
The basic functionality to control RGB LEDs (APA102 & WS2812B) has been tested in the below platforms, BUT, it is intended to work in any Linux distribution:
* Minnowboard Max
* Xubuntu 16.04.2
* Arch Linux

The Ambilight-like function has been tested using the below desktop environments (Using X11):
* Gnome
* Xfce

As the hardware used by this driver already has an official Linux module it is needed to blacklist those modules. Add the below lines into your /etc/modprobe.d/blaklist.conf file:
```
blacklist dw_dmac_core
blacklist dw_dmac
blacklist dw_dmac_pci
blacklist pxa2xx_spi_pci
blacklist pxa2xx_spi
```
Note: Recent kernels contain dw_dmac driver as a built-in module, if it is your case, it is needed to recompile the kernel and modularize the dw_dmac driver so it can be blacklisted.

### Compilation
In the working directory there are two important files to get started:
* compile.sh
* Makefile

You would need to install some dependencies in order to compile the driver and the libraries
* Kernel headers
* Building Libraries (Ubuntu: build-esential; ArchLinux: base-devel)
* X11 dev libraries (Ubuntu: libx11-dev libxtst-dev; Archlinux: libx11 libxrender)

To compile and install the driver simply use:
```
make
make install
```
Reboot for the changes take effect, or run:
```
modprobe main_rgbled.ko
```

For the libraries, the project comes with an example main.c file containing two of the basic functions, the example main.c file is compiled by running the compile.sh script:
```
./compile.sh
```



