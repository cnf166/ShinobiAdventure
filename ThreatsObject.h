#ifndef THREATS_OBJECT_H
#define THREATS_OBJECT_H

#include "CommonFunc.h"
#include "BaseObject.h"
#include "BulletObject.h"
#define THREAT_SPEED 3
#define THREAT_GRAVITY_SPEED 0.8
#define THREAT_MAX_FALL_SPEED 10
#define THREAT_FRAME_NUM 10
#define THREAT_FRAME_FLYING 7
#define BOSS_FRAME_NUM 6
#define THREAT_BOSS_SPEED 1.5
class ThreatsObject : public BaseObject
{
public:
    ThreatsObject();
    ~ThreatsObject();


    enum TypeMove
    {
        STATIC_THREAT = 0,
        MOVE_IN_SPACE_THREAT =1,
        MOVE_LEFT = 2,
        FLYING_THREAT = 3,
        MOVE_RIGHT = 4,
    };
    // toc do di chuyen cua ke dich
    void set_x_val(const float& xVal){x_val = xVal;}
    void set_y_val(const float& yVal ){y_val = yVal;}
    // cai dat toa do cho ke dich
    void set_x_pos(const float& xp){x_pos = xp;}
    void set_y_pos(const float& yp){y_pos = yp;}
    float get_x_pos() const {return x_pos;}
    float get_y_pos() const {return y_pos;}

    //xét tọa độ map của kẻ địch
    void SetMapXY(const int& mp_x , const int& mp_y){map_x = mp_x; map_y = mp_y;}

    void set_clips(); // chia frame
    //hàm load ảnh kẻ địch
    bool LoadImg(std :: string path , SDL_Renderer * screen);
    bool LoadImgFlying(std :: string path , SDL_Renderer* screen);
    bool LoadImgBoss(std :: string path , SDL_Renderer* screen);


    void show(SDL_Renderer * des);
    int get_width_frame() const {return width_frame;}
    int get_height_frame() const {return height_frame;}
    void DoPlayer(Map& gMap);
    void checkToMap(Map& gMap);

    SDL_Rect GetRectFrame();

    void set_type_move(const int& typeMove){type_move = typeMove;}
    int get_type_move() const { return type_move; }
    void set_input_left(const int& ipleft ){input_type.left = ipleft;}
    void set_input_right(const int& ipright){input_type.right = ipright;}
    void ImpMoveType(SDL_Renderer * screen); // trạng thái di chuyển của nhân vật

    std :: vector<BulletObject*> get_bullet_list()const{return bullet_list;}
    void set_bullet_list(const std:: vector<BulletObject*>& bl_list){bullet_list = bl_list;} // tạo một danh sách quản lý đạn
    void InitBullet(BulletObject* p_bullet , SDL_Renderer *screen); // khởi tạo đạn
    void MakeBullet(SDL_Renderer * screen , const int& x_limit , const int& y_limit); // giới hạn đạn di chuyển trong một phạm vi nhất định
    void removeBullet(const int& idx); // xóa đạn
    void ResetBullet(SDL_Renderer* screen);// reset lại đạn

private:
    int frame;
    int width_frame;
    int height_frame;
    SDL_Rect frame_clip[THREAT_FRAME_NUM];
    int come_back_time;
    bool on_ground;
    float x_pos;
    float y_pos;
    float x_val;
    float y_val;
    int map_x;
    int map_y;
    int direction;
    int type_move;
    Input input_type;
    int num_frame;

    std :: vector<BulletObject*> bullet_list;

};




#endif // THREATS_OBJECT_H
