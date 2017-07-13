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
/* #undef CONFIG_ENV_SIZE */ /* uncomment after removing temporary */
#undef DFU_ALT_INFO_MMC
#undef DFU_ALT_INFO_NOR
/* End of undefs */

#if defined(CONFIG_SPL_BUILD) && defined(CONFIG_SPL_USBETH_SUPPORT)
/* Remove other SPL modes. */
#define CONFIG_ENV_IS_NOWHERE
#undef CONFIG_ENV_IS_IN_NAND
/* disable host part of MUSB in SPL */
/* disable EFI partitions and partition UUID support */
#undef CONFIG_PARTITION_UUIDS
#undef CONFIG_EFI_PARTITION
#else

/* EXT4 */
#ifdef CONFIG_CMD_EXT4
#define CONFIG_EXT4_WRITE

/* U-boot env file in user data partition */
#define CONFIG_ENV_IS_IN_EXT4    1
#define EXT4_ENV_INTERFACE       "mmc"
#define EXT4_ENV_DEVICE_AND_PART "0:3" /* TODO */
#define EXT4_ENV_FILE            "/system/etc/uboot.env"
#define CONFIG_ENV_SIZE         1 * 1024 /* Assume sector size of 1024 */
#endif

#endif /* CONFIG_SPL_BUILD */

/* File updates */
#ifdef CONFIG_UPDATE_KUBOS
#define CONFIG_SYS_DFU_DATA_BUF_SIZE 500 * SZ_1K /* File transfer chunk size */
#define CONFIG_SYS_DFU_MAX_FILE_SIZE 4 * SZ_1M   /* Maximum size for a single file.  Currently zImage (~2.5M) */

/* DFU Configuration TODO*/
#define DFU_ALT_INFO_MMC \
	"dfu_alt_info_mmc=" 		\
	"kernel fat 0 1;" 		\
	"rootfs part 0 2; " \
	"uboot fat 0 1;" \
	"dtb fat 0 1" \
	"\0"

#define DFU_ALT_INFO_NOR ""
#else
#define DFU_ALT_INFO_MMC ""
#define DFU_ALT_INFO_NOR ""
#endif /* CONFIG_UPDATE_KUBOS */

/* TODO: The mmc device will change when we start booting from emmc rather than directly from the sd card */
#define CONFIG_BOOTCOMMAND \
	"fatload mmc 0:1 ${fdtaddr} /pumpkin-mbm2.dtb; " \
	"fatload mmc 0:1 ${loadaddr} /kernel; " \
	"bootm ${loadaddr} - ${fdtaddr}"

#define CONFIG_BOOTARGS \
	"console=ttyS0,115200 "				\
	"root=/dev/mmcblk0p2 ext4 rootwait"

#ifndef CONFIG_SPL_BUILD
#define CONFIG_EXTRA_ENV_SETTINGS \
	DEFAULT_LINUX_BOOT_ENV \
	NETARGS \
	BOOTENV \
	KUBOS_UPDATE_ARGS
#endif

