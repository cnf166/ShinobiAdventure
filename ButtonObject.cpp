#include "ButtonObject.h"
#include "SoundManager.h"

ButtonObject::ButtonObject() {
    is_clicked = false;
}

ButtonObject::~ButtonObject() {}

void ButtonObject::SetRect(const int& x, const int& y, const int& w, const int& h, const std::string& type) {
    button_rect.x = x;
    button_rect.y = y;
    button_rect.w = w;
    button_rect.h = h;
    button_type = type;
}

bool ButtonObject::CheckClick(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mouse_x = e.button.x;
        int mouse_y = e.button.y;
        if (mouse_x >= button_rect.x && mouse_x <= button_rect.x + button_rect.w &&
            mouse_y >= button_rect.y && mouse_y <= button_rect.y + button_rect.h) {
            if (button_type == "pause" || button_type == "continue" || button_type == "music" || button_type == "restart" || button_type == "home" || button_type == "setting") {
                is_clicked = true;
            }
            return true;
        }
    }
    return false;
}

void ButtonObject::Show(SDL_Renderer* des) {
    SDL_Rect renderQuad = { button_rect.x, button_rect.y, button_rect.w, button_rect.h };
    SDL_RenderCopy(des, p_object, NULL, &renderQuad);
}




void ButtonObject::ToggleMusic(SDL_Renderer* renderer, SoundManager* sound_manager, bool& music_state) {
    if (button_type == "music" && is_clicked) {
        music_state = !music_state;
        if (music_state) {
            LoadImg("img//Volume.png", renderer);
            sound_manager->PlayMusic(-1);
        } else {
            LoadImg("img//VolumeMute.png", renderer);
            sound_manager->PauseMusic();
        }
        is_clicked = false; // Reset trạng thái clicked sau khi xử lý
    }
}
