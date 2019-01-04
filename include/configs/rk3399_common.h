/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2016 Rockchip Electronics Co., Ltd
 */

#ifndef __CONFIG_RK3399_COMMON_H
#define __CONFIG_RK3399_COMMON_H

#include "rockchip-common.h"

#define CFG_IRAM_BASE			0xff8c0000

#define CFG_SYS_SDRAM_BASE		0
#define SDRAM_MAX_SIZE			0xf8000000

#ifndef CONFIG_XPL_BUILD

#ifndef ROCKCHIP_DEVICE_SETTINGS
#define ROCKCHIP_DEVICE_SETTINGS
#endif

#define ENV_MEM_LAYOUT_SETTINGS		\
	"scriptaddr=0x00c00000\0"	\
	"script_offset_f=0xffe000\0"	\
	"script_size_f=0x2000\0"	\
	"pxefile_addr_r=0x00e00000\0"	\
	"kernel_addr_r=0x02000000\0"	\
	"kernel_comp_addr_r=0x0a000000\0"	\
	"fdt_addr_r=0x12000000\0"	\
	"fdtoverlay_addr_r=0x12100000\0"	\
	"ramdisk_addr_r=0x12180000\0"	\
	"kernel_comp_size=0x8000000\0"

#define CFG_EXTRA_ENV_SETTINGS		\
	"fdtfile=" CONFIG_DEFAULT_FDT_FILE "\0"	\
	"partitions=" PARTS_DEFAULT	\
	ENV_MEM_LAYOUT_SETTINGS		\
	ROCKCHIP_DEVICE_SETTINGS	\
	"boot_targets=" BOOT_TARGETS "\0"

#undef BOOTENV
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

#endif /* CONFIG_XPL_BUILD */

#endif /* __CONFIG_RK3399_COMMON_H */
