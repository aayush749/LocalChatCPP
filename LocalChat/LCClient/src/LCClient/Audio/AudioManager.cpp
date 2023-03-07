#include <LCClient/Audio/AudioManager.h>
#include <Logger/Logger.h>

#include <cstdlib>

void AudioManager::Init()
{
    ma_result result;

    result = ma_engine_init(NULL, &s_AudioEngine);
    if (result != MA_SUCCESS)
    {
        Logger::LogError("Could not initialize audio engine.");
        std::exit(EXIT_FAILURE);
    }
}

void AudioManager::Cleanup()
{
    ma_engine_uninit(&s_AudioEngine);
}

void AudioManager::LoadAudio(AudioAlert audioType, std::string_view audioFile)
{
    ma_result result;
    ma_sound& sound = s_AudioCache[audioType];

    result = ma_sound_init_from_file(&s_AudioEngine, audioFile.data(), 0, NULL, NULL, &sound);
    if (result != MA_SUCCESS)
    {
        Logger::logfmt<Log::ERR>("Could not load audio from file: \"%s\"", audioFile);
    }
}

void AudioManager::PlayAudio(AudioAlert audioType)
{
    ma_sound_start(&s_AudioCache.at(audioType));
}