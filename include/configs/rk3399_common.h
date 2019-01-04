/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2016 Rockchip Electronics Co., Ltd
 */

#ifndef __CONFIG_RK3399_COMMON_H
#define __CONFIG_RK3399_COMMON_H

#include "rockchip-common.h"

#define CONFIG_SYS_MALLOC_LEN		(32 << 20)
#define CONFIG_SYS_CBSIZE		1024
#define CONFIG_SKIP_LOWLEVEL_INIT

#define COUNTER_FREQUENCY               24000000

#define CONFIG_SYS_NS16550_MEM32

#define CONFIG_SYS_INIT_SP_ADDR		0x00300000
#define CONFIG_SYS_LOAD_ADDR		0x00800800
#define CONFIG_SPL_STACK		0xff8effff
#define CONFIG_SPL_TEXT_BASE		0xff8c2000
#define CONFIG_SPL_MAX_SIZE		0x30000 - 0x2000
/*  BSS setup */
#define CONFIG_SPL_BSS_START_ADDR       0xff8e0000
#define CONFIG_SPL_BSS_MAX_SIZE         0x10000

#define CONFIG_SYS_BOOTM_LEN	(64 << 20)	/* 64M */

/* MMC/SD IP block */
#define CONFIG_BOUNCE_BUFFER
#define CONFIG_ROCKCHIP_SDHCI_MAX_FREQ	200000000

/* RAW SD card / eMMC locations. */
#define CONFIG_SYS_SPI_U_BOOT_OFFS	(128 << 10)

/* FAT sd card locations. */
#define CONFIG_SYS_MMCSD_FS_BOOT_PARTITION	1
#define CONFIG_SYS_SDRAM_BASE		0
#define SDRAM_MAX_SIZE			0xf8000000

#define CONFIG_SF_DEFAULT_SPEED 20000000

#ifndef CONFIG_SPL_BUILD

#define ENV_MEM_LAYOUT_SETTINGS \
	"scriptaddr=0x00500000\0" \
	"pxefile_addr_r=0x00600000\0" \
	"fdt_addr_r=0x01f00000\0" \
	"kernel_addr_r=0x02080000\0" \
	"ramdisk_addr_r=0x04000000\0"

/* #include <config_distro_bootcmd.h> */
#define BOOTENV \
	"arch=arm\0" \
	"baudrate=" __stringify(CONFIG_BAUDRATE) "\0" \
	"bootcmd=run setbootargs_cmd; ext4load ${boot_media} ${boot_dev}:${boot_part} ${fdt_addr_r} ${bootroot}/dtbs/${vendor}/${soc}-${board}.dtb; " \
	"ext4load ${boot_media} ${boot_dev}:${boot_part} ${kernel_addr_r} ${bootroot}/Image; booti ${kernel_addr_r} - ${fdt_addr_r}\0" \
	"bootroot=/boot\0" \
	"boot_dev=0\0" \
	"boot_media=mmc\0" \
	"boot_part=2\0" \
	"console=ttyS2\0" \
	"root_dev=mmcblk2\0" \
	"root_part=p2\0" \
	"setbootargs_cmd=setenv bootargs \"console=${console},${baudrate}n8 root=/dev/${root_dev}${root_part} rootwait ${extra_params}\0" \
	"soc=rk3399\0" \
	"vendor=rockchip\0"

#define CONFIG_EXTRA_ENV_SETTINGS \
	ENV_MEM_LAYOUT_SETTINGS \
	"fdtfile=" CONFIG_DEFAULT_FDT_FILE "\0" \
	"partitions=" PARTS_DEFAULT \
	BOOTENV

#endif

/* enable usb config for usb ether */

#endif
