#include "Audio.h"
#include "Log.h"
#include <map>

//sounds to be added
static std::map<Music, const char*> music_paths =
{
    { m_title, "Assets/Audio/Music/title music.wav" },
	{ m_battle, "Assets/Audio/Music/battle music.wav" },
    { m_roof, "Assets/Audio/Music/roof.wav" },
    { m_roof_drums, "Assets/Audio/Music/drums.wav" },
    { m_restaurant, "Assets/Audio/Music/rest.wav" },
    { m_supermarket, "Assets/Audio/Music/regular market.wav" },
	{ m_rest_dungeon, "Assets/Audio/Music/rest dungeon.wav" }

	
};

static std::map<Sfx, const char*> sfx_paths =
{
    { s_title_name, "Assets/Audio/Fx/crazy-supermarket.wav" },
    { s_epic_reveal, "Assets/Audio/Fx/logo-epic-reveal.wav" },
    {jumpscare, "Assets/Audio/Fx/jumpscare.wav"},
    {s_button, "Assets/Audio/Fx/button.wav"},
    {s_slider, "Assets/Audio/Fx/slider.wav"},
    {s_punch, "Assets/Audio/Fx/punch.wav" },
    {s_kick, "Assets/Audio/Fx/kick.wav" }


};

Audio::Audio() {
    name = "audio";
}

Audio::~Audio() {
    // Make sure everything is freed in CleanUp
}


bool Audio::LoadWavFile(const char* path, SoundData& out) {
    // SDL_LoadWAV fills spec + allocates buf; free with SDL_free() later.
    if (!SDL_LoadWAV(path, &out.spec, &out.buf, &out.len)) {
        SDL_Log("SDL_LoadWAV failed for %s: %s", path, SDL_GetError());
        return false;
    }
    return true;
}


void Audio::FreeSound(SoundData& s) {
    if (s.buf) {
        SDL_free(s.buf);
        s.buf = nullptr;
        s.len = 0;
        s.spec = SDL_AudioSpec{};
    }
}

bool Audio::EnsureDeviceOpen() {
    if (device_ != 0) return true;

    // Ask for a reasonable default device format (float32, stereo, 48k).
    SDL_AudioSpec want{};
    want.format = SDL_AUDIO_F32;
    want.channels = 2;
    want.freq = 48000;

    device_ = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &want);
    if (device_ == 0) {
        LOG("Audio: SDL_OpenAudioDevice failed: %s", SDL_GetError());
        return false;
    }

    // Query actual device format (may differ from 'want')
    if (!SDL_GetAudioDeviceFormat(device_, &device_spec_, nullptr)) {
        LOG("Audio: SDL_GetAudioDeviceFormat failed: %s", SDL_GetError());
        SDL_CloseAudioDevice(device_);
        device_ = 0;
        return false;
    }

    // Start audio
    SDL_ResumeAudioDevice(device_);

    return true;
}

bool Audio::EnsureStreams() {
    if (!EnsureDeviceOpen()) return false;

    if (!music_stream_) {
        music_stream_ = SDL_CreateAudioStream(nullptr, &device_spec_);
        if (!music_stream_) {
            LOG("Audio: SDL_CreateAudioStream (music) failed: %s", SDL_GetError());
            return false;
        }
        if (!SDL_BindAudioStream(device_, music_stream_)) {
            LOG("Audio: SDL_BindAudioStream (music) failed: %s", SDL_GetError());
            SDL_DestroyAudioStream(music_stream_);
            music_stream_ = nullptr;
            return false;
        }
    }

	// Set music volume
    SDL_SetAudioStreamGain(music_stream_, music_volume_);

    

	// Set SFX volume
    //SDL_SetAudioStreamGain(sfx_stream_, sfx_volume_);

    return true;
}


bool Audio::Awake() {
    LOG("Audio: initializing SDL3 audio");
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != true /* SDL3 returns bool */) {
        LOG("SDL_INIT_AUDIO failed: %s", SDL_GetError());
        active = false;
        return true; // don't hard-fail the app
    }

    if (!EnsureDeviceOpen()) {
        active = false;
        return true;
    }

    return true;
}

bool Audio::CleanUp() {
    // If audio is inactive or already quit elsewhere, don't touch SDL objects.
    if (!active || !SDL_WasInit(SDL_INIT_AUDIO)) {
        music_stream_ = nullptr;
 
        device_ = 0;
        sfx_.clear();
        FreeSound(music_data_);
        return true;
    }

    LOG("Audio: cleaning up");

    // Optional: stop pulling data while we tear down.
    if (device_ != 0) SDL_PauseAudioDevice(device_);

    // Destroy streams (auto-unbinds if bound).
    if (music_stream_) {
        SDL_DestroyAudioStream(music_stream_);
        music_stream_ = nullptr;
    }
    FreeSound(music_data_);

    
    for (auto& s : sfx_) FreeSound(s);
    sfx_.clear();

    // Close device after streams are gone.
    if (device_ != 0) {
        SDL_CloseAudioDevice(device_);
        device_ = 0;
    }

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    active = false;
    return true;
}


bool Audio::PlayMusic(Music id, float fadeTime, int repeat) {

    auto it = music_paths.find(id);


    if (it == music_paths.end()) {
        LOG("Music id not found");
        return false;
    }

    const char* path = it->second;

    if (currentMusic_ == id && !IsMusicFinished())
        return true;
    if (!active) return false;
    if (!EnsureStreams()) return false;

    if (music_stream_) SDL_ClearAudioStream(music_stream_);

    FreeSound(music_data_);

    if (!LoadWavFile(path, music_data_)) {
        LOG("Audio: cannot load music %s", path);
        return false;
    }

    if (!SDL_SetAudioStreamFormat(music_stream_, &music_data_.spec, &device_spec_)) {
        LOG("Audio: stream format failed");
        return false;
    }

    if (repeat == -1) { // -1 = infinito
        SDL_PutAudioStreamData(music_stream_, music_data_.buf, music_data_.len);
        music_loop_ = true;
    }
    else {
        music_loop_ = false;
        for (int i = 0; i <= repeat; i++) {
            SDL_PutAudioStreamData(music_stream_, music_data_.buf, music_data_.len);
        }
    }
    currentMusic_ = id;
    LOG("Playing music %s", path);
    return true;
}

int Audio::LoadFx(const char* path) {
    if (!active) return 0;
    if (!EnsureDeviceOpen()) return 0;

    SoundData s{};
    if (!LoadWavFile(path, s)) {
        LOG("Audio: cannot load fx %s", path);
        return 0;
    }

    sfx_.push_back(s);
    return (int)sfx_.size(); // 1-based outward index
}

bool Audio::PlayFx(Sfx id, int repeat) {

    auto it = sfx_paths.find(id);
    if (it == sfx_paths.end()) {
        LOG("SFX id not found");
        return false;
    }

    const char* path = it->second;

    int fx = LoadFx(path);

    if (fx == 0) return false;

    

    if (!EnsureDeviceOpen()) return false;

    const SoundData& s = sfx_[fx - 1];

    // Create a new stream for this sound effect
    SDL_AudioStream* stream = SDL_CreateAudioStream(&s.spec, &device_spec_);
    if (!stream) {
        LOG("Audio: SDL_CreateAudioStream(sfx) failed: %s", SDL_GetError());
        return false;
    }

    if (!SDL_BindAudioStream(device_, stream)) {
        LOG("Audio: SDL_BindAudioStream(sfx) failed: %s", SDL_GetError());
        SDL_DestroyAudioStream(stream);

        return false;
    }

    // Queue the sound data 'repeat + 1' times
    for (int i = 0; i <= repeat; i++) {
        if (!SDL_PutAudioStreamData(stream, s.buf, s.len)) {
            LOG("Audio: SDL_PutAudioStreamData(sfx) failed: %s", SDL_GetError());
            SDL_DestroyAudioStream(stream);
            return false;
        }
    }
    // Keep track of the active stream to manage its lifetime
    active_sfx_streams_.push_back(stream);
    return true;
}

void Audio::SetMusicVolume(float volume)
{
    // clamp
    if (volume < 0.0f) volume = 0.0f;
    else if (volume > 1.0f) volume = 1.0f;

    music_volume_ = volume;

    if (music_stream_) {
        SDL_SetAudioStreamGain(music_stream_, music_volume_);
    }
}

void Audio::SetSFXVolume(float volume)
{

    // clamp
    if (volume < 0.0f) volume = 0.0f;
    else if (volume > 1.0f) volume = 1.0f;

    sfx_volume_ = volume;

    /*if (sfx_stream_) {
        SDL_SetAudioStreamGain(sfx_stream_, sfx_volume_);
    }*/
}

bool Audio::Update(float dt)
{
    if (!active) return true;

    // Loops
    if (music_loop_ && music_stream_ && music_data_.buf)
    {
        if (SDL_GetAudioStreamAvailable(music_stream_) == 0)
        {
            SDL_PutAudioStreamData(music_stream_, music_data_.buf, music_data_.len);
        }
    }

    // Clean up finished sound effect streams
    for (auto it = active_sfx_streams_.begin(); it != active_sfx_streams_.end(); )
    {
        SDL_AudioStream* stream = *it;
        if (!stream) {
            it = active_sfx_streams_.erase(it);
            continue;
        }
        int queued = SDL_GetAudioStreamQueued(stream);
        if (queued == 0) {
            SDL_DestroyAudioStream(stream);
            it = active_sfx_streams_.erase(it);
        }
        else {
            ++it;
        }
    }

    return true;
}