/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or(at your option) any later version.
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

#include "ags/plugins/ags_waves/ags_waves.h"

namespace AGS3 {
namespace Plugins {
namespace AGSWaves {

const uint32 Magic = 0xACAB0000;
const uint32 Version = 1;
const uint32 SaveMagic = Magic + Version;
const float PI = 3.14159265f;

const char *AGSWaves::AGS_GetPluginName() {
	return "AGS Waves";
}

void AGSWaves::AGS_EngineStartup(IAGSEngine *engine) {
	PluginBase::AGS_EngineStartup(engine);

	if (engine->version < 13)
		engine->AbortGame("Engine interface is too old, need newer version of AGS.");

	StartingValues();

	Character_GetX = engine->GetScriptFunctionAddress("Character::get_X");
	Character_GetY = engine->GetScriptFunctionAddress("Character::get_Y");
	Character_ID = engine->GetScriptFunctionAddress("Character::ID");

	SCRIPT_METHOD(DrawScreenEffect, AGSWaves::DrawScreenEffect);
	SCRIPT_METHOD(SFX_Play, AGSWaves::SFX_Play);
	SCRIPT_METHOD(SFX_SetVolume, AGSWaves::SFX_SetVolume);
	SCRIPT_METHOD(SFX_GetVolume, AGSWaves::SFX_GetVolume);
	SCRIPT_METHOD(Music_Play, AGSWaves::Music_Play);
	SCRIPT_METHOD(Music_GetVolume, AGSWaves::Music_GetVolume);
	SCRIPT_METHOD(Music_SetVolume, AGSWaves::Music_SetVolume);
	SCRIPT_METHOD(SFX_Stop, AGSWaves::SFX_Stop);
	SCRIPT_METHOD(SFX_SetPosition, AGSWaves::SFX_SetPosition);
	SCRIPT_METHOD(SFX_SetGlobalVolume, AGSWaves::SFX_SetGlobalVolume);
	SCRIPT_METHOD(Load_SFX, AGSWaves::Load_SFX);
	SCRIPT_METHOD(Audio_Apply_Filter, AGSWaves::Audio_Apply_Filter);
	SCRIPT_METHOD(Audio_Remove_Filter, AGSWaves::Audio_Remove_Filter);
	SCRIPT_METHOD(SFX_AllowOverlap, AGSWaves::SFX_AllowOverlap);
	SCRIPT_METHOD(SFX_Filter, AGSWaves::SFX_Filter);
	SCRIPT_METHOD(DrawBlur, AGSWaves::DrawBlur);
	SCRIPT_METHOD(DrawTunnel, AGSWaves::DrawTunnel);
	SCRIPT_METHOD(DrawCylinder, AGSWaves::DrawCylinder);
	SCRIPT_METHOD(DrawForceField, AGSWaves::DrawForceField);
	SCRIPT_METHOD(Grayscale, AGSWaves::Grayscale);
	SCRIPT_METHOD(ReadWalkBehindIntoSprite, AGSWaves::ReadWalkBehindIntoSprite);
	SCRIPT_METHOD(AdjustSpriteFont, AGSWaves::AdjustSpriteFont);
	SCRIPT_METHOD(SpriteGradient, AGSWaves::SpriteGradient);
	SCRIPT_METHOD(Outline, AGSWaves::Outline);
	SCRIPT_METHOD(OutlineOnly, AGSWaves::OutlineOnly);
	SCRIPT_METHOD(SaveVariable, AGSWaves::SaveVariable);
	SCRIPT_METHOD(ReadVariable, AGSWaves::ReadVariable);
	SCRIPT_METHOD(GameDoOnceOnly, AGSWaves::GameDoOnceOnly);
	SCRIPT_METHOD(SetGDState, AGSWaves::SetGDState);
	SCRIPT_METHOD(GetGDState, AGSWaves::GetGDState);
	SCRIPT_METHOD(ResetAllGD, AGSWaves::ResetAllGD);
	SCRIPT_METHOD(SpriteSkew, AGSWaves::SpriteSkew);
	SCRIPT_METHOD(FireUpdate, AGSWaves::FireUpdate);
	SCRIPT_METHOD(WindUpdate, AGSWaves::WindUpdate);
	SCRIPT_METHOD(SetWindValues, AGSWaves::SetWindValues);
	SCRIPT_METHOD(ReturnWidth, AGSWaves::ReturnWidth);
	SCRIPT_METHOD(ReturnHeight, AGSWaves::ReturnHeight);
	SCRIPT_METHOD(ReturnNewHeight, AGSWaves::ReturnNewHeight);
	SCRIPT_METHOD(ReturnNewWidth, AGSWaves::ReturnNewWidth);
	SCRIPT_METHOD(Warper, AGSWaves::Warper);
	SCRIPT_METHOD(SetWarper, AGSWaves::SetWarper);
	SCRIPT_METHOD(RainUpdate, AGSWaves::RainUpdate);
	SCRIPT_METHOD(BlendTwoSprites, AGSWaves::BlendTwoSprites);
	SCRIPT_METHOD(Blend, AGSWaves::Blend);
	SCRIPT_METHOD(Dissolve, AGSWaves::Dissolve);
	SCRIPT_METHOD(ReverseTransparency, AGSWaves::ReverseTransparency);
	SCRIPT_METHOD(NoiseCreator, AGSWaves::NoiseCreator);
	SCRIPT_METHOD(TintProper, AGSWaves::TintProper);
	SCRIPT_METHOD(GetWalkbehindBaserine, AGSWaves::GetWalkbehindBaserine);
	SCRIPT_METHOD(SetWalkbehindBaserine, AGSWaves::SetWalkbehindBaserine);
}

void AGSWaves::FireUpdate(ScriptMethodParams &params) {
	//PARAMS2(int, getDynamicSprite, bool, Fire2Visible));
}

void AGSWaves::WindUpdate(ScriptMethodParams &params) {
	//PARAMS4(int, ForceX, int, ForceY, int, Transparency, int, sprite);
}

void AGSWaves::SetWindValues(ScriptMethodParams &params) {
	//PARAMS4(int, w, int, h, int, pr, int, prev);
}

void AGSWaves::RainUpdate(ScriptMethodParams &params) {
	//PARAMS7(int, rdensity, int, FX, int, FY, int, RW, int, RH, int, graphic, float, perc);
}

void AGSWaves::NoiseCreator(ScriptMethodParams &params) {
	//PARAMS2(int, graphic, int, setA);
}

void AGSWaves::StartingValues() {
	GeneralAudio.NumOfChannels = 0;
	GeneralAudio.Initialized = false;
	GeneralAudio.Disabled = false;
	GeneralAudio.FilterFrequency = 10;
	GeneralAudio.SoundValue = 0;
	MFXStream.ID = 0;
	MFXStream.Channel = -1;
	MFXStream.Switch = false;
	MFXStream.FadeTime = 0;
	MFXStream.FadeRate = 0.0;
	MFXStream.FadeVolume = 0.0;
	MFXStream.HaltedZero = false;
	MFXStream.HaltedOne = false;

	int j = 0;
	while (j < 2) {
		globalStream[j].Filename = NULL;
		globalStream[j].repeat = 0;
		globalStream[j].volume = 0;
		globalStream[j].Vorbis = NULL;
		globalStream[j].fix_click = false;
		j++;
	}
}

} // namespace AGSWaves
} // namespace Plugins
} // namespace AGS3
