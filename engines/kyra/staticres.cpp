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
 * $URL$
 * $Id$
 *
 */


#include "common/endian.h"
#include "common/md5.h"
#include "kyra/kyra.h"
#include "kyra/kyra_v1.h"
#include "kyra/kyra_v2.h"
#include "kyra/kyra_v3.h"
#include "kyra/screen.h"
#include "kyra/resource.h"

namespace Kyra {

#define RESFILE_VERSION 21

bool StaticResource::checkKyraDat() {
	Common::File kyraDat;
	if (!kyraDat.open(StaticResource::staticDataFilename()))
		return false;

	uint32 size = kyraDat.size() - 16;
	uint8 digest[16];
	kyraDat.seek(size, SEEK_SET);
	if (kyraDat.read(digest, 16) != 16)
		return false;
	kyraDat.close();

	uint8 digestCalc[16];
	if (!Common::md5_file(StaticResource::staticDataFilename().c_str(), digestCalc, size))
		return false;

	for (int i = 0; i < 16; ++i)
		if (digest[i] != digestCalc[i])
			return false;
	return true;
}

// used for the KYRA.DAT file which still uses
// the old flag system, we just convert it, which
// is less work than to change KYRA.DAT again
enum {
	GF_FLOPPY	= 1 <<  0,
	GF_TALKIE	= 1 <<  1,
	GF_FMTOWNS	= 1 <<  2,
	GF_DEMO		= 1 <<  3,
	GF_ENGLISH	= 1 <<  4,
	GF_FRENCH	= 1 <<  5,
	GF_GERMAN	= 1 <<  6,
	GF_SPANISH	= 1 <<  7,
	GF_ITALIAN	= 1 <<  8,
	GF_JAPANESE = 1 <<  9,
	// other languages here
	GF_LNGUNK	= 1 << 16,	// also used for multi language in kyra3
	GF_AMIGA	= 1 << 17
};

#define GAME_FLAGS (GF_FLOPPY | GF_TALKIE | GF_DEMO | GF_FMTOWNS | GF_AMIGA)
#define LANGUAGE_FLAGS (GF_ENGLISH | GF_FRENCH | GF_GERMAN | GF_SPANISH | GF_ITALIAN | GF_JAPANESE | GF_LNGUNK)

uint32 createFeatures(const GameFlags &flags) {
	if (flags.isTalkie)
		return GF_TALKIE;
	if (flags.isDemo)
		return GF_DEMO;
	if (flags.platform == Common::kPlatformFMTowns || flags.platform == Common::kPlatformPC98)
		return GF_FMTOWNS;
	if (flags.platform == Common::kPlatformAmiga)
		return GF_AMIGA;
	return GF_FLOPPY;
}

uint32 createLanguage(const GameFlags &flags) {
	if (flags.lang == Common::EN_ANY)
		return GF_ENGLISH;
	if (flags.lang == Common::DE_DEU)
		return GF_GERMAN;
	if (flags.lang == Common::FR_FRA)
		return GF_FRENCH;
	if (flags.lang == Common::ES_ESP)
		return GF_SPANISH;
	if (flags.lang == Common::IT_ITA)
		return GF_ITALIAN;
	if (flags.lang == Common::JA_JPN)
		return GF_JAPANESE;
	return GF_LNGUNK;
}

struct LanguageTypes {
	uint32 flags;
	const char *ext;
};

static const LanguageTypes languages[] = {
	{ GF_ENGLISH, "ENG" },	// this is the default language
	{ GF_FRENCH, "FRE" },
	{ GF_GERMAN, "GER" },
	{ GF_SPANISH, "SPA" },
	{ GF_ITALIAN, "ITA" },
	{ GF_JAPANESE, "JPN" },
	{ 0, 0 }
};

bool StaticResource::init() {
#define proc(x) &StaticResource::x
	static const FileType fileTypeTable[] = {
		{ kLanguageList, proc(loadLanguageTable), proc(freeStringTable) },
		{ kStringList, proc(loadStringTable), proc(freeStringTable) },
		{ StaticResource::kRoomList, proc(loadRoomTable), proc(freeRoomTable) },
		{ kShapeList, proc(loadShapeTable), proc(freeShapeTable) },
		{ kRawData, proc(loadRawData), proc(freeRawData) },
		{ kPaletteTable, proc(loadPaletteTable), proc(freePaletteTable) },
		{ 0, 0, 0 }
	};
#undef proc
	_fileLoader = fileTypeTable;

	// Kyrandia 1 Filenames
	static const FilenameTable kyra1StaticRes[] = {
		// INTRO / OUTRO sequences
		{ kForestSeq, kRawData, "FOREST.SEQ" },
		{ kKallakWritingSeq, kRawData, "KALLAK-WRITING.SEQ" },
		{ kKyrandiaLogoSeq, kRawData, "KYRANDIA-LOGO.SEQ" },
		{ kKallakMalcolmSeq, kRawData, "KALLAK-MALCOLM.SEQ" },
		{ kMalcolmTreeSeq, kRawData, "MALCOLM-TREE.SEQ" },
		{ kWestwoodLogoSeq, kRawData, "WESTWOOD-LOGO.SEQ" },
		{ kDemo1Seq, kRawData, "DEMO1.SEQ" },
		{ kDemo2Seq, kRawData, "DEMO2.SEQ" },
		{ kDemo3Seq, kRawData, "DEMO3.SEQ" },
		{ kDemo4Seq, kRawData, "DEMO4.SEQ" },
		{ kOutroReunionSeq, kRawData, "REUNION.SEQ" },

		// INTRO / OUTRO strings
		{ kIntroCPSStrings, kStringList, "INTRO-CPS.TXT" },
		{ kIntroCOLStrings, kStringList, "INTRO-COL.TXT" },
		{ kIntroWSAStrings, kStringList, "INTRO-WSA.TXT" },
		{ kIntroStrings, kLanguageList, "INTRO-STRINGS." },
		{ kOutroHomeString, kLanguageList, "HOME." },

		// INGAME strings
		{ kItemNames, kLanguageList, "ITEMLIST." },
		{ kTakenStrings, kLanguageList, "TAKEN." },
		{ kPlacedStrings, kLanguageList, "PLACED." },
		{ kDroppedStrings, kLanguageList, "DROPPED." },
		{ kNoDropStrings, kLanguageList, "NODROP." },
		{ kPutDownString, kLanguageList, "PUTDOWN." },
		{ kWaitAmuletString, kLanguageList, "WAITAMUL." },
		{ kBlackJewelString, kLanguageList, "BLACKJEWEL." },
		{ kPoisonGoneString, kLanguageList, "POISONGONE." },
		{ kHealingTipString, kLanguageList, "HEALINGTIP." },
		{ kThePoisonStrings, kLanguageList, "THEPOISON." },
		{ kFluteStrings, kLanguageList, "FLUTE." },
		{ kWispJewelStrings, kLanguageList, "WISPJEWEL." },
		{ kMagicJewelStrings, kLanguageList, "MAGICJEWEL." },
		{ kFlaskFullString, kLanguageList, "FLASKFULL." },
		{ kFullFlaskString, kLanguageList, "FULLFLASK." },
		{ kVeryCleverString, kLanguageList, "VERYCLEVER." },
		{ kNewGameString, kLanguageList, "NEWGAME." },

		// GUI strings table
		{ kGUIStrings, kLanguageList, "GUISTRINGS." },
		{ kConfigStrings, kLanguageList, "CONFIGSTRINGS." },

		// ROOM table/filenames
		{ Kyra::kRoomList, StaticResource::kRoomList, "ROOM-TABLE.ROOM" },
		{ kRoomFilenames, kStringList, "ROOM-FILENAMES.TXT" },

		// SHAPE tables
		{ kDefaultShapes, kShapeList, "SHAPES-DEFAULT.SHP" },
		{ kHealing1Shapes, kShapeList, "HEALING.SHP" },
		{ kHealing2Shapes, kShapeList, "HEALING2.SHP" },
		{ kPoisonDeathShapes, kShapeList, "POISONDEATH.SHP" },
		{ kFluteShapes, kShapeList, "FLUTE.SHP" },
		{ kWinter1Shapes, kShapeList, "WINTER1.SHP" },
		{ kWinter2Shapes, kShapeList, "WINTER2.SHP" },
		{ kWinter3Shapes, kShapeList, "WINTER3.SHP" },
		{ kDrinkShapes, kShapeList, "DRINK.SHP" },
		{ kWispShapes, kShapeList, "WISP.SHP" },
		{ kMagicAnimShapes, kShapeList, "MAGICANIM.SHP" },
		{ kBranStoneShapes, kShapeList, "BRANSTONE.SHP" },

		// IMAGE filename table
		{ kCharacterImageFilenames, kStringList, "CHAR-IMAGE.TXT" },

		// AMULET anim
		{ kAmuleteAnimSeq, kRawData, "AMULETEANIM.SEQ" },

		// PALETTE table
		{ kPaletteList, kPaletteTable, "1 33 PALTABLE" },

		// AUDIO files
		{ kAudioTracks, kStringList, "TRACKS.TXT" },
		{ kAudioTracksIntro, kStringList, "TRACKSINT.TXT" },

		// FM-TOWNS specific
		{ kKyra1TownsSFXwdTable, kRawData, "SFXWDTABLE" },
		{ kKyra1TownsSFXbtTable, kRawData, "SFXBTTABLE" },
		{ kKyra1TownsCDATable, kRawData, "CDATABLE" },
		{ kCreditsStrings, kRawData, "CREDITS" },

		{ 0, 0, 0 }
	};

	static const FilenameTable kyra2StaticRes[] = {
		// Sequence Player
		{ k2SeqplayPakFiles, kStringList, "S_PAKFILES.TXT" },
		{ k2SeqplayCredits, kRawData, "S_CREDITS.TXT" },
		{ k2SeqplayCreditsSpecial, kStringList, "S_CREDITS2.TXT" },
		{ k2SeqplayStrings, kLanguageList, "S_STRINGS." },
		{ k2SeqplaySfxFiles, kStringList, "S_SFXFILES.TXT" },
		{ k2SeqplayTlkFiles, kLanguageList, "S_TLKFILES." },
		{ k2SeqplaySeqData, kRawData, "S_DATA.SEQ" },
		{ k2SeqplayIntroTracks, kStringList, "S_INTRO.TRA" },
		{ k2SeqplayFinaleTracks, kStringList, "S_FINALE.TRA" },
		{ k2SeqplayIntroCDA, kRawData, "S_INTRO.CDA" },
		{ k2SeqplayFinaleCDA, kRawData, "S_FINALE.CDA" },
		{ k2SeqplayShapeDefs, kRawData, "S_DEMO.SHP" },

		// Ingame
		{ k2IngamePakFiles, kStringList, "I_PAKFILES.TXT" },
		{ k2IngameSfxFiles, kStringList, "I_SFXFILES.TRA" },
		{ k2IngameSfxIndex, kRawData, "I_SFXINDEX.MAP" },
		{ k2IngameTracks, kStringList, "I_TRACKS.TRA" },
		{ k2IngameCDA, kRawData, "I_TRACKS.CDA" },
		{ k2IngameTalkObjIndex, kRawData, "I_TALKOBJECTS.MAP" },
		{ k2IngameTimJpStrings, kStringList, "I_TIMJPSTR.TXT" },

		{ 0, 0, 0 }
	};

	if (_vm->game() == GI_KYRA1) {
		_builtIn = 0;
		_filenameTable = kyra1StaticRes;
	} else if (_vm->game() == GI_KYRA2) {
		_builtIn = 0;
		_filenameTable = kyra2StaticRes;
	} else if (_vm->game() == GI_KYRA3) {
		return true;
	} else {
		error("unknown game ID");
	}

	int tempSize = 0;
	uint8 *temp = getFile("INDEX", tempSize);
	if (!temp) {
		warning("no matching INDEX file found");
		return false;
	}

	uint32 version = READ_BE_UINT32(temp);
	uint32 gameID = READ_BE_UINT32((temp+4));
	uint32 featuresValue = READ_BE_UINT32((temp+8));

	delete [] temp;
	temp = 0;

	if (version != RESFILE_VERSION)
		error("invalid KYRA.DAT file version (%u, required %d)", version, RESFILE_VERSION);
	if (gameID != _vm->game())
		error("invalid game id (%u)", gameID);

	uint32 gameFeatures = createFeatures(_vm->gameFlags());
	if ((featuresValue & GAME_FLAGS) != gameFeatures)
		error("your data file has a different game flags (0x%.08X has the data and your version has 0x%.08X)", (featuresValue & GAME_FLAGS), gameFeatures);

	// load all tables for now
	if (!prefetchId(-1))
		error("couldn't load all needed resources from 'KYRA.DAT'");
	return true;
}

void StaticResource::deinit() {
	unloadId(-1);
}

const char * const*StaticResource::loadStrings(int id, int &strings) {
	const char * const*temp = (const char* const*)getData(id, kStringList, strings);
	if (temp)
		return temp;
	return (const char* const*)getData(id, kLanguageList, strings);
}

const uint8 *StaticResource::loadRawData(int id, int &size) {
	return (const uint8*)getData(id, kRawData, size);
}

const Shape *StaticResource::loadShapeTable(int id, int &entries) {
	return (const Shape*)getData(id, kShapeList, entries);
}

const Room *StaticResource::loadRoomTable(int id, int &entries) {
	return (const Room*)getData(id, StaticResource::kRoomList, entries);
}

const uint8 * const*StaticResource::loadPaletteTable(int id, int &entries) {
	return (const uint8* const*)getData(id, kPaletteTable, entries);
}

bool StaticResource::prefetchId(int id) {
	if (id == -1) {
		for (int i = 0; _filenameTable[i].filename; ++i)
			prefetchId(_filenameTable[i].id);
		return true;
	}
	const void *ptr = 0;
	int type = -1, size = -1;

	if (checkResList(id, type, ptr, size))
		return true;

	if (checkForBuiltin(id, type, size))
		return true;

	const FilenameTable *filename = searchFile(id);
	if (!filename)
		return false;
	const FileType *filetype = getFiletype(filename->type);
	if (!filetype)
		return false;

	ResData data;
	data.id = id;
	data.type = filetype->type;
	if (!(this->*(filetype->load))(filename->filename, data.data, data.size))
		return false;
	_resList.push_back(data);

	return true;
}

void StaticResource::unloadId(int id) {
	Common::List<ResData>::iterator pos = _resList.begin();
	for (; pos != _resList.end(); ++pos) {
		if (pos->id == id || id == -1) {
			const FileType *filetype = getFiletype(pos->type);
			(this->*(filetype->free))(pos->data, pos->size);
			if (id != -1)
				break;
		}
	}
}

bool StaticResource::checkResList(int id, int &type, const void *&ptr, int &size) {
	Common::List<ResData>::iterator pos = _resList.begin();
	for (; pos != _resList.end(); ++pos) {
		if (pos->id == id) {
			size = pos->size;
			type = pos->type;
			ptr = pos->data;
			return true;
		}
	}
	return false;
}

const void *StaticResource::checkForBuiltin(int id, int &type, int &size) {
	if (!_builtIn)
		return 0;

	for (int i = 0; _builtIn[i].data; ++i) {
		if (_builtIn[i].id == id) {
			size = _builtIn[i].size;
			type = _builtIn[i].type;
			return _builtIn[i].data;
		}
	}

	return 0;
}

const StaticResource::FilenameTable *StaticResource::searchFile(int id) {
	if (!_filenameTable)
		return 0;

	for (int i = 0; _filenameTable[i].filename; ++i) {
		if (_filenameTable[i].id == id)
			return &_filenameTable[i];
	}

	return 0;
}

const StaticResource::FileType *StaticResource::getFiletype(int type) {
	if (!_fileLoader)
		return 0;

	for (int i = 0; _fileLoader[i].load; ++i) {
		if (_fileLoader[i].type == type)
			return &_fileLoader[i];
	}

	return 0;
}

const void *StaticResource::getData(int id, int requesttype, int &size) {
	const void *ptr = 0;
	int type = -1;
	size = 0;

	if (checkResList(id, type, ptr, size)) {
		if (type == requesttype)
			return ptr;
		return 0;
	}

	ptr = checkForBuiltin(id, type, size);
	if (ptr) {
		if (type == requesttype)
			return ptr;
		return 0;
	}

	if (!prefetchId(id))
		return 0;

	if (checkResList(id, type, ptr, size)) {
		if (type == requesttype)
			return ptr;
	}

	return 0;
}

bool StaticResource::loadLanguageTable(const char *filename, void *&ptr, int &size) {
	char file[64];
	for (int i = 0; languages[i].ext; ++i) {
		if (languages[i].flags != createLanguage(_vm->gameFlags()))
			continue;

		strcpy(file, filename);
		strcat(file, languages[i].ext);
		if (loadStringTable(file, ptr, size))
			return true;
	}

	strcpy(file, filename);
	strcat(file, languages[0].ext);
	if (loadStringTable(file, ptr, size)) {
		static bool warned = false;
		if (!warned) {
			warned = true;
			warning("couldn't find specific language table for your version, using English now");
		}
		return true;
	}

	return false;
}

bool StaticResource::loadStringTable(const char *filename, void *&ptr, int &size) {
	uint8 *filePtr = getFile(filename, size);
	if (!filePtr)
		return false;
	uint8 *src = filePtr;

	uint32 count = READ_BE_UINT32(src); src += 4;
	size = count;
	char **output = new char*[count];
	assert(output);

	const char *curPos = (const char*)src;
	for (uint32 i = 0; i < count; ++i) {
		int strLen = strlen(curPos);
		output[i] = new char[strLen+1];
		assert(output[i]);
		memcpy(output[i], curPos, strLen+1);
		curPos += strLen+1;
	}

	delete [] filePtr;
	ptr = output;

	return true;
}

bool StaticResource::loadRawData(const char *filename, void *&ptr, int &size) {
	ptr = getFile(filename, size);
	if (!ptr)
		return false;
	return true;
}

bool StaticResource::loadShapeTable(const char *filename, void *&ptr, int &size) {
	uint8 *filePtr = getFile(filename, size);
	if (!filePtr)
		return false;
	uint8 *src = filePtr;

	uint32 count = READ_BE_UINT32(src); src += 4;
	size = count;
	Shape *loadTo = new Shape[count];
	assert(loadTo);

	for (uint32 i = 0; i < count; ++i) {
		loadTo[i].imageIndex = *src++;
		loadTo[i].x = *src++;
		loadTo[i].y = *src++;
		loadTo[i].w = *src++;
		loadTo[i].h = *src++;
		loadTo[i].xOffset = *src++;
		loadTo[i].yOffset = *src++;
	}

	delete [] filePtr;
	ptr = loadTo;

	return true;
}

bool StaticResource::loadRoomTable(const char *filename, void *&ptr, int &size) {
	uint8 *filePtr = getFile(filename, size);
	if (!filePtr)
		return false;
	uint8 *src = filePtr;

	uint32 count = READ_BE_UINT32(src); src += 4;
	size = count;
	Room *loadTo = new Room[count];
	assert(loadTo);

	for (uint32 i = 0; i < count; ++i) {
		loadTo[i].nameIndex = *src++;
		loadTo[i].northExit = READ_BE_UINT16(src); src += 2;
		loadTo[i].eastExit = READ_BE_UINT16(src); src += 2;
		loadTo[i].southExit = READ_BE_UINT16(src); src += 2;
		loadTo[i].westExit = READ_BE_UINT16(src); src += 2;
		memset(&loadTo[i].itemsTable[0], 0xFF, sizeof(byte)*6);
		memset(&loadTo[i].itemsTable[6], 0, sizeof(byte)*6);
		memset(loadTo[i].itemsXPos, 0, sizeof(uint16)*12);
		memset(loadTo[i].itemsYPos, 0, sizeof(uint8)*12);
		memset(loadTo[i].needInit, 0, sizeof(loadTo[i].needInit));
	}

	delete [] filePtr;
	ptr = loadTo;

	return true;
}

bool StaticResource::loadPaletteTable(const char *filename, void *&ptr, int &size) {
	const char *temp = filename;
	int start = atoi(temp);
	temp = strstr(temp, " ");
	if (temp == NULL)
		return false;
	++temp;
	int end = atoi(temp);

	char **table = new char*[end-start+1];
	assert(table);

	char file[64];
	temp = filename;
	temp = strstr(temp, " ");
	++temp;
	temp = strstr(temp, " ");
	if (temp == NULL)
		return false;
	++temp;
	strncpy(file, temp, 64);

	char name[64];
	for (int i = start; i <= end; ++i) {
		snprintf(name, 64, "%s%d.PAL", file, i);
		table[(start != 0) ? (i-start) : i] = (char*)getFile(name, size);
		if (!table[(start != 0) ? (i-start) : i]) {
			delete [] table;
			return false;
		}
	}

	ptr = table;
	size = end - start + 1;
	return true;
}

void StaticResource::freeRawData(void *&ptr, int &size) {
	uint8 *data = (uint8*)ptr;
	delete [] data;
	ptr = 0;
	size = 0;
}

void StaticResource::freeStringTable(void *&ptr, int &size) {
	char **data = (char**)ptr;
	while (size--)
		delete [] data[size];
	ptr = 0;
	size = 0;
}

void StaticResource::freeShapeTable(void *&ptr, int &size) {
	Shape *data = (Shape*)ptr;
	delete [] data;
	ptr = 0;
	size = 0;
}

void StaticResource::freeRoomTable(void *&ptr, int &size) {
	Room *data = (Room*)ptr;
	delete [] data;
	ptr = 0;
	size = 0;
}

void StaticResource::freePaletteTable(void *&ptr, int &size) {
	uint8 **data = (uint8**)ptr;
	while (size--)
		delete [] data[size];
	ptr = 0;
	size = 0;
}

uint8 *StaticResource::getFile(const char *name, int &size) {
	char buffer[64];
	const char *ext = "";
	if (_vm->gameFlags().gameID == GI_KYRA2)
		ext = ".K2";
	snprintf(buffer, 64, "%s%s", name, ext);
	ext = "";

	if (_vm->gameFlags().isTalkie)
		ext = ".CD";
	else if (_vm->gameFlags().isDemo)
		ext = ".DEM";
	else if (_vm->gameFlags().platform == Common::kPlatformFMTowns || _vm->gameFlags().platform == Common::kPlatformPC98)
		ext = ".TNS";
	else if (_vm->gameFlags().platform == Common::kPlatformAmiga)
		ext = ".AMG";
	strcat(buffer, ext);
	uint32 tempSize = 0;
	uint8 *data = _vm->resource()->fileData(buffer, &tempSize);
	size = tempSize;
	return data;
}

#pragma mark -

void KyraEngine_v1::initStaticResource() {
	int temp = 0;
	_seq_Forest = _staticres->loadRawData(kForestSeq, temp);
	_seq_KallakWriting = _staticres->loadRawData(kKallakWritingSeq, temp);
	_seq_KyrandiaLogo = _staticres->loadRawData(kKyrandiaLogoSeq, temp);
	_seq_KallakMalcolm = _staticres->loadRawData(kKallakMalcolmSeq, temp);
	_seq_MalcolmTree = _staticres->loadRawData(kMalcolmTreeSeq, temp);
	_seq_WestwoodLogo = _staticres->loadRawData(kWestwoodLogoSeq, temp);
	_seq_Demo1 = _staticres->loadRawData(kDemo1Seq, temp);
	_seq_Demo2 = _staticres->loadRawData(kDemo2Seq, temp);
	_seq_Demo3 = _staticres->loadRawData(kDemo3Seq, temp);
	_seq_Demo4 = _staticres->loadRawData(kDemo4Seq, temp);
	_seq_Reunion = _staticres->loadRawData(kOutroReunionSeq, temp);

	_seq_WSATable = _staticres->loadStrings(kIntroWSAStrings, _seq_WSATable_Size);
	_seq_CPSTable = _staticres->loadStrings(kIntroCPSStrings, _seq_CPSTable_Size);
	_seq_COLTable = _staticres->loadStrings(kIntroCOLStrings, _seq_COLTable_Size);
	_seq_textsTable = _staticres->loadStrings(kIntroStrings, _seq_textsTable_Size);

	_itemList = _staticres->loadStrings(kItemNames, _itemList_Size);
	_takenList = _staticres->loadStrings(kTakenStrings, _takenList_Size);
	_placedList = _staticres->loadStrings(kPlacedStrings, _placedList_Size);
	_droppedList = _staticres->loadStrings(kDroppedStrings, _droppedList_Size);
	_noDropList = _staticres->loadStrings(kNoDropStrings, _noDropList_Size);
	_putDownFirst = _staticres->loadStrings(kPutDownString, _putDownFirst_Size);
	_waitForAmulet = _staticres->loadStrings(kWaitAmuletString, _waitForAmulet_Size);
	_blackJewel = _staticres->loadStrings(kBlackJewelString, _blackJewel_Size);
	_poisonGone = _staticres->loadStrings(kPoisonGoneString, _poisonGone_Size);
	_healingTip = _staticres->loadStrings(kHealingTipString, _healingTip_Size);
	_thePoison = _staticres->loadStrings(kThePoisonStrings, _thePoison_Size);
	_fluteString = _staticres->loadStrings(kFluteStrings, _fluteString_Size);
	_wispJewelStrings = _staticres->loadStrings(kWispJewelStrings, _wispJewelStrings_Size);
	_magicJewelString = _staticres->loadStrings(kMagicJewelStrings, _magicJewelString_Size);
	_flaskFull = _staticres->loadStrings(kFlaskFullString, _flaskFull_Size);
	_fullFlask = _staticres->loadStrings(kFullFlaskString, _fullFlask_Size);
	_veryClever = _staticres->loadStrings(kVeryCleverString, _veryClever_Size);
	_homeString = _staticres->loadStrings(kOutroHomeString, _homeString_Size);
	_newGameString = _staticres->loadStrings(kNewGameString, _newGameString_Size);

	_healingShapeTable = _staticres->loadShapeTable(kHealing1Shapes, _healingShapeTableSize);
	_healingShape2Table = _staticres->loadShapeTable(kHealing2Shapes, _healingShape2TableSize);
	_posionDeathShapeTable = _staticres->loadShapeTable(kPoisonDeathShapes, _posionDeathShapeTableSize);
	_fluteAnimShapeTable = _staticres->loadShapeTable(kFluteShapes, _fluteAnimShapeTableSize);
	_winterScrollTable = _staticres->loadShapeTable(kWinter1Shapes, _winterScrollTableSize);
	_winterScroll1Table = _staticres->loadShapeTable(kWinter2Shapes, _winterScroll1TableSize);
	_winterScroll2Table = _staticres->loadShapeTable(kWinter3Shapes, _winterScroll2TableSize);
	_drinkAnimationTable = _staticres->loadShapeTable(kDrinkShapes, _drinkAnimationTableSize);
	_brandonToWispTable = _staticres->loadShapeTable(kWispShapes, _brandonToWispTableSize);
	_magicAnimationTable = _staticres->loadShapeTable(kMagicAnimShapes, _magicAnimationTableSize);
	_brandonStoneTable = _staticres->loadShapeTable(kBranStoneShapes, _brandonStoneTableSize);

	_characterImageTable = _staticres->loadStrings(kCharacterImageFilenames, _characterImageTableSize);

	_roomFilenameTable = _staticres->loadStrings(kRoomFilenames, _roomFilenameTableSize);

	_amuleteAnim = _staticres->loadRawData(kAmuleteAnimSeq, temp);

	_specialPalettes = _staticres->loadPaletteTable(kPaletteList, temp);

	_guiStrings = _staticres->loadStrings(kGUIStrings, _guiStringsSize);
	_configStrings = _staticres->loadStrings(kConfigStrings, _configStringsSize);

	_soundFiles = _staticres->loadStrings(kAudioTracks, _soundFilesSize);
	_soundFilesIntro = _staticres->loadStrings(kAudioTracksIntro, _soundFilesIntroSize);
	_cdaTrackTable = (const int32*) _staticres->loadRawData(kKyra1TownsCDATable, _cdaTrackTableSize);

	// copied static res

	// room list
	const Room *tempRoomList = _staticres->loadRoomTable(kRoomList, _roomTableSize);

	if (_roomTableSize > 0) {
		_roomTable = new Room[_roomTableSize];
		assert(_roomTable);

		memcpy(_roomTable, tempRoomList, _roomTableSize*sizeof(Room));
		tempRoomList = 0;

		_staticres->unloadId(kRoomList);
	}

	// default shape table
	const Shape *tempShapeTable = _staticres->loadShapeTable(kDefaultShapes, _defaultShapeTableSize);

	if (_defaultShapeTableSize > 0) {
		_defaultShapeTable = new Shape[_defaultShapeTableSize];
		assert(_defaultShapeTable);

		memcpy(_defaultShapeTable, tempShapeTable, _defaultShapeTableSize*sizeof(Shape));
		tempShapeTable = 0;

		_staticres->unloadId(kDefaultShapes);
	}

	// audio data tables
	static const AudioDataStruct soundData_PC[] = {
		{ _soundFilesIntro, _soundFilesIntroSize, 0, 0 },
		{ _soundFiles, _soundFilesSize, 0, 0 },
		{ 0, 0, 0, 0}
	};

	static const AudioDataStruct soundData_TOWNS[] = {
		{ _soundFiles, _soundFilesSize, _cdaTrackTable, _cdaTrackTableSize },
		{ _soundFiles, _soundFilesSize, _cdaTrackTable, _cdaTrackTableSize },
		{ 0, 0, 0, 0}
	};
	_soundData = (_flags.platform == Common::kPlatformPC) ? soundData_PC : soundData_TOWNS;
}

void KyraEngine_v1::loadMouseShapes() {
	_screen->loadBitmap("MOUSE.CPS", 3, 3, 0);
	_screen->_curPage = 2;
	_shapes[0] = _screen->encodeShape(0, 0, 8, 10, 0);
	_shapes[1] = _screen->encodeShape(0, 0x17, 0x20, 7, 0);
	_shapes[2] = _screen->encodeShape(0x50, 0x12, 0x10, 9, 0);
	_shapes[3] = _screen->encodeShape(0x60, 0x12, 0x10, 11, 0);
	_shapes[4] = _screen->encodeShape(0x70, 0x12, 0x10, 9, 0);
	_shapes[5] = _screen->encodeShape(0x80, 0x12, 0x10, 11, 0);
	_shapes[6] = _screen->encodeShape(0x90, 0x12, 0x10, 10, 0);
	_shapes[360] = _screen->encodeShape(0x28, 0, 0x10, 13, 0);
	_screen->setMouseCursor(1, 1, 0);
	_screen->setMouseCursor(1, 1, _shapes[0]);
	_screen->setShapePages(5, 3);
}

void KyraEngine_v1::loadCharacterShapes() {
	int curImage = 0xFF;
	int videoPage = _screen->_curPage;
	_screen->_curPage = 2;
	for (int i = 0; i < 115; ++i) {
		assert(i < _defaultShapeTableSize);
		Shape *shape = &_defaultShapeTable[i];
		if (shape->imageIndex == 0xFF) {
			_shapes[i+7] = 0;
			continue;
		}
		if (shape->imageIndex != curImage) {
			assert(shape->imageIndex < _characterImageTableSize);
			_screen->loadBitmap(_characterImageTable[shape->imageIndex], 3, 3, 0);
			curImage = shape->imageIndex;
		}
		_shapes[i+7] = _screen->encodeShape(shape->x<<3, shape->y, shape->w<<3, shape->h, 1);
	}
	_screen->_curPage = videoPage;
}

void KyraEngine_v1::loadSpecialEffectShapes() {
	_screen->loadBitmap("EFFECTS.CPS", 3, 3, 0);
	_screen->_curPage = 2;

	int currShape;
	for (currShape = 173; currShape < 183; currShape++)
		_shapes[currShape] = _screen->encodeShape((currShape-173) * 24, 0, 24, 24, 1);

	for (currShape = 183; currShape < 190; currShape++)
		_shapes[currShape] = _screen->encodeShape((currShape-183) * 24, 24, 24, 24, 1);

	for (currShape = 190; currShape < 201; currShape++)
		_shapes[currShape] = _screen->encodeShape((currShape-190) * 24, 48, 24, 24, 1);

	for (currShape = 201; currShape < 206; currShape++)
		_shapes[currShape] = _screen->encodeShape((currShape-201) * 16, 106, 16, 16, 1);
}

void KyraEngine_v1::loadItems() {
	int shape;

	_screen->loadBitmap("JEWELS3.CPS", 3, 3, 0);
	_screen->_curPage = 2;

	_shapes[323] = 0;

	for (shape = 1; shape < 6; shape++ )
		_shapes[323 + shape] = _screen->encodeShape((shape - 1) * 32, 0, 32, 17, 0);

	for (shape = 330; shape <= 334; shape++)
		_shapes[shape] = _screen->encodeShape((shape-330) * 32, 102, 32, 17, 0);

	for (shape = 335; shape <= 339; shape++)
		_shapes[shape] = _screen->encodeShape((shape-335) * 32, 17,  32, 17, 0);

	for (shape = 340; shape <= 344; shape++)
		_shapes[shape] = _screen->encodeShape((shape-340) * 32, 34,  32, 17, 0);

	for (shape = 345; shape <= 349; shape++)
		_shapes[shape] = _screen->encodeShape((shape-345) * 32, 51,  32, 17, 0);

	for (shape = 350; shape <= 354; shape++)
		_shapes[shape] = _screen->encodeShape((shape-350) * 32, 68,  32, 17, 0);

	for (shape = 355; shape <= 359; shape++)
		_shapes[shape] = _screen->encodeShape((shape-355) * 32, 85,  32, 17, 0);


	_screen->loadBitmap("ITEMS.CPS", 3, 3, 0);
	_screen->_curPage = 2;

	for (int i = 0; i < 107; i++) {
		shape = findDuplicateItemShape(i);

		if (shape != -1)
			_shapes[216 + i] = _shapes[216 + shape];
		else
			_shapes[216 + i] = _screen->encodeShape( (i % 20) * 16, i/20 * 16, 16, 16, 0);
	}

	uint32 size;
	uint8 *fileData = _res->fileData("_ITEM_HT.DAT", &size);
	assert(fileData);

	for (int i = 0; i < 107; i++) {
		_itemTable[i].height = fileData[i];
		_itemTable[i].unk1 = _itemTable[i].unk2 = 0;
	}

	delete[] fileData;
}

void KyraEngine_v1::loadButtonShapes() {
	_screen->loadBitmap("BUTTONS2.CPS", 3, 3, 0);
	_screen->_curPage = 2;
	_scrollUpButton.process0PtrShape = _screen->encodeShape(0, 0, 24, 14, 1);
	_scrollUpButton.process1PtrShape = _screen->encodeShape(24, 0, 24, 14, 1);
	_scrollUpButton.process2PtrShape = _screen->encodeShape(48, 0, 24, 14, 1);
	_scrollDownButton.process0PtrShape = _screen->encodeShape(0, 15, 24, 14, 1);
	_scrollDownButton.process1PtrShape = _screen->encodeShape(24, 15, 24, 14, 1);
	_scrollDownButton.process2PtrShape = _screen->encodeShape(48, 15, 24, 14, 1);
	_screen->_curPage = 0;
}

void KyraEngine_v1::loadMainScreen(int page) {
	_screen->clearPage(page);

	if (_flags.lang == Common::EN_ANY && !_flags.isTalkie && (_flags.platform == Common::kPlatformPC || _flags.platform == Common::kPlatformAmiga))
		_screen->loadBitmap("MAIN15.CPS", page, page, _screen->getPalette(0));
	else if (_flags.lang == Common::EN_ANY || _flags.lang == Common::JA_JPN || (_flags.isTalkie && _flags.lang == Common::IT_ITA))
		_screen->loadBitmap("MAIN_ENG.CPS", page, page, 0);
	else if (_flags.lang == Common::FR_FRA)
		_screen->loadBitmap("MAIN_FRE.CPS", page, page, 0);
	else if (_flags.lang == Common::DE_DEU)
		_screen->loadBitmap("MAIN_GER.CPS", page, page, 0);
	else if (_flags.lang == Common::ES_ESP)
		_screen->loadBitmap("MAIN_SPA.CPS", page, page, 0);
	else if (_flags.lang == Common::IT_ITA)
		_screen->loadBitmap("MAIN_ITA.CPS", page, page, 0);
	else
		warning("no main graphics file found");

	if (_flags.platform == Common::kPlatformAmiga)
		memcpy(_screen->getPalette(1), _screen->getPalette(0), 32*3);

	_screen->copyRegion(0, 0, 0, 0, 320, 200, page, 0);
}

void KyraEngine_v2::initStaticResource() {
	int tmpSize = 0;

	_sequencePakList = _staticres->loadStrings(k2SeqplayPakFiles, _sequencePakListSize);
	_ingamePakList = _staticres->loadStrings(k2IngamePakFiles, _ingamePakListSize);
	_sequenceStrings = _staticres->loadStrings(k2SeqplayStrings, _sequenceStringsSize);	
	_ingameSoundList = _staticres->loadStrings(k2IngameSfxFiles, _ingameSoundListSize);
	_ingameSoundIndex = (const uint16*) _staticres->loadRawData(k2IngameSfxIndex, _ingameSoundIndexSize);
	_musicFileListIntro = _staticres->loadStrings(k2SeqplayIntroTracks, _musicFileListIntroSize);
	_musicFileListIngame = _staticres->loadStrings(k2IngameTracks, _musicFileListIngameSize);
	_musicFileListFinale = _staticres->loadStrings(k2SeqplayFinaleTracks, _musicFileListFinaleSize);
	_cdaTrackTableIntro = _staticres->loadRawData(k2SeqplayIntroCDA, _cdaTrackTableIntroSize);
	_cdaTrackTableIngame = _staticres->loadRawData(k2IngameCDA, _cdaTrackTableIngameSize);
	_cdaTrackTableFinale = _staticres->loadRawData(k2SeqplayFinaleCDA, _cdaTrackTableFinaleSize);
	_ingameTalkObjIndex = (const uint16*) _staticres->loadRawData(k2IngameTalkObjIndex, _ingameTalkObjIndexSize);
	_ingameTimJpStr = _staticres->loadStrings(k2IngameTimJpStrings, _ingameTimJpStrSize);

	// replace sequence talkie files with localized versions and cut off .voc
	// suffix from voc files so as to allow compression specific file extensions
	const char* const* seqSoundList = _staticres->loadStrings(k2SeqplaySfxFiles, _sequenceSoundListSize);
	const char* const* tlkfiles = _staticres->loadStrings(k2SeqplayTlkFiles, tmpSize);
	char ** tmpSndLst = new char*[_sequenceSoundListSize];

	for (int i = 0; i < _sequenceSoundListSize; i++) {
		int len = strlen(seqSoundList[i]);

		tmpSndLst[i] = new char[len + 1];
		tmpSndLst[i][0] = 0;

		if (_flags.platform == Common::kPlatformPC)
			len -= 4;

		if (tlkfiles) {
			for (int ii = 0; ii < tmpSize; ii++) {
				if (!scumm_stricmp(&seqSoundList[i][1], &tlkfiles[ii][1]))
					strcpy(tmpSndLst[i], tlkfiles[ii]);
			}
		}

		if (tmpSndLst[i][0] == 0)
			strcpy(tmpSndLst[i], seqSoundList[i]);

		tmpSndLst[i][len] = 0;
	}

	tlkfiles = seqSoundList = 0;
	_staticres->unloadId(k2SeqplayTlkFiles);
	_staticres->unloadId(k2SeqplaySfxFiles);
	_sequenceSoundList = tmpSndLst;

	// assign music data
	static const char *fmtMusicFileListIntro[] = { "intro" };
	static const char *fmtMusicFileListFinale[] = { "finale" };
	static const char *fmtMusicFileListIngame[] = { "k2" };

	static const AudioDataStruct soundData_PC[] = {
		{ _musicFileListIntro, _musicFileListIntroSize, 0, 0 },
		{ _musicFileListIngame, _musicFileListIngameSize, 0, 0},
		{ _musicFileListFinale, _musicFileListIntroSize, 0, 0 }
	};

	static const AudioDataStruct soundData_TOWNS[] = {
		{ fmtMusicFileListIntro, 1, _cdaTrackTableIntro, _cdaTrackTableIntroSize >> 1 },
		{ fmtMusicFileListIngame, 1, _cdaTrackTableIngame, _cdaTrackTableIngameSize >> 1 },
		{ fmtMusicFileListFinale, 1, _cdaTrackTableFinale, _cdaTrackTableFinaleSize >> 1 }
	};
	_soundData = (_flags.platform == Common::kPlatformPC) ? soundData_PC : soundData_TOWNS;

	// setup sequence data
	const uint8 *seqData = _staticres->loadRawData(k2SeqplaySeqData, tmpSize);

	static const Seqproc hofSequenceCallbacks[] = { 0,
		&KyraEngine_v2::seq_introWestwood,
		&KyraEngine_v2::seq_introTitle, &KyraEngine_v2::seq_introOverview,
		&KyraEngine_v2::seq_introLibrary, &KyraEngine_v2::seq_introHand,
		&KyraEngine_v2::seq_introPoint, &KyraEngine_v2::seq_introZanfaun,
		&KyraEngine_v2::seq_finaleFunters, &KyraEngine_v2::seq_finaleFerb,
		&KyraEngine_v2::seq_finaleFish, &KyraEngine_v2::seq_finaleFheep,
		&KyraEngine_v2::seq_finaleFarmer, &KyraEngine_v2::seq_finaleFuards,
		&KyraEngine_v2::seq_finaleFirates, &KyraEngine_v2::seq_finaleFrash
	};

	static const Seqproc hofNestedSequenceCallbacks[] = {
		&KyraEngine_v2::seq_finaleFiggle, &KyraEngine_v2::seq_introOver1,
		&KyraEngine_v2::seq_introOver2, &KyraEngine_v2::seq_introForest,
		&KyraEngine_v2::seq_introDragon, &KyraEngine_v2::seq_introDarm,
		&KyraEngine_v2::seq_introLibrary2, &KyraEngine_v2::seq_introLibrary2,
		&KyraEngine_v2::seq_introMarco, &KyraEngine_v2::seq_introHand1a,
		&KyraEngine_v2::seq_introHand1b, &KyraEngine_v2::seq_introHand1c,
		&KyraEngine_v2::seq_introHand2,	&KyraEngine_v2::seq_introHand3, 0
	};

	static const Seqproc hofDemoSequenceCallbacks[] = {
		&KyraEngine_v2::seq_demoVirgin, &KyraEngine_v2::seq_demoWestwood,
		&KyraEngine_v2::seq_demoTitle, &KyraEngine_v2::seq_demoHill,
		&KyraEngine_v2::seq_demoOuthome, &KyraEngine_v2::seq_demoWharf,
		&KyraEngine_v2::seq_demoDinob, &KyraEngine_v2::seq_demoFisher, 0
	};

	static const Seqproc hofDemoNestedSequenceCallbacks[] = {
		&KyraEngine_v2::seq_demoWharf2, &KyraEngine_v2::seq_demoDinob2,
		&KyraEngine_v2::seq_demoWater, &KyraEngine_v2::seq_demoBail,
		&KyraEngine_v2::seq_demoDig, 0
	};

	const uint16 *hdr = (const uint16 *) seqData;
	uint16 numSeq = READ_LE_UINT16(hdr++);
	uint16 hdrSize = READ_LE_UINT16(hdr) - 1;

	const Seqproc *cb = (_flags.isDemo && !_flags.isTalkie) ? hofDemoSequenceCallbacks : hofSequenceCallbacks;
	const Seqproc *ncb = (_flags.isDemo && !_flags.isTalkie) ? hofDemoNestedSequenceCallbacks : hofNestedSequenceCallbacks;

	_sequences = new Sequence[numSeq];
	for (int i = 0; i < numSeq; i++) {
		const uint8 *offset = (const uint8 *)(seqData + READ_LE_UINT16(hdr++));
		_sequences[i].flags = READ_LE_UINT16(offset);
		offset += 2;
		_sequences[i].wsaFile = (const char *)offset;
		offset += 14;
		_sequences[i].cpsFile = (const char *)offset;
		offset += 14;
		_sequences[i].startupCommand = *offset++;
		_sequences[i].finalCommand = *offset++;
		_sequences[i].stringIndex1 = READ_LE_UINT16(offset);
		offset += 2;
		_sequences[i].stringIndex2 = READ_LE_UINT16(offset);
		offset += 2;
		_sequences[i].startFrame = READ_LE_UINT16(offset);
		offset += 2;
		_sequences[i].numFrames = READ_LE_UINT16(offset);
		offset += 2;
		_sequences[i].frameDelay = READ_LE_UINT16(offset);
		offset += 2;
		_sequences[i].xPos = READ_LE_UINT16(offset);
		offset += 2;
		_sequences[i].yPos = READ_LE_UINT16(offset);
		offset += 2;
		_sequences[i].duration = READ_LE_UINT16(offset);
		_sequences[i].callback = cb[i];
	}

	if (hdr > ((const uint16*)(seqData + hdrSize)))
		return;

	numSeq = READ_LE_UINT16(hdr++);
	_nSequences = new NestedSequence[numSeq];
	for (int i = 0; i < numSeq; i++) {
		const uint8 *offset = (const uint8 *)(seqData + READ_LE_UINT16(hdr++));
		_nSequences[i].flags = READ_LE_UINT16(offset);
		offset += 2;
		_nSequences[i].wsaFile = (const char *)offset;
		offset += 14;
		_nSequences[i].startframe = READ_LE_UINT16(offset);
		offset += 2;
		_nSequences[i].endFrame = READ_LE_UINT16(offset);
		offset += 2;
		_nSequences[i].frameDelay = READ_LE_UINT16(offset);
		offset += 2;
		_nSequences[i].x = READ_LE_UINT16(offset);
		offset += 2;
		_nSequences[i].y = READ_LE_UINT16(offset);
		offset += 2;
		uint16 ctrlOffs = READ_LE_UINT16(offset);
		offset += 2;
		_nSequences[i].startupCommand = READ_LE_UINT16(offset);
		offset += 2;
		_nSequences[i].finalCommand = READ_LE_UINT16(offset);
		_nSequences[i].callback = ncb[i];
		_nSequences[i].wsaControl = ctrlOffs ? (const uint16*) (seqData + ctrlOffs) : 0;
	}
}

const ScreenDim Screen::_screenDimTable[] = {
	{ 0x00, 0x00, 0x28, 0xC8, 0x0F, 0x0C, 0x00, 0x00 },
	{ 0x08, 0x48, 0x18, 0x38, 0x0F, 0x0C, 0x00, 0x00 },
	{ 0x01, 0x08, 0x26, 0x80, 0x0F, 0x0C, 0x00, 0x00 },
	{ 0x00, 0xC2, 0x28, 0x06, 0x0F, 0x0C, 0x00, 0x00 },
	{ 0x00, 0x90, 0x28, 0x38, 0x04, 0x0C, 0x00, 0x00 },
	{ 0x01, 0x94, 0x26, 0x30, 0x04, 0x1B, 0x00, 0x00 },
	{ 0x00, 0x90, 0x28, 0x38, 0x0F, 0x0D, 0x00, 0x00 },
	{ 0x01, 0x96, 0x26, 0x32, 0x0F, 0x0D, 0x00, 0x00 },
	{ 0x00, 0x00, 0x28, 0x88, 0x0F, 0x0C, 0x00, 0x00 },
	{ 0x01, 0x20, 0x26, 0x80, 0x0F, 0x0C, 0x00, 0x00 },
	{ 0x03, 0x28, 0x22, 0x46, 0x0F, 0x0D, 0x00, 0x00 }
};

const int Screen::_screenDimTableCount = ARRAYSIZE(Screen::_screenDimTable);

const ScreenDim Screen_v2::_screenDimTable[] = {
	{ 0x00, 0x00, 0x28, 0xC8, 0xC7, 0xCF, 0x00, 0x00 },
	{ 0x08, 0x48, 0x18, 0x38, 0xC7, 0xCF, 0x00, 0x00 },
	{ 0x00, 0x00, 0x28, 0x90, 0xC7, 0xCF, 0x00, 0x00 },
	{ 0x00, 0xC2, 0x28, 0x06, 0xC7, 0xCF, 0x00, 0x00 },
	{ 0x00, 0x90, 0x28, 0x38, 0x96, 0xCF, 0x00, 0x00 },
	{ 0x01, 0x94, 0x26, 0x30, 0x96, 0x1B, 0x00, 0x00 },
	{ 0x00, 0x90, 0x28, 0x38, 0xC7, 0xCC, 0x00, 0x00 },
	{ 0x01, 0x96, 0x26, 0x32, 0xC7, 0xCC, 0x00, 0x00 },
	{ 0x00, 0x00, 0x28, 0x88, 0xC7, 0xCF, 0x00, 0x00 },
	{ 0x00, 0x08, 0x28, 0xB8, 0xC7, 0xCF, 0x00, 0x00 },
	{ 0x01, 0x28, 0x26, 0x46, 0xC7, 0xCC, 0x00, 0x00 },
	{ 0x0A, 0x96, 0x14, 0x30, 0x19, 0xF0, 0x00, 0x00 }	// menu, just present for current menu code
};

const int Screen_v2::_screenDimTableCount = ARRAYSIZE(Screen_v2::_screenDimTable);

const ScreenDim Screen_v2::_screenDimTableK3[] = {
	{ 0x00, 0x00, 0x28, 0xC8, 0xFF, 0xF0, 0x00, 0x00 },
	{ 0x08, 0x48, 0x18, 0x38, 0xFF, 0xF0, 0x00, 0x00 },
	{ 0x00, 0x00, 0x28, 0xBC, 0xFF, 0xF0, 0x00, 0x00 },
	{ 0x0A, 0x96, 0x14, 0x30, 0x19, 0xF0, 0x00, 0x00 }
};

const int Screen_v2::_screenDimTableCountK3 = ARRAYSIZE(Screen_v2::_screenDimTableK3);

const int8 KyraEngine::_addXPosTable[] = {
	 4,  4,  0, -4, -4, -4,  0,  4
};

const int8 KyraEngine::_addYPosTable[] = {
	 0, -2, -2, -2,  0,  2,  2,  2
};

const int8 KyraEngine_v1::_charXPosTable[] = {
	 0,  4,  4,  4,  0, -4, -4, -4
};

const int8 KyraEngine_v1::_charYPosTable[] = {
	-2, -2,  0,  2,  2,  2,  0, -2
};

const uint16 KyraEngine_v1::_itemPosX[] = {
	95, 115, 135, 155, 175, 95, 115, 135, 155, 175
};

const uint8 KyraEngine_v1::_itemPosY[] = {
	160, 160, 160, 160, 160, 181, 181, 181, 181, 181
};

void KyraEngine_v1::setupButtonData() {
	static Button buttonData[] = {
		{ 0, 0x02, /*XXX,*/0, 0, 0, /*XXX,*/ 0x0400, 0, 0, 0, 0, 0, 0, 0, 0x05D, 0x9E, 0x13, 0x14, /*XXX,*/ 0, &KyraEngine_v1::buttonInventoryCallback/*, XXX*/ },
		{ 0, 0x01, /*XXX,*/1, 1, 1, /*XXX,*/ 0x0487, 0, 0, 0, 0, 0, 0, 0, 0x009, 0xA4, 0x36, 0x1E, /*XXX,*/ 0, &KyraEngine_v1::buttonMenuCallback/*, XXX*/ },
		{ 0, 0x03, /*XXX,*/0, 0, 0, /*XXX,*/ 0x0400, 0, 0, 0, 0, 0, 0, 0, 0x071, 0x9E, 0x13, 0x14, /*XXX,*/ 0, &KyraEngine_v1::buttonInventoryCallback/*, XXX*/ },
		{ 0, 0x04, /*XXX,*/0, 0, 0, /*XXX,*/ 0x0400, 0, 0, 0, 0, 0, 0, 0, 0x085, 0x9E, 0x13, 0x14, /*XXX,*/ 0, &KyraEngine_v1::buttonInventoryCallback/*, XXX*/ },
		{ 0, 0x05, /*XXX,*/0, 0, 0, /*XXX,*/ 0x0400, 0, 0, 0, 0, 0, 0, 0, 0x099, 0x9E, 0x13, 0x14, /*XXX,*/ 0, &KyraEngine_v1::buttonInventoryCallback/*, XXX*/ },
		{ 0, 0x06, /*XXX,*/0, 0, 0, /*XXX,*/ 0x0400, 0, 0, 0, 0, 0, 0, 0, 0x0AD, 0x9E, 0x13, 0x14, /*XXX,*/ 0, &KyraEngine_v1::buttonInventoryCallback/*, XXX*/ },
		{ 0, 0x07, /*XXX,*/0, 0, 0, /*XXX,*/ 0x0400, 0, 0, 0, 0, 0, 0, 0, 0x05D, 0xB3, 0x13, 0x14, /*XXX,*/ 0, &KyraEngine_v1::buttonInventoryCallback/*, XXX*/ },
		{ 0, 0x08, /*XXX,*/0, 0, 0, /*XXX,*/ 0x0400, 0, 0, 0, 0, 0, 0, 0, 0x071, 0xB3, 0x13, 0x14, /*XXX,*/ 0, &KyraEngine_v1::buttonInventoryCallback/*, XXX*/ },
		{ 0, 0x09, /*XXX,*/0, 0, 0, /*XXX,*/ 0x0400, 0, 0, 0, 0, 0, 0, 0, 0x085, 0xB3, 0x13, 0x14, /*XXX,*/ 0, &KyraEngine_v1::buttonInventoryCallback/*, XXX*/ },
		{ 0, 0x0A, /*XXX,*/0, 0, 0, /*XXX,*/ 0x0400, 0, 0, 0, 0, 0, 0, 0, 0x099, 0xB3, 0x13, 0x14, /*XXX,*/ 0, &KyraEngine_v1::buttonInventoryCallback/*, XXX*/ },
		{ 0, 0x0B, /*XXX,*/0, 0, 0, /*XXX,*/ 0x0400, 0, 0, 0, 0, 0, 0, 0, 0x0AD, 0xB3, 0x13, 0x14, /*XXX,*/ 0, &KyraEngine_v1::buttonInventoryCallback/*, XXX*/ },
		{ 0, 0x15, /*XXX,*/1, 1, 1, /*XXX,*/ 0x0487, 0, 0, 0, 0, 0, 0, 0, 0x0FD, 0x9C, 0x1A, 0x12, /*XXX,*/ 0, &KyraEngine_v1::buttonAmuletCallback/*, XXX*/ },
		{ 0, 0x16, /*XXX,*/1, 1, 1, /*XXX,*/ 0x0487, 0, 0, 0, 0, 0, 0, 0, 0x0E7, 0xAA, 0x1A, 0x12, /*XXX,*/ 0, &KyraEngine_v1::buttonAmuletCallback/*, XXX*/ },
		{ 0, 0x17, /*XXX,*/1, 1, 1, /*XXX,*/ 0x0487, 0, 0, 0, 0, 0, 0, 0, 0x0FD, 0xB5, 0x1A, 0x12, /*XXX,*/ 0, &KyraEngine_v1::buttonAmuletCallback/*, XXX*/ },
		{ 0, 0x18, /*XXX,*/1, 1, 1, /*XXX,*/ 0x0487, 0, 0, 0, 0, 0, 0, 0, 0x113, 0xAA, 0x1A, 0x12, /*XXX,*/ 0, &KyraEngine_v1::buttonAmuletCallback/*, XXX*/ }
	};

	static Button *buttonDataListPtr[] = {
		&buttonData[1],
		&buttonData[2],
		&buttonData[3],
		&buttonData[4],
		&buttonData[5],
		&buttonData[6],
		&buttonData[7],
		&buttonData[8],
		&buttonData[9],
		&buttonData[10],
		&buttonData[11],
		&buttonData[12],
		&buttonData[13],
		&buttonData[14],
		0
	};

	_buttonData = buttonData;
	_buttonDataListPtr = buttonDataListPtr;
}

Button KyraEngine_v1::_scrollUpButton =  {0, 0x12, 1, 1, 1, 0x483, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x18, 0x0f, 0, 0};
Button KyraEngine_v1::_scrollDownButton = {0, 0x13, 1, 1, 1, 0x483, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x18, 0x0f, 0, 0};

Button KyraEngine_v1::_menuButtonData[] = {
	{ 0, 0x0c, /*XXX,*/1, 1, 1, /*XXX,*/ 0x487, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*XXX,*/ 0, 0 /*, XXX*/ },
	{ 0, 0x0d, /*XXX,*/1, 1, 1, /*XXX,*/ 0x487, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*XXX,*/ 0, 0 /*, XXX*/ },
	{ 0, 0x0e, /*XXX,*/1, 1, 1, /*XXX,*/ 0x487, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*XXX,*/ 0, 0 /*, XXX*/ },
	{ 0, 0x0f, /*XXX,*/1, 1, 1, /*XXX,*/ 0x487, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*XXX,*/ 0, 0 /*, XXX*/ },
	{ 0, 0x10, /*XXX,*/1, 1, 1, /*XXX,*/ 0x487, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*XXX,*/ 0, 0 /*, XXX*/ },
	{ 0, 0x11, /*XXX,*/1, 1, 1, /*XXX,*/ 0x487, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /*XXX,*/ 0, 0 /*, XXX*/ }
};

void KyraEngine_v1::setupMenu() {
	static Menu menu[] = {
		{ -1, -1, 208, 136, 248, 249, 250, 0, 251, -1, 8, 0, 5, -1, -1, -1, -1,
			{
				{1, 0, 0, 0, -1, -1, 30, 148, 15, 252, 253, 24, 0,
				248, 249, 250, &KyraEngine_v1::gui_loadGameMenu, -1, 0, 0, 0, 0, 0},

				{1, 0, 0, 0, -1, -1, 47, 148, 15, 252, 253, 24, 0,
				248, 249, 250, &KyraEngine_v1::gui_saveGameMenu, -1, 0, 0, 0, 0, 0},

				{1, 0, 0, 0, -1, -1, 64, 148, 15, 252, 253, 24, 0,
				248, 249, 250, &KyraEngine_v1::gui_gameControlsMenu, -1, 0, 0, 0, 0, 0},

				{1, 0, 0, 0, -1, -1, 81, 148, 15, 252, 253, 24, 0,
				248, 249, 250, &KyraEngine_v1::gui_quitPlaying, -1, 0, 0, 0, 0, 0},

				{1, 0, 0, 0, 86, 0, 110, 92, 15, 252, 253, -1, 255,
				248, 249, 250, &KyraEngine_v1::gui_resumeGame, -1, 0, 0, 0, 0, 0}
			}
		},
		{ -1, -1, 288, 56, 248, 249, 250, 0, 254,-1, 8, 0, 2, -1, -1, -1, -1,
			{
				{1, 0, 0, 0, 24, 0, 30, 72, 15, 252, 253, -1, 255,
				248, 249, 250, &KyraEngine_v1::gui_quitConfirmYes, -1, 0, 0, 0, 0, 0},

				{1, 0, 0, 0, 192, 0, 30, 72, 15, 252, 253, -1, 255,
				248, 249, 250, &KyraEngine_v1::gui_quitConfirmNo, -1, 0, 0, 0, 0, 0}
			}
		},
		{ -1, -1, 288, 160, 248, 249, 250, 0, 251, -1, 8, 0, 6, 132, 22, 132, 124,
			{
				{1, 0, 0, 0, -1, 255, 39, 256, 15, 252, 253, 5, 0,
				248, 249, 250, 0, -1, 0, 0, 0, 0, 0},

				{1, 0, 0, 0, -1, 255, 56, 256, 15, 252, 253, 5, 0,
				248, 249, 250, 0, -1, 0, 0, 0, 0, 0},

				{1, 0, 0, 0, -1, 255, 73, 256, 15, 252, 253, 5, 0,
				248, 249, 250, 0, -1, 0, 0, 0, 0, 0},

				{1, 0, 0, 0, -1, 255, 90, 256, 15, 252, 253, 5, 0,
				248, 249, 250, 0, -1, 0, 0, 0, 0, 0},

				{1, 0, 0, 0, -1, 255, 107, 256, 15, 252, 253, 5, 0,
				248, 249, 250, 0, -1, 0, 0, 0, 0, 0},

				{1, 0, 0, 0, 184, 0, 134, 88, 15, 252, 253, -1, 255,
				248, 249, 250, &KyraEngine_v1::gui_cancelSubMenu, -1, 0, 0, 0, 0, 0},
			}
		},
		{ -1, -1, 288, 67, 248, 249, 250, 0, 251, -1, 8, 0, 3, -1, -1, -1, -1,
			{
				{1, 0, 0, 0, 24, 0, 44, 72, 15, 252, 253, -1, 255,
				248, 249, 250, &KyraEngine_v1::gui_savegameConfirm, -1, 0, 0, 0, 0, 0},

				{1, 0, 0, 0, 192, 0, 44, 72, 15, 252, 253, -1, 255,
				248, 249, 250, &KyraEngine_v1::gui_cancelSubMenu, -1, 0, 0, 0, 0, 0}
			}
		},
		{ -1, -1, 208, 76, 248, 249, 250, 0, 251, -1, 8, 0, 2, -1, -1, -1, -1,
			{
				{1, 0, 0, 0, -1, -1, 30, 148, 15, 252, 253, 24, 0,
				248, 249, 250, &KyraEngine_v1::gui_loadGameMenu, -1, 0, 0, 0, 0, 0},

				{1, 0, 0, 0, -1, -1, 47, 148, 15, 252, 253, 24, 0,
				248, 249, 250, &KyraEngine_v1::gui_quitPlaying, -1, 0, 0, 0, 0, 0}
			}
		},
		{ -1, -1, 208, 153, 248, 249, 250, 0, 251, -1, 8, 0, 6, -1, -1, -1, -1,
			{
				{1, 0, 0, 0, 110, 0, 30, 64, 15, 252, 253, 5, 0,
				248, 249, 250, &KyraEngine_v1::gui_controlsChangeMusic, -1, 0, 34, 32, 0, 0},

				{1, 0, 0, 0, 110, 0, 47, 64, 15, 252, 253, 5, 0,
				248, 249, 250, &KyraEngine_v1::gui_controlsChangeSounds, -1, 0, 34, 49, 0, 0},

				{1, 0, 0, 0, 110, 0, 64, 64, 15, 252, 253, 5, 0,
				248, 249, 250, &KyraEngine_v1::gui_controlsChangeWalk, -1, 0, 34, 66, 0, 0},

				{1, 0, 0, 0, 110, 0, 81, 64, 15, 252, 253, 5, 0,
				248, 249, 250, 0, -1, 0, 34, 83, 0, 0 },

				{1, 0, 0, 0, 110, 0, 98, 64, 15, 252, 253, 5, 0,
				248, 249, 250, &KyraEngine_v1::gui_controlsChangeText, -1, 0, 34, 100, 0, 0 },

				{1, 0, 0, 0, 64, 0, 127, 92, 15, 252, 253, -1, 255,
				248, 249, 250, &KyraEngine_v1::gui_controlsApply, -1, -0, 0, 0, 0, 0}
			}
		}
	};

	_menu = menu;
}

const uint8 KyraEngine_v1::_magicMouseItemStartFrame[] = {
	0xAD, 0xB7, 0xBE, 0x00
};

const uint8 KyraEngine_v1::_magicMouseItemEndFrame[] = {
	0xB1, 0xB9, 0xC2, 0x00
};

const uint8 KyraEngine_v1::_magicMouseItemStartFrame2[] = {
	0xB2, 0xBA, 0xC3, 0x00
};

const uint8 KyraEngine_v1::_magicMouseItemEndFrame2[] = {
	0xB6, 0xBD, 0xC8, 0x00
};

const uint16 KyraEngine_v1::_amuletX[] = { 231, 275, 253, 253 };
const uint16 KyraEngine_v1::_amuletY[] = { 170, 170, 159, 181 };

const uint16 KyraEngine_v1::_amuletX2[] = { 0x000, 0x0FD, 0x0E7, 0x0FD, 0x113, 0x000 };
const uint16 KyraEngine_v1::_amuletY2[] = { 0x000, 0x09F, 0x0AA, 0x0B5, 0x0AA, 0x000 };

const int8 KyraEngine_v1::_dosTrackMap[] = {
	-1,   0,  -1,   1,   0,   3,   0,   2,
	 0,   4,   1,   2,   1,   3,   1,   4,
	 1,  92,   1,   6,   1,   7,   2,   2,
	 2,   3,   2,   4,   2,   5,   2,   6,
	 2,   7,   3,   3,   3,   4,   1,   8,
	 1,   9,   4,   2,   4,   3,   4,   4,
	 4,   5,   4,   6,   4,   7,   4,   8,
	 1,  11,   1,  12,   1,  14,   1,  13,
	 4,   9,   5,  12,   6,   2,   6,   6,
	 6,   7,   6,   8,   6,   9,   6,   3,
	 6,   4,   6,   5,   7,   2,   7,   3,
	 7,   4,   7,   5,   7,   6,   7,   7,
	 7,   8,   7,   9,   8,   2,   8,   3,
	 8,   4,   8,   5,   6,  11,   5,  11
};

const int KyraEngine_v1::_dosTrackMapSize = ARRAYSIZE(KyraEngine_v1::_dosTrackMap);

// Kyra 2 and 3 main menu

const char *KyraEngine_v2::_mainMenuStrings[] = {
	"Start a new game",
	"Introduction",
	"Load a game",
	"Exit the game",
	"Nouvelle Partie",
	"Introduction",
	"Charger une partie",
	"Quitter le jeu",
	"Neues Spiel starten",
	"Intro",
	"Spielstand laden",
	"Spiel beenden",
	0
};

// kyra 2 static res

const uint8 KyraEngine_v2::_seqTextColorPresets[] = { 0x01, 0x01, 0x00, 0x3f, 0x3f, 0x3f };

const char *KyraEngine_v2::_languageExtension[] = {
	"ENG",
	"FRE",
	"GER",/*,
	"ITA",		Italian and Spanish was never included
	"SPA"*/
	"JPN"
};

const char *KyraEngine_v2::_scriptLangExt[] = {
	"EMC",
	"FMC",
	"GMC",/*,
	"IMC",		Italian and Spanish was never included
	"SMC"*/
	"JMC"
};

const int KyraEngine_v2::_characterFrameTable[] = {
	0x19, 0x09, 0x09, 0x12, 0x12, 0x12, 0x09, 0x09
};

const int KyraEngine_v2::_inventoryX[] = {
	0x4F, 0x63, 0x77, 0x8B, 0x9F, 0x4F, 0x63, 0x77, 0x8B, 0x9F
};

const int KyraEngine_v2::_inventoryY[] = {
	0x95, 0x95, 0x95, 0x95, 0x95, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA
};

const byte KyraEngine_v2::_itemStringMap[] = {
	2,    2,    0,    0,    2,    2,    2,    0,
	2,    2,    0,    0,    0,    2,    0,    0,
	0,    0,    0,    0,    2,    0,    0,    0,
	0,    1,    0,    2,    2,    2,    2,    0,
	3,    0,    3,    2,    2,    2,    3,    2,
	2,    2,    0,    0,    0,    0,    0,    0,
	0,    0,    0,    0,    2,    0,    0,    0,
	0,    0,    0,    0,    0,    2,    0,    0,
	2,    0,    0,    0,    0,    0,    0,    2,
	2,    0,    0,    0,    2,    2,    2,    2,
	2,    2,    2,    2,    2,    2,    2,    2,
	2,    2,    2,    2,    2,    2,    2,    0,
	2,    2,    2,    0,    0,    1,    3,    2,
	2,    2,    2,    2,    2,    0,    0,    0,
	0,    2,    2,    1,    0,    1,    2,    0,
	0,    0,    0,    0,    0,    2,    2,    2,
	2,    2,    2,    2,    0,    2,    2,    2,
	2,    3,    2,    0,    0,    0,    0,    1,
	2,    0,    0,    0,    0,    0,    0,    0,
	0,    0,    0,    0,    0,    0,    0,    0,
	2,    2,    0,    0,    0,    0,    0,    2,
	0,    2,    0,    0,    0,    0,    0,    0
};

const int KyraEngine_v2::_itemStringMapSize = ARRAYSIZE(KyraEngine_v2::_itemStringMap);

const int8 KyraEngine_v2::_dosTrackMap[] = {
	-1,    0,   -1,    1,    9,    6,    5,    4,
	 8,    3,   -2,    0,   -2,    0,    2,    3,
	-2,    0,   -2,    0,   -2,    0,   -2,    0,
	 0,    2,    0,    3,    1,    2,    1,    3,
	 2,    2,    2,    0,    3,    2,    3,    3,
	 3,    4,    4,    2,    5,    2,    5,    3,
	 5,    4,    6,    2,    6,    3,    6,    4,
	 6,    5,    6,    6,    6,    7,    6,    8,
	 6,    0,    6,    9,    7,    2,    7,    3,
	 7,    4,    7,    5,    8,    6,    7,    6,
	 7,    7,    7,    8,    7,    9,    8,    2,
	14,    2,    8,    4,    8,    7,    8,    8,
	 8,    9,    9,    2,    9,    3,    9,    4,
	 9,    5,    9,    7,    9,    8,    9,    9,
	10,    2,   10,    3,   10,    4,   10,    5,
	10,    6,   10,    7,   11,    2,   11,    3,
	11,    4,   11,    5,   11,    6,   11,    7,
	11,    8,   11,    9,   12,    2,   12,    3,
	12,    4,   12,    5,   12,    6,   12,    7,
	12,    8,   12,    9,   13,    2,    4,    7,
	14,    3,   14,    4,   14,    5,    4,    2,
	 4,    3,    4,    4,    4,    5,    4,    6
};

const int KyraEngine_v2::_dosTrackMapSize = ARRAYSIZE(KyraEngine_v2::_dosTrackMap);

void KyraEngine_v2::initMainButtonList() {
	// note: _buttonDataListPtr
	static Button mainButtons[] = {
		{ 0, 0x1, 0x4F, 0, 1, 1, 1, 0x4487, 0, 0, 0, 0, 0x00A, 0x95, 0x39, 0x1D, 0xC7, 0xCF, 0xC7, 0xCF, 0xC7, 0xCF, 0, /*&KyraEngine_v2::sub_C9A1*/0 },
		{ 0, 0x2, 0x00, 0, 1, 1, 1, 0x4487, 0, 0, 0, 0, 0x104, 0x90, 0x3C, 0x2C, 0xC7, 0xCF, 0xC7, 0xCF, 0xC7, 0xCF, 0, &KyraEngine_v2::cauldronButton },
		{ 0, 0x5, 0x00, 0, 1, 1, 1, 0x4487, 0, 0, 0, 0, 0x0FA, 0x90, 0x0A, 0x2C, 0xC7, 0xCF, 0xC7, 0xCF, 0xC7, 0xCF, 0, &KyraEngine_v2::cauldronClearButton },
		{ 0, 0x3, 0x00, 0, 1, 1, 1, 0x4487, 0, 0, 0, 0, 0x0CE, 0x90, 0x2C, 0x2C, 0xC7, 0xCF, 0xC7, 0xCF, 0xC7, 0xCF, 0, &KyraEngine_v2::bookButton },
		{ 0, 0x4, 0x00, 0, 1, 1, 1, 0x4487, 0, 0, 0, 0, 0x0B6, 0x9D, 0x18, 0x1E, 0xC7, 0xCF, 0xC7, 0xCF, 0xC7, 0xCF, 0, &KyraEngine_v2::scrollInventory },
		{ 0, 0x6, 0x00, 0, 0, 0, 0, 0x1100, 0, 0, 0, 0, 0x04D, 0x92, 0x13, 0x15, 0xC7, 0xCF, 0xC7, 0xCF, 0xC7, 0xCF, 0, &KyraEngine_v2::buttonInventory },
		{ 0, 0x7, 0x00, 0, 0, 0, 0, 0x1100, 0, 0, 0, 0, 0x061, 0x92, 0x13, 0x15, 0xC7, 0xCF, 0xC7, 0xCF, 0xC7, 0xCF, 0, &KyraEngine_v2::buttonInventory },
		{ 0, 0x8, 0x00, 0, 0, 0, 0, 0x1100, 0, 0, 0, 0, 0x075, 0x92, 0x13, 0x15, 0xC7, 0xCF, 0xC7, 0xCF, 0xC7, 0xCF, 0, &KyraEngine_v2::buttonInventory },
		{ 0, 0x9, 0x00, 0, 0, 0, 0, 0x1100, 0, 0, 0, 0, 0x089, 0x92, 0x13, 0x15, 0xC7, 0xCF, 0xC7, 0xCF, 0xC7, 0xCF, 0, &KyraEngine_v2::buttonInventory },
		{ 0, 0xA, 0x00, 0, 0, 0, 0, 0x1100, 0, 0, 0, 0, 0x09D, 0x92, 0x13, 0x15, 0xC7, 0xCF, 0xC7, 0xCF, 0xC7, 0xCF, 0, &KyraEngine_v2::buttonInventory },
		{ 0, 0xB, 0x00, 0, 0, 0, 0, 0x1100, 0, 0, 0, 0, 0x04D, 0xA8, 0x13, 0x14, 0xC7, 0xCF, 0xC7, 0xCF, 0xC7, 0xCF, 0, &KyraEngine_v2::buttonInventory },
		{ 0, 0xC, 0x00, 0, 0, 0, 0, 0x1100, 0, 0, 0, 0, 0x061, 0xA8, 0x13, 0x14, 0xC7, 0xCF, 0xC7, 0xCF, 0xC7, 0xCF, 0, &KyraEngine_v2::buttonInventory },
		{ 0, 0xD, 0x00, 0, 0, 0, 0, 0x1100, 0, 0, 0, 0, 0x075, 0xA8, 0x13, 0x14, 0xC7, 0xCF, 0xC7, 0xCF, 0xC7, 0xCF, 0, &KyraEngine_v2::buttonInventory },
		{ 0, 0xE, 0x00, 0, 0, 0, 0, 0x1100, 0, 0, 0, 0, 0x089, 0xA8, 0x13, 0x14, 0xC7, 0xCF, 0xC7, 0xCF, 0xC7, 0xCF, 0, &KyraEngine_v2::buttonInventory },
		{ 0, 0xF, 0x00, 0, 0, 0, 0, 0x1100, 0, 0, 0, 0, 0x09D, 0xA8, 0x13, 0x14, 0xC7, 0xCF, 0xC7, 0xCF, 0xC7, 0xCF, 0, &KyraEngine_v2::buttonInventory }
	};

	switch (_lang) {
	case 0:
		mainButtons[0].shapePtr0 = _buttonShapes[6];
		mainButtons[0].shapePtr1 = mainButtons[0].shapePtr2 = _buttonShapes[7];
		break;

	case 1:
		mainButtons[0].shapePtr0 = _buttonShapes[8];
		mainButtons[0].shapePtr1 = mainButtons[0].shapePtr2 = _buttonShapes[9];
		break;

	case 2:
		mainButtons[0].shapePtr0 = _buttonShapes[10];
		mainButtons[0].shapePtr1 = mainButtons[0].shapePtr2 = _buttonShapes[11];
		break;

	default:
		mainButtons[0].shapePtr0 = _buttonShapes[6];
		mainButtons[0].shapePtr1 = mainButtons[0].shapePtr2 = _buttonShapes[7];
		break;
	}

	_buttonList = &mainButtons[0];
	for (size_t i = 1; i < ARRAYSIZE(mainButtons); ++i)
		_buttonList = addButtonToList(_buttonList, &mainButtons[i]);
}

const uint16 KyraEngine_v2::_itemMagicTable[] = {
	0x0D,  0x0A,  0x0B,    0,
	0x0D,  0x0B,  0x0A,    0,
	0x0D,  0x38,  0x37,    0,
	0x0D,  0x37,  0x38,    0,
	0x0D,  0x35,  0x36,    0,
	0x0D,  0x36,  0x35,    0,
	0x34,  0x27,  0x33,    0,
	0x41,  0x29,  0x49,    0,
	0x45,  0x29,  0x4A,    1,
	0x46,  0x29,  0x4A,    1,
	0x3C,  0x29,  0x4B,    1,
	0x34,  0x29,  0x4C,    0,
	0x3C,  0x49,  0x3B,    1,
	0x41,  0x4B,  0x3B,    0,
	0x3C,  0x4A,  0x3B,    1,
	0x34,  0x49,  0x3B,    0,
	0x41,  0x4C,  0x3B,    0,
	0x45,  0x4C,  0x3B,    1,
	0x46,  0x4C,  0x3B,    1,
	0x34,  0x4A,  0x3B,    0,
	0x0D,  0x67,  0x68,    0,
	0x0D,  0x68,  0x67,    0,
	0x0D,  0x69,  0x6A,    0,
	0x0D,  0x6A,  0x69,    0,
	0x0D,  0x6B,  0x6C,    0,
	0x0D,  0x6C,  0x6B,    0,
	0x0D,  0x88,  0x87,    0,
	0x0D,  0x87,  0x88,    0,
	0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF
};

const int KyraEngine_v2::_bookPageYOffset[] = {
	0, 0, 2, 2,
	0, 0, 2, 2,
	0, 0, 2, 2
};

const byte KyraEngine_v2::_bookTextColorMap[] = {
	0x00, 0xC7, 0xCF, 0x00
};

const int16 KyraEngine_v2::_cauldronProtectedItems[] = {
	0x07, 0x0D, 0x47, 0x48,
	0x29, 0x1A, 0x1C, 0x6D,
	0x4D, 0x3A, 0x0E, 0x0F,
	0x10, 0x11, 0x26, 0x3E,
	0x35, 0x40, 0x42, 0xA6,
	0xA4, 0xA5, 0x91, 0x95,
	0x99, 0xAC, 0xAE, 0xAF,
	0x8A, 0x79, 0x61, -1
};

const int16 KyraEngine_v2::_cauldronBowlTable[] = {
	0x0027, 0x0029,
	0x0028, 0x0029,
	0x0033, 0x0029,
	0x0049, 0x0029,
	0x004A, 0x0029,
	0x004B, 0x0029,
	0x004C, 0x0029,
	0x003B, 0x0029,
	0x0034, 0x0034,
	-1, -1
};

const int16 KyraEngine_v2::_cauldronMagicTable[] = {
	0x0, 0x16, 0x2, 0x1A,
	0x7, 0xA4, 0x5, 0x4D,
	0x1, 0xA5, 0x3, 0xA6,
	0x6, 0x6D, 0x4, 0x91,
	0xA, 0x99, 0xC, 0x95,
	0x9, 0xAC, -1, -1
};

const int16 KyraEngine_v2::_cauldronMagicTableScene77[] = {
	0x0, 0x16, 0x2, 0x1A,
	0x7, 0xAB, 0x5, 0x4D,
	0x1, 0xAE, 0x3, 0xAF,
	0x6, 0x6D, 0x4, 0x91,
	0xA, 0x99, 0xC, 0x95,
	0x9, 0xAC, -1, -1
};

const uint8 KyraEngine_v2::_cauldronStateTable[] = {
	3, 1, 3, 1, 1, 4, 4, 2,
	3, 1, 1, 3, 1, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3
};

const int16 KyraEngine_v2::_flaskTable[] = {
	0x19, 0x14, 0x15, 0x16, 0x17, 0x18, 0x34,
	0x1B, 0x39, 0x1A, 0x3A, 0x4D, 0x72, -1
};

// kyra 3 static res

const char *KyraEngine_v3::_soundList[] = {
	"ARREST1.AUD",
	"BATH1.AUD",
	"OCEAN1.AUD",
	"CLOWN1.AUD",
	"DARM2.AUD",
	"FALL1M.AUD",
	"FALL2.AUD",
	"FISH1.AUD",
	"FISHWNDR.AUD",
	"HERMAN1.AUD",
	"JAIL1.AUD",
	"JUNGLE1.AUD",
	"KATHY1.AUD",
	"NICESINE.AUD",
	"PEGASUS1.AUD",
	"PIRATE1.AUD",
	"PIRATE2.AUD",
	"PIRATE3.AUD",
	"POP3.AUD",
	"PORT1.AUD",
	"QUEEN1.AUD",
	"RUINS1.AUD",
	"SNAKES1.AUD",
	"SPRING1.AUD",
	"STATUE1.AUD",
	"STATUE2.AUD",
	"TITLE1.AUD",
	"UNDER1.AUD",
	"WALKCHP1.AUD",
	"YANK1.AUD",
	"ZAN2.AUD",
	"GROOVE2.AUD",
	"GROOVE3.AUD",
	"KING1.AUD",
	"KING2.AUD",
	"GROOVE1.AUD",
	"JAIL2.AUD",
	"SPIRIT1.AUD",
	"SPRING1A.AUD",
	"POP1.AUD",
	"POP2.AUD",
	"SQUIRL1.AUD"
};

const int KyraEngine_v3::_soundListSize = ARRAYSIZE(KyraEngine_v3::_soundList);

const char *KyraEngine_v3::_languageExtension[] = {
	"TRE",
	"TRF",
	"TRG"/*,
	"TRI",		Italian and Spanish was never included
	"TRS"*/
};

const int KyraEngine_v3::_languageExtensionSize = ARRAYSIZE(KyraEngine_v3::_languageExtension);

} // End of namespace Kyra



