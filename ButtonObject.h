#ifndef BUTTON_OBJECT_H_
#define BUTTON_OBJECT_H_

#include "BaseObject.h"
#include <SDL.h>
#include <string>

class SoundManager; //khai báo để tránh lỗi biên dịch
class ButtonObject : public BaseObject {
public:
    ButtonObject();
    ~ButtonObject();

    void SetRect(const int& x, const int& y, const int& w, const int& h, const std::string& type = "");

    bool CheckClick(const SDL_Event& e);
    bool IsClicked() const { return is_clicked; }

    void ResetClicked() { is_clicked = false; } // Reset trạng thái clicked

    std::string GetType() const { return button_type; }

    void Show(SDL_Renderer* des);

    // Hàm để xử lý trạng thái âm thanh
    void ToggleMusic(SDL_Renderer* renderer, SoundManager* sound_manager, bool& music_state);

private:
    SDL_Rect button_rect;
    bool is_clicked;
    std::string button_type;
};

#endif
