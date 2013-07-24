/*
 * drivers/video/sunxi/lcd/lcd_panel_cfg_generic.c
 *
 * (C) Copyright 2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Danling <danliang@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include "lcd_panel_cfg.h"

#define CMD_WIRTE_DELAY 2

//#define SPI_DATA_PRINT

#ifdef SPI_DATA_PRINT
#define lcd_spi_dbg(x,arg...) printk(KERN_INFO"[LCD_SPI]"x,##arg)
#else
#define lcd_spi_dbg(x,arg...)
#endif

static __s32 lcd_spi_cs = 0;
static __s32 lcd_spi_clk = 0;
static __s32 lcd_spi_mosi = 0;
static __s32 lcd_spi_used = 0;
static __s32 lcd_spi_module = -1;

#ifdef CONFIG_ARCH_SUN5I
#include "../../../power/axp_power/axp-gpio.h"
extern int axp_gpio_set_io(int gpio, int io_state);
extern int axp_gpio_get_io(int gpio, int *io_state);
extern int axp_gpio_set_value(int gpio, int value);
extern int axp_gpio_get_value(int gpio, int *value);
#endif

static void check_spi_used_value()
{
	if (SCRIPT_PARSER_OK
			!= OSAL_Script_FetchParser_Data("lcd_spi_para", "lcd_spi_used",
					&lcd_spi_used, 1)) {
		__inf("LCD SPI doesn't use.\n");
	}

	if (0 == lcd_spi_used) {
		__inf("LCD SPI doesn't use.\n");
	}
}


void LCD_SPI_Init(__u32 sel)
{
	if (SCRIPT_PARSER_OK
			!= OSAL_Script_FetchParser_Data("lcd_spi_para", "lcd_spi_module",
					&lcd_spi_module, 1)) {
		__wrn("There is no LCD SPI module input.\n");
		return;
	}

	lcd_spi_cs = OSAL_GPIO_Request_Ex("lcd_spi_para", "lcd_spi_cs");
	if (!lcd_spi_cs) {
		__wrn("request gpio lcd_spi_cs error.\n");
		goto ERR1;
	}
	lcd_spi_clk = OSAL_GPIO_Request_Ex("lcd_spi_para", "lcd_spi_clk");
	if (!lcd_spi_clk) {
		__wrn("request gpio lcd_spi_clk error.\n");
		goto ERR2;
	}
	lcd_spi_mosi = OSAL_GPIO_Request_Ex("lcd_spi_para", "lcd_spi_mosi");
	if (!lcd_spi_mosi) {
		__wrn("request gpio lcd_spi_mosi error.\n");
		goto ERR3;
	}
	return;

	lcd_spi_dbg("release GPIO src : lcd_spi_mosi\n");
	OSAL_GPIO_Release(lcd_spi_mosi, 2);
ERR3:
	lcd_spi_dbg("release GPIO src : lcd_spi_clk\n");
	OSAL_GPIO_Release(lcd_spi_clk, 2);
ERR2:
	lcd_spi_dbg("release GPIO src : lcd_spi_cs\n");
	OSAL_GPIO_Release(lcd_spi_cs, 2);
ERR1:
	return;
}

void LCD_SPI_Write(__u32 sel)
{
	int i = 0, j = 0, offset = 0, bit_val = 0, ret = 0;
	u16 data[9] = { /* module 0 data */
			0x0029, /* reset */
			0x0025, /* standby */
			0x0840, /* enable normally black */
			0x0430, /* enable FRC/dither */
			0x385f, /* enter test mode(1) */
			0x3ca4, /* enter test mode(2) */
			0x3409, /* enable SDRRS, enlarge OE width */
			0x4041, /* adopt 2 line / 1 dot */
			/* wait 100ms */
			0x00ad, /* display on */
	};

	lcd_spi_dbg("============ start LCD SPI data write, module = %d============\n", lcd_spi_module);

	switch (lcd_spi_module) {
	case 0: { /* rili 7inch */
		for (i = 0; i < 8; i++) {
			OSAL_GPIO_DevWRITE_ONEPIN_DATA(lcd_spi_cs, 0, "lcd_spi_cs");
			lcd_spi_dbg("write data[%d]:", i);

			for (j = 0; j < 16; j++) {
				OSAL_GPIO_DevWRITE_ONEPIN_DATA(lcd_spi_clk, 0, "lcd_spi_clk");
				offset = 15 - j;
				bit_val = (0x0001 & (data[i] >> offset));
				ret = OSAL_GPIO_DevWRITE_ONEPIN_DATA(lcd_spi_mosi, bit_val,
						"lcd_spi_mosi");
#ifdef SPI_DATA_PRINT
				if (ret == 0) {
					lcd_spi_dbg("%d-", bit_val);
				} else {
					lcd_spi_dbg("write[bit:%d]ERR", j);
				}
#endif
				LCD_delay_us(CMD_WIRTE_DELAY);
				OSAL_GPIO_DevWRITE_ONEPIN_DATA(lcd_spi_clk, 1, "lcd_spi_clk");
				LCD_delay_us(CMD_WIRTE_DELAY);
			}

			lcd_spi_dbg("\n");
			OSAL_GPIO_DevWRITE_ONEPIN_DATA(lcd_spi_cs, 1, "lcd_spi_cs");
			OSAL_GPIO_DevWRITE_ONEPIN_DATA(lcd_spi_clk, 1, "lcd_spi_clk");
			LCD_delay_us(CMD_WIRTE_DELAY);
		}
		LCD_delay_ms(50);
		OSAL_GPIO_DevWRITE_ONEPIN_DATA(lcd_spi_cs, 0, "lcd_spi_cs");

		lcd_spi_dbg("write data[8]:");

		for (j = 0; j < 16; j++) {
			OSAL_GPIO_DevWRITE_ONEPIN_DATA(lcd_spi_clk, 0, "lcd_spi_clk");
			offset = 15 - j;
			bit_val = (0x0001 & (data[i] >> offset));
			ret = OSAL_GPIO_DevWRITE_ONEPIN_DATA(lcd_spi_mosi, bit_val,
					"lcd_spi_mosi");
#ifdef SPI_DATA_PRINT
			if (ret == 0) {
				lcd_spi_dbg("%d-", bit_val);
			} else {
				lcd_spi_dbg("write[bit:%d]ERR", j);
			}
#endif
			LCD_delay_us(CMD_WIRTE_DELAY);
			OSAL_GPIO_DevWRITE_ONEPIN_DATA(lcd_spi_clk, 1, "lcd_spi_clk");
			LCD_delay_us(CMD_WIRTE_DELAY);
		}

		lcd_spi_dbg("\n");
		OSAL_GPIO_DevWRITE_ONEPIN_DATA(lcd_spi_cs, 1, "lcd_spi_cs");
		OSAL_GPIO_DevWRITE_ONEPIN_DATA(lcd_spi_clk, 1, "lcd_spi_clk");
		LCD_delay_us(CMD_WIRTE_DELAY);
		lcd_spi_dbg("========== LCD SPI data translation finished ===========\n");
		break;
	}
	default: {
		lcd_spi_dbg("%s Unknow lcd_spi_module\n", __func__);
		break;
	}
	}
}

void LCD_SPI_Dinit(__u32 sel)
{
	lcd_spi_dbg("release GPIO src : lcd_spi_mosi\n");
	if (lcd_spi_mosi) {
		OSAL_GPIO_Release(lcd_spi_mosi, 2);
	}

	lcd_spi_dbg("release GPIO src : lcd_spi_clk\n");
	if (lcd_spi_clk) {
		OSAL_GPIO_Release(lcd_spi_clk, 2);
	}

	lcd_spi_dbg("release GPIO src : lcd_spi_cs\n");
	if (lcd_spi_cs) {
		OSAL_GPIO_Release(lcd_spi_cs, 2);
	}
}

void LCD_power_on_generic(__u32 sel)
{
#ifdef CONFIG_ARCH_SUN5I
	axp_gpio_set_io(0, 1);
	axp_gpio_set_value(0, 1);
#else
	LCD_POWER_EN(sel, 1); /* config lcd_power pin to open lcd power */
#endif
}

void LCD_power_off_generic(__u32 sel)
{
#ifdef CONFIG_ARCH_SUN5I
	axp_gpio_set_io(0, 1);
	axp_gpio_set_value(0, 0);
#else
	LCD_POWER_EN(sel, 0); /* config lcd_power pin to close lcd power */
#endif
}

void LCD_bl_open_generic(__u32 sel)
{
	LCD_PWM_EN(sel, 1); /* open pwm module */
#ifdef CONFIG_ARCH_SUN5I
	axp_gpio_set_io(1, 1);
	axp_gpio_set_value(1, 1);
#else
	LCD_BL_EN(sel, 1); /* config lcd_bl_en pin to open lcd backlight */
#endif
}

void LCD_bl_close_generic(__u32 sel)
{
#ifdef CONFIG_ARCH_SUN5I
	axp_gpio_set_io(1, 1);
	axp_gpio_set_value(1, 0);
#else
	LCD_BL_EN(sel, 0); /* config lcd_bl_en pin to close lcd backlight */
#endif
	LCD_PWM_EN(sel, 0); /* close pwm module */
}

__s32 LCD_open_flow_generic(__u32 sel)
{
	check_spi_used_value();
	LCD_OPEN_FUNC(sel, LCD_power_on_generic, 50); /* open lcd power, and delay 50ms */
	if (lcd_spi_used) {
		/* request and init gpio, and delay 20ms */
		LCD_OPEN_FUNC(sel, LCD_SPI_Init, 20);
		/* use gpio to config lcd module to the  work mode, and delay 10ms */
		LCD_OPEN_FUNC(sel, LCD_SPI_Write, 10);
	}
	LCD_OPEN_FUNC(sel, TCON_open, 500); /* open lcd controller, and delay 500ms */
	LCD_OPEN_FUNC(sel, LCD_bl_open_generic, 0); /* open lcd backlight, and delay 0ms */

	return 0;
}

__s32 LCD_close_flow_generic(__u32 sel)
{
	LCD_CLOSE_FUNC(sel, LCD_bl_close_generic, 0); /* close lcd backlight, and delay 0ms */
	LCD_CLOSE_FUNC(sel, TCON_close, 0); /* close lcd controller, and delay 0ms */
	if (lcd_spi_used) {
		LCD_CLOSE_FUNC(sel, LCD_SPI_Dinit, 0); /* release gpio, and delay 0ms */
	}
	LCD_CLOSE_FUNC(sel, LCD_power_off_generic, 1000); /* close lcd power, and delay 1000ms */

	return 0;
}

/* sel: 0:lcd0; 1:lcd1 */
__s32 LCD_user_defined_func_generic(__u32 sel, __u32 para1, __u32 para2,
		__u32 para3)
{
	return 0;
}

void LCD_get_panel_funs_generic(__lcd_panel_fun_t * fun)
{
	fun->cfg_panel_info = 0; /* lcd panel info defined in sys_config1.fex */
	fun->cfg_open_flow = LCD_open_flow_generic;
	fun->cfg_close_flow = LCD_close_flow_generic;
	fun->lcd_user_defined_func = LCD_user_defined_func_generic;
}
