/*
 * drivers/video/sunxi/lcd/lcd_bak/lvds_1080.c
 *
 * (C) Copyright 2007-2012
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

//delete this line if you want to use the lcd para define in sys_config1.fex
//#define LCD_PARA_USE_CONFIG

#ifdef LCD_PARA_USE_CONFIG
static __u8 g_gamma_tbl[][2] = 
{
//{input value, corrected value}
    {0, 0},
    {15, 15},
    {30, 30},
    {45, 45},
    {60, 60},
    {75, 75},
    {90, 90},
    {105, 105},
    {120, 120},
    {135, 135},
    {150, 150},
    {165, 165},
    {180, 180},
    {195, 195},
    {210, 210},
    {225, 225},
    {240, 240},
    {255, 255},
};

static void LCD_cfg_panel_info(__panel_para_t * info)
{
    __u32 i = 0, j=0;
    
    memset(info,0,sizeof(__panel_para_t));

    info->lcd_x             = 1920;
    info->lcd_y             = 1080;
    info->lcd_dclk_freq     = 142;      //MHz
    
    info->lcd_pwm_not_used  = 0;
    info->lcd_pwm_ch        = 0;
    info->lcd_pwm_freq      = 10000;     //Hz
    info->lcd_pwm_pol       = 0;

    info->lcd_if            = 3;        //0:hv(sync+de); 1:8080; 2:ttl; 3:lvds

    info->lcd_hbp           = 13;      //hsync back porch
    info->lcd_ht            = 2160;     //hsync total cycle
    info->lcd_hv_hspw       = 0;        //hsync plus width
    info->lcd_vbp           = 13;       //vsync back porch
    info->lcd_vt            = 2 * 1125;  //vysnc total cycle *2
    info->lcd_hv_vspw       = 0;        //vysnc plus width

    info->lcd_hv_if         = 0;        //0:hv parallel 1:hv serial 
    info->lcd_hv_smode      = 0;        //0:RGB888 1:CCIR656
    info->lcd_hv_s888_if    = 0;        //serial RGB format
    info->lcd_hv_syuv_if    = 0;        //serial YUV format

    info->lcd_cpu_if        = 0;        //0:18bit 4:16bit
    info->lcd_frm           = 0;        //0: disable; 1: enable rgb666 dither; 2:enable rgb656 dither

    info->lcd_lvds_ch       = 1;        //0:single channel; 1:dual channel
    info->lcd_lvds_mode     = 0;        //0:NS mode; 1:JEIDA mode
    info->lcd_lvds_bitwidth = 0;        //0:24bit; 1:18bit
    info->lcd_lvds_io_cross = 1;        //0:normal; 1:pn cross

    info->lcd_io_cfg0       = 0x10000000;

    info->lcd_gamma_correction_en = 0;
    if(info->lcd_gamma_correction_en)
    {
        __u32 items = sizeof(g_gamma_tbl)/2;
        
        for(i=0; i<items-1; i++)
        {
            __u32 num = g_gamma_tbl[i+1][0] - g_gamma_tbl[i][0];

            //__inf("handling{%d,%d}\n", g_gamma_tbl[i][0], g_gamma_tbl[i][1]);
            for(j=0; j<num; j++)
            {
                __u32 value = 0;

                value = g_gamma_tbl[i][1] + ((g_gamma_tbl[i+1][1] - g_gamma_tbl[i][1]) * j)/num;
                info->lcd_gamma_tbl[g_gamma_tbl[i][0] + j] = (value<<16) + (value<<8) + value;
                //__inf("----gamma %d, %d\n", g_gamma_tbl[i][0] + j, value);
            }
        }
        info->lcd_gamma_tbl[255] = (g_gamma_tbl[items-1][1]<<16) + (g_gamma_tbl[items-1][1]<<8) + g_gamma_tbl[items-1][1];
        //__inf("----gamma 255, %d\n", g_gamma_tbl[items-1][1]);
    }
}
#endif

void LCD_get_panel_funs_0(__lcd_panel_fun_t * fun)
{
#ifdef LCD_PARA_USE_CONFIG
    fun->cfg_panel_info = LCD_cfg_panel_info;//delete this line if you want to use the lcd para define in sys_config1.fex
#endif
}

