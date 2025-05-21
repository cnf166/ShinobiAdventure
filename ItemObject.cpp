#include "ItemObject.h"

ItemObject::ItemObject()
{
    x_pos = 0;
    y_pos = 0;
    width_frame = 0;
    height_frame = 0;
    frame = 0;
    last_frame_time = 0;
    item_type = CHERRIES;
    is_active = true;
    map_x = 0;
    map_y = 0;
    num_frames = 0;
}

ItemObject::~ItemObject()
{
}

bool ItemObject::LoadImg(std::string path, SDL_Renderer* screen, ItemType type)
{
    bool ret = BaseObject::LoadImg(path, screen);
    if (ret)
    {
        num_frames = (type == CHERRIES) ? 17 : 8; // Cherries: 17 frames, Cup: 8 frames
        width_frame = (type == CHERRIES) ? 32 : 64; // Cherries: 32px, Cup: 64px
        height_frame = (type == CHERRIES) ? 32 : 64; // Cherries: 32px, Cup: 64px
        rect_.w = width_frame * num_frames; // Total sprite sheet width
        rect_.h = height_frame;
        SetClips();
    }
    return ret;
}

void ItemObject::SetClips()
{
    if (width_frame > 0 && height_frame > 0)
    {
        for (int i = 0; i < num_frames; i++)
        {
            frame_clip[i].x = i * width_frame;
            frame_clip[i].y = 0;
            frame_clip[i].w = width_frame;
            frame_clip[i].h = height_frame;
        }
    }
}

void ItemObject::Show(SDL_Renderer* des)
{
    if (!is_active)
        return;

    Uint32 current_time = SDL_GetTicks();
    if (current_time - last_frame_time >= FRAME_DELAY_MS_ITEM)
    {
        frame++;
        if (frame >= num_frames)
            frame = 0;
        last_frame_time = current_time;
    }

    rect_.x = x_pos - map_x;
    rect_.y = y_pos - map_y;

    SDL_Rect* current_clip = &frame_clip[frame];
    SDL_Rect renderQuad = { rect_.x, rect_.y, width_frame, height_frame };
    SDL_RenderCopy(des, p_object, current_clip, &renderQuad);
}

SDL_Rect ItemObject::GetRectFrame()
{
    SDL_Rect rect;
    rect.x = x_pos - map_x; // Adjust for map offset
    rect.y = y_pos - map_y;
    rect.w = width_frame;
    rect.h = height_frame;
    return rect;
}
