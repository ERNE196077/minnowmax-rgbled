#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xef1c5950, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x7ea76efd, __VMLINUX_SYMBOL_STR(pci_unregister_driver) },
	{ 0x4c9d28b0, __VMLINUX_SYMBOL_STR(phys_base) },
	{ 0x7d4fa5a0, __VMLINUX_SYMBOL_STR(__pci_register_driver) },
	{ 0xbf656318, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x767789b9, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x51c3c51e, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0xb1ed8d7d, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x42ee214b, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x2a87f290, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0xde407e3c, __VMLINUX_SYMBOL_STR(try_module_get) },
	{ 0x3a4dddd9, __VMLINUX_SYMBOL_STR(dma_ops) },
	{ 0x42c8de35, __VMLINUX_SYMBOL_STR(ioremap_nocache) },
	{ 0x771cf835, __VMLINUX_SYMBOL_STR(dma_pool_alloc) },
	{ 0xcd4edb8e, __VMLINUX_SYMBOL_STR(dma_pool_create) },
	{ 0xa10a62f3, __VMLINUX_SYMBOL_STR(pci_set_master) },
	{ 0x621b72ec, __VMLINUX_SYMBOL_STR(pci_dev_get) },
	{ 0xb91d9a7, __VMLINUX_SYMBOL_STR(dma_supported) },
	{ 0x513dfde6, __VMLINUX_SYMBOL_STR(pci_request_regions) },
	{ 0x17b62112, __VMLINUX_SYMBOL_STR(pci_enable_device) },
	{ 0xa179afa5, __VMLINUX_SYMBOL_STR(pci_disable_device) },
	{ 0xe0f176a6, __VMLINUX_SYMBOL_STR(pci_release_regions) },
	{ 0xff1db77c, __VMLINUX_SYMBOL_STR(pci_dev_put) },
	{ 0xb5aa7165, __VMLINUX_SYMBOL_STR(dma_pool_destroy) },
	{ 0x2a37d074, __VMLINUX_SYMBOL_STR(dma_pool_free) },
	{ 0xedc03953, __VMLINUX_SYMBOL_STR(iounmap) },
	{ 0x167e7f9d, __VMLINUX_SYMBOL_STR(__get_user_1) },
	{ 0xf9a482f9, __VMLINUX_SYMBOL_STR(msleep) },
	{ 0x6729d3df, __VMLINUX_SYMBOL_STR(__get_user_4) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xd66074e6, __VMLINUX_SYMBOL_STR(module_put) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("pci:v00008086d00000F06sv*sd*bc*sc*i*");
