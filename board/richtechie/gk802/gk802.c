/*
 * Copyright (C) 2010-2013 Freescale Semiconductor, Inc.
 * Copyright (C) 2013, Boundary Devices <info@boundarydevices.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/sys_proto.h>
#include <malloc.h>
#include <asm/arch/mx6-pins.h>
#include <asm/errno.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/imx-common/mxc_i2c.h>
#include <asm/imx-common/boot_mode.h>
#include <asm/imx-common/video.h>
#include <dm/platform_data/serial_mxc.h>
#include <mmc.h>
#include <fsl_esdhc.h>
#include <micrel.h>
#include <miiphy.h>
#include <netdev.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/mxc_hdmi.h>
#include <i2c.h>

DECLARE_GLOBAL_DATA_PTR;

#define UART_PAD_CTRL	\
	(PAD_CTL_PKE | PAD_CTL_PUE | PAD_CTL_PUS_100K_UP |		\
	 PAD_CTL_SPEED_MED | PAD_CTL_DSE_40ohm |			\
	 PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define USDHC_PAD_CTRL \
	(PAD_CTL_PKE | PAD_CTL_PUE | PAD_CTL_PUS_47K_UP |	\
	 PAD_CTL_SPEED_LOW | PAD_CTL_DSE_80ohm |		\
	 PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

static iomux_v3_cfg_t const uart4_pads[] = {
	IOMUX_PADS(PAD_KEY_COL0__UART4_TX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL)),
	IOMUX_PADS(PAD_KEY_ROW0__UART4_RX_DATA | MUX_PAD_CTRL(UART_PAD_CTRL)),
};

static iomux_v3_cfg_t const sd3_pads[] = {
	IOMUX_PADS(PAD_SD3_CLK__SD3_CLK    | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD3_CMD__SD3_CMD    | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD3_DAT0__SD3_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD3_DAT1__SD3_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD3_DAT2__SD3_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD3_DAT3__SD3_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_NANDF_CS0__GPIO6_IO11 | MUX_PAD_CTRL(NO_PAD_CTRL)), /* CD */
};

static iomux_v3_cfg_t const sd4_pads[] = {
	IOMUX_PADS(PAD_SD4_CLK__SD4_CLK    | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD4_CMD__SD4_CMD    | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD4_DAT0__SD4_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD4_DAT1__SD4_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD4_DAT2__SD4_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	IOMUX_PADS(PAD_SD4_DAT3__SD4_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL)),
	//IOMUX_PADS(PAD_NANDF_D6__GPIO2_IO06    | MUX_PAD_CTRL(NO_PAD_CTRL), /* CD */
};

static void setup_iomux_uart(void)
{
	SETUP_IOMUX_PADS(uart4_pads);
}

#ifdef CONFIG_FSL_ESDHC
struct fsl_esdhc_cfg usdhc_cfg[2] = {
	{USDHC3_BASE_ADDR},
	{USDHC4_BASE_ADDR},
};

int board_mmc_getcd(struct mmc *mmc)
{
	struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;
	int ret;

	if (cfg->esdhc_base == USDHC3_BASE_ADDR) {
		ret = !gpio_get_value(IMX_GPIO_NR(6, 11));
	} else {
		/*gpio_direction_input(IMX_GPIO_NR(2, 6));
		  ret = !gpio_get_value(IMX_GPIO_NR(2, 6)); */
		ret = 1;
	}

	return ret;
}

int board_mmc_init(bd_t *bis)
{
	int status = 0;

	SETUP_IOMUX_PADS(sd3_pads);
	SETUP_IOMUX_PADS(sd4_pads);

	usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC3_CLK);
	usdhc_cfg[1].sdhc_clk = mxc_get_clock(MXC_ESDHC4_CLK);

	gpio_direction_input(IMX_GPIO_NR(6, 11));

	status = fsl_esdhc_initialize(bis, &usdhc_cfg[1]);
	status |= fsl_esdhc_initialize(bis, &usdhc_cfg[0]);

	return status;
}
#endif

static void do_enable_hdmi(struct display_info_t const *dev)
{
}

struct display_info_t const displays[] = {
{
	.bus	= -1,
	.addr	= 0,
	.pixfmt	= IPU_PIX_FMT_RGB24,
	.detect = detect_hdmi,
	.enable = do_enable_hdmi,
	.mode	= {
		.name	 = "HDMI",
		.refresh = 60,
		.xres	 = 1024,
		.yres           = 768,
		.pixclock       = 15385,
		.left_margin    = 220,
		.right_margin   = 40,
		.upper_margin   = 21,
		.lower_margin   = 7,
		.hsync_len      = 60,
		.vsync_len      = 10,
		.sync           = FB_SYNC_EXT,
		.vmode          = FB_VMODE_NONINTERLACED
	}
} };

size_t display_count = ARRAY_SIZE(displays);

int board_early_init_f(void)
{
	setup_iomux_uart();

	return 0;
}

/*
 * Do not overwrite the console
 * Use always serial for U-Boot console
 */
int overwrite_console(void)
{
	return 1;
}

int dram_init(void)
{
	gd->ram_size = imx_ddr_size();

	return 0;
}

int board_init(void)
{
	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

	return 0;
}

int checkboard(void)
{
	puts("Board: Richtechie GK802\n");

	return 0;
}

int misc_init_r(void)
{
	return 0;
}

static struct mxc_serial_platdata gk802_serial_plat = {
	.reg = (struct mxc_uart *)UART4_BASE,
};

U_BOOT_DEVICE(gk802_serial) = {
	.name = "serial_mxc",
	.platdata = &gk802_serial_plat,
};
