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
#include "chewy/global.h"
#include "chewy/ani_dat.h"
#include "chewy/episode2.h"
#include "chewy/rooms/rooms.h"

namespace Chewy {

void calc_person_look() {
	int16 i;
	for (i = 1; i < MAX_PERSON; i++) {
		if (spieler_mi[i].Id != NO_MOV_OBJ) {

			if (spieler_vector[i].Xypos[0] > spieler_vector[P_CHEWY].Xypos[0])
				person_end_phase[i] = P_LEFT;
			else
				person_end_phase[i] = P_RIGHT;
		}
	}
}

} // namespace Chewy
