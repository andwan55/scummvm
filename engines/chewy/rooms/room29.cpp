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

#include "chewy/defines.h"
#include "chewy/events.h"
#include "chewy/global.h"
#include "chewy/ani_dat.h"
#include "chewy/rooms/room29.h"

namespace Chewy {
namespace Rooms {

void Room29::entry() {
	if (_G(spieler).R29Schlauch1)
		det->show_static_spr(7);
	else if (_G(spieler).R29Schlauch2) {
		det->show_static_spr(8);
		det->show_static_spr(10);
	}
	if (_G(spieler).R29AutoSitz)
		det->show_static_spr(9);
}

void Room29::xit() {
	if (obj->check_inventar(PUMPE_INV)) {
		del_inventar(PUMPE_INV);
	}
}

int16 Room29::use_pumpe() {
	int16 action_flag = false;
	if (!_G(spieler).R29Pumpe) {
		action_flag = true;
		if (is_cur_inventar(SCHLAUCH_INV)) {
			_G(spieler).R29Pumpe = true;
			_G(spieler).R29Schlauch1 = true;
			auto_move(1, P_CHEWY);
			start_spz_wait(CH_LGET_O, 1, ANI_VOR, P_CHEWY);
			det->show_static_spr(7);
			atds->del_steuer_bit(218, ATS_AKTIV_BIT, ATS_DATEI);
			del_inventar(SCHLAUCH_INV);
		} else if (!_G(spieler).inv_cur)
			start_aad_wait(62, -1);
	}
	return action_flag;
}

int16 Room29::get_schlauch() {
	int16 action_flag = false;
	if (_G(spieler).R29Schlauch1 && !_G(spieler).inv_cur) {
		action_flag = true;
		auto_move(2, P_CHEWY);
		start_spz_wait(CH_LGET_U, 1, ANI_VOR, P_CHEWY);
		new_invent_2_cur(PUMPE_INV);
	}
	return action_flag;
}

bool Room29::use_schlauch() {
	bool result = false;

	if (is_cur_inventar(PUMPE_INV)) {
		result = true;
		auto_move(2, P_CHEWY);
		start_spz_wait(CH_LGET_U, 1, ANI_VOR, P_CHEWY);
		det->hide_static_spr(7);
		_G(spieler).PersonHide[P_CHEWY] = true;
		start_detail_wait(4, 1, ANI_VOR);
		det->show_static_spr(8);
		det->show_static_spr(10);
		atds->del_steuer_bit(219, ATS_AKTIV_BIT, ATS_DATEI);
		atds->set_ats_str(218, 1, ATS_DATEI);
		_G(spieler).R29Schlauch1 = false;
		_G(spieler).R29Schlauch2 = true;
		del_inventar(PUMPE_INV);
		set_person_pos(308, 105, P_CHEWY, P_RIGHT);
		_G(spieler).PersonHide[P_CHEWY] = false;
	}

	return result;
}

void Room29::schlitz_sitz() {
	if (!_G(spieler).R29AutoSitz) {
		_G(spieler).R29AutoSitz = true;
		_G(spieler).PersonHide[P_CHEWY] = true;
		det->hide_static_spr(4);
		start_aad(63);
		start_ani_block(4, ablock26);
		det->show_static_spr(9);
		while (flags.AdsDialog && !SHOULD_QUIT) {
			set_up_screen(DO_SETUP);
		}
		det->stop_detail(2);
		atds->del_steuer_bit(212, ATS_AKTIV_BIT, ATS_DATEI);
		_G(spieler).PersonHide[P_CHEWY] = false;
		kbinfo.scan_code = Common::KEYCODE_INVALID;
	}
}

int16 Room29::zaun_sprung() {
	int16 action_flag = false;
	if (_G(spieler).R29AutoSitz && !_G(spieler).inv_cur) {
		action_flag = true;
		auto_move(3, P_CHEWY);
		_G(spieler).PersonHide[P_CHEWY] = true;
		start_detail_frame(3, 1, ANI_VOR, 7);
		det->hide_static_spr(9);
		start_ani_block(5, ablock27);
		set_up_screen(DO_SETUP);
		switch_room(37);
		_G(spieler).PersonHide[P_CHEWY] = false;
	}
	return action_flag;
}

} // namespace Rooms
} // namespace Chewy
