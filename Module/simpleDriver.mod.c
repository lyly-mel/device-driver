#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x999e8297, "vfree" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0x122c3a7e, "_printk" },
	{ 0xa65c6def, "alt_cb_patch_nops" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0x3678ca37, "system_cpucaps" },
	{ 0x4b0a3f52, "gic_nonsecure_priorities" },
	{ 0x3fd78f3b, "register_chrdev_region" },
	{ 0x3ff12ca3, "cdev_init" },
	{ 0xa07e16d2, "cdev_add" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x8a1ad681, "cdev_del" },
	{ 0x19c9d3b8, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "D8427094EE237388AE6D630");
