/*
 * (C) Copyright 2007-2008
 * Stelian Pop <stelian@popies.net>
 * Lead Tech Design <www.leadtechdesign.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/at91_rstc.h>
#include <asm/arch/at91sam9_sdramc.h>

/* Reset the cpu by telling the reset controller to do so */
void reset_cpu(ulong ignored)
{
	at91_rstc_t *rstc = (at91_rstc_t *) ATMEL_BASE_RSTC;
	struct sdramc_reg *ramc = (struct sdramc_reg *)ATMEL_BASE_SDRAMC;

	puts("reset_cpu\r\n");

	//Need to disable running from SDRAM before rebooting or the system will hang
	writel(1, &ramc->tr);
	writel(AT91_SDRAMC_LPCB_POWER_DOWN, &ramc->lpr);

	writel(AT91_RSTC_KEY
		| AT91_RSTC_CR_PROCRST	 /*Processor Reset*/
		| AT91_RSTC_CR_PERRST	 /*Peripheral Reset*/
#ifdef CONFIG_AT91RESET_EXTRST
		| AT91_RSTC_CR_EXTRST	 /*External Reset (assert nRST pin)*/
#endif
		, &rstc->cr);
	/* never reached */
	while (1)
		;
}
