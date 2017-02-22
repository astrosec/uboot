/*
 * (C) Copyright 2007-2008
 * Stelian Pop <stelian@popies.net>
 * Lead Tech Design <www.leadtechdesign.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 * Modified for Kubos Linux:
 *   This file was originally based on at91sam920ek.c and
 *   has been modified for the at91sam9g20isis board.
 *   Extraneous options have been removed and some code
 *   to initialize the SD card port has been added.
 * Author: Catherine Freed <catherine@kubos.co>
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/at91sam9_matrix.h>
#include <asm/arch/at91sam9_smc.h>
#include <asm/arch/at91_common.h>
#include <asm/arch/clk.h>
#include <asm/arch/gpio.h>
#include <atmel_mci.h>
#include <mmc.h>

#include <netdev.h>

DECLARE_GLOBAL_DATA_PTR;

/* ------------------------------------------------------------------------- */
/*
 * Miscellaneous platform dependent initializations
 */

#ifdef CONFIG_GENERIC_ATMEL_MCI
/* this is a weak define that we are overriding */
int board_mmc_init(bd_t *bd)
{
	at91_mci_hw_init();

	debug("board_mmc_init turn on power pin\r\n");

	/* Turn on the SD0 power pin - value must be LOW */
	at91_set_pio_output(AT91_PIO_PORTB, 6, 0);

	return atmel_mci_init((void *)ATMEL_BASE_MCI);
}
#endif

int board_early_init_f(void)
{
	at91_periph_clk_enable(ATMEL_ID_PIOA);
	at91_periph_clk_enable(ATMEL_ID_PIOB);
	at91_periph_clk_enable(ATMEL_ID_PIOC);

	return 0;
}

int board_init(void)
{
	/* adress of boot parameters */
	gd->bd->bi_boot_params = CONFIG_SYS_SDRAM_BASE + 0x100;

	at91_seriald_hw_init();

	return 0;
}

int dram_init(void)
{
	gd->ram_size = get_ram_size(
		(void *)CONFIG_SYS_SDRAM_BASE,
		CONFIG_SYS_SDRAM_SIZE);
	return 0;
}

#ifdef CONFIG_RESET_PHY_R
void reset_phy(void)
{
}
#endif

int board_eth_init(bd_t *bis)
{
	int rc = 0;
#ifdef CONFIG_MACB
	rc = macb_eth_initialize(0, (void *)ATMEL_BASE_EMAC0, 0x00);
#endif
	return rc;
}


void board_isis_mmc_power_init(struct mmc *mmc)
{
	struct mmc_cmd cmd;
	int err;

	udelay(1000);

	cmd.cmdidx = MMC_CMD_GO_IDLE_STATE;
	cmd.resp_type = MMC_RSP_NONE;
	cmd.cmdarg = 0;

	if (err)
		debug("board_isis_mmc_power_init err=%d\r\n", err);

	udelay(2000);
}
