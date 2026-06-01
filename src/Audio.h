#pragma once

#include "Module.h"
#include <SDL3/SDL.h>
#include "Defs.h"
#include "List.h"
#include <vector>
#include <string>

#define DEFAULT_MUSIC_FADE_TIME 2.0f
#define Mix_LoadWAV(file)   Mix_LoadWAV_RW(SDL_RWFromFile(file, "rb"), 1)


struct _Mix_Music;
struct Mix_Chunk;


enum Music {
    m_OFF = 0,
    m_title,
    m_battle,
    m_roof,
    m_roof_drums,
	m_restaurant,
	m_supermarket
};

enum Sfx {
    s_OFF = 0,
    s_title_name,
    s_epic_reveal,
    jumpscare,
    s_button,
    s_slider,
    s_punch,
    s_kick
};


class Audio : public Module
{
public:

	Audio();

	// Destructor
	virtual ~Audio();

	// Called before render is available
	bool Awake();

	// Called before quitting
	bool CleanUp();

    bool Update(float dt) override;

	// Play a music file
	bool PlayMusic(Music id, float fadeTime, int repeat = -1);

	// Load a WAV in memory
	int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(Sfx id, int repeat);

	// Check if music is currently playing
    bool IsMusicFinished() const
    {
        if (!music_stream_) return true;
        return SDL_GetAudioStreamAvailable(music_stream_) == 0;
    }

    // 
    // Music track
    bool ChangeMusic(int id, float fadeInTime = DEFAULT_MUSIC_FADE_TIME, float fadeOutTime = DEFAULT_MUSIC_FADE_TIME);

    void StopFx()
    {
        for (SDL_AudioStream* stream : active_sfx_streams_)
        {
            if (stream) SDL_DestroyAudioStream(stream);
        }
        active_sfx_streams_.clear();
    }

	// Volume control
    void SetMusicVolume(float volume); // 0.0f � 1.0f
    void SetSFXVolume(float volume);   // 0.0f � 1.0f



private:

    struct SoundData {
        SDL_AudioSpec spec{};  // source format
        Uint8* buf{ nullptr };
        Uint32 len{ 0 };  // bytes
    };

    // Currently playing sound effect
    struct PlayingSfx {
        SDL_AudioStream* stream{ nullptr };
        float timeLeft{ 0.0f };   // segundos restantes
    };

    // Device and default output format
    SDL_AudioDeviceID device_{ 0 };
    SDL_AudioSpec     device_spec_{};

    // Streams
    SDL_AudioStream* music_stream_{ nullptr }; // for background music (single)
    

    // Loaded sounds
    SoundData music_data_{};
    std::vector<SoundData> sfx_; // 1-based indexing outwardly
    std::vector<SDL_AudioStream*> active_sfx_streams_;

	// Volume control
    float music_volume_ = 1.0f; // 0.0 = mute, 1.0 = full
    float sfx_volume_ = 1.0f;

    // helpers
    bool LoadWavFile(const char* path, SoundData& out);
    void FreeSound(SoundData& s);
    bool EnsureDeviceOpen();
    bool EnsureStreams();
    
    bool music_loop_ = false;
    Music currentMusic_ = m_OFF;
    _Mix_Music* music;
    List<Mix_Chunk*>	fx;

    
};
