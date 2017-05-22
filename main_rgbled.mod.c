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
	{ 0x5b407e3c, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xf170812e, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x14831722, __VMLINUX_SYMBOL_STR(pci_disable_device) },
	{ 0x9133da87, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x6729d3df, __VMLINUX_SYMBOL_STR(__get_user_4) },
	{ 0xc7d12060, __VMLINUX_SYMBOL_STR(pci_dev_get) },
	{ 0x27ef9dd7, __VMLINUX_SYMBOL_STR(pci_release_regions) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0xb5aa7165, __VMLINUX_SYMBOL_STR(dma_pool_destroy) },
	{ 0x70201cb, __VMLINUX_SYMBOL_STR(pci_set_master) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x94ee6c92, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x2a37d074, __VMLINUX_SYMBOL_STR(dma_pool_free) },
	{ 0x2072ee9b, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0x9eeb2268, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x550d1fd2, __VMLINUX_SYMBOL_STR(module_put) },
	{ 0x42c8de35, __VMLINUX_SYMBOL_STR(ioremap_nocache) },
	{ 0x771cf835, __VMLINUX_SYMBOL_STR(dma_pool_alloc) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0x3f358e91, __VMLINUX_SYMBOL_STR(pci_unregister_driver) },
	{ 0xbd06f800, __VMLINUX_SYMBOL_STR(pci_request_regions) },
	{ 0x4b215b4e, __VMLINUX_SYMBOL_STR(dma_supported) },
	{ 0xedc03953, __VMLINUX_SYMBOL_STR(iounmap) },
	{ 0x864b164c, __VMLINUX_SYMBOL_STR(__pci_register_driver) },
	{ 0xe6831eb9, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0xb18fc606, __VMLINUX_SYMBOL_STR(pci_dev_put) },
	{ 0x71451036, __VMLINUX_SYMBOL_STR(pci_enable_device) },
	{ 0xe17e1d04, __VMLINUX_SYMBOL_STR(dma_pool_create) },
	{ 0x1cbc30f1, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0xe8e28830, __VMLINUX_SYMBOL_STR(dma_ops) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0x7c54c7ed, __VMLINUX_SYMBOL_STR(try_module_get) },
	{ 0xf20dabd8, __VMLINUX_SYMBOL_STR(free_irq) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("pci:v00008086d00000F0Esv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00000F06sv*sd*bc*sc*i*");
