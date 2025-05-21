#ifndef PLAYER_OBJECT_H
#define PLAYER_OBJECT_H

#include "CommonFunc.h"
#include "BaseObject.h"
#include "GameMap.h"
#include "BulletObject.h"
#include "SoundManager.h"
#include <iostream>
#include <vector>
#include <map>
#include <SDL_keycode.h>

#define MAX_FRAME 6         // Idle: 6 frames
#define MAX_FRAME_JUMP 12   // Jump: 12 frames
#define MAX_FRAME_ATTACK_2 4 // Attack_2: 4 frames
#define MAX_FRAME_ATTACK_3 3 // Attack_3: 3 frames
#define MAX_FRAME_HIT 2        // HIT: 2 frames
#define MAX_FRAME_DIE 3        // Die: 3 frames
#define PLAYER_JUMP_VAL 14
#define GRAVITY_SPEED 20
#define MAX_FALL_SPEED 10
#define PLAYER_SPEED 8
#define FRAME_DELAY_MS 100  // Thời gian mỗi frame (ms), điều chỉnh để animation mượt hơn
#define INVULNERABLE_TIME 1000
#define RESPAWN_DELAY 1000

class PlayerObject : public BaseObject
{
public:
    PlayerObject();
    ~PlayerObject();

    enum WalkType
    {
        WALK_RIGHT = 0,
        WALK_LEFT = 1,
        IDLE_RIGHT = 2,
        IDLE_LEFT = 3,
        ATTACK_RIGHT = 4,
        ATTACK_LEFT = 5,
        HIT_RIGHT = 6,
        HIT_LEFT = 7,
        DIE_RIGHT = 8,
        DIE_LEFT = 9
    };

    bool LoadImg(std::string path, SDL_Renderer *screen);
    bool LoadImgJump(std::string path, SDL_Renderer *screen);
    bool LoadImgAttack(std::string path, SDL_Renderer *screen, int attack_type = 2);
    bool LoadImgHit(std::string path, SDL_Renderer *screen);
    bool LoadImgDie(std::string path, SDL_Renderer *screen);
    void PreloadTextures(SDL_Renderer *screen); // Hàm mới để tải trước texture
    void SwitchTexture(const std::string& key, int num_frames); // Hàm mới để chuyển texture

    void show(SDL_Renderer *des);
    void HandelInputAction(SDL_Event event, SDL_Renderer *screen, SoundManager* sound_manager);
    void set_clips();
    void DoPlayer(Map &map_data);
    void CheckToMap(Map &map_data);
    void CenterEnityOnMap(Map &map_data);
    SDL_Rect GetRectFrame();
    void set_bullet_list(std::vector<BulletObject *> bullet_list) { p_bullet_list = bullet_list; }
    std::vector<BulletObject *> get_bullet_list() const { return p_bullet_list; }
    void HandleBullet(SDL_Renderer *des);
    void removeBullet(const int &idx);
    void setMapXY(const int map_x_, const int map_y_) { map_x = map_x_; map_y = map_y_; }
    void ResetPlayer();
    void set_check_time_skill(bool time_skill_) { check_time_skill = time_skill_; }
    bool get_check_time_skill() const { return check_time_skill; }
    void set_num_lives(int lives) { num_lives = lives; }
    int get_num_lives() const { return num_lives; }
    void decrease_life() { if (num_lives > 0) num_lives--; }
    bool is_invulnerable() const { return invulnerable; }
    void set_invulnerable(bool inv) { invulnerable = inv; }
    Uint32 get_invulnerable_time() const { return invulnerable_start_time; }
    void set_invulnerable_time(Uint32 time) { invulnerable_start_time = time; }
    bool is_dead() const { return is_dead_; }
    void set_dead(bool dead) { is_dead_ = dead; }
    Uint32 get_death_time() const { return death_start_time; }
    void set_death_time(Uint32 time) { death_start_time = time; }
    void set_status(int s) { status = s; }
    int get_status() const { return status; }
    int GetMoneyCount() const { return money_count; }
    void IncreaseMoney(int amount) { money_count += amount; }

private:
    int money_count;
    std::vector<BulletObject *> p_bullet_list;
    float x_val;
    float y_val;
    float x_pos;
    float y_pos;
    int num_frame;
    int width_frame;
    int height_frame;
    SDL_Rect frame_clip[12];
    Input input_type;
    int frame;
    int status;
    bool on_ground;
    int map_x;
    int map_y;
    bool is_attacking;
    bool check_time_skill;
    Uint32 last_frame_time;
    int num_lives;
    bool invulnerable;
    Uint32 invulnerable_start_time;
    bool is_dead_;
    Uint32 death_start_time;
    std::map<std::string, std::pair<SDL_Texture*, SDL_Rect>> texture_cache; // Bộ nhớ cache texture
};

#endif // PLAYER_OBJECT_H
