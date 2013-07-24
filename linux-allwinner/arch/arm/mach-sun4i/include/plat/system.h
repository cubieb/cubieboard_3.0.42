/*
 * arch/arm/plat-sunxi/include/plat/system.h
 *
 * (C) Copyright 2007-2012
 * Allwinner Technology Co., Ltd. <www.allwinnertech.com>
 * Benn Huang <benn@allwinnertech.com>
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

#ifndef __SW_SYSTEM_H
#define __SW_SYSTEM_H

#include <asm/proc-fns.h>
#include <plat/platform.h>
#include <asm/delay.h>

extern unsigned long fb_start;
extern unsigned long fb_size;
extern unsigned long g2d_start;
extern unsigned long g2d_size;

static inline void arch_idle(void)
{
	cpu_do_idle();
}

static inline void arch_reset(char mode, const char *cmd)
{
	/* use watch-dog to reset system */
#define WATCH_DOG_CTRL_REG  (SW_VA_TIMERC_IO_BASE + 0x0094)
	*(volatile unsigned int *)WATCH_DOG_CTRL_REG = 0;
	__delay(100000);
#ifdef CONFIG_ARCH_SUN5I
	*(volatile unsigned int *)WATCH_DOG_CTRL_REG |= 2;
	while(1) {
		__delay(100);
		*(volatile unsigned int *)WATCH_DOG_CTRL_REG |= 1;
	}
#else
	*(volatile unsigned int *)WATCH_DOG_CTRL_REG = 3;
	while(1);
#endif
}

enum sw_ic_ver {
	MAGIC_VER_A = 0,
	MAGIC_VER_B,
	MAGIC_VER_C
};

enum sw_ic_ver sw_get_ic_ver(void);

#ifdef CONFIG_ARCH_SUN5I
struct sw_chip_id
{
    unsigned int sid_rkey0;
    unsigned int sid_rkey1;
    unsigned int sid_rkey2;
    unsigned int sid_rkey3;
};

int sw_get_chip_id(struct sw_chip_id *);
#endif

#endif
