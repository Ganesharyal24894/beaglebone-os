// SPDX-License-Identifier: GPL-2.0+
/*
 * FB driver for the ILI9488 LCD Controller (18-bit color)
 * Based on fb_ili9481.c
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <video/mipi_display.h>

#include "fbtft.h"

#define DRVNAME		"fb_ili9488"
#define WIDTH		320
#define HEIGHT		480

/* ILI9488 requires 18-bit color (3 bytes per pixel) */
static int init_display(struct fbtft_par *par)
{
	par->fbtftops.reset(par);
	mdelay(120);

	/* Software Reset */
	write_reg(par, 0x01);
	mdelay(120);

	/* Positive Gamma Control */
	write_reg(par, 0xE0, 0x00, 0x03, 0x09, 0x08, 0x16, 0x0A, 0x3F, 0x78,
		  0x4C, 0x09, 0x0A, 0x08, 0x16, 0x1A, 0x0F);

	/* Negative Gamma Control */
	write_reg(par, 0xE1, 0x00, 0x16, 0x19, 0x03, 0x0F, 0x05, 0x32, 0x45,
		  0x46, 0x04, 0x0E, 0x0D, 0x35, 0x37, 0x0F);

	/* Power Control 1 */
	write_reg(par, 0xC0, 0x17, 0x15);

	/* Power Control 2 */
	write_reg(par, 0xC1, 0x41);

	/* VCOM Control */
	write_reg(par, 0xC5, 0x00, 0x12, 0x80);

	/* Memory Access Control */
	write_reg(par, MIPI_DCS_SET_ADDRESS_MODE, 0x48);

	/* Interface Pixel Format - 18 bit */
	write_reg(par, MIPI_DCS_SET_PIXEL_FORMAT, 0x66);

	/* Interface Mode Control */
	write_reg(par, 0xB0, 0x00);

	/* Frame Rate Control */
	write_reg(par, 0xB1, 0xA0, 0x11);

	/* Display Inversion Control */
	write_reg(par, 0xB4, 0x02);

	/* Display Function Control */
	write_reg(par, 0xB6, 0x02, 0x02, 0x3B);

	/* Entry Mode Set */
	write_reg(par, 0xB7, 0xC6);

	/* Set Image Function */
	write_reg(par, 0xE9, 0x00);

	/* Adjust Control 3 */
	write_reg(par, 0xF7, 0xA9, 0x51, 0x2C, 0x82);

	/* Sleep Out */
	write_reg(par, MIPI_DCS_EXIT_SLEEP_MODE);
	mdelay(120);

	/* Display ON */
	write_reg(par, MIPI_DCS_SET_DISPLAY_ON);
	mdelay(20);

	return 0;
}

static void set_addr_win(struct fbtft_par *par, int xs, int ys, int xe, int ye)
{
	write_reg(par, MIPI_DCS_SET_COLUMN_ADDRESS,
		  xs >> 8, xs & 0xff, xe >> 8, xe & 0xff);

	write_reg(par, MIPI_DCS_SET_PAGE_ADDRESS,
		  ys >> 8, ys & 0xff, ye >> 8, ye & 0xff);

	write_reg(par, MIPI_DCS_WRITE_MEMORY_START);
}

/*
 * Convert 16-bit RGB565 to 18-bit RGB666 and write to display
 * ILI9488 in SPI mode only supports 18-bit color (3 bytes per pixel)
 */
static int write_vmem(struct fbtft_par *par, size_t offset, size_t len)
{
	u16 *vmem16;
	__be16 *txbuf16 = par->txbuf.buf;
	size_t remain;
	size_t to_copy;
	size_t tx_array_size;
	int i;
	int ret = 0;
	size_t startbyte_size = 0;

	if (!par->txbuf.buf) {
		dev_err(par->info->device, "txbuf.buf is NULL\n");
		return -EINVAL;
	}

	if (!par->info->screen_buffer) {
		dev_err(par->info->device, "screen_buffer is NULL\n");
		return -EINVAL;
	}

	/* Calculate available space for pixel data (leave room for startbyte if used) */
	if (par->startbyte) {
		txbuf16 = par->txbuf.buf + 1;
		startbyte_size = 1;
	}

	/* Buffer size in pixels (accounting for 3 bytes per pixel output) */
	tx_array_size = (par->txbuf.len - startbyte_size) / 3;

	gpiod_set_value(par->gpio.dc, 1);

	vmem16 = (u16 *)(par->info->screen_buffer + offset);
	remain = len / 2;  /* Number of 16-bit pixels to convert */

	while (remain) {
		u8 *tx8 = (u8 *)txbuf16;

		to_copy = min(remain, tx_array_size);

		/* Convert RGB565 to RGB666 (3 bytes per pixel) */
		for (i = 0; i < to_copy; i++) {
			u16 pixel = vmem16[i];
			u8 r = (pixel >> 11) & 0x1F;
			u8 g = (pixel >> 5) & 0x3F;
			u8 b = pixel & 0x1F;

			/* Scale 5/6 bit color to 8 bit (only upper 6 bits used by display) */
			tx8[i * 3 + 0] = (r << 3) | (r >> 2);
			tx8[i * 3 + 1] = (g << 2) | (g >> 4);
			tx8[i * 3 + 2] = (b << 3) | (b >> 2);
		}

		vmem16 += to_copy;
		ret = par->fbtftops.write(par, par->txbuf.buf,
					  startbyte_size + to_copy * 3);
		if (ret < 0)
			return ret;

		remain -= to_copy;
	}

	return ret;
}

#define HFLIP 0x01
#define VFLIP 0x02
#define ROW_X_COL 0x20

static int set_var(struct fbtft_par *par)
{
	u8 val;

	switch (par->info->var.rotate) {
	case 270:
		val = ROW_X_COL | HFLIP | VFLIP | (par->bgr << 3);
		break;
	case 180:
		val = VFLIP | (par->bgr << 3);
		break;
	case 90:
		val = ROW_X_COL | (par->bgr << 3);
		break;
	default:
		val = HFLIP | (par->bgr << 3);
		break;
	}

	write_reg(par, MIPI_DCS_SET_ADDRESS_MODE, val);

	return 0;
}

static struct fbtft_display display = {
	.regwidth = 8,
	.width = WIDTH,
	.height = HEIGHT,
	.txbuflen = 4096,  /* Use smaller buffer, will be called multiple times */
	.fbtftops = {
		.init_display = init_display,
		.set_addr_win = set_addr_win,
		.write_vmem = write_vmem,
		.set_var = set_var,
	},
};

FBTFT_REGISTER_DRIVER(DRVNAME, "ilitek,ili9488", &display);

MODULE_ALIAS("spi:" DRVNAME);
MODULE_ALIAS("platform:" DRVNAME);
MODULE_ALIAS("spi:ili9488");
MODULE_ALIAS("platform:ili9488");

MODULE_DESCRIPTION("FB driver for the ILI9488 LCD Controller");
MODULE_AUTHOR("Custom");
MODULE_LICENSE("GPL");
