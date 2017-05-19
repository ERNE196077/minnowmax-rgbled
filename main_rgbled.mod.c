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
	{ 0xadcaf24a, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x865df16d, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0xf67729d6, __VMLINUX_SYMBOL_STR(pci_disable_device) },
	{ 0x8a246fad, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x6729d3df, __VMLINUX_SYMBOL_STR(__get_user_4) },
	{ 0x4de1996a, __VMLINUX_SYMBOL_STR(pci_dev_get) },
	{ 0x5cbbbf9a, __VMLINUX_SYMBOL_STR(pci_release_regions) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0xb5aa7165, __VMLINUX_SYMBOL_STR(dma_pool_destroy) },
	{ 0xf1c2da44, __VMLINUX_SYMBOL_STR(pci_set_master) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xd316f4f6, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x2a37d074, __VMLINUX_SYMBOL_STR(dma_pool_free) },
	{ 0x2072ee9b, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0x7203f885, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x6318c44, __VMLINUX_SYMBOL_STR(module_put) },
	{ 0x42c8de35, __VMLINUX_SYMBOL_STR(ioremap_nocache) },
	{ 0x771cf835, __VMLINUX_SYMBOL_STR(dma_pool_alloc) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0xf1afd4e2, __VMLINUX_SYMBOL_STR(pci_unregister_driver) },
	{ 0x5be8a92c, __VMLINUX_SYMBOL_STR(pci_request_regions) },
	{ 0x1c677ae3, __VMLINUX_SYMBOL_STR(dma_supported) },
	{ 0xedc03953, __VMLINUX_SYMBOL_STR(iounmap) },
	{ 0x6794fa64, __VMLINUX_SYMBOL_STR(__pci_register_driver) },
	{ 0x9b643cb5, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x40b7be16, __VMLINUX_SYMBOL_STR(pci_dev_put) },
	{ 0x6e114cd5, __VMLINUX_SYMBOL_STR(pci_enable_device) },
	{ 0x69de7a82, __VMLINUX_SYMBOL_STR(dma_pool_create) },
	{ 0x5c93cf07, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x5dc8c595, __VMLINUX_SYMBOL_STR(dma_ops) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0x8fe9fb63, __VMLINUX_SYMBOL_STR(try_module_get) },
	{ 0xf20dabd8, __VMLINUX_SYMBOL_STR(free_irq) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("pci:v00008086d00000F0Esv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00000F06sv*sd*bc*sc*i*");
