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


#define CONFIG_EXT4_WRITE

#include <net.h>
#include <ext4fs.h>
#include <fat.h>
#include <fs.h>
#include <mmc.h>


#define KERNEL "kernel.itb"
#define ROOTFS "rootfs"
#define KERNEL_PART  5
#define UPGRADE_PART 7

#define MAX_LOAD 10000000 //Max file size: 10MB

//The boot partition is FAT format, not EXT4
int update_kubos_kernel(struct blk_desc *block_dev, void *addr, loff_t len)
{
	disk_partition_t part_info = {};
	char * file = "zImage";
	loff_t actlen;
	int ret = 0;

	//Mount kernel partition
	if(part_get_info(block_dev, KERNEL_PART, &part_info))
	{
		printf("ERROR: Could not mount kernel partition.  No partition table\n");
	}

	fat_set_blk_dev(block_dev, &part_info);

	//Copy in new file
	/*
	 * Erase old file?  Rename old file?
	 * For right now, probably just erase it...but definitely not in final version...
	 */
	printf("Info: Writing new kernel file\n");

	ret = file_fat_write(file, addr, 0, len, &actlen);
	if (ret < 0)
	{
		printf("ERROR: Couldn't write kernel file - %d\n", ret);

		return -1;
	}


	return 0;
}

int update_kubos(void)
{
	struct mmc *mmc;
	disk_partition_t part_info = {};
	char * file;
	char * env_addr;
	loff_t actlen;
	ulong addr;

	int i, err, ret = 0;

	/*
	 * Load the SD card
	 */
	mmc = find_mmc_device(0);
	if(!mmc)
	{
		printf("ERROR: Could not access SD card - %d\n", ret);
	}

	ret = mmc_init(mmc);
	if(ret)
	{
		printf("ERROR: Could not init SD card - %d\n", ret);
	}

	/*
	 * Get the name of the update file to load
	 * Default will eventually be kpack.itb, probably
	 * The thought is that non-default would be a version-specific file, like kpack-v2.0.1.itb
	 * Allows us a) to track what version is being loaded and b) to rollback to a previous version
	 */
	file = getenv("updatefile");
	if (file == NULL)
	{
		printf("INFO: updatefile envar not found. Using default\n");
		file = KERNEL;
	}

	/* Temp address to load to */
	if ((env_addr = getenv("loadaddr")) != NULL)
	{
		addr = simple_strtoul(env_addr, NULL, 16);
	}
	else
	{
		addr = CONFIG_SYS_SDRAM_BASE + 0x200;
	}

	//Mount the upgrade partition
	if(part_get_info(&mmc->block_dev, UPGRADE_PART, &part_info))
	{
		printf("ERROR: Could not mount upgrade partition.  No partition table\n");
	}


	printf("Info: Checking for new firmware files\n");

	ext4fs_set_blk_dev(&mmc->block_dev, &part_info);

	ret = ext4fs_mount(0);
	if (!ret) {
		printf("ERROR: Could not mount upgrade partition. ext4fs mount err - %d\n", ret);

		return -1;
	}

	//This will be a loop eventually.  Right now just processing zImage
	//for(i = 0; i < #_files; i++)
	for(i = 0; i < 1; i++)
	{

		ret = ext4fs_exists(file);
		if(!ret)
		{
			continue;
		}

		printf("INFO: Found file to upgrade - %s\n", file);

		ret = ext4_read_file(file, (void *)addr, 0, 0, &actlen);
		if (ret < 0)
		{
			printf("ERROR: Couldn't read %s file - %d\n", file, ret);

			continue;
		}

		ret = update_tftp(addr, "mmc", "0");
		if(ret)
		{
			printf("ERROR: System update failed - %d\n", ret);
		}

	}

	return 0;
}
