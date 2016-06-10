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
	{ 0x34d1b86, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x27991d2a, __VMLINUX_SYMBOL_STR(pci_unregister_driver) },
	{ 0x753a3218, __VMLINUX_SYMBOL_STR(__pci_register_driver) },
	{ 0xd5f15861, __VMLINUX_SYMBOL_STR(try_module_get) },
	{ 0xa1f7b21e, __VMLINUX_SYMBOL_STR(dma_ops) },
	{ 0xa09ecd2, __VMLINUX_SYMBOL_STR(pci_set_master) },
	{ 0xae799a4c, __VMLINUX_SYMBOL_STR(pci_dev_get) },
	{ 0x3a8c628e, __VMLINUX_SYMBOL_STR(dma_supported) },
	{ 0x842d6a16, __VMLINUX_SYMBOL_STR(pci_request_regions) },
	{ 0x9341d2ea, __VMLINUX_SYMBOL_STR(pci_enable_device) },
	{ 0xa84d8157, __VMLINUX_SYMBOL_STR(pci_disable_device) },
	{ 0xeb8f6012, __VMLINUX_SYMBOL_STR(pci_release_regions) },
	{ 0x82aadd14, __VMLINUX_SYMBOL_STR(pci_dev_put) },
	{ 0xedc03953, __VMLINUX_SYMBOL_STR(iounmap) },
	{ 0x42c8de35, __VMLINUX_SYMBOL_STR(ioremap_nocache) },
	{ 0x8f9c199c, __VMLINUX_SYMBOL_STR(__get_user_2) },
	{ 0x167e7f9d, __VMLINUX_SYMBOL_STR(__get_user_1) },
	{ 0x4c9d28b0, __VMLINUX_SYMBOL_STR(phys_base) },
	{ 0x6729d3df, __VMLINUX_SYMBOL_STR(__get_user_4) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xc7be4853, __VMLINUX_SYMBOL_STR(module_put) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("pci:v00008086d00000F06sv*sd*bc*sc*i*");
