#ifndef BASE_OBJECT_H_
#define BASE_OBJECT_H_

#include "CommonFunc.h"

// xây dựng hàm cơ sở để load ảnh các đối tượng
class BaseObject
{
public:
    BaseObject();
    virtual ~BaseObject(); // Thêm hàm hủy ảo
    void SetRect(const int x, const int y, const int w = 0, const int h = 0) {
        rect_.x = x;
        rect_.y = y;
        if (w > 0) rect_.w = w;
        if (h > 0) rect_.h = h;
    }
    SDL_Rect GetRect() const {return rect_;}
    SDL_Texture* GetOBject() {return p_object;}
    virtual bool LoadImg(std :: string path , SDL_Renderer * screen);
    void Render(SDL_Renderer * des , const SDL_Rect *clip = NULL);
    void Free();
protected:
    SDL_Texture* p_object;
    SDL_Rect rect_;
};



#endif // BASE_OBJECT_H_
