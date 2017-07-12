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

/* File updates */
#ifdef CONFIG_UPDATE_KUBOS
#define CONFIG_SYS_DFU_DATA_BUF_SIZE 500 * SZ_1K /* File transfer chunk size */
#define CONFIG_SYS_DFU_MAX_FILE_SIZE 4 * SZ_1M   /* Maximum size for a single file.  Currently zImage (~2.5M) */
#endif

/* Update Definitions */
#ifdef CONFIG_UPDATE_KUBOS

#define KUBOS_CURR_VERSION "kubos_curr_version"
#define KUBOS_PREV_VERSION "kubos_prev_version"
#define KUBOS_CURR_TRIED   "kubos_curr_tried"
#define KUBOS_BASE         "kpack-base.itb"
#define KUBOS_UPDATE_FILE  "kubos_updatefile"

#endif

#ifdef TEMP_FAKE_DEFINE
#ifdef CONFIG_UPDATE_KUBOS

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

#define CONFIG_EXTRA_ENV_SETTINGS \
	"altbootcmd=setenv recovery_available 0; setenv bootcmd; saveenv\0" \
	"recovery_available=1\0" \
    "bootlimit=3\0" \
	KUBOS_CURR_VERSION "=" KUBOS_BASE "\0" \
	KUBOS_PREV_VERSION "=" KUBOS_BASE "\0" \
	KUBOS_CURR_TRIED "=0\0" \
	DFU_ALT_INFO_MMC \
	DFU_ALT_INFO_NOR

#else

#define CONFIG_EXTRA_ENV_SETTINGS \
	"altbootcmd=setenv recovery_available 0; setenv bootcmd; saveenv\0" \
	"recovery_available=1\0" \
    "bootlimit=1\0" \

#endif /* CONFIG_UPDATE_KUBOS */
#endif /* Placeholder */
