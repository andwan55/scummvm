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

#ifndef TWINE_PARSER_BODY_H
#define TWINE_PARSER_BODY_H

#include "common/array.h"
#include "common/memstream.h"
#include "common/stream.h"
#include "twine/parser/anim.h"
#include "twine/parser/bodytypes.h"
#include "twine/parser/parser.h"
#include "twine/shared.h"

namespace TwinE {

class BodyData : public Parser {
private:
	void loadVertices(Common::SeekableReadStream &stream);
	void loadBones(Common::SeekableReadStream &stream);
	void loadShades(Common::SeekableReadStream &stream);
	void loadPolygons(Common::SeekableReadStream &stream);
	void loadLines(Common::SeekableReadStream &stream);
	void loadSpheres(Common::SeekableReadStream &stream);

	Common::Array<BodyPolygon> _polygons;
	Common::Array<BodyVertex> _vertices;
	Common::Array<BodySphere> _spheres;
	Common::Array<BodyShade> _shades;
	Common::Array<BodyLine> _lines;
	Common::Array<BodyBone> _bones;

	BoneFrame _boneStates[560];

protected:
	void reset() override;

public:
	union BodyFlags {
		struct BitMask {
			uint32 unk1 : 1;            // 1 << 0
			uint32 animated : 1;        // 1 << 1
			uint32 unk3 : 1;            // 1 << 2
			uint32 unk4 : 1;            // 1 << 3
			uint32 unk5 : 1;            // 1 << 4
			uint32 unk6 : 1;            // 1 << 5
			uint32 unk7 : 1;            // 1 << 6
			uint32 alreadyPrepared : 1; // 1 << 7
			uint32 unk9 : 1;            // 1 << 8
			uint32 unk10 : 1;           // 1 << 9
			uint32 unk11 : 1;           // 1 << 10
			uint32 unk12 : 1;           // 1 << 11
			uint32 unk13 : 1;           // 1 << 12
			uint32 unk14 : 1;           // 1 << 13
			uint32 unk15 : 1;           // 1 << 14
			uint32 unk16 : 1;           // 1 << 15
		} mask;
		uint32 value;
	} bodyFlag;

	BoundingBox bbox;
	int16 offsetToData = 0;

	inline bool isAnimated() const {
		return bodyFlag.mask.animated;
	}

	inline uint getNumBones() const {
		return _bones.size();
	}

	inline uint getNumVertices() const {
		return _vertices.size();
	}

	BoneFrame *getBoneState(int16 boneIdx) {
		return &_boneStates[boneIdx];
	}

	const BoneFrame *getBoneState(int16 boneIdx) const {
		return &_boneStates[boneIdx];
	}

	const Common::Array<BodyPolygon> &getPolygons() const {
		return _polygons;
	}

	const Common::Array<BodyVertex> &getVertices() const {
		return _vertices;
	}

	const Common::Array<BodySphere> &getSpheres() const {
		return _spheres;
	}

	const Common::Array<BodyShade> &getShades() const {
		return _shades;
	}

	const BodyShade &getShade(int16 shadeIdx) const {
		return _shades[shadeIdx];
	}

	const Common::Array<BodyLine> &getLines() const {
		return _lines;
	}

	const Common::Array<BodyBone> &getBones() const {
		return _bones;
	}

	const BodyBone &getBone(int16 boneIdx) const {
		return _bones[boneIdx];
	}

	bool loadFromStream(Common::SeekableReadStream &stream, bool lba1) override;
};

} // End of namespace TwinE

#endif
