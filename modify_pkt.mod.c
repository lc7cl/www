#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
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
	{ 0x51fa3f29, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xbb29b267, __VMLINUX_SYMBOL_STR(__gcov_merge_add) },
	{ 0x9dc5ee7, __VMLINUX_SYMBOL_STR(__gcov_init) },
	{ 0xde455f19, __VMLINUX_SYMBOL_STR(nf_unregister_hook) },
	{ 0x7cb3845d, __VMLINUX_SYMBOL_STR(nf_register_hook) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

