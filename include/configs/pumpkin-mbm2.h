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
*/

#pragma once

#include "am335x_evm.h"
#include "kubos-common.h"

/* Undo things we don't want to include from the base Beaglebone Black configuration */
#undef CONFIG_SYS_LDSCRIPT /* For NOR flash, which we (and the BBB) don't support */
#undef CONFIG_BOOTCOMMAND
#undef BOOT_TARGET_DEVICES
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
    "run distro_bootcmd"

#define BOOT_TARGET_DEVICES(func) \
    func(LEGACY_MMC, legacy_mmc, 0) \
    func(LEGACY_MMC, legacy_mmc, 1) \

#ifndef CONFIG_SPL_BUILD
#define CONFIG_EXTRA_ENV_SETTINGS \
    DEFAULT_LINUX_BOOT_ENV \
    "mmcdev=0\0" \
    "mmcrootfstype=ext4 rootwait\0" \
    "finduuid=part uuid mmc ${bootpart} uuid\0" \
    "args_mmc=run finduuid;setenv bootargs console=${console} " \
        "${optargs} " \
        "root=PARTUUID=${uuid} ro " \
        "rootfstype=${mmcrootfstype}\0" \
    "bootfile=kernel\0" \
    "console=ttyS0,115200\0" \
    "optargs=\0" \
    "loadimage=fatload mmc ${mmcdev}:1 ${loadaddr} /${bootfile}\0" \
    "loadfdt=fatload mmc ${mmcdev}:1 ${fdtaddr} /${board}.dtb\0" \
    "mmcloados=run args_mmc; " \
        "if run loadfdt; then " \
            "bootm ${loadaddr} - ${fdtaddr}; " \
        "else " \
            "echo ERROR: Failed to load ${board}.dtb; " \
        "fi;\0" \
    "mmcboot=mmc dev ${mmcdev}; " \
        "if mmc rescan; then " \
            "echo SD/MMC found on device ${mmcdev};" \
            "if run loadimage; then " \
                "run mmcloados;" \
            "fi;" \
        "fi;\0" \
	NETARGS \
	BOOTENV \
	KUBOS_UPDATE_ARGS
#endif

