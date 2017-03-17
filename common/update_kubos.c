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

#include <common.h>
#include <net.h>
#include <ext4fs.h>
#include <fat.h>
#include <fs.h>
#include <mmc.h>
#include <kubos.h>

#define UPGRADE_PART 7
#define COUNT_ENVAR "kubos_updatecount"
#define FILE_ENVAR  "kubos_updatefile"
#define PART_ENVAR  "kubos_updatepart"
#define LOAD_ENVAR  "kubos_loadaddr"

int update_kubos_count(void)
{

	ulong count;
	int ret = -1;

	char *val = getenv(COUNT_ENVAR);

	if (val == NULL)
	{
		setenv(COUNT_ENVAR, "1");
	}
	else
	{
		count = simple_strtoul(val, NULL, 10);

		if (count > 1)
		{
			setenv(FILE_ENVAR, "bad");
			setenv(COUNT_ENVAR, "0");
			ret = -1;
		}
		else
		{
			count++;
			sprintf(val, "%lu", count);
			setenv(COUNT_ENVAR, val);
		}
	}

	saveenv();
	return ret;
}

/*
 * update_kubos
 *
 * U-Boot has a DFU utility which currently allows a board to download a new firmware package and
 * distribute the package components to the appropriate end-point locations.
 *
 * We want to leverage the second half of this utility and distribute a new firmware package that's
 * been copied into the upgrade partition, rather than via a USB/TFTP connection.
 *
 * Returns:
 *    0 - An upgrade package was successfully installed
 *   -1 - No upgrade package could be installed (either because of system error or because no package exists)
 *   -2 - No upgrade package could be installed, but trigger a reboot so that we can retry.
 */

int update_kubos(void)
{
	struct mmc *mmc;
	disk_partition_t part_info = {};
	char * file;
	char * env_addr;
	char * dfu_info;
	loff_t actlen;
	ulong addr, part = 0;

	int ret = ERR_NO_REBOOT;

	/*
	 * Get the name of the update file to load
	 */
	file = getenv(FILE_ENVAR);
	if (file == NULL)
	{
		debug("INFO: %s envar not found\n", FILE_ENVAR);
		return ERR_NO_REBOOT;
	}
	else if (!strcmp(file, "none") || !strcmp(file, "bad"))
	{
		debug("INFO: No update file specified\n");
		return ERR_NO_REBOOT;
	}

	/*
	 * Temp SDRAM address to load to
	 */
	if ((env_addr = getenv(LOAD_ENVAR)) != NULL)
	{
		addr = simple_strtoul(env_addr, NULL, 16);
	}
	else
	{
		addr = CONFIG_SYS_SDRAM_BASE + 0x200;
	}

	/*
	 * Load the SD card
	 */
	mmc = find_mmc_device(0);
	if (!mmc)
	{
		error("Could not access SD card\n");
		return ERR_NO_REBOOT;

	}

	ret = mmc_init(mmc);
	if (ret)
	{
		error("Could not init SD card - %d\n", ret);
		return ERR_NO_REBOOT;
	}

	/*
	 * Get and mount the upgrade partition
	 */
	if ((env_addr = getenv(PART_ENVAR)) != NULL)
	{
		part = simple_strtoul(env_addr, NULL, 16);
	}
	else
	{
		part = UPGRADE_PART;
	}

	if (part_get_info(&mmc->block_dev, part, &part_info))
	{
		error("Could not mount upgrade partition.  No partition table\n");
		return ERR_NO_REBOOT;
	}

	debug("INFO: Checking for new firmware files\n");

	ext4fs_set_blk_dev(&mmc->block_dev, &part_info);

	ret = ext4fs_mount(0);
	if (!ret) {

		error("Could not mount upgrade partition. ext4fs mount err - %d\n", ret);
		return ERR_NO_REBOOT;
	}

	ret = ext4fs_exists(file);

	if (update_kubos_count() != 0)
	{
		error("Number of update attempts exceeded. Abandoning update\n");
		return ERR_NO_REBOOT;
	}

	/*
	 * Upgrade file found, call the existing DFU utility
	 */
	if (ret)
	{
		debug("INFO: Found file to upgrade - %s\n", file);

		ret = ext4_read_file(file, (void *)addr, 0, 0, &actlen);

		if (ret < 0)
		{
			error("Couldn't read %s file - %d\n", file, ret);
			return ERR_REBOOT;
		}
		else
		{
			if (strstr(file,"nor") != NULL)
			{
				if ((dfu_info = getenv("dfu_alt_info_nor")) == NULL)
				{
					error("Can't upgrade nor files, dfu_alt_info_nor not defined\n");
					return ERR_REBOOT;
				}

				setenv("dfu_alt_info", dfu_info);

				/* The "0" parameter isn't used for NOR flash, but it has to be non-NULL */
				ret = update_tftp(addr, "nor", "0");
			}
			else
			{
				if ((dfu_info = getenv("dfu_alt_info_mmc")) == NULL)
				{
					error("Can't upgrade mmc files, dfu_alt_info_mmc not defined\n");
					return ERR_REBOOT;
				}

				setenv("dfu_alt_info", dfu_info);

				ret = update_tftp(addr, "mmc", "0");
			}

			if (ret)
			{
				error("System upgrade failed - %d\n", ret);
				return ERR_REBOOT;
			}
			else
			{
				debug("INFO: Upgrade completed successfully\n");
			}
		}
	}
	else
	{
		debug("INFO: Upgrade file not found '%s'\n", file);
		return ERR_REBOOT;
	}

	/* Reset the updatefile name so that we resume usual boot after rebooting */
	setenv(FILE_ENVAR, "none");
	setenv(COUNT_ENVAR, "0");
	saveenv();

	return OK_REBOOT;
}
