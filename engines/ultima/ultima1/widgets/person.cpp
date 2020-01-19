/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "ultima/ultima1/widgets/person.h"
#include "ultima/ultima1/map/map_city_castle.h"

namespace Ultima {
namespace Ultima1 {
namespace Widgets {

bool Person::areGuardsHostile() const {
	Map::MapCityCastle *cityCastle = static_cast<Map::MapCityCastle *>(_map);
	return cityCastle->_guardsHostile;
}

int Person::getRandomDelta() const {
	return _game->getRandomNumber(2) - 1;
}

bool Person::moveBy(const Point &delta) {
	// TODO: Movement allowed on tile 63.. is this the gate of the princess' cells?
	Point newPos = _position + delta;
	if (canMoveTo(newPos)) {
		_position = newPos;
		return true;
	} else {
		return false;
	}
}

} // End of namespace Widgets
} // End of namespace Ultima1
} // End of namespace Ultima
