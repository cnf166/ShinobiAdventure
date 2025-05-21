#ifndef SOUND_MANAGER_H_
#define SOUND_MANAGER_H_

#include <SDL_mixer.h>
#include <string>
#include <map>

class SoundManager {
public:
    SoundManager();
    ~SoundManager();

    bool Init();
    bool LoadMusic(const char* file);
    bool LoadSoundEffect(const char* file, const std::string& id);
    void PlayMusic(int loop);
    void PauseMusic();
    void ResumeMusic();
    void StopMusic();
    bool IsPaused();
    void PlaySoundEffect(const std::string& id);
    void Free();

    void SetVolume(int volume);
    int GetVolume() const { return volume_level; }
    std::map<std::string, Mix_Chunk*>& GetSoundEffects() { return sound_effects; }

private:
    Mix_Music* music;
    std::map<std::string, Mix_Chunk*> sound_effects;
    int volume_level;
};

#endif
