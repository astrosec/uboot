/*
 * (C) Copyright 2007-2008
 * Stelian Pop <stelian@popies.net>
 * Lead Tech Design <www.leadtechdesign.com>
 *
 * Configuration settings for the AT91SAM9G20ISIS board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 * Modified for Kubos Linux:
 *   This file was originally based on at91sam9260ek.h and
 *   has been modified for the at91sam9g20isis board.
 *   Changes:
 *   	Configuration for NOR flash added
 *   	Un-needed configurations removed
 *   	Bootargs to load kernel from SD card added
 * Author: Catherine Freed <catherine@kubos.co>
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * SoC must be defined first, before hardware.h is included.
 * In this case SoC is defined in boards.cfg.
 */
#include <asm/hardware.h>

#include <linux/sizes.h>

/*
 * CONFIG_SYS_TEXT_BASE - The starting address of U-Boot.
 * Warning: changing CONFIG_SYS_TEXT_BASE requires
 * adapting the initial boot program.
 * Since the linker has to swallow that define, we must use a pure
 * hex number here!
 */
#define CONFIG_SYS_TEXT_BASE		0x20000000

/* ARM asynchronous clock */
#define CONFIG_SYS_AT91_SLOW_CLOCK	32768		/* slow clock xtal */
#define CONFIG_SYS_AT91_MAIN_CLOCK	18432000	/* main clock xtal */

/* Define actual evaluation board type from used processor type */
#ifdef CONFIG_AT91SAM9G20
# define CONFIG_AT91SAM9G20ISIS	/* It's a Kubos AT91SAM9G20ISIS board*/
#else
#error Messed up board configuration!
#endif

/* Misc CPU related */
#define CONFIG_ARCH_CPU_INIT
#define CONFIG_CMDLINE_TAG		/* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_SKIP_LOWLEVEL_INIT
#define CONFIG_BOARD_EARLY_INIT_F
#define CONFIG_DISPLAY_CPUINFO

/* general purpose I/O */
#define CONFIG_ATMEL_LEGACY		/* required until (g)pio is fixed */
#define CONFIG_AT91_GPIO
#define CONFIG_AT91_GPIO_PULLUP	1	/* keep pullups on peripheral pins */

/* serial console */
#define CONFIG_ATMEL_USART
#define CONFIG_USART_BASE		ATMEL_BASE_DBGU
#define	CONFIG_USART_ID			ATMEL_ID_SYS
#define CONFIG_BAUDRATE			115200

/* LED */
#define CONFIG_AT91_LED
#define	CONFIG_RED_LED		AT91_PIN_PC12	/* this is the power led */
#define	CONFIG_GREEN_LED	AT91_PIN_PC13	/* this is the user led */
#define CONFIG_YELLOW_LED   AT91_PIN_PC14
#define CONFIG_BLUE_LED     AT91_PIN_PC15   /* Unused */

/* File updates */
#define CONFIG_SYS_DFU_DATA_BUF_SIZE 500 * SZ_1K /* File transfer chunk size */
#define CONFIG_SYS_DFU_MAX_FILE_SIZE 2 * SZ_1M   /* Maximum size for a single file.  Currently zImage (~1M) */
#define CONFIG_UPDATE_KUBOS

/*
 * Command line configuration.
 */
#define CONFIG_CMD_NOR		1

/*
 * SDRAM: 1 bank, 32MB
 * Initialized before u-boot gets started.
 */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_SDRAM_BASE		ATMEL_BASE_CS1
#define CONFIG_SYS_SDRAM_SIZE		0x02000000

/*
 * Initial stack pointer: 4k - GENERATED_GBL_DATA_SIZE in internal SRAM,
 * leaving the correct space for initial global data structure above
 * that address while providing maximum stack area below.
 */
# define CONFIG_SYS_INIT_SP_ADDR \
	(ATMEL_BASE_SRAM1 + 0x1000 - GENERATED_GBL_DATA_SIZE)

/*
 * The (arm)linux board id set by generic code depending on configured board
 * (see boards.cfg for different boards)
 */
#ifdef CONFIG_AT91SAM9G20
	/* Make sure we're using the right board */
# ifdef CONFIG_AT91SAM9G20ISIS
	/* And set the machine type appropriately */
#  define CONFIG_MACH_TYPE MACH_TYPE_AT91SAM9G20ISIS
# else
#  error Unknown bard
# endif
#else
# error Unknown board
#endif

/* NOR flash */
#ifdef CONFIG_CMD_NOR
#define CONFIG_SYS_USE_NORFLASH 1
#endif

/* MMC - Turned on in defconfig file*/
#ifdef CONFIG_CMD_MMC
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_GENERIC_ATMEL_MCI
#endif

/* FAT */
#ifdef CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION
#define CONFIG_FAT_WRITE
#endif

/* EXT4 */
/* #ifdef CONFIG_CMD_EXT4 */
#define CONFIG_EXT4
#define CONFIG_EXT4_WRITE
/* #endif */

/* USB */
#define CONFIG_USB_ATMEL
#define CONFIG_USB_ATMEL_CLK_SEL_PLLB
#define CONFIG_USB_OHCI_NEW		1
#define CONFIG_SYS_USB_OHCI_CPU_INIT		1
#define CONFIG_SYS_USB_OHCI_REGS_BASE		0x00500000	/* AT91SAM9G20_UHP_BASE */
#define CONFIG_SYS_USB_OHCI_SLOT_NAME		"at91sam9g20"
#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS	2

#define CONFIG_SYS_LOAD_ADDR			0x21880000	/* load address to load zImage to */

#define CONFIG_SYS_MEMTEST_START		CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END			0x20e00000

#if defined(CONFIG_SYS_USE_NORFLASH)
/* (bootstrap + u-boot + env +dtb in flash) + (linux in mmc) */
#define CONFIG_ENV_IS_IN_FLASH	1
#define CONFIG_ENV_OFFSET		0x70000 /* Must start on a sector boundary */
#define CONFIG_ENV_SIZE		0x10000		/* 1 sector = 65 kB */
/* Copy .dtb file (NORFLASH @ 0x80000, size = 0x5000) and kernel (SD card, partition 5) into SDRAM, then boot them */
#define CONFIG_BOOTCOMMAND	"cp.b 0x10080000 0x21800000 0x5000; " \
				"fatload mmc 0:5 0x21880000 zImage; " \
				"bootz 0x21880000 - 0x21800000"
/* Define the initial console connection and rootfs location */
#define CONFIG_BOOTARGS							\
	"console=ttyS0,115200 "				\
	"root=/dev/mmcblk0p6 rootwait"

/* DFU Configuration */
#define CONFIG_DFU_ALT_INFO \
	"dfu_alt_info=" 		\
	"zImage fat 0 5;" 		\
	"rootfs part 0 6\0"

#define CONFIG_EXTRA_ENV_SETTINGS \
	CONFIG_DFU_ALT_INFO

#define CONFIG_SYS_FLASH_CFI			1
#define CONFIG_FLASH_CFI_DRIVER			1
#define PHYS_FLASH_1				0x10000000
#define CONFIG_SYS_FLASH_BASE			PHYS_FLASH_1
/* 1 Bank of flash memory.  8 small sectors (8192 bytes),
 * followed by 15 large sectors (65536 bytes).
 * Confirm by issuing 'flinfo' command from U-Boot prompt.
 */
#define CONFIG_SYS_MAX_FLASH_SECT		23
#define CONFIG_SYS_MAX_FLASH_BANKS		1
#endif

#define CONFIG_SYS_CBSIZE		256
#define CONFIG_SYS_MAXARGS		16
#define CONFIG_SYS_LONGHELP		1
#define CONFIG_CMDLINE_EDITING	1
#define CONFIG_AUTO_COMPLETE

/*
 * Size of malloc() pool
 */
/*#define CONFIG_SYS_MALLOC_LEN		ROUND(3 * CONFIG_ENV_SIZE + 128*1024, 0x1000)*/
#define CONFIG_SYS_MALLOC_LEN 	 	10 * SZ_1M
#endif
