#pragma once

#include <LCCLient/Audio/AudioAlert.h>

#include <miniaudio/miniaudio.h>

#include <string_view>
#include <unordered_map>

class AudioManager
{
public:
	static void Init();
	static void Cleanup();
	static void LoadAudio(AudioAlert audioType, std::string_view audioFilePath);
	static void PlayAudio(AudioAlert audioType);
private:
	inline static ma_engine s_AudioEngine;
	inline static bool s_IsInitialized = false;
	inline static std::unordered_map<AudioAlert, ma_sound> s_AudioCache;
};