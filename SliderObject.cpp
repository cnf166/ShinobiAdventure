#include "SliderObject.h"
#include "SoundManager.h"
#include <algorithm>

SliderObject::SliderObject() {
    is_dragging = false;
    min_value = 0;
    max_value = MIX_MAX_VOLUME;
    current_value = max_value / 2;
}

SliderObject::~SliderObject() {}

void SliderObject::SetRect(int x, int y, int bar_w, int bar_h, int slider_w, int slider_h) {
    bar_rect.x = x;
    bar_rect.y = y;
    bar_rect.w = bar_w;
    bar_rect.h = bar_h;

    slider_rect.w = slider_w;
    slider_rect.h = slider_h;
    slider_rect.y = y + (bar_h - slider_h) / 2;

    float ratio = (float)(current_value - min_value) / (max_value - min_value);
    slider_rect.x = x + (float)(ratio * (bar_w - slider_w));
}

bool SliderObject::HandleInput(const SDL_Event& e, SoundManager* sound_manager) {
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mouse_x = e.button.x;
        int mouse_y = e.button.y;
        if (mouse_x >= slider_rect.x && mouse_x <= slider_rect.x + slider_rect.w &&
            mouse_y >= slider_rect.y && mouse_y <= slider_rect.y + slider_rect.h) {
            is_dragging = true;
            sound_manager->PlaySoundEffect("button_click");
            return true;
        }
    }
    else if (e.type == SDL_MOUSEMOTION && is_dragging) {
        int mouse_x = e.motion.x;
        slider_rect.x = std::max(bar_rect.x, std::min(mouse_x - slider_rect.w / 2, bar_rect.x + bar_rect.w - slider_rect.w));
        UpdateVolume(sound_manager);
        return true;
    }
    else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        is_dragging = false;
    }
    return false;
}

void SliderObject::Show(SDL_Renderer* des) {
    SDL_SetRenderDrawColor(des, 100, 100, 100, 255);
    SDL_RenderFillRect(des, &bar_rect);

    if (p_object) {
        SDL_Rect renderQuad = { slider_rect.x, slider_rect.y, slider_rect.w, slider_rect.h };
        SDL_RenderCopy(des, p_object, NULL, &renderQuad);
    } else {
        SDL_SetRenderDrawColor(des, 255, 255, 255, 255);
        SDL_RenderFillRect(des, &slider_rect);
    }
}

void SliderObject::UpdateVolume(SoundManager* sound_manager) {
    float ratio = (float)(slider_rect.x - bar_rect.x) / (bar_rect.w - slider_rect.w);
    current_value = min_value + (int)(ratio * (max_value - min_value));
    sound_manager->SetVolume(current_value);
}
