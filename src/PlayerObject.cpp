#include "PlayerObject.h"


PlayerObject::PlayerObject()
{
    frame = 0;
    x_pos = 0;
    y_pos = 0;
    x_val = 0;
    y_val = 0;
    width_frame = 0;
    height_frame = 0;
    num_frame = 0;
    status = IDLE_RIGHT;
    input_type.down = 0;
    input_type.left = 0;
    input_type.right = 0;
    input_type.up = 0;
    on_ground = false;
    map_x = 0;
    map_y = 0;
    check_time_skill = false;
    is_attacking = false;
    money_count = 0;
    last_frame_time = 0;
    num_lives = 3;
    invulnerable = false;
    invulnerable_start_time = 0;
    is_dead_ = false;
    death_start_time = 0;
}

PlayerObject::~PlayerObject()
{
    for (auto& pair : texture_cache) {
        if (pair.second.first) {
            SDL_DestroyTexture(pair.second.first);
        }
    }
    texture_cache.clear();
}

void PlayerObject::PreloadTextures(SDL_Renderer* screen)
{
    std::vector<std::pair<std::string, int>> textures = {
        {"Idle_Right", MAX_FRAME},
        {"Idle_Left", MAX_FRAME},
        {"Run_Right", 8},
        {"Run_Left", 8},
        {"Jump_Right", MAX_FRAME_JUMP},
        {"Jump_Left", MAX_FRAME_JUMP},
        {"Attack_2_Right", MAX_FRAME_ATTACK_2},
        {"Attack_2_Left", MAX_FRAME_ATTACK_2},
        {"Attack_3_Right", MAX_FRAME_ATTACK_3},
        {"Attack_3_Left", MAX_FRAME_ATTACK_3},
        {"Hit_Right", MAX_FRAME_HIT},
        {"Hit_Left", MAX_FRAME_HIT},
        {"Die_Right", MAX_FRAME_DIE},
        {"Die_Left", MAX_FRAME_DIE}
    };

    for (const auto& tex : textures) {
        std::string path = "img//" + tex.first + ".png";
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            SDL_Log("Không thể tải %s: %s", path.c_str(), IMG_GetError());
            continue;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(screen, surface);
        SDL_Rect rect = {0, 0, surface->w, surface->h};
        SDL_FreeSurface(surface);
        if (texture) {
            texture_cache[tex.first] = {texture, rect};
        }
    }
}

void PlayerObject::SwitchTexture(const std::string& key, int num_frames)
{
    auto it = texture_cache.find(key);
    if (it != texture_cache.end()) {
        p_object = it->second.first;
        rect_ = it->second.second;
        num_frame = num_frames;
        width_frame = rect_.w / num_frame;
        height_frame = rect_.h;
        set_clips();
    }
}

bool PlayerObject::LoadImg(std::string path, SDL_Renderer* screen)
{
    bool ret = BaseObject::LoadImg(path, screen);
    if (ret == true)
    {
        num_frame = (path.find("Idle") != std::string::npos) ? MAX_FRAME : (path.find("Run") != std::string::npos) ? 8 : num_frame;
        width_frame = rect_.w / num_frame;
        height_frame = rect_.h;
        set_clips();
    }
    return ret;
}

bool PlayerObject::LoadImgJump(std::string path, SDL_Renderer* screen)
{
    bool ret = BaseObject::LoadImg(path, screen);
    if (ret == true)
    {
        num_frame = MAX_FRAME_JUMP;
        width_frame = rect_.w / MAX_FRAME_JUMP;
        height_frame = rect_.h;
        set_clips();
    }
    return ret;
}

bool PlayerObject::LoadImgAttack(std::string path, SDL_Renderer* screen, int attack_type)
{
    bool ret = BaseObject::LoadImg(path, screen);
    if (ret == true)
    {
        num_frame = (attack_type == 2) ? MAX_FRAME_ATTACK_2 : MAX_FRAME_ATTACK_3;
        width_frame = rect_.w / num_frame;
        height_frame = rect_.h;
        set_clips();
    }
    return ret;
}

bool PlayerObject::LoadImgHit(std::string path, SDL_Renderer* screen)
{
    bool ret = BaseObject::LoadImg(path, screen);
    if (ret == true)
    {
        num_frame = MAX_FRAME_HIT;
        width_frame = rect_.w / MAX_FRAME_HIT;
        height_frame = rect_.h;
        set_clips();
    }
    return ret;
}

bool PlayerObject::LoadImgDie(std::string path, SDL_Renderer* screen)
{
    bool ret = BaseObject::LoadImg(path, screen);
    if (ret == true)
    {
        num_frame = MAX_FRAME_DIE;
        width_frame = rect_.w / MAX_FRAME_DIE;
        height_frame = rect_.h;
        set_clips();
    }
    return ret;
}

void PlayerObject::set_clips()
{
    if (width_frame > 0 && height_frame > 0)
    {
        for (int i = 0; i < num_frame; i++)
        {
            frame_clip[i].x = i * width_frame;
            frame_clip[i].y = 0;
            frame_clip[i].w = width_frame;
            frame_clip[i].h = height_frame;
        }
    }
}

SDL_Rect PlayerObject::GetRectFrame()
{
    SDL_Rect rect;
    rect.x = rect_.x;
    rect.y = rect_.y;
    rect.w = width_frame;
    rect.h = height_frame;
    return rect;
}

void PlayerObject::HandelInputAction(SDL_Event event, SDL_Renderer* screen, SoundManager* sound_manager)
{
    if (is_dead_) return;
    if (event.type == SDL_KEYDOWN)
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_d:
        {
            if (!is_attacking)
            {
                status = WALK_RIGHT;
                input_type.right = 1;
                input_type.left = 0;
                if (on_ground == true)
                {
                    SwitchTexture("Run_Right", 8);
                }
                else
                {
                    SwitchTexture("Jump_Right", MAX_FRAME_JUMP);
                }
            }
        }
        break;
        case SDLK_a:
        {
            if (!is_attacking)
            {
                status = WALK_LEFT;
                input_type.left = 1;
                input_type.right = 0;
                if (on_ground == true)
                {
                    SwitchTexture("Run_Left", 8);
                }
                else
                {
                    SwitchTexture("Jump_Left", MAX_FRAME_JUMP);
                }
            }
        }
        break;
        case SDLK_w:
        {
            if (!is_attacking && on_ground)
            {
                input_type.up = 1;
                if (input_type.right == 1)
                    SwitchTexture("Jump_Right", MAX_FRAME_JUMP);
                else
                    SwitchTexture("Jump_Left", MAX_FRAME_JUMP);
            }
        }
        break;
        case SDLK_j:
        {
            if (!is_attacking && on_ground == true)
            {
                sound_manager->PlaySoundEffect("def"); // Phát Def.mp3
                BulletObject* p_bullet = new BulletObject();
                p_bullet->LoadImageSkill("img//defensive_skill.png", screen);
                p_bullet->set_bullet_type(BulletObject::DEFENSIVE_SKILL);
                p_bullet->set_bullet_dir(BulletObject::DIR_ALL);
                int effect_size = 250;
                p_bullet->SetRect(this->rect_.x - (effect_size - width_frame) / 2,
                                  this->rect_.y - (effect_size - height_frame) / 2);
                if (status == IDLE_RIGHT || status == WALK_RIGHT)
                {
                    status = ATTACK_RIGHT;
                    input_type.right = 0;
                    input_type.left = 0;
                    SwitchTexture("Attack_2_Right", MAX_FRAME_ATTACK_2);
                    frame = 0;
                    is_attacking = true;
                }
                else if (status == IDLE_LEFT || status == WALK_LEFT)
                {
                    status = ATTACK_LEFT;
                    input_type.right = 0;
                    input_type.left = 0;
                    SwitchTexture("Attack_2_Left", MAX_FRAME_ATTACK_2);
                    frame = 0;
                    is_attacking = true;
                }
                p_bullet->set_is_move(true);
                p_bullet_list.push_back(p_bullet);
            }
        }
        break;
        case SDLK_k:
        {
            if (check_time_skill == false)
            {
                if (!is_attacking)
                {
                    sound_manager->PlaySoundEffect("attack"); // Phát Attack.mp3
                    BulletObject* p_bullet = new BulletObject();
                    if (status == IDLE_RIGHT || status == WALK_RIGHT)
                    {
                        p_bullet->LoadImageSkill("img//tornado.png", screen);
                        p_bullet->set_bullet_type(BulletObject::EFFECT_SKILL);
                        p_bullet->set_bullet_dir(BulletObject::DIR_RIGHT);
                        p_bullet->SetRect(this->rect_.x + width_frame - 20, this->rect_.y + height_frame * 0.5);
                        status = ATTACK_RIGHT;
                        input_type.right = 0;
                        input_type.left = 0;
                        SwitchTexture("Attack_3_Right", MAX_FRAME_ATTACK_3);
                        frame = 0;
                        is_attacking = true;
                    }
                    else if (status == IDLE_LEFT || status == WALK_LEFT)
                    {
                        p_bullet->LoadImageSkill("img//tornado.png", screen);
                        p_bullet->set_bullet_type(BulletObject::EFFECT_SKILL);
                        p_bullet->set_bullet_dir(BulletObject::DIR_LEFT);
                        p_bullet->SetRect(this->rect_.x + 20, this->rect_.y + height_frame * 0.5);
                        status = ATTACK_LEFT;
                        input_type.right = 0;
                        input_type.left = 0;
                        SwitchTexture("Attack_3_Left", MAX_FRAME_ATTACK_3);
                        frame = 0;
                        is_attacking = true;
                    }
                    p_bullet->set_x_val(20);
                    p_bullet->set_is_move(true);
                    p_bullet_list.push_back(p_bullet);
                }
                check_time_skill = true;
            }
        }
        break;
        }
    }
    else if (event.type == SDL_KEYUP)
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_d:
        {
            if (!is_attacking)
            {
                status = IDLE_RIGHT;
                input_type.right = 0;
                SwitchTexture("Idle_Right", MAX_FRAME);
            }
        }
        break;
        case SDLK_a:
        {
            if (!is_attacking)
            {
                status = IDLE_LEFT;
                input_type.left = 0;
                SwitchTexture("Idle_Left", MAX_FRAME);
            }
        }
        break;
        }
    }
}

void PlayerObject::DoPlayer(Map& map_data)
{
    if (is_dead_) return;
    if (x_pos == 0 && y_pos == 0)
    {
        x_pos = SCREEN_WIDTH / 2;
        int tile_y = (SCREEN_HEIGHT / TILE_SIZE) - 1;
        int ground_y = tile_y * TILE_SIZE;
        y_pos = ground_y - height_frame;
    }
    x_val = 0;
    y_val += 0.8;
    if (y_val >= MAX_FALL_SPEED)
    {
        y_val = MAX_FALL_SPEED;
    }
    if (input_type.left == 1)
    {
        x_val -= PLAYER_SPEED;
    }
    if (input_type.right == 1)
    {
        x_val += PLAYER_SPEED;
    }
    if (input_type.up == 1 && on_ground)
    {
        y_val = -PLAYER_JUMP_VAL;
        on_ground = false;
        input_type.up = 0;
    }
    CheckToMap(map_data);
    CenterEnityOnMap(map_data);
}

void PlayerObject::CheckToMap(Map& map_data)
{
    int x1 = 0;
    int x2 = 0;
    int y1 = 0;
    int y2 = 0;

    int width_min = width_frame < TILE_SIZE ? width_frame : TILE_SIZE;
    x1 = x_pos / TILE_SIZE;
    x2 = (x_pos + width_min) / TILE_SIZE;
    y1 = (y_pos + y_val) / TILE_SIZE;
    y2 = (y_pos + y_val + height_frame - 1) / TILE_SIZE;

    if (x1 >= 0 && x2 < MAX_MAP_X && y1 >= 0 && y2 < MAX_MAP_Y)
    {
        if (y_val > 0)
        {
            if (map_data.tile[y2][x1] != BLANK_TILE || map_data.tile[y2][x2] != BLANK_TILE)
            {
                y_pos = y2 * TILE_SIZE - (height_frame + 1);
                y_val = 0;
                on_ground = true;
            }
        }
        else if (y_val < 0)
        {
            if (map_data.tile[y1][x1] != BLANK_TILE || map_data.tile[y1][x2] != BLANK_TILE)
            {
                y_pos = (y1 + 1) * TILE_SIZE;
                y_val = 0;
            }
        }
    }

    x_pos += x_val;
    y_pos += y_val;

    if (x_pos < 0)
        x_pos = 0;
    else if (x_pos + width_frame > map_data.max_x)
        x_pos = map_data.max_x - width_frame - 1;
}

void PlayerObject::CenterEnityOnMap(Map& map_data)
{
    map_data.start_x = x_pos - (SCREEN_WIDTH / 3);
    map_data.start_y = y_pos - (SCREEN_HEIGHT / 2);

    if (map_data.start_x < 0)
    {
        map_data.start_x = 0;
    }
    else if (map_data.start_x + SCREEN_WIDTH > map_data.max_x)
    {
        map_data.start_x = map_data.max_x - SCREEN_WIDTH;
    }

    if (map_data.start_y < 0)
    {
        map_data.start_y = 0;
    }
    else if (map_data.start_y + SCREEN_HEIGHT > map_data.max_y)
    {
        map_data.start_y = map_data.max_y - SCREEN_HEIGHT;
    }

    if (map_data.max_x < SCREEN_WIDTH)
    {
        map_data.start_x = 0;
    }
    if (map_data.max_y < SCREEN_HEIGHT)
    {
        map_data.start_y = 0;
    }
}

void PlayerObject::show(SDL_Renderer* des)
{
    int old_height_frame = height_frame;
    Uint32 current_time = SDL_GetTicks();

    if (invulnerable && current_time - invulnerable_start_time >= INVULNERABLE_TIME)
    {
        invulnerable = false;
        if (status == HIT_RIGHT)
        {
            status = IDLE_RIGHT;
            SwitchTexture("Idle_Right", MAX_FRAME);
        }
        else if (status == HIT_LEFT)
        {
            status = IDLE_LEFT;
            SwitchTexture("Idle_Left", MAX_FRAME);
        }
    }

    if (is_dead_ && current_time - death_start_time >= RESPAWN_DELAY)
    {
        is_dead_ = false;
        if (status == DIE_RIGHT)
        {
            status = IDLE_RIGHT;
            SwitchTexture("Idle_Right", MAX_FRAME);
        }
        else if (status == DIE_LEFT)
        {
            status = IDLE_LEFT;
            SwitchTexture("Idle_Left", MAX_FRAME);
        }
        invulnerable = true;
        invulnerable_start_time = current_time;
    }

    if (current_time - last_frame_time >= FRAME_DELAY_MS)
    {
        if (input_type.left == 1 || input_type.right == 1 || status == ATTACK_LEFT || status == ATTACK_RIGHT ||
            status == IDLE_LEFT || status == IDLE_RIGHT || status == HIT_LEFT || status == HIT_RIGHT ||
            status == DIE_LEFT || status == DIE_RIGHT)
        {
            frame++;
            last_frame_time = current_time;
        }
    }

    if (frame >= num_frame)
    {
        if (status == ATTACK_LEFT)
        {
            status = IDLE_LEFT;
            SwitchTexture("Idle_Left", MAX_FRAME);
            is_attacking = false;
        }
        else if (status == ATTACK_RIGHT)
        {
            status = IDLE_RIGHT;
            SwitchTexture("Idle_Right", MAX_FRAME);
            is_attacking = false;
        }
        else if (status == HIT_LEFT && !is_dead_)
        {
            status = IDLE_LEFT;
            SwitchTexture("Idle_Left", MAX_FRAME);
        }
        else if (status == HIT_RIGHT && !is_dead_)
        {
            status = IDLE_RIGHT;
            SwitchTexture("Idle_Right", MAX_FRAME);
        }
        if (on_ground && !is_attacking && !is_dead_)
        {
            if (input_type.right == 1)
            {
                status = WALK_RIGHT;
                SwitchTexture("Run_Right", 8);
            }
            else if (input_type.left == 1)
            {
                status = WALK_LEFT;
                SwitchTexture("Run_Left", 8);
            }
        }
        int new_height_frame = height_frame;
        if (on_ground)
        {
            y_pos += (old_height_frame - new_height_frame);
        }
        frame = 0;
    }

    rect_.x = x_pos - map_x;
    rect_.y = y_pos - map_y;

    SDL_Rect* current_clip = &frame_clip[frame];
    SDL_Rect renderQuad = { rect_.x, rect_.y, width_frame, height_frame };
    SDL_RenderCopy(des, p_object, current_clip, &renderQuad);
}

void PlayerObject::HandleBullet(SDL_Renderer* des)
{
    for (size_t i = 0; i < p_bullet_list.size(); i++)
    {
        BulletObject* p_bullet = p_bullet_list.at(i);
        if (p_bullet->get_is_move() == true)
        {
            if (p_bullet->get_bullet_type() == BulletObject::EFFECT_SKILL)
            {
                p_bullet->HandleMove(SCREEN_WIDTH, SCREEN_HEIGHT);
            }
            else if (p_bullet->get_bullet_type() == BulletObject::DEFENSIVE_SKILL)
            {
                p_bullet->HandleMove(SCREEN_WIDTH, SCREEN_HEIGHT);
            }
            else
            {
                p_bullet->HandleMove(500, SCREEN_HEIGHT);
            }
            p_bullet->Render(des);
        }
        else
        {
            p_bullet_list.erase(p_bullet_list.begin() + i);
            if (p_bullet != NULL)
            {
                delete p_bullet;
                p_bullet = NULL;
            }
        }
    }
}

void PlayerObject::removeBullet(const int& idx)
{
    int size = p_bullet_list.size();
    if (size > 0 && idx < size)
    {
        BulletObject* p_bullet = p_bullet_list.at(idx);
        p_bullet_list.erase(p_bullet_list.begin() + idx);
        if (p_bullet)
        {
            delete p_bullet;
            p_bullet = NULL;
        }
    }
}

void PlayerObject::ResetPlayer()
{
    x_val = 0;
    y_val = 0;
    x_pos = 0;
    y_pos = 0;
    input_type.right = 0;
    input_type.left = 0;
    for (size_t i = 0; i < p_bullet_list.size(); i++)
    {
        BulletObject* p_bullet = p_bullet_list.at(i);
        if (p_bullet)
        {
            delete p_bullet;
            p_bullet = NULL;
        }
    }
    p_bullet_list.clear();
    map_x = 0;
    map_y = 0;
    check_time_skill = false;
    last_frame_time = 0;
    num_lives = 3;
    invulnerable = false;
    invulnerable_start_time = 0;
    is_dead_ = false;
    death_start_time = 0;
    status = IDLE_RIGHT;
}
