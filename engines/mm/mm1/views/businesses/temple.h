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

#ifndef MM1_VIEWS_BUSINESSES_TEMPLE_H
#define MM1_VIEWS_BUSINESSES_TEMPLE_H

#include "mm/mm1/views/businesses/business.h"

namespace MM {
namespace MM1 {
namespace Views {
namespace Businesses {

class Temple : public Business {
private:
	bool _isEradicated = false;
	int _healCost = 0, _uncurseCost = 0;
	int _alignmentCost = 0, _donateCost = 0;

	/**
	 * Selects a character
	 */
	void selectCharacter(uint charIndex);
public:
	Temple();
	virtual ~Temple() {}

	bool msgFocus(const FocusMessage &msg) override;
	bool msgKeypress(const KeypressMessage &msg) override;
	void draw() override;
};

} // namespace Businesses
} // namespace Views
} // namespace MM1
} // namespace MM

#endif
