# minnowmax-rgbled
WS2812B/APA102 Linux driver for Minnowboard Max
This is an experimental scholar project for the Minnowboard Max platform. The objective is to provide a Linux driver that use the SPI and DMA of the Minnowboard Max to interface with the LED strips of WS2812B and APA102 types. 

In addition, the libraries provide an Ambilight-like function just as the [Boblight](https://www.tweaking4all.com/home-theatre/xbmc/xbmc-boblight-openelec-ws2811-ws2812/) project.

## Requirements
As the hardware used by this driver already has an official Linux module it is needed to blacklist those modules. Add the below lines into your /etc/modprobe.d/blacklist.conf file:
```
blacklist dw_dmac_core
```
