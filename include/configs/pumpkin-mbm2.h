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
/* End of undefs */

/* File updates */
#ifdef CONFIG_UPDATE_KUBOS
#define CONFIG_SYS_DFU_DATA_BUF_SIZE 500 * SZ_1K /* File transfer chunk size */
#define CONFIG_SYS_DFU_MAX_FILE_SIZE 4 * SZ_1M   /* Maximum size for a single file.  Currently zImage (~2.5M) */

/* DFU Configuration */
#define DFU_ALT_INFO_MMC \
	"dfu_alt_info_mmc=" 		\
	"kernel fat 0 5;" 		\
	"rootfs part 0 6\0"

#define DFU_ALT_INFO_NOR \
	"dfu_alt_info_nor="		    \
	"uboot raw 0xA000 0x56000;" \
	"dtb raw 0x70000 0x10000" \
	"\0"
#else
#define DFU_ALT_INFO_MMC ""
#define DFU_ALT_INFO_NOR ""
#endif

#define CONFIG_BOOTCOMMAND \
	"run distro_bootcmd"

#define CONFIG_EXTRA_ENV_SETTINGS \
	DEFAULT_LINUX_BOOT_ENV \
	DEFAULT_MMC_TI_ARGS \
	"bootpart=0:2\0" \
	"bootdir=/boot\0" \
	"bootfile=zImage\0" \
	"fdtfile=pumpkin-mbm2.dtb\0" \
	"console=ttyS0,115200n8\0" \
	"partitions=" \
		"uuid_disk=${uuid_gpt_disk};" \
		"name=rootfs,start=2MiB,size=-,uuid=${uuid_gpt_rootfs}\0" \
	"optargs=\0" \
	"loadimage=load mmc ${bootpart} ${loadaddr} ${bootdir}/${bootfile}\0" \
	"loadfdt=load mmc ${bootpart} ${fdtaddr} ${bootdir}/${fdtfile}\0" \
	"mmcloados=run args_mmc; run loadfdt; " \
				"bootz ${loadaddr} - ${fdtaddr}\0"
	"mmcboot=mmc dev ${mmcdev}; " \
		"if mmc rescan; then " \
			"echo SD/MMC found on device ${mmcdev};" \
			"run envboot; " \
			"if run loadimage; then " \
				"run mmcloados;" \
			"fi;" \
		"fi;\0" \
	NETARGS \
	DFUARGS \
	BOOTENV \
	KUBOS_UPDATE_ARGS
#endif

