#ifndef ITEM_OBJECT_H
#define ITEM_OBJECT_H

#include "CommonFunc.h"
#include "BaseObject.h"
#include <string>

#define MAX_FRAME_ITEM_CHERRIES 17 // Số frame cho Cherries
#define MAX_FRAME_ITEM_CUP 8       // Số frame cho Cup
#define FRAME_DELAY_MS_ITEM 100    // Thời gian mỗi frame (ms)

class ItemObject : public BaseObject
{
public:
    enum ItemType
    {
        CHERRIES = 0, // Tăng điểm
        CUP = 1       // Hồi mạng
    };

    ItemObject();
    ~ItemObject();

    // Tải spritesheet cho vật phẩm
    bool LoadImg(std::string path, SDL_Renderer* screen, ItemType type);

    // Hiển thị vật phẩm với animation
    void Show(SDL_Renderer* des);

    // Thiết lập loại vật phẩm
    void SetItemType(ItemType type) { item_type = type; }
    ItemType GetItemType() const { return item_type; }

    // Thiết lập vị trí vật phẩm
    void SetPos(float x, float y) { x_pos = x; y_pos = y; }

    // Lấy vị trí và kích thước khung hình
    SDL_Rect GetRectFrame();

    // Thiết lập tọa độ bản đồ
    void SetMapXY(int map_x_, int map_y_) { map_x = map_x_; map_y = map_y_; }

    // Kiểm tra xem vật phẩm có còn tồn tại không
    bool IsActive() const { return is_active; }
    void SetActive(bool active) { is_active = active; }

private:
    float x_pos; // Vị trí X
    float y_pos; // Vị trí Y
    int width_frame; // Chiều rộng frame
    int height_frame; // Chiều cao frame
    int frame; // Frame hiện tại
    SDL_Rect frame_clip[17]; // Mảng frame, sử dụng số frame lớn nhất (17)
    ItemType item_type; // Loại vật phẩm
    Uint32 last_frame_time; // Thời gian frame cuối
    bool is_active; // Trạng thái hoạt động
    int map_x; // Tọa độ bản đồ X
    int map_y; // Tọa độ bản đồ Y
    int num_frames; // Số frame thực tế dựa trên loại vật phẩm

    // Thiết lập các clip cho spritesheet
    void SetClips();
};

#endif // ITEM_OBJECT_H
