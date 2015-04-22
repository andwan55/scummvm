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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "sherlock/sherlock.h"
#include "sherlock/saveload.h"
#include "sherlock/scalpel/scalpel.h"
#include "sherlock/tattoo/tattoo.h"
#include "common/system.h"
#include "engines/advancedDetector.h"

namespace Sherlock {

struct SherlockGameDescription {
	ADGameDescription desc;

	int gameID;
	uint32 features;
};

uint32 SherlockEngine::getGameID() const {
	return _gameDescription->gameID;
}

uint32 SherlockEngine::getGameFeatures() const {
	return _gameDescription->features;
}

Common::Platform SherlockEngine::getPlatform() const {
	return _gameDescription->desc.platform;
}

} // End of namespace Sherlock

static const PlainGameDescriptor sherlockGames[] = {
	{"sherlock", "The Lost Files of Sherlock Holmes"},
	{ "scalpel", "The Case of the Serrated Scalpel" },
	{ "rosetattoo", "The Case of the Rose Tattoo" },
	{0, 0}
};

#include "sherlock/detection_tables.h"

class SherlockMetaEngine : public AdvancedMetaEngine {
public:
	SherlockMetaEngine() : AdvancedMetaEngine(Sherlock::gameDescriptions, sizeof(Sherlock::gameDescriptions), sherlockGames) {}

	virtual const char *getName() const {
		return "Sherlock Engine";
	}

	virtual const char *getOriginalCopyright() const {
		return "Sherlock Engine (C) 1992-1996 Mythos Software, 1992-1996 (C) Electronic Arts";
	}

	virtual bool createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const;
	virtual bool hasFeature(MetaEngineFeature f) const;
	virtual SaveStateList listSaves(const char *target) const;
	virtual int getMaximumSaveSlot() const;
	virtual void removeSaveState(const char *target, int slot) const;
	SaveStateDescriptor querySaveMetaInfos(const char *target, int slot) const;
};

bool SherlockMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	const Sherlock::SherlockGameDescription *gd = (const Sherlock::SherlockGameDescription *)desc;
	if (gd) {
		switch (gd->gameID) {
		case Sherlock::GType_SerratedScalpel:
			*engine = new Sherlock::Scalpel::ScalpelEngine(syst, gd);
			break;
		case Sherlock::GType_RoseTattoo:
			*engine = new Sherlock::Tattoo::TattooEngine(syst, gd);
			break;
		default:
			error("Unknown game");
			break;
		}
	}
	return gd != 0;
}

bool SherlockMetaEngine::hasFeature(MetaEngineFeature f) const {
	return
		(f == kSupportsListSaves) ||
		(f == kSupportsLoadingDuringStartup) ||
		(f == kSupportsDeleteSave) ||
		(f == kSavesSupportMetaInfo) ||
		(f == kSavesSupportThumbnail);
}

bool Sherlock::SherlockEngine::hasFeature(EngineFeature f) const {
	return
		(f == kSupportsRTL) ||
		(f == kSupportsLoadingDuringRuntime) ||
		(f == kSupportsSavingDuringRuntime);
}

SaveStateList SherlockMetaEngine::listSaves(const char *target) const {
	return Sherlock::SaveManager(nullptr, "").getSavegameList(target);
}

int SherlockMetaEngine::getMaximumSaveSlot() const {
	return MAX_SAVEGAME_SLOTS;
}

void SherlockMetaEngine::removeSaveState(const char *target, int slot) const {	
	Common::String filename = Sherlock::SaveManager(nullptr, target).generateSaveName(slot);
	g_system->getSavefileManager()->removeSavefile(filename);
}

SaveStateDescriptor SherlockMetaEngine::querySaveMetaInfos(const char *target, int slot) const {
	Common::String filename = Sherlock::SaveManager(nullptr, target).generateSaveName(slot);
	Common::InSaveFile *f = g_system->getSavefileManager()->openForLoading(filename);

	if (f) {
		Sherlock::SherlockSavegameHeader header;
		Sherlock::SaveManager::readSavegameHeader(f, header);
		delete f;

		// Create the return descriptor
		SaveStateDescriptor desc(slot, header._saveName);
		desc.setThumbnail(header._thumbnail);
		desc.setSaveDate(header._year, header._month, header._day);
		desc.setSaveTime(header._hour, header._minute);
		desc.setPlayTime(header._totalFrames * GAME_FRAME_TIME);

		return desc;
	}

	return SaveStateDescriptor();
}


#if PLUGIN_ENABLED_DYNAMIC(SHERLOCK)
REGISTER_PLUGIN_DYNAMIC(SHERLOCK, PLUGIN_TYPE_ENGINE, SherlockMetaEngine);
#else
REGISTER_PLUGIN_STATIC(SHERLOCK, PLUGIN_TYPE_ENGINE, SherlockMetaEngine);
#endif
