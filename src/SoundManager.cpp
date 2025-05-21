#include "SoundManager.h"
#include <iostream>

SoundManager::SoundManager() {
    music = nullptr;
    volume_level = MIX_MAX_VOLUME / 2;
}

SoundManager::~SoundManager() {
    Free();
}

//khởi tạo
bool SoundManager::Init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "Mix_OpenAudio: " << Mix_GetError() << std::endl;
        return false;
    }
    return true;
}

//dừng nhạc
void SoundManager::StopMusic() {
    Mix_HaltMusic();
}


//tải nhạc
bool SoundManager::LoadMusic(const char* file) {
    if (music) {
        Mix_FreeMusic(music);
        music = nullptr;
    }
    music = Mix_LoadMUS(file);
    if (!music) {
        std::cout << "Mix_LoadMUS: " << Mix_GetError() << std::endl;
        return false;
    }
    Mix_VolumeMusic(volume_level);
    return true;
}

//tải hiệu ứng âm thanh
bool SoundManager::LoadSoundEffect(const char* file, const std::string& id) {
    Mix_Chunk* sound = Mix_LoadWAV(file);
    if (!sound) {
        std::cout << "Mix_LoadWAV: " << Mix_GetError() << std::endl;
        return false;
    }
    sound_effects[id] = sound;
    Mix_VolumeChunk(sound, volume_level);
    return true;
}

//phát nhạc (-1 lặp vô hạn, 0 phát một lần)
void SoundManager::PlayMusic(int loop) {
    if (music) {
        Mix_PlayMusic(music, loop);
    }
}
//tiếp tục phát nhạc
void SoundManager::PauseMusic() {
    Mix_PauseMusic();
}
//tạm dừng nhạc
void SoundManager::ResumeMusic() {
    Mix_ResumeMusic();
}
//kiểm tra nhạc có tạm dừng không
bool SoundManager::IsPaused() {
    return Mix_PausedMusic() == 1;
}
//phát hiệu ứng âm thanh dựa trên định danh
void SoundManager::PlaySoundEffect(const std::string& id) {
    auto it = sound_effects.find(id);
    if (it != sound_effects.end()) {
        Mix_PlayChannel(-1, it->second, 0);
    }
}

void SoundManager::Free() {
    if (music) {
        Mix_FreeMusic(music);
        music = nullptr;
    }
    for (auto& pair : sound_effects) {
        Mix_FreeChunk(pair.second);
    }
    sound_effects.clear();
}

void SoundManager::SetVolume(int volume) {
    volume_level = std::max(0, std::min(volume, MIX_MAX_VOLUME));
    Mix_VolumeMusic(volume_level);
    for (auto& pair : sound_effects) {
        Mix_VolumeChunk(pair.second, volume_level);
    }
}
