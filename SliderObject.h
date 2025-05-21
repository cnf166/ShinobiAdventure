#ifndef SLIDER_OBJECT_H_
#define SLIDER_OBJECT_H_

#include "BaseObject.h"
#include <SDL.h>
#include <string>

class SoundManager;

class SliderObject : public BaseObject {
public:
    SliderObject();
    ~SliderObject();

    void SetRect(int x, int y, int bar_w, int bar_h, int slider_w, int slider_h);
    bool HandleInput(const SDL_Event& e, SoundManager* sound_manager);
    void Show(SDL_Renderer* des);
    void UpdateVolume(SoundManager* sound_manager);

private:
    SDL_Rect bar_rect;
    SDL_Rect slider_rect;
    bool is_dragging;
    int min_value;
    int max_value;
    int current_value;
};

#endif
