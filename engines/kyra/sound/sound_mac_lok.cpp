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

#include "kyra/sound/sound_intern.h"
#include "kyra/sound/sound_mac_res.h"
#include "kyra/sound/drivers/halestorm.h"

#include "common/config-manager.h"
#include "common/macresman.h"
#include "common/punycode.h"

#include "audio/mixer.h"


#define HS_16BITOUTPUT		false
#define HS_INTERPOLATION	kNone


namespace Kyra {

SoundMacRes::SoundMacRes() : _macRes(0) {
	_macRes = new Common::MacResManager();
}

SoundMacRes::~SoundMacRes() {
	delete _macRes;
}

bool SoundMacRes::init() {
	if (!_macRes)
		return false;

	// The original executable has a TM char as its last character (character 0xaa
	// from Mac code page). Depending on the emulator or platform used to copy the
	// file it might have been reencoded to something else. So I look for multiple
	// versions, also for punycode encoded files and also for the case where the
	// user might have just removed the last character by renaming the file.

	const char *const tryExeNames[] = {
		"Legend of Kyrandia\xaa",
		"Legend of Kyrandia"
	};

	const Common::CodePage tryCodePages[] = {
		Common::kMacRoman,
		Common::kISO8859_1
	};

	for (int i = 0; i < ARRAYSIZE(tryCodePages); ++i) {
		for (int ii = 0; ii < ARRAYSIZE(tryExeNames); ++ii) {
			Common::U32String fn(tryExeNames[ii], tryCodePages[i]);
			_kyraMacExe = fn.encode(Common::kUtf8);
			if (_macRes->exists(_kyraMacExe))
				break;
			_kyraMacExe = Common::punycode_encodefilename(fn);
			if (_macRes->exists(_kyraMacExe))
				break;
			_kyraMacExe.clear();
		}
		if (!_kyraMacExe.empty())
			break;
	}

	if (_kyraMacExe.empty()) {
		warning("SoundMacRes::init(): Legend of Kyrandia resource fork not found");
		return false;
	}

	setQuality(true);

	for (Common::StringArray::iterator i = _resFiles.begin(); i != _resFiles.end(); ++i) {
		if (!_macRes->exists(*i)) {
			warning("SoundMacRes::init(): Error opening data file: '%s'", i->c_str());
			return false;
		}
	}

	// Test actual resource fork reading...
	Common::SeekableReadStream *test = getResource(2, 'SMOD');
	if (!test) {
		warning("SoundMacRes::init(): Resource fork read test failed for 'Legend of Kyrandia' executable");
		return false;
	}
	delete test;

	test = getResource(202, 'SONG');
	if (!test) {
		warning("SoundMacRes::init(): Resource fork read test failed for 'HQ_Music.res'");
		return false;
	}
	delete test;

	return true;
}

Common::SeekableReadStream *SoundMacRes::getResource(uint16 id, uint32 type) {
	Common::SeekableReadStream *res = 0;
	for (Common::StringArray::iterator i = _resFiles.begin(); i != _resFiles.end(); ++i) {
		if (!_macRes->open(Common::Path(*i)))
			warning("SoundMacRes::getResource(): Error opening data file: '%s'", i->c_str());
		if ((res = _macRes->getResource(type, id)))
			break;
	}
	return res;
}

void SoundMacRes::setQuality(bool hi) {
	_resFiles.clear();
	_resFiles.push_back(hi ? "HQ_Music.res" : "LQ_Music.res");
	_resFiles.push_back(_kyraMacExe);
}

SoundMac::SoundMac(KyraEngine_v1 *vm, Audio::Mixer *mixer) : Sound(vm, mixer), _driver(0), _res(0), _currentResourceSet(-1), _resIDMusic(0), _ready(false) {
}

SoundMac::~SoundMac() {
	delete _driver;
	delete _res;
}

Sound::kType SoundMac::getMusicType() const {
	return kMac;
}

bool SoundMac::init(bool hiQuality) {
	_res = new SoundMacRes();
	if (!(_res && _res->init()))
		return false;

	_driver = new HalestormDriver(_res, _mixer);

	if (!(_driver && _driver->init(hiQuality, HalestormDriver::HS_INTERPOLATION, HS_16BITOUTPUT)))
		return false;

	setQuality(hiQuality);

	return (_ready = true);
}

void SoundMac::selectAudioResourceSet(int set) {
	if (set < 0 || set > 2 || set == _currentResourceSet)
		return;

	_currentResourceSet = set;

	if (set == 0)
		_resIDMusic = _resIDMusicIntro;
	else if (set == 1)
		_resIDMusic = _resIDMusicIngame;
}

void SoundMac::playTrack(uint8 track) {
	if (!_musicEnabled || !_ready)
		return;

	int loop = 0;

	if (_currentResourceSet == kMusicIntro) {
		track -= 2;
		assert(track < 4);
	} else if (track == 0xff || track == 3) {
		return;
	} else if (track == 0 || track == 1) {
		beginFadeOut();
		return;
	} else if (_currentResourceSet == kMusicFinale && track == 2) {
		_driver->doCommand(1, 0x12c);
		return;
	} else {
		track -= 11;
		assert(track < 35);
		loop = _musicLoopTable[track];
	}

	_driver->doCommand(loop, _resIDMusic[track]);
}

void SoundMac::haltTrack() {
	if (_ready)
		_driver->doCommand(2);
}

void SoundMac::playSoundEffect(uint16 track, uint8) {
	if (!_sfxEnabled || !_ready)
		return;

	if (_currentResourceSet == kMusicIntro) {
		if (track > 21 && track < 38)
			_driver->startSoundEffect(_resIDSfxIntro[_soundEffectDefsIntro[track - 22].number]);
	} else {
		const SoundEffectDef *se = &_soundEffectDefsIngame[track];
		if (se->note)
			_driver->enqueueSoundEffect(_resIDSfxIngame[se->number], se->rate, se->note);
	}
}

bool SoundMac::isPlaying() const {
	return _ready && _driver->doCommand(3);
}

void SoundMac::beginFadeOut() {
	if (!_ready)
		return;

	if (_currentResourceSet == kMusicIngame) {
		haltTrack();
		return;
	}

	_driver->doCommand(10, 30);
	while (_driver->doCommand(12) >= 16)
		_vm->delay(8);
	_driver->doCommand(2);
	_driver->doCommand(11, 30);
}

void SoundMac::updateVolumeSettings() {
	if (!_ready)
		return;

	bool mute = ConfMan.hasKey("mute") ? ConfMan.getBool("mute") : false;
	_driver->setMusicVolume(CLIP<int>(mute ? 0 : ConfMan.getInt("music_volume"), 0, Audio::Mixer::kMaxMixerVolume));
	_driver->setSoundEffectVolume(CLIP<int>(mute ? 0 : ConfMan.getInt("sfx_volume"), 0, Audio::Mixer::kMaxMixerVolume));
}

void SoundMac::setQuality(bool hi) {
	static const uint16 resIds[] = {
		0x1b5b, 0x1b5c, 0x1b5e, 0x1b62, 0x1b63, 0x1b6b, 0x1b6c, 0x1b6d,
		0x1b6e, 0x1b6f, 0x1b70, 0x1b71, 0x1b72, 0x1b73, 0x1b74, 0x1b75,
		0x1b76, 0x1b77, 0x1b78, 0x1b79, 0x1b7a, 0x1b7b, 0x1b7c, 0x1b7d,
		0x1b7e, 0x1b8a, 0x1bbc, 0x1bbd, 0x1bbe, 0xffff
	};

	if (!(_driver && _res))
		return;

	_driver->doCommand(2);
	_driver->stopAllSoundEffects();
	_driver->releaseSamples();

	_res->setQuality(hi);

	if (hi) {
		_driver->changeSystemVoices(7, 4, 1);
		_driver->doCommand(14, 3);
	} else {
		_driver->changeSystemVoices(4, 3, 1);
		_driver->doCommand(14, 2);
	}

	_driver->registerSamples(resIds, true);
}

const uint16 SoundMac::_resIDMusicIntro[4] {
	0x00c8, 0x00c9, 0x00ca, 0x00cb
};

const uint16 SoundMac::_resIDMusicIngame[35] {
	0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b,
	0x006c, 0x006d, 0x006e, 0x006f, 0x0070, 0x0071, 0x0072, 0x0073,
	0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007a, 0x01f4,
	0x01f5, 0x01f6, 0x01f7, 0x01f8, 0x01f9, 0x01fa, 0x01fb, 0x01fc,
	0x01fd, 0x01fe, 0x01ff
};


const uint8 SoundMac::_musicLoopTable[35] {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01,
	0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x00, 0x00
};

const uint16 SoundMac::_resIDSfxIntro[39] {
	0x1b58, 0x1b59, 0x1b5a, 0x1b5b, 0x1b5c, 0x1b5d, 0x1b5e, 0x1b5f,
	0x1b60, 0x1b61, 0x1b62, 0x1b63, 0x1b64, 0x1b65, 0x1b66, 0x1b67,
	0x1b68, 0x1b69, 0x1b6a, 0x1b6d, 0x1b6c, 0x1b7a, 0x1bbc, 0x1bbd,
	0x1bbe, 0x1b71, 0x1b72, 0x1b73, 0x1b74, 0x1b75, 0x1b76, 0x1b77,
	0x1b78, 0x1b79, 0x1b7a, 0x1b7b, 0x1b7c, 0x1b7d, 0x1b7e
};

const uint16 SoundMac::_resIDSfxIngame[39] {
	0x1b58, 0x1b59, 0x1b5a, 0x1b5b, 0x1b5c, 0x1b5d, 0x1b5e, 0x1b5f,
	0x1b60, 0x1b61, 0x1b62, 0x1b63, 0x1b64, 0x1b65, 0x1b66, 0x1b67,
	0x1b68, 0x1b69, 0x1b6a, 0x1b6b, 0x1b6c, 0x1b6d, 0x1b6e, 0x1b6f,
	0x1b70, 0x1b71, 0x1b72, 0x1b73, 0x1b74, 0x1b75, 0x1b76, 0x1b77,
	0x1b78, 0x1b8a, 0x1b7a, 0x1b7b, 0x1b7c, 0x1b7d, 0x1b7e
};

const SoundMac::SoundEffectDef SoundMac::_soundEffectDefsIntro[16] = {
	{ 0x3c, 0x19, 0x252c, 0x6e }, { 0x3c, 0x19, 0x252c, 0x6e }, { 0x3c, 0x19, 0x252c, 0x6e }, { 0x3c, 0x13, 0x1B91, 0x6e },
	{ 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x3c, 0x16, 0x2677, 0x6e }, { 0x00, 0x00, 0x0000, 0x00 },
	{ 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x3c, 0x17, 0x1198, 0x6e }, { 0x3c, 0x19, 0x252c, 0x6e },
	{ 0x3c, 0x18, 0x22d1, 0x6e }, { 0x3c, 0x19, 0x252c, 0x6e },	{ 0x45, 0x03, 0x0224, 0x6e }, { 0x3c, 0x16, 0x2677, 0x6e }
};

const SoundMac::SoundEffectDef SoundMac::_soundEffectDefsIngame[120] = {
	{ 0x00, 0x00, 0x0000, 0x00 }, { 0x01, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 },
	{ 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 },
	{ 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 },
	{ 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 },
	{ 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 },
	{ 0x3c, 0x13, 0x0156, 0x78 }, { 0x3c, 0x14, 0x272c, 0x78 }, { 0x3c, 0x15, 0x1b91, 0x78 }, { 0x3c, 0x16, 0x1e97, 0x78 },
	{ 0x3c, 0x17, 0x122b, 0x78 }, { 0x3c, 0x16, 0x1e97, 0x78 }, { 0x45, 0x03, 0x0224, 0x78 }, { 0x3c, 0x16, 0x1e97, 0x78 },
	{ 0x00, 0x00, 0x0000, 0x00 }, { 0x3c, 0x19, 0x252c, 0x78 }, { 0x2c, 0x04, 0x0910, 0x78 }, { 0x3c, 0x19, 0x252c, 0x78 },
	{ 0x3c, 0x1a, 0x3aeb, 0x78 }, { 0x25, 0x1b, 0x138b, 0x78 }, { 0x18, 0x03, 0x0f52, 0x78 }, { 0x3e, 0x1c, 0x0622, 0x78 },
	{ 0x3b, 0x1c, 0x0754, 0x78 }, { 0x16, 0x03, 0x206f, 0x78 }, { 0x3c, 0x19, 0x252c, 0x78 }, { 0x3c, 0x1d, 0x09ea, 0x78 },
	{ 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x3c, 0x14, 0x272c, 0x78 },
	{ 0x3c, 0x1e, 0x036e, 0x78 }, { 0x3c, 0x17, 0x122b, 0x78 }, { 0x4e, 0x0b, 0x0991, 0x78 }, { 0x47, 0x1b, 0x02bc, 0x78 },
	{ 0x4c, 0x1b, 0x0211, 0x78 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x3c, 0x13, 0x0156, 0x78 }, { 0x3c, 0x13, 0x0156, 0x78 },
	{ 0x3c, 0x1f, 0x0e9e, 0x78 }, { 0x3c, 0x20, 0x010c, 0x78 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x3c, 0x19, 0x252c, 0x78 },
	{ 0x3c, 0x21, 0x0f7c, 0x78 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 },
	{ 0x2a, 0x0b, 0x4c47, 0x78 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 },
	{ 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x3c, 0x1b, 0x0528, 0x78 }, { 0x00, 0x00, 0x0000, 0x00 },
	{ 0x00, 0x00, 0x0000, 0x00 }, { 0x2c, 0x04, 0x0910, 0x78 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x3c, 0x22, 0x0aee, 0x78 },
	{ 0x3c, 0x16, 0x1e97, 0x78 }, { 0x3c, 0x15, 0x1b91, 0x78 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 },
	{ 0x00, 0x00, 0x0000, 0x00 }, { 0x3c, 0x14, 0x272c, 0x78 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x3c, 0x22, 0x0aee, 0x78 },
	{ 0x3c, 0x14, 0x272c, 0x78 }, { 0x32, 0x23, 0x1419, 0x9c }, { 0x3c, 0x19, 0x171c, 0x78 }, { 0x3c, 0x14, 0x272c, 0x78 },
	{ 0x3e, 0x1c, 0x0622, 0x78 }, { 0x43, 0x13, 0x0201, 0x78 }, { 0x3c, 0x24, 0x1243, 0x5a }, { 0x3e, 0x20, 0x00ee, 0x78 },
	{ 0x3c, 0x19, 0x252c, 0x78 }, { 0x29, 0x04, 0x19ea, 0x78 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 },
	{ 0x00, 0x00, 0x0000, 0x00 }, { 0x3c, 0x20, 0x010c, 0x78 }, { 0x3c, 0x25, 0x30b6, 0x78 }, { 0x3c, 0x19, 0x252c, 0x78 },
	{ 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 },
	{ 0x00, 0x00, 0x0000, 0x00 }, { 0x3c, 0x16, 0x1e97, 0x78 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x3c, 0x1a, 0x3aeb, 0x78 },
	{ 0x1b, 0x04, 0x39f3, 0x78 }, { 0x30, 0x23, 0x1699, 0x50 }, { 0x3c, 0x15, 0x1b91, 0x78 }, { 0x29, 0x06, 0x19ea, 0x50 },
	{ 0x3c, 0x19, 0x252c, 0x78 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x3c, 0x1a, 0x3aeb, 0x78 },
	{ 0x3c, 0x19, 0x252c, 0x78 }, { 0x3c, 0x26, 0x0713, 0x78 }, { 0x3c, 0x26, 0x0713, 0x78 }, { 0x3c, 0x14, 0x272c, 0x78 },
	{ 0x30, 0x23, 0x1699, 0x50 }, { 0x30, 0x23, 0x1699, 0x50 }, { 0x00, 0x00, 0x0000, 0x00 }, { 0x3c, 0x13, 0x0156, 0x78 }
};

#undef HS_16BITOUTPUT
#undef HS_INTERPOLATION

} // End of namespace Kyra
