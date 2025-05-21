#include "BulletObject.h"

BulletObject::BulletObject()
{
    x_val = 0;
    y_val = 0;
    is_move = false;
    bullet_type = SPHERE_BULLET;
    effect_timer = 0;
}

BulletObject::~BulletObject()
{
}

bool BulletObject::LoadImageBullet(SDL_Renderer* des)
{
    bool res = false;
    if (bullet_type == LASER_BULLET)
    {
        res = LoadImg("img//laser_bullet.png", des);
    }
    else if (bullet_type == FIRE_BALL)
    {
        res = LoadImg("img//fire_ball.png", des); // Mặc định sử dụng fire_ball.png
        if (res)
        {
            rect_.w = 24; // Kích thước cố định
            rect_.h = 16;
        }
    }
    else
    {
        res = LoadImg("img//dan_ban.png", des);
    }
    return res;
}

bool BulletObject::LoadImageSkill(std::string path, SDL_Renderer* des)
{
    bool res = BaseObject::LoadImg(path, des);
    return res;
}

void BulletObject::HandleMove(const int& x_border, const int& y_border)
{
    if (bullet_dir == DIR_RIGHT)
    {
        rect_.x += x_val;
        if (rect_.x > x_border)
        {
            is_move = false;
        }
    }
    else if (bullet_dir == DIR_LEFT)
    {
        rect_.x -= x_val;
        if (rect_.x < 0)
        {
            is_move = false;
        }
    }
    else if (bullet_dir == DIR_ALL)
    {
        effect_timer++;
        if (effect_timer >= 3)
        {
            is_move = false;
            effect_timer = 0;
        }
    }
}
