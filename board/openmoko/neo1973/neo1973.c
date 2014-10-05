/*
 * (C) Copyright 2014
 * Liviu Dudau <liviu@dudau.co.uk>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/s3c24x0_cpu.h>
#include <errno.h>
#include <power/pcf50606.h>
#include <power/pmic.h>

DECLARE_GLOBAL_DATA_PTR;

/* Fout = 266Mhz */
#define M_MDIV	0x7d
#define M_PDIV	0x1
#define M_SDIV	0x1

#define U_M_MDIV	0x78
#define U_M_PDIV	0x2
#define U_M_SDIV	0x3

static inline void pll_delay(unsigned long loops)
{
	__asm__ volatile ("1:\n"
	  "subs %0, %1, #1\n"
	  "bne 1b":"=r" (loops):"0" (loops));
}

/*
 * Miscellaneous platform dependent initialisations
 */

int board_early_init_f(void)
{
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();
#if 0
	struct s3c24x0_clock_power * const clk_power =
					s3c24x0_get_base_clock_power();

	/* to reduce PLL lock time, adjust the LOCKTIME register */
	writel(0xFFFFFF, &clk_power->locktime);

	/* configure MPLL */
	writel((M_MDIV << 12) + (M_PDIV << 4) + M_SDIV,
	       &clk_power->mpllcon);

	/* some delay between MPLL and UPLL */
	pll_delay(4000);

	/* configure UPLL */
	writel((U_M_MDIV << 12) + (U_M_PDIV << 4) + U_M_SDIV,
	       &clk_power->upllcon);

	/* some delay between MPLL and UPLL */
	pll_delay(8000);
#endif

	/* set up the I/O ports */
	writel(0x005E0FFF, &gpio->gpacon);
	writel(readl(&gpio->gpadat) | 0x00010000, &gpio->gpadat);	/* GPA16 = nNAND_WP */
	writel(0x00045455, &gpio->gpbcon);
	writel(0x000007FF, &gpio->gpbup);
	writel(readl(&gpio->gpbdat) | 0x4, &gpio->gpbdat);		/* GPB2 = nSD_PWR */
	writel(0xAAAA12A9, &gpio->gpccon);
	writel(0x0000FFFF, &gpio->gpcup);
	writel(0xAAAAAAAA, &gpio->gpdcon);
	writel(0x0000FFFF, &gpio->gpdup);
	writel(0xAAAAAAAA, &gpio->gpecon);
	writel(0x0000FFFF, &gpio->gpeup);
	writel(0x0000AA99, &gpio->gpfcon);
	writel(0x000000FF, &gpio->gpfup);
	writel(readl(&gpio->gpfdat) | 0x4, &gpio->gpfdat);		/* GPF2 = nGSM_EN */
	writel(0xFF14F0F8, &gpio->gpgcon);
	writel(0x0000AFEF, &gpio->gpgup);
	writel(0x0000FAAA, &gpio->gphcon);
	writel(0x000007FF, &gpio->gphup);

	return 0;
}

const u_int8_t pcf50606_init_regs[][2] = {
	{ PCF50606_REG_OOCS, 0 },
	{ PCF50606_REG_INT1M, 0 },
	{ PCF50606_REG_INT2M, 0 },
	{ PCF50606_REG_OOCC1, PCF50606_OOCC1_RTCWAK |	\
				PCF50606_OOCC1_CHGWAK | \
				PCF50606_OOCC1_EXTONWAK_HIGH },
	{ PCF50606_REG_OOCC2, PCF50606_OOCC2_ONKEYDB_14ms | PCF50606_OOCC2_EXTONDB_14ms },
	{ PCF50606_REG_PSSC, 0x00 },
	{ PCF50606_REG_PWROKM, 0x00 },
	{ PCF50606_REG_DCDC1, 0x18 },	/* GL_1V5: off */
	{ PCF50606_REG_DCDC2, 0x00 },
	{ PCF50606_REG_DCDC3, 0x00 },
	{ PCF50606_REG_DCDC4, 0x30 }, /* 1.25A */
	{ PCF50606_REG_DCDEC1, 0xe8 }, /* IO_3V3: on */
	{ PCF50606_REG_DCDEC2, 0x00 },
	{ PCF50606_REG_DCUDC1, 0xc4 }, /* CORE_1V8: 2.1V if PWREN2 = HIGH */
	{ PCF50606_REG_DCUDC2, 0x30 }, /* 1.25A current limit */
	//see internal bug 94 { PCF50606_REG_IOREGC,  0x18 }, /* CODEC_3V3: off */
	{ PCF50606_REG_IOREGC, 0xf8 }, /* CODEC_3V3: on */
	{ PCF50606_REG_D1REGC1, 0x16 }, /* BT_3V15: off */
	{ PCF50606_REG_D2REGC1, 0x10 }, /* GL_2V5: off */
	{ PCF50606_REG_D3REGC1, 0xec }, /* STBY_1V8: 2.1V */
	{ PCF50606_REG_LPREGC1, 0xf8 }, /* LCM_3V3: on */
	{ PCF50606_REG_LPREGC2, 0x00 },
	{ PCF50606_REG_MBCC1, 0x0f }, /* CHGAPE | AUTOFST | fast_cccv */
	{ PCF50606_REG_MBCC2, 0x00 },	/* unlimited charging */
	{ PCF50606_REG_MBCC3, 0x2a }, /* 0.2*Ifast, 4.20V */
	{ PCF50606_REG_BBCC, 0x1f }, /* 400uA */
	{ PCF50606_REG_ADCC1, 0x00 },
	{ PCF50606_REG_ADCC2, 0x00 },
	{ PCF50606_REG_ACDC1, 0x86 },	/* ACD thresh 1.6V, enabled */
	{ PCF50606_REG_BVMC, PCF50606_BVMC_THRSHLD_3V3 },
	{ PCF50606_REG_PWMC1, 0x00 },
	{ PCF50606_REG_LEDC1, 0x00 },
	{ PCF50606_REG_LEDC2, 0x00 },
	{ PCF50606_REG_GPOC1, 0x00 },
	{ PCF50606_REG_GPOC2, 0x00 },
	{ PCF50606_REG_GPOC3, 0x00 },
	{ PCF50606_REG_GPOC4, 0x00 },
	{ PCF50606_REG_GPOC5, 0x00 },
};

int board_init(void)
{
	struct pmic *p;
	int i;

	/* arch number of Neo1973 board */
	gd->bd->bi_arch_number = MACH_TYPE_NEO1973;

	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0x30000100;

	if (pmic_init(I2C_PMIC))
		return -1;

	p = pmic_get("PCF50606_PMIC");
	if (!p)
		return -ENODEV;

	if (pmic_probe(p))
		return -1;

	for (i = 0; i < ARRAY_SIZE(pcf50606_init_regs); i++)
		if (pmic_reg_write(p, pcf50606_init_regs[i][0], pcf50606_init_regs[i][1]))
			return -1;

	return 0;
}

int dram_init(void)
{
	/* dram_init must store complete ramsize in gd->ram_size */
	gd->ram_size = PHYS_SDRAM_1_SIZE;
	return 0;
}

void enable_caches(void)
{
	icache_enable();
	dcache_enable();
}
