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

#include "gui/filebrowser-dialog.h"

#include "director/director.h"
#include "director/lingo/lingo.h"
#include "director/lingo/xlibs/fileio.h"

namespace Director {

static const char *xlibName = "FileIO";

static struct MethodProto {
	const char *name;
	void (*func)(int);
	int minArgs;	// -1 -- arglist
	int maxArgs;
	int type;
	int version;
} xlibMethods[] = {
	{ "delete",					FileIO::m_delete,			 0, 0,	kXObj | kXtraObj,		2 },	// D2
	{ "dispose",				FileIO::m_dispose,			 0, 0,	kXObj,					2 },	// D2
	{ "fileName",				FileIO::m_fileName,			 0, 0,	kXObj | kXtraObj,		2 },	// D2
	{ "getLength",				FileIO::m_getLength,		 0, 0,	kXObj | kXtraObj,		2 },	// D2
	{ "getPosition",			FileIO::m_getPosition,		 0, 0,	kXObj | kXtraObj,		2 },	// D2
	{ "new",					FileIO::m_new,				 2, 2,	kXObj | kXtraObj,		2 },	// D2
	{ "readChar",				FileIO::m_readChar,			 0, 0,	kXObj | kXtraObj,		2 },	// D2
	{ "readLine",				FileIO::m_readLine,			 0, 0,	kXObj | kXtraObj,		2 },	// D2
	{ "readToken",				FileIO::m_readToken,		 2, 2,	kXObj | kXtraObj,		2 },	// D2
	{ "readWord",				FileIO::m_readWord,			 0, 0,	kXObj | kXtraObj,		2 },	// D2
	{ "setPosition",			FileIO::m_setPosition,		 1, 1,	kXObj | kXtraObj,		2 },	// D2
	{ "writeChar",				FileIO::m_writeChar,		 1, 1,	kXObj | kXtraObj,		2 },	// D2
	{ "writeString",			FileIO::m_writeString,		 1, 1,	kXObj | kXtraObj,		2 },	// D2
	{ 0, 0, 0, 0, 0, 0 }
};

void FileIO::initialize(int type) {
	if (type & kXObj) {
		if (!g_lingo->_globalvars.contains(xlibName)) {
			FileObject *xobj = new FileObject(kXObj);
			xobj->initMethods();
			g_lingo->_globalvars[xlibName] = Symbol();
			g_lingo->_globalvars[xlibName].name = new Common::String(xlibName);
			g_lingo->_globalvars[xlibName].global = true;
			g_lingo->_globalvars[xlibName].type = OBJECT;
			g_lingo->_globalvars[xlibName].u.obj = xobj;
		} else {
			warning("FileIO XObject already initialized");
		}
	}
	if (type & kXtraObj) {
		// TODO - Implement Xtra
	}
}

// Initialization/disposal

void FileObject::initMethods() {
	for (MethodProto *mtd = xlibMethods; mtd->name; mtd++) {
		if (mtd->version > g_lingo->_vm->getVersion() || !(type & mtd->type))
			continue;

		Symbol sym;
		sym.name = new Common::String(mtd->name);
		sym.type = FBLTIN;
		sym.nargs = mtd->minArgs;
		sym.maxArgs = mtd->maxArgs;
		sym.targetType = mtd->type;
		sym.u.bltin = mtd->func;
		methods[mtd->name] = sym;
	}
}

Object *FileObject::clone() {
	FileObject *res = new FileObject(type);
	res->disposed = disposed;
	res->prototype = this;
	res->properties = properties;
	res->methods = methods;
	res->inheritanceLevel = inheritanceLevel + 1;
	return res;
}

void FileObject::dispose() {
	disposed = true;

	if (filename) {
		delete filename;
		filename = nullptr;
	}
	if (inFile) {
		delete inFile;
		if (inStream != inFile)
			delete inStream;
		inFile = nullptr;
		inStream = nullptr;
	}
	if (outFile) {
		outFile->write(outStream->getData(), outStream->size());
		outFile->finalize();
		delete outFile;
		delete outStream;
		outFile = nullptr;
		outStream = nullptr;
	}
}

void FileIO::m_new(int nargs) {
	FileObject *me = static_cast<FileObject *>(g_lingo->_currentMeObj);

	Datum d2 = g_lingo->pop();
	Datum d1 = g_lingo->pop();

	Common::String option = *d1.u.s;
	Common::String filename = *d2.u.s;

	if (option.hasPrefix("?")) {
		option = option.substr(1);
		GUI::FileBrowserDialog browser(0, filename.c_str(), option.equalsIgnoreCase("read") ? GUI::kFBModeLoad : GUI::kFBModeSave);
		if (browser.runModal() <= 0) {
			g_lingo->push(Datum(kErrorFileNotFound));
			return;
		}
		filename = browser.getResult();
	}

	if (option.equalsIgnoreCase("read")) {
		me->inFile = g_system->getSavefileManager()->openForLoading(filename);
		me->inStream = me->inFile;
		if (!me->inFile) {
			delete me;
			g_lingo->push(Datum(kErrorIO));
			return;
		}
	} else if (option.equalsIgnoreCase("write")) {
		// OutSaveFile is not seekable so create a separate seekable stream
		// which will be written to the outfile upon disposal
		me->outFile = g_system->getSavefileManager()->openForSaving(filename);
		me->outStream = new Common::MemoryWriteStreamDynamic(DisposeAfterUse::YES);
		if (!me->outFile) {
			delete me;
			g_lingo->push(Datum(kErrorIO));
			return;
		}
	} else if (option.equalsIgnoreCase("append")) {
		Common::InSaveFile *inFile = g_system->getSavefileManager()->openForLoading(filename);
		if (!inFile) {
			delete me;
			g_lingo->push(Datum(kErrorIO));
			return;
		}
		me->outFile = g_system->getSavefileManager()->openForSaving(filename);
		me->outStream = new Common::MemoryWriteStreamDynamic(DisposeAfterUse::YES);
		if (!me->outFile) {
			delete me;
			g_lingo->push(Datum(kErrorIO));
			return;
		}
		while (!inFile->eos() && !inFile->err()) {
			me->outStream->writeByte(inFile->readByte());
		}
		delete inFile;
	} else {
		error("Unsupported FileIO option: '%s'", option.c_str());
	}

	me->filename = new Common::String(filename);

	Datum res;
	res.type = OBJECT;
	res.u.obj = me;
	g_lingo->push(res);
}

void FileIO::m_dispose(int nargs) {
	FileObject *me = static_cast<FileObject *>(g_lingo->_currentMeObj);
	me->dispose();
}

// Read

void FileIO::m_readChar(int nargs) {
	FileObject *me = static_cast<FileObject *>(g_lingo->_currentMeObj);

	if (!me->inStream || me->inStream->eos() || me->inStream->err()) {
		g_lingo->push(Datum(kErrorEOF));
		return;
	}

	int ch = me->inStream->readByte();
	if (me->inStream->eos() || me->inStream->err()) {
		ch = kErrorEOF;
	}
	g_lingo->push(Datum(ch));
}

void FileIO::m_readLine(int nargs) {
	// file(mReadLine) is equivalent to file(mReadToken, "", RETURN)
	// See D4 Using Lingo p. 323

	g_lingo->push(Datum(""));
	g_lingo->push(Datum("\n"));
	FileIO::m_readToken(2);
}

void FileIO::m_readWord(int nargs) {
	// file(mReadWord) is equivalent to file(mReadToken, " ", " " & RETURN)
	// See D4 Using Lingo p. 323

	g_lingo->push(Datum(" "));
	g_lingo->push(Datum(" \n"));
	FileIO::m_readToken(2);
}

void FileIO::m_readToken(int nargs) {
	FileObject *me = static_cast<FileObject *>(g_lingo->_currentMeObj);

	Datum d2 = g_lingo->pop();
	Datum d1 = g_lingo->pop();

	Common::String skipString = *d1.u.s;
	Common::String breakString = *d2.u.s;

	if (!me->inStream || me->inStream->eos() || me->inStream->err()) {
		g_lingo->push(Datum(""));
		return;
	}

	Common::String tok = "";
	char ch;
	do {
		ch = me->inStream->readByte();
		if (me->inStream->eos() || me->inStream->err()) {
			g_lingo->push(Datum(tok));
			return;
		}
	} while (skipString.contains(ch));

	while (!breakString.contains(ch)) {
		tok += ch;
		ch = me->inStream->readByte();

		if (me->inStream->eos() || me->inStream->err()) {
			g_lingo->push(Datum(tok));
			return;
		}
	}

	// Include the break character when the skipString is empty
	if (skipString.size() == 0) {
		tok += ch;
	} else {
		me->inStream->seek(-1, SEEK_CUR);
	}

	g_lingo->push(Datum(tok));
}

// Write

void FileIO::m_writeChar(int nargs) {
	FileObject *me = static_cast<FileObject *>(g_lingo->_currentMeObj);
	Datum d = g_lingo->pop();

	if (!me->outStream) {
		g_lingo->push(Datum(kErrorReadOnly));
		return;
	}

	me->outStream->writeByte(d.asInt());
	g_lingo->push(Datum(kErrorNone));
}

void FileIO::m_writeString(int nargs) {
	FileObject *me = static_cast<FileObject *>(g_lingo->_currentMeObj);
	Datum d = g_lingo->pop();

	if (!me->outStream) {
		g_lingo->push(Datum(kErrorReadOnly));
		return;
	}

	me->outStream->writeString(d.asString());
	g_lingo->push(Datum(kErrorNone));
}

// Other

void FileIO::m_getPosition(int nargs) {
	FileObject *me = static_cast<FileObject *>(g_lingo->_currentMeObj);

	if (me->inStream) {
		g_lingo->push(Datum(me->inStream->pos()));
	} else if (me->outStream) {
		g_lingo->push(Datum(me->outStream->pos()));
	} else {
		warning("FileIO: No file open");
		g_lingo->push(Datum(kErrorFileNotOpen));
	}
}

void FileIO::m_setPosition(int nargs) {
	FileObject *me = static_cast<FileObject *>(g_lingo->_currentMeObj);
	Datum d = g_lingo->pop();
	int pos = d.asInt();

	if (me->inStream) {
		if (pos <= me->inStream->size()) {
			me->inStream->seek(pos, SEEK_SET);
			g_lingo->push(Datum(kErrorNone));
		} else {
			me->inStream->seek(me->inStream->size(), SEEK_SET);
			g_lingo->push(Datum(kErrorInvalidPos));
		}
	} else if (me->outStream) {
		if (pos <= me->outStream->size()) {
			me->outStream->seek(pos, SEEK_SET);
			g_lingo->push(Datum(kErrorNone));
		} else {
			me->outStream->seek(me->outStream->size(), SEEK_SET);
			g_lingo->push(Datum(kErrorInvalidPos));
		}
	} else {
		warning("FileIO: No file open");
		g_lingo->push(Datum(kErrorFileNotOpen));
	}
}

void FileIO::m_getLength(int nargs) {
	FileObject *me = static_cast<FileObject *>(g_lingo->_currentMeObj);

	if (me->inStream) {
		g_lingo->push(Datum(me->inStream->size()));
	} else if (me->outStream) {
		g_lingo->push(Datum(me->outStream->size()));
	} else {
		warning("FileIO: No file open");
		g_lingo->push(Datum(kErrorFileNotOpen));
	}
}

void FileIO::m_fileName(int nargs) {
	FileObject *me = static_cast<FileObject *>(g_lingo->_currentMeObj);

	if (me->filename) {
		g_lingo->push(Datum(*me->filename));
	} else {
		warning("FileIO: No file open");
		g_lingo->push(Datum(kErrorFileNotOpen));
	}
}

void FileIO::m_delete(int nargs) {
	FileObject *me = static_cast<FileObject *>(g_lingo->_currentMeObj);

	if (me->filename) {
		Common::String filename = *me->filename;
		me->dispose();
		if (g_system->getSavefileManager()->removeSavefile(filename)) {
			g_lingo->push(Datum(kErrorNone));
		} else {
			g_lingo->push(Datum(kErrorIO));
		}
	} else {
		warning("FileIO: No file open");
		g_lingo->push(Datum(kErrorFileNotOpen));
	}
}

} // End of namespace Director
