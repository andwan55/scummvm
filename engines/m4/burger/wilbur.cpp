/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "m4/burger/wilbur.h"
#include "m4/burger/vars.h"

namespace M4 {
namespace Burger {

#define _WIL(X) _G(wilbur)._##X

void wilbur_speech(const char *name, int trigger, int room, byte flags, int vol, int channel) {
	KernelTriggerType oldMode = _G(kernel).trigger_mode;
	_WIL(name) = name;
	_WIL(channel) = channel;
	_WIL(room) = room;
	_WIL(vol) = vol;
	_WIL(trigger) = kernel_trigger_create(trigger);
	_WIL(flag) = (flags & 1) != 0;

	_G(kernel).trigger_mode = KT_DAEMON;
	kernel_trigger_dispatch_now(gWILBURS_SPEECH_START);
	_G(kernel).trigger_mode = oldMode;
}

void wilbur_say() {
	KernelTriggerType oldMode = _G(kernel).trigger_mode;

	if (_WIL(flag) && _G(player).walker_in_this_scene && _G(roomVal2))
		SendWSMessage(0x140000, 0, _G(my_walker), 0, 0, 1);

	term_message("wilbur_say:  wilburs_talk_trigger = %d", _WIL(trigger));
	digi_stop(_WIL(channel));

	_G(kernel).trigger_mode = KT_DAEMON;
	kernel_trigger_dispatch_now(10013);
	digi_play(_WIL(name), _WIL(channel), _WIL(vol), gWILBURS_SPEECH_FINISHED, _WIL(room));

	_G(kernel).trigger_mode = oldMode;
}

void wilburs_speech_finished() {
	if (_WIL(flag) && !_G(player).walker_in_this_scene && !_G(roomVal2))
		SendWSMessage(0x150000, 0, _G(my_walker), 0, 0, 1);

	term_message("wilburs_speech_finished: dispatching wilburs_talk_trigger = %d", _WIL(trigger));
	kernel_trigger_dispatchx(_WIL(trigger));
}

} // namespace Burger
} // namespace M4
