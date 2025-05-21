#include "GameMap.h"

TileMat:: TileMat(){}
TileMat:: ~TileMat(){}

GameMap::GameMap() {

}

GameMap::~GameMap() {

}
void GameMap::LoadMap(char* name) { // gán giá trị cho một ô
    FILE* fp = NULL;
    fopen_s(&fp, name, "rb");
    if (fp == NULL) {
        return;
    }

    game_map.max_x = 0;
    game_map.max_y = 0;

    for (int i = 0; i < MAX_MAP_Y; i++) {
        for (int j = 0; j < MAX_MAP_X; j++) {
            fscanf(fp, "%d", &game_map.tile[i][j]); // doc so nguyen va luu vao mang
            int val = game_map.tile[i][j];
            if (val > 0) {
                if (j > game_map.max_x) game_map.max_x = j;
                if (i > game_map.max_y) game_map.max_y = i;
            }
        }
    }

    game_map.max_x = (game_map.max_x + 1) * TILE_SIZE; // tinh kich thuoc thuc te cua ban do
    game_map.max_y = (game_map.max_y + 1) * TILE_SIZE;

    game_map.start_x = 0;
    game_map.start_y = 0;
    game_map.file_name = name;

    fclose(fp);
}

void GameMap::LoadTiles(SDL_Renderer* screen) {
    char file_img[30];
    FILE* fp = NULL;

    for (int i = 0; i < MAX_TILES; i++) {
        sprintf_s(file_img, "map/%d.png", i);
        fopen_s(&fp, file_img, "rb"); // kiem tra file anh co ton tai khong

        if (fp == NULL) {
            continue;
        }

        fclose(fp);
        tile_mat[i].LoadImg(file_img, screen);
    }
}

void GameMap::DrawMap(SDL_Renderer* screen) {
    int map_x = game_map.start_x / TILE_SIZE;
    int map_y = game_map.start_y / TILE_SIZE;

    int x1 = -(game_map.start_x % TILE_SIZE);
    int x2 = x1 + SCREEN_WIDTH + (x1 == 0 ? 0 : TILE_SIZE);

    int y1 = -(game_map.start_y % TILE_SIZE);
    int y2 = y1 + SCREEN_HEIGHT + (y1 == 0 ? 0 : TILE_SIZE);

    for (int i = y1; i < y2; i += TILE_SIZE) {
        map_x = game_map.start_x / TILE_SIZE;
        for (int j = x1; j < x2; j += TILE_SIZE) {


                int val = game_map.tile[map_y][map_x];
                if (val > 0) {
                    tile_mat[val].SetRect(j, i);
                    tile_mat[val].Render(screen);
                }

            map_x++;
        }
        map_y++;
    }
}
