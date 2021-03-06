/* Xemu - Somewhat lame emulation (running on Linux/Unix/Windows/OSX, utilizing
   SDL2) of some 8 bit machines, including the Commodore LCD and Commodore 65
   and some Mega-65 features as well.
   Copyright (C)2016 LGB (Gábor Lénárt) <lgblgblgb@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#ifndef __XEMU_COMMON_C64_KBD_MAPPING
#define __XEMU_COMMON_C64_KBD_MAPPING

// Keyboard position of "shift" which is "virtually pressed" ie for cursor up/left
#define VIRTUAL_SHIFT_POS	0x64

#define RESTORE_KEY_POS		0x80
#define IS_RESTORE_PRESSED()	(!(kbd_matrix[RESTORE_KEY_POS >> 4] & (1 << (RESTORE_KEY_POS & 7))))

extern const struct KeyMapping c64_key_map[];
extern int joystick_emu;

extern Uint8 c64_get_joy_state  ( void );
extern void  c64_toggle_joy_emu ( void );

static INLINE Uint8 c64_keyboard_read_on_CIA1_B ( Uint8 kbsel_a, Uint8 effect_b, Uint8 joy_state )
{
	// selected line(s) for scan: LOW pin state on port A, while reading in port B
	// CIA uses pull-ups, output can be LOW only, if port data is zero, and ddr is output (thus being 1)
	// so the "caller" of this func will do something like this:
	// kbsel_a = cia1.PRA | (~cia1.DDRA)
	// effect_b = cia1.PRB | (~cia1.DDRB)
	// the second is needed: the read value is still low, if the port we're reading on is configured for output with port value set 0
	return
		((kbsel_a &   1) ? 0xFF : kbd_matrix[0]) &
		((kbsel_a &   2) ? 0xFF : kbd_matrix[1]) &
		((kbsel_a &   4) ? 0xFF : kbd_matrix[2]) &
		((kbsel_a &   8) ? 0xFF : kbd_matrix[3]) &
		((kbsel_a &  16) ? 0xFF : kbd_matrix[4]) &
		((kbsel_a &  32) ? 0xFF : kbd_matrix[5]) &
		((kbsel_a &  64) ? 0xFF : kbd_matrix[6]) &
		((kbsel_a & 128) ? 0xFF : kbd_matrix[7]) &
		joy_state &
		effect_b
	;
}

static INLINE Uint8 c64_keyboard_read_on_CIA1_A ( Uint8 kbsel_b, Uint8 effect_a, Uint8 joy_state )
{
	// For description on this logic, please see comments at c64_keyboard_read_on_CIA1_B() above.
	// The theory is the same, however here we use the negated value. it's not because it's different by hardware
	// but because we handle kbsel_b value differently than with c64_keyboard_read_on_CIA1_B(), that's all!
	kbsel_b = ~kbsel_b;
	return (
		(((kbd_matrix[0] & kbsel_b) == kbsel_b) ?   1 : 0) |
		(((kbd_matrix[1] & kbsel_b) == kbsel_b) ?   2 : 0) |
		(((kbd_matrix[2] & kbsel_b) == kbsel_b) ?   4 : 0) |
		(((kbd_matrix[3] & kbsel_b) == kbsel_b) ?   8 : 0) |
		(((kbd_matrix[4] & kbsel_b) == kbsel_b) ?  16 : 0) |
		(((kbd_matrix[5] & kbsel_b) == kbsel_b) ?  32 : 0) |
		(((kbd_matrix[6] & kbsel_b) == kbsel_b) ?  64 : 0) |
		(((kbd_matrix[7] & kbsel_b) == kbsel_b) ? 128 : 0)
	) &
		joy_state &
		effect_a
	;
}

#endif
