/*  Copyright 2007,2013 Theo Berkau

    This file is part of Iapetus.

    Iapetus is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Iapetus is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Iapetus; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "../iapetus.h"

#define SH2_REG_WTCSR_R         (*(volatile u8 *)0xFFFFFE80)
#define SH2_REG_WTCSR_W(val)    (*((volatile u16 *)0xFFFFFE80) = 0xA500 | (u8)(val))
#define SH2_REG_WTCNT_R         (*(volatile u8 *)0xFFFFFE81)
#define SH2_REG_WTCNT_W(val)    (*((volatile u16 *)0xFFFFFE80) = 0x5A00 | (u8)(val))
                                                                          
#define SH2_REG_IPRA            (*(volatile u16 *)0xFFFFFEE2)
#define SH2_REG_VCRWDT          (*(volatile u16 *)0xFFFFFEE4)

#define SH2_REG_RTCSR_R           (*(volatile u32 *)0xFFFFFFF0)
#define SH2_REG_RTCSR_W(val)      (*((volatile u32 *)0xFFFFFFF0) = 0xA55A0000 | (u16)(val))
#define SH2_REG_RTCNT_R           (*(volatile u32 *)0xFFFFFFF4)
#define SH2_REG_RTCNT_W(val)      (*((volatile u32 *)0xFFFFFFF4) = 0xA55A0000 | (u16)(val))
#define SH2_REG_RTCOR_R           (*(volatile u32 *)0xFFFFFFF8)
#define SH2_REG_RTCOR_W(val)      (*((volatile u32 *)0xFFFFFFF8) = 0xA55A0000 | (u16)(val))

volatile u32 timercounter;
u32 timerfreq;

void timer_rtc_increment(void) __attribute__ ((interrupt_handler));
void timer_frt_increment(void) __attribute__ ((interrupt_handler));
void timer_wdt_increment(void) __attribute__ ((interrupt_handler));

//////////////////////////////////////////////////////////////////////////////

void timer_rtc_increment(void)
{
   timercounter++;
}

//////////////////////////////////////////////////////////////////////////////

void timer_frt_increment(void)
{
   timercounter++;
}

//////////////////////////////////////////////////////////////////////////////

void timer_wdt_increment(void)
{
   timercounter++;
}

//////////////////////////////////////////////////////////////////////////////

void timer_hblank_increment(void)
{
   timercounter++;
}

//////////////////////////////////////////////////////////////////////////////

int timer_setup(int type, u32 *freq)
{
   u32 clock=0;
   int old_level_mask = interrupt_get_level_mask();

   if (freq == NULL)
      return LAPETUS_ERR_INVALIDPOINTER;

   freq[0] = 0;

   interrupt_set_level_mask(0xF);

   if (bios_get_clock_speed == 0)
      clock = 26846587;
   else
      clock = 28636360;

   switch (type)
   {
      case TIMER_RTC:
         // Disable RTC
         SH2_REG_RTCSR_W(0);
         // Setup Interrupt
         bios_set_sh2_interrupt(0x7F, timer_rtc_increment);
         // Setup vector
         SH2_REG_VCRWDT = 0x7F7F;
         // Setup level
         SH2_REG_IPRA = (0xF << 4);
         freq[0] = clock / 4;
         // Enable RTC
         SH2_REG_RTCNT_W(0);
         SH2_REG_RTCOR_W(0xFF);
         SH2_REG_RTCSR_W(0x40 | (0x7 << 3)); // I may change the increment speed
         break;
//      case TIMER_FRT:
         // Disable FRT

         // Setup so that FRC is cleared on compare match A
//         SH2_REG_FTCSR = 1; 
         // Setup Interrupt
//         bios_set_sh2_interrupt(0x7F, TimerFRTIncrement);
         // Setup vector
//         SH2_REG_VCRWDT = 0x7F7F;
         // Setup level
//         SH2_REG_IPRA = (0xF << 4);
//         freq[0] = clock / 4;
         // Enable FRT
//         SH2_REG_RTCNT = 0;
         // Setup the internal clock;
//         SH2_REG_TCR = 0; 
//         SH2_REG_FRC = 0;
//         SH2_REG_TIER = ???;
//         break;
      case TIMER_WDT:
         // Disable WDT interval timer
         SH2_REG_WTCSR_W(SH2_REG_WTCSR_R & 0x18);
         // Setup Interrupt
         bios_set_sh2_interrupt(0x7F, timer_wdt_increment);
         // Setup vector
         SH2_REG_VCRWDT = 0x7F7F;
         // Setup level
         SH2_REG_IPRA = (0xF << 4);
         freq[0] = clock / 2 / 256; // double check this
         // Enable WDT interval timer
         SH2_REG_WTCNT_W(0);
         SH2_REG_WTCSR_W((SH2_REG_WTCSR_R & 0xDF) | 0x20 | 7);
         break;
      case TIMER_HBLANK:
         // Setup Interrupt
         bios_set_scu_interrupt(0x42, timer_hblank_increment);
         bios_change_scu_interrupt_mask(~MASK_HBLANKIN, 0);
         freq[0] = 224; // fix me

         if (old_level_mask > 0xC)
            old_level_mask = 0xC;

         break;
      default:
         return LAPETUS_ERR_INVALIDARG;
   }

   timercounter = 0;
   if (old_level_mask == 0xF)
      old_level_mask = 0xE;

   interrupt_set_level_mask(old_level_mask);

   return LAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

u32 timer_counter()
{
   return timercounter;
}

//////////////////////////////////////////////////////////////////////////////

void timer_stop()
{
   bios_set_sh2_interrupt(0x7F, 0);  
}

//////////////////////////////////////////////////////////////////////////////

