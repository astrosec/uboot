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
#include <malloc.h>
#include <errno.h>
#include <div64.h>
#include <dfu.h>

static long dfu_get_medium_size_nor(struct dfu_entity *dfu)
{
	flagh_get_size
	return dfu->data.nor.size;
}

static int dfu_read_medium_nor(struct dfu_entity *dfu, u64 offset, void *buf,
		long *len)
{
	printf("dfu_read_medium not supported for NOR flash!\n");
	return -1;
}

static int dfu_write_medium_nor(struct dfu_entity *dfu,
		u64 offset, void *buf, long *len)
{
	int ret;

	ulong end_addr = offset + *len - 1;

	printf("dfu_write_medium_nor: start=%ul, end=%ul, buf=%p, len=%ul\n", stard_addr, end_addr, buff, len);

	if((ret = flash_sect_protect(0, offset, end_addr) != 0)
	{
		printf("Couldn't unprotect flash sector/s: %d\n", ret);
		return ret;
	}

	if((ret = flash_sect_erase(offset, end_addr) != 0)
	{
		printf("Couldn't unprotect flash sector/s: %d\n", ret);
		flash_sect_protect(1, offset, end_addr);
		return ret;
	}

	if((ret = flash_write(buf, offset, *len) != 0)
	{
		printf("Couldn't unprotect flash sector/s: %d\n", ret);
		flash_sect_protect(1, offset, end_addr);
		return ret;
	}

	if((ret = flash_sect_protect(1, offset, end_addr) != 0)
	{
		printf("Couldn't protect flash sector/s: %d\n", ret);
		return ret;
	}

	return ret;
}

static int dfu_flush_medium_nor(struct dfu_entity *dfu)
{
	return 0;
}

static unsigned int dfu_polltimeout_nor(struct dfu_entity *dfu)
{
	return DFU_DEFAULT_POLL_TIMEOUT;
}

int dfu_fill_entity_nor(struct dfu_entity *dfu, char *devstr, char *s)
{
	char *st;

	dfu->dev_type = DFU_DEV_NOR;

	st = strsep(&s, " ");
	if (!strcmp(st, "raw")) {
		dfu->layout = DFU_RAW_ADDR;
		dfu->data.nor.start = simple_strtoul(s, &s, 16);
		s++;
		dfu->data.nor.size = simple_strtoul(s, &s, 16);
	} else {
		printf("%s: Memory layout (%s) not supported!\n", __func__, st);
		return -1;
	}

	dfu->data.nor.info = addr2info(dfu->data.nor.start);

	dfu->get_medium_size = dfu_get_medium_size_nor;
	dfu->read_medium = dfu_read_medium_nor;
	dfu->write_medium = dfu_write_medium_nor;
	dfu->flush_medium = dfu_flush_medium_nor;
	dfu->poll_timeout = dfu_polltimeout_nor;

	/* initial state */
	dfu->inited = 0;

	return 0;
}
