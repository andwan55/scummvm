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

/*
 * This code is based on the CRAB engine
 *
 * Copyright (c) Arvind Raja Yadav
 *
 * Licensed under MIT
 *
 */

#ifndef CRAB_MODMENU_H
#define CRAB_MODMENU_H

#include "crab/ui/FileMenu.h"
#include "crab/common_header.h"

namespace Crab {

namespace pyrodactyl {
namespace ui {
class ModMenu : public FileMenu<ModFileData> {
public:
	ModMenu() {}
	~ModMenu() {}

	void load(const Common::String &filename);

	bool HandleEvents(const Common::Event &Event);
#if 0
	bool HandleEvents(const SDL_Event &Event);
#endif
	void Draw();
};
} // End of namespace ui
} // End of namespace pyrodactyl

} // End of namespace Crab

#endif // CRAB_MODMENU_H
