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

#include "crab/crab.h"
#include "crab/ui/QuestText.h"

namespace Crab {

using namespace pyrodactyl::ui;
using namespace pyrodactyl::text;
using namespace pyrodactyl::input;

void QuestText::load(rapidxml::xml_node<char> *node) {
	ParagraphData::load(node);
	loadNum(col_s, "color_s", node);

	if (nodeValid("line", node))
		loadNum(lines_per_page, "page", node->first_node("line"));

	if (nodeValid("inc", node))
		inc.load(node->first_node("inc"));

	if (nodeValid("img", node))
		img.load(node->first_node("img"));

	if (nodeValid("prev", node)) {
		prev.load(node->first_node("prev"));
		prev._hotkey.set(IU_PAGE_PREV);
	}

	if (nodeValid("next", node)) {
		next.load(node->first_node("next"));
		next._hotkey.set(IU_PAGE_NEXT);
	}

	if (nodeValid("status", node))
		status.load(node->first_node("status"));
}

void QuestText::draw(pyrodactyl::event::Quest &q) {
	// First, we must scan and find the part of the quest text we should draw

	// Assign default values to start and stop
	start = 0;
	stop = q._text.size();

	// Keep count of lines and pages - remember a single entry can take more than one line
	unsigned int page_count = 0, page_start = 0;

	// Start from line 0, page 0 and scan the list of entries
	for (unsigned int i = 0, line_count = 0; i < q._text.size(); ++i) {
		// Increment the number of lines by one text entry
		line_count += (q._text[i].size() / _line.x) + 1;

		// If we go over the quota for lines per page, go to next page and reset line counter to 0
		if (line_count > lines_per_page) {
			// We are about to go to next page, stop at this entry
			if (page_count == current_page) {
				start = page_start;
				stop = i;
			}

			page_count++;
			line_count = 0;

			// This is the start of the next page
			page_start = i;
		}
	}

	// Used for the final page, because the page count won't be incremented for the last one
	if (page_count == current_page) {
		start = page_start;
		stop = q._text.size();
	}

	// Find out how many pages the lines need
	total_page = page_count + 1;

	// Update the text
	status._text = (NumberToString(current_page + 1) + " of " + NumberToString(total_page));

	// Now, start drawing the quest
	status.draw();

	if (current_page > 0)
		prev.draw();

	if (current_page < total_page - 1)
		next.draw();

	// Draw the current page of quest text
	if (!q._text.empty()) {
		// Count the number of lines, because a single entry can take more than one line
		int count = 0;

		for (unsigned int i = start; i < (unsigned int)stop; ++i) {
			img.draw(inc.x * count, inc.y * count);

			// Draw first entry in selected color, and older quest entries in standard color
			if (i == 0)
				g_engine->_textManager->draw(x, y, q._text[i], col_s, _font, _align, _line.x, _line.y);
			else
				ParagraphData::draw(q._text[i], inc.x * count, inc.y * count);

			// Count is reduced extra by the amount of lines it takes for the message to be drawn
			count += (q._text[i].size() / _line.x) + 1;
		}
	}
}

void QuestText::handleEvents(pyrodactyl::event::Quest &q, const Common::Event &Event) {
	if (current_page > 0 && prev.handleEvents(Event) == BUAC_LCLICK)
		current_page--;

	if (current_page < total_page - 1 && next.handleEvents(Event) == BUAC_LCLICK) {
		current_page++;

		if (current_page >= total_page)
			current_page = total_page - 1;
	}
}

#if 0
void QuestText::handleEvents(pyrodactyl::event::Quest &q, const SDL_Event &Event) {
	if (current_page > 0 && prev.handleEvents(Event) == BUAC_LCLICK) {
		current_page--;

		if (current_page < 0)
			current_page = 0;
	}

	if (current_page < total_page - 1 && next.handleEvents(Event) == BUAC_LCLICK) {
		current_page++;

		if (current_page >= total_page)
			current_page = total_page - 1;
	}
}
#endif

void QuestText::setUI() {
	ParagraphData::setUI();
	img.setUI();
	prev.setUI();
	next.setUI();
	status.setUI();
}

} // End of namespace Crab
