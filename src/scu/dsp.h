/*  Copyright 2005-2007,2013 Theo Berkau

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

#ifndef DSP_H

#define SCUREG_DSTP  (*(volatile u32 *)0x25FE0060)
#define SCUREG_DSTA  (*(volatile u32 *)0x25FE0070)

#define SCUREG_PPAF  (*(volatile u32 *)0x25FE0080)
#define SCUREG_PPD   (*(volatile u32 *)0x25FE0084)
#define SCUREG_PDA   (*(volatile u32 *)0x25FE0088)
#define SCUREG_PDD   (*(volatile u32 *)0x25FE008C)

int dsp_load(u32 *program, u8 offset, u8 size);
void dsp_exec(u8 PC);
int dsp_is_exec();
void dsp_stop();
void dsp_pause();
void dsp_unpause();
void dsp_step();

#endif
