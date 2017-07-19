/*
* Copyright (C) 2017 Kubos Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* pumpkin-mbm2.h
*
* Configuration file for the Pumpkin Motherboard Module 2, using a Beaglebone Black
* as the OBC.
*
* Note: By default, KubOS Linux will be booted from the eMMC storage. This is defined
* to the system as the SECOND MMC device. The microSD card is defined as the FIRST MMC
* device.
*/

#pragma once

#include "am335x_evm.h"
#include "kubos-common.h"

/* Undo things we don't want to include from the base Beaglebone Black configuration */
#undef CONFIG_SYS_LDSCRIPT /* For NOR flash, which we (and the BBB) don't support */
#undef CONFIG_BOOTCOMMAND
#undef CONFIG_EXTRA_ENV_SETTINGS
#undef CONFIG_ENV_IS_IN_MMC
#undef CONFIG_ENV_IS_IN_FAT
#undef CONFIG_ENV_IS_NOWHERE
#undef CONFIG_ENV_SIZE
#undef DFU_ALT_INFO_MMC
#undef DFU_ALT_INFO_NOR
#undef CONFIG_BOOTCOUNT_AM33XX
/* End of undefs */

/* If we're compiling for the SPL, we don't have an env area */
#if defined(CONFIG_SPL_BUILD) && defined(CONFIG_SPL_USBETH_SUPPORT)
#define CONFIG_ENV_IS_NOWHERE
#else
/* Otherwise, it's in an ext4 partition */
#ifdef CONFIG_CMD_EXT4
#define CONFIG_EXT4_WRITE
#define CONFIG_ENV_IS_IN_EXT4    1
#define EXT4_ENV_INTERFACE       "mmc"
#define EXT4_ENV_DEVICE_AND_PART "1:3"
#define EXT4_ENV_FILE            "/system/etc/uboot.env"
#define CONFIG_ENV_SIZE         10 * 1024 /* Assume sector size of 1024 */
#endif
#endif /* CONFIG_SPL_BUILD */

/* File updates */
#ifdef CONFIG_UPDATE_KUBOS
#define CONFIG_SYS_DFU_DATA_BUF_SIZE 500 * SZ_1K /* File transfer chunk size */
#define CONFIG_SYS_DFU_MAX_FILE_SIZE 4 * SZ_1M   /* Maximum size for a single file.  Currently kernel (~2.5M) */

#define KUBOS_UPGRADE_DEVICE 0
#define KUBOS_UPGRADE_PART   1
#define KUBOS_UPGRADE_STORAGE CONFIG_SYS_LOAD_ADDR /* Temporary SDRAM storage location */

/* DFU Configuration */
#define DFU_ALT_INFO_MMC \
	"dfu_alt_info_mmc=" 		\
	"kernel fat 1 1;" 		\
	"rootfs part 1 2;" \
	"uboot fat 1 1;" \
	"dtb fat 1 1" \
	"\0"

#define DFU_ALT_INFO_NOR ""
#else
#define DFU_ALT_INFO_MMC ""
#define DFU_ALT_INFO_NOR ""
#endif /* CONFIG_UPDATE_KUBOS */

#define CONFIG_BOOTCOMMAND \
	"setenv bootargs console=ttyS0,115200 root=/dev/mmcblk${boot_dev}p2 ext4 rootwait; " \
	"fatload mmc ${boot_dev}:1 ${fdtaddr} /pumpkin-mbm2.dtb; " \
	"fatload mmc ${boot_dev}:1 ${loadaddr} /kernel; " \
	"bootm ${loadaddr} - ${fdtaddr}"

#ifndef CONFIG_SPL_BUILD
#define CONFIG_EXTRA_ENV_SETTINGS \
	"boot_dev=1\0" \
	DEFAULT_LINUX_BOOT_ENV \
	NETARGS \
	BOOTENV \
	KUBOS_UPDATE_ARGS
#endif

/* Status LEDs */
#define CONFIG_RED_LED					0x53
#define CONFIG_GREEN_LED				0x54
#define CONFIG_YELLOW_LED				0x55
#define CONFIG_BLUE_LED					0x56
