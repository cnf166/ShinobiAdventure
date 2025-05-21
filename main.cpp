#include "CommonFunc.h"
#include "GameMap.h"
#include "Time.h"
#include "PlayerObject.h"
#include "ThreatsObject.h"
#include "TextObject.h"
#include "ButtonObject.h"
#include "SoundManager.h"
#include "SliderObject.h"
#include "ItemObject.h"
#include "Geometric.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <fstream>

BaseObject loadBackground;
BaseObject loadGameOver;
BaseObject shinobi_adventure;
BaseObject start_text_img;
BaseObject heart_full;
BaseObject heart_empty;
TTF_Font* font = NULL;
TextObject time_game;
TextObject volume_text;
TextObject HighScore;
TextObject Score;
TextObject money_text;
ButtonObject music_button;
ButtonObject restart_button;
ButtonObject continue_button;
ButtonObject home_button;
ButtonObject setting_button;
ButtonObject pause_button;
SliderObject volume_slider;
float max_x_pos_fly = 0;
float max_x_pos_dynamic = 0;
int high_score = 0;
int high_money = 0;
int skill_activation_time = 0;
int last_time_val = 0;
Time game_timer;
Uint32 last_item_spawn_time = 0;
const Uint32 ITEM_SPAWN_INTERVAL = 3000;

bool InitData() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        return false;
    }
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) & (IMG_INIT_PNG | IMG_INIT_JPG))) {
        return false;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    const char* title = "Shinobi's Adventure";
    g_window = SDL_CreateWindow(title,
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                SCREEN_WIDTH, SCREEN_HEIGHT,
                                SDL_WINDOW_SHOWN);
    if (!g_window) {
        return false;
    }
    g_screen = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!g_screen) {
        return false;
    }
    SDL_SetRenderDrawColor(g_screen, 255, 255, 255, 255);
    if (TTF_Init() == -1) return false;
    font = TTF_OpenFont("font//dlxfont_.ttf", 15);
    if (font == NULL) {
        return false;
    }
    std::ifstream high_score_file("high_score.txt");
    if (high_score_file.is_open()) {
        high_score_file >> high_money;
        high_score_file.close();
    }
    return true;
}

void SaveHighScore() {
    std::ofstream high_score_file("high_score.txt");
    if (high_score_file.is_open()) {
        high_score_file << high_money;
        high_score_file.close();
    }
}

void close() {
    SaveHighScore();
    loadBackground.Free();
    loadGameOver.Free();
    shinobi_adventure.Free();
    start_text_img.Free();
    heart_full.Free();
    heart_empty.Free();
    time_game.Free();
    volume_text.Free();
    HighScore.Free();
    Score.Free();
    money_text.Free();
    music_button.Free();
    volume_slider.Free();
    continue_button.Free();
    home_button.Free();
    setting_button.Free();
    pause_button.Free();
    TTF_CloseFont(font);
    font = nullptr;
    SDL_DestroyRenderer(g_screen);
    SDL_DestroyWindow(g_window);
    g_window = nullptr;
    g_screen = nullptr;
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

// Hàm vẽ UI (hearts, score, time)
void RenderUI(SDL_Renderer* screen, PlayerObject& p_player, TextObject& time_game, TextObject& money_text, TTF_Font* font, Time& game_timer) {
    int num_lives = p_player.get_num_lives();
    for (int i = 0; i < 3; i++) {
        if (i < num_lives) {
            heart_full.SetRect(10 + i * 42, 0, 32, 32);
            heart_full.Render(screen);
        } else {
            heart_empty.SetRect(10 + i * 42, 0, 32, 32);
            heart_empty.Render(screen);
        }
    }
    int current_time_val = game_timer.get_ticks() / 1000;
    int current_ms = game_timer.get_ticks() % 1000;
    char time_str[32];
    snprintf(time_str, sizeof(time_str), "Time: %d.%03d", current_time_val, current_ms);
    time_game.SetText(time_str);
    time_game.LoadFromRenderText(font, screen);
    time_game.RenderText(screen, SCREEN_WIDTH - time_game.GetWidth() - 10, 10);
    char score_str[32];
    snprintf(score_str, sizeof(score_str), "Score: %d.000", p_player.GetMoneyCount());
    money_text.SetText(score_str);
    money_text.LoadFromRenderText(font, screen);
    money_text.RenderText(screen, (SCREEN_WIDTH - money_text.GetWidth()) / 2, 10);
}

std::vector<ThreatsObject*> MakeThreatList() {
    std::vector<ThreatsObject*> list_threats;
    ThreatsObject* flying_threats = new ThreatsObject[10];
    for (int i = 0; i < 10; i++) {
        ThreatsObject* p_threat = (flying_threats + i);
        if (p_threat != NULL) {
            p_threat->LoadImgFlying("img//Bat.png", g_screen);
            p_threat->set_type_move(ThreatsObject::FLYING_THREAT);
            p_threat->set_x_pos(1200 * (i + 1));
            max_x_pos_fly = std::max(max_x_pos_fly, p_threat->get_x_pos());
            p_threat->set_y_pos(SCREEN_HEIGHT * 0.5);
            p_threat->set_input_left(1);
            BulletObject* p_bullet = new BulletObject();
            p_threat->InitBullet(p_bullet, g_screen);
            list_threats.push_back(p_threat);
        }
    }
    ThreatsObject* dynamic_threats = new ThreatsObject[10];
    for (int i = 0; i < 10; i++) {
        ThreatsObject* p1_threat = (dynamic_threats + i);
        if (p1_threat != NULL) {
            p1_threat->LoadImg("img//Slime.png", g_screen);
            p1_threat->set_type_move(ThreatsObject::MOVE_LEFT);
            p1_threat->set_x_pos(1500 * (i + 1));
            max_x_pos_dynamic = std::max(max_x_pos_dynamic, p1_threat->get_x_pos());
            int tile_y = (SCREEN_HEIGHT / TILE_SIZE) - 1;
            int ground_y = tile_y * TILE_SIZE;
            p1_threat->set_y_pos(ground_y - p1_threat->get_height_frame());
            p1_threat->set_input_left(1);
            BulletObject* p1_bullet = new BulletObject();
            p1_threat->InitBullet(p1_bullet, g_screen);
            list_threats.push_back(p1_threat);
        }
    }
    return list_threats;
}

void SpawnItem(std::vector<ItemObject*>& items_list, int map_x, int map_y, Map& map_data) {
    Uint32 current_time = SDL_GetTicks();
    if (current_time - last_item_spawn_time < ITEM_SPAWN_INTERVAL) return;
    ItemObject* item = new ItemObject();
    if (item != NULL) {
        ItemObject::ItemType type = (rand() % 2 == 0) ? ItemObject::CHERRIES : ItemObject::CUP;
        std::string path = (type == ItemObject::CHERRIES) ? "img//cherries.png" : "img//cup.png";
        item->LoadImg(path, g_screen, type);
        item->SetItemType(type);
        int tile_y = (SCREEN_HEIGHT / TILE_SIZE) - 1;
        int ground_y = tile_y * TILE_SIZE;
        int slime_y = ground_y - 30;
        int bat_height = SCREEN_HEIGHT * 0.5;
        int item_height = (type == ItemObject::CHERRIES) ? 32 : 64;
        float x_pos = map_x + SCREEN_WIDTH + (rand() % 500);
        float y_pos = bat_height + (rand() % (slime_y - bat_height - item_height + 1));
        if (y_pos < bat_height) y_pos = bat_height;
        if (y_pos > slime_y - item_height) y_pos = slime_y - item_height;
        item->SetPos(x_pos, y_pos);
        item->SetMapXY(map_x, map_y);
        items_list.push_back(item);
        last_item_spawn_time = current_time;
    }
}

void ResetGame(std::vector<ThreatsObject*>& threats_list, std::vector<ItemObject*>& items_list, PlayerObject& p_player) {
    for (size_t i = 0; i < threats_list.size(); i++) {
        ThreatsObject* p_threat = threats_list.at(i);
        if (p_threat) {
            p_threat->Free();
            p_threat = NULL;
        }
    }
    threats_list.clear();
    max_x_pos_fly = 0;
    max_x_pos_dynamic = 0;
    threats_list = MakeThreatList();
    for (size_t i = 0; i < items_list.size(); i++) {
        ItemObject* item = items_list.at(i);
        if (item) {
            item->Free();
            item = NULL;
        }
    }
    items_list.clear();
    p_player.ResetPlayer();
    p_player.set_check_time_skill(false);
    p_player.SwitchTexture("Idle_Right", MAX_FRAME);
    skill_activation_time = 0;
    last_item_spawn_time = 0;
    game_timer.start();
}

int main(int argc, char* argv[])
{
    srand(time(NULL));
    bool check_continue = false;
    Time fps_timer;
    game_timer.start();
    if (!InitData()) return -1;

    loadBackground.LoadImg("img//main_background.png", g_screen);
    loadGameOver.LoadImg("img//game_over.png", g_screen);
    shinobi_adventure.LoadImg("img//shinobiadventure.png", g_screen);
    start_text_img.LoadImg("img//startText.png", g_screen);
    heart_full.LoadImg("img//HeartsFull.png", g_screen);
    heart_empty.LoadImg("img//HeartsEmpty.png", g_screen);

    bool music_state = true;
    if (!music_button.LoadImg("img//Volume.png", g_screen)) {
        return -1;
    }
    music_button.SetRect(SCREEN_WIDTH - 31, SCREEN_HEIGHT - 32, 21, 22, "music");

    SoundManager sound_manager;
    if (!sound_manager.Init()) {
        return -1;
    }
    if (!sound_manager.LoadMusic("audio//background_music.mp3")) {
    }
    if (!sound_manager.LoadSoundEffect("audio//button_click.wav", "button_click")) {
    }
    if (!sound_manager.LoadSoundEffect("audio//lose_life.wav", "lose_life")) {
    }
    if (!sound_manager.LoadSoundEffect("audio//game_over.wav", "game_over")) {
    }
    if (!sound_manager.LoadSoundEffect("audio//item_pickup.wav", "item_pickup")) {
    }
    if (!sound_manager.LoadSoundEffect("audio//Attack.mp3", "attack")) {
    }
    if (!sound_manager.LoadSoundEffect("audio//Def.mp3", "def")) {
    }
    sound_manager.PlayMusic(-1);

    volume_slider.SetRect(SCREEN_WIDTH/2 - 50, SCREEN_HEIGHT/2 - 50, 150, 20, 10, 20);
    if (!volume_slider.LoadImg("img//slider.png", g_screen)) {
    }
    volume_text.SetColor(TextObject::WHITE_TEXT);

    enum GameState { START_SCREEN, PLAYING, PAUSED, OPTIONS, GAME_OVER };
    GameState game_state = START_SCREEN;

    if (!continue_button.LoadImg("img//Play.png", g_screen)) {
        return -1;
    }
    if (!home_button.LoadImg("img//Next.png", g_screen)) {
        return -1;
    }
    if (!setting_button.LoadImg("img//Settings.png", g_screen)) {
        return -1;
    }
    if (!restart_button.LoadImg("img//Restart.png", g_screen)) {
        return -1;
    }
    if (!pause_button.LoadImg("img//Previous.png", g_screen)) {
        return -1;
    }

    HighScore.SetColor(TextObject::WHITE_TEXT);
    Score.SetColor(TextObject::WHITE_TEXT);
    money_text.SetColor(TextObject::WHITE_TEXT);

    GameMap loadmap;
    loadmap.LoadMap("map//map01_generated.dat");
    loadmap.LoadTiles(g_screen);
    PlayerObject p_player;
    p_player.PreloadTextures(g_screen);
    p_player.SwitchTexture("Idle_Right", MAX_FRAME);

    std::vector<ThreatsObject*> threats_list = MakeThreatList();
    std::vector<ItemObject*> items_list;

    time_game.SetColor(TextObject::WHITE_TEXT);

    int last_volume = -1;

    bool quit = false;
    while (!quit) {
        fps_timer.start();
        while (SDL_PollEvent(&g_event)) {
            if (g_event.type == SDL_QUIT) quit = true;
            if (g_event.type == SDL_KEYDOWN) {
                if (g_event.key.keysym.sym == SDLK_q) {
                    quit = true;
                }
            }

            if (game_state == START_SCREEN) {
                loadBackground.LoadImg("img//main_background.png", g_screen);
                if (g_event.type == SDL_MOUSEBUTTONDOWN) {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    SDL_Rect startButton = {(SCREEN_WIDTH - 196) / 2, 335, 196, 84};
                    if (x >= startButton.x && x <= startButton.x + startButton.w &&
                        y >= startButton.y && y <= startButton.y + startButton.h) {
                        game_state = PLAYING;
                        sound_manager.PlaySoundEffect("button_click");
                        sound_manager.LoadMusic("audio//battle.mp3");
                        if (music_state) sound_manager.PlayMusic(-1);
                        ResetGame(threats_list, items_list, p_player);
                        high_money = std::max(p_player.GetMoneyCount(), high_money);
                        SaveHighScore();
                    }
                }
                music_button.SetRect(SCREEN_WIDTH - 31, SCREEN_HEIGHT - 32, 21, 22, "music");
                if (music_button.CheckClick(g_event)) {
                    music_button.ToggleMusic(g_screen, &sound_manager, music_state);
                    sound_manager.PlaySoundEffect("button_click");
                }
            } else if (game_state == PLAYING) {
                loadBackground.LoadImg("img//Background.png", g_screen);
                p_player.HandelInputAction(g_event, g_screen, &sound_manager);
                pause_button.SetRect(10, 40, 21, 22, "pause");
                if (pause_button.CheckClick(g_event)) {
                    if (pause_button.IsClicked()) {
                        game_state = PAUSED;
                        game_timer.paused();
                        sound_manager.PauseMusic();
                        pause_button.ResetClicked();
                    }
                }
            } else if (game_state == PAUSED) {
                loadBackground.LoadImg("img//Background.png", g_screen);
                // Điều chỉnh tọa độ buttons: continue_button ở giữa và phía trên setting_button
                continue_button.SetRect(SCREEN_WIDTH / 2 - 15, SCREEN_HEIGHT / 2 - 50, 32, 32, "continue");
                home_button.SetRect(SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 10, 32, 32, "home");
                setting_button.SetRect(SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 + 10, 32, 32, "setting");
                restart_button.SetRect(SCREEN_WIDTH / 2 + 50, SCREEN_HEIGHT / 2 + 10, 32, 32, "restart");
                if (continue_button.CheckClick(g_event) && continue_button.IsClicked()) {
                    check_continue = false;
                    game_state = PLAYING;
                    game_timer.unpaused();
                    continue_button.ResetClicked();
                    if (music_state && sound_manager.IsPaused()) {
                        sound_manager.ResumeMusic();
                    }
                }
                if (restart_button.CheckClick(g_event)) {
                    ResetGame(threats_list, items_list, p_player);
                    game_state = PLAYING;
                    restart_button.ResetClicked();
                    sound_manager.StopMusic();
                    sound_manager.LoadMusic("audio//battle.mp3");
                    if (music_state) sound_manager.PlayMusic(-1);
                    game_timer.start();
                    high_money = std::max(p_player.GetMoneyCount(), high_money);
                    SaveHighScore();
                }
                if (home_button.CheckClick(g_event)) {
                    check_continue = false;
                    game_state = START_SCREEN;
                    game_timer.stop();
                    last_time_val = 0;
                    sound_manager.StopMusic();
                    sound_manager.LoadMusic("audio//background_music.mp3");
                    if (music_state) sound_manager.PlayMusic(-1);
                    home_button.ResetClicked();
                    high_money = std::max(p_player.GetMoneyCount(), high_money);
                    SaveHighScore();
                }
                if (setting_button.CheckClick(g_event)) {
                    check_continue = true;
                    game_state = OPTIONS;
                    setting_button.ResetClicked();
                }
            } else if (game_state == OPTIONS) {
                loadBackground.LoadImg("img//main_background.png", g_screen);
                volume_slider.HandleInput(g_event, &sound_manager);
                home_button.SetRect(SCREEN_WIDTH / 2 - 31, SCREEN_HEIGHT / 2 - 11, 21, 22, "home");
                continue_button.SetRect(SCREEN_WIDTH / 2 + 60, SCREEN_HEIGHT / 2 - 11, 21, 22, "continue");
                if (home_button.CheckClick(g_event)) {
                    game_state = START_SCREEN;
                    home_button.ResetClicked();
                    high_money = std::max(p_player.GetMoneyCount(), high_money);
                    SaveHighScore();
                }
                if (check_continue == true) {
                    if (continue_button.CheckClick(g_event)) {
                        game_state = PAUSED;
                        continue_button.ResetClicked();
                    }
                }
            } else if (game_state == GAME_OVER) {
                loadBackground.LoadImg("img//main_background.png", g_screen);
                loadGameOver.SetRect(SCREEN_WIDTH / 2 - 125, SCREEN_HEIGHT / 2 - 125, 250, 250);
                home_button.SetRect(SCREEN_WIDTH / 2 - 26, 359, 21, 22, "home");
                restart_button.SetRect(SCREEN_WIDTH / 2 + 5, 359, 21, 22, "restart");
                if (home_button.CheckClick(g_event)) {
                    game_state = START_SCREEN;
                    game_timer.stop();
                    last_time_val = 0;
                    sound_manager.StopMusic();
                    sound_manager.LoadMusic("audio//background_music.mp3");
                    if (music_state) sound_manager.PlayMusic(-1);
                    home_button.ResetClicked();
                    high_money = std::max(p_player.GetMoneyCount(), high_money);
                    SaveHighScore();
                }
                if (restart_button.CheckClick(g_event)) {
                    game_state = PLAYING;
                    sound_manager.StopMusic();
                    sound_manager.LoadMusic("audio//battle.mp3");
                    if (music_state) sound_manager.PlayMusic(-1);
                    ResetGame(threats_list, items_list, p_player);
                    restart_button.ResetClicked();
                    high_money = std::max(p_player.GetMoneyCount(), high_money);
                    SaveHighScore();
                }
            }
        }

        SDL_SetRenderDrawColor(g_screen, 255, 255, 255, 255);
        SDL_RenderClear(g_screen);

        loadBackground.Render(g_screen);

        if (game_state == START_SCREEN) {
            shinobi_adventure.SetRect((SCREEN_WIDTH - 690) / 2, 30, 690, 285);
            start_text_img.SetRect((SCREEN_WIDTH - 196) / 2, 335, 196, 84);
            shinobi_adventure.Render(g_screen);
            start_text_img.Render(g_screen);
            music_button.Show(g_screen);
        } else if (game_state == OPTIONS) {
            volume_slider.Show(g_screen);
            int current_volume = sound_manager.GetVolume();
            if (current_volume != last_volume) {
                std::string volume_str = "Volume: " + std::to_string(current_volume * 100 / MIX_MAX_VOLUME) + "%";
                volume_text.SetText(volume_str);
                volume_text.LoadFromRenderText(font, g_screen);
                last_volume = current_volume;
            }
            volume_text.RenderText(g_screen, (SCREEN_WIDTH - volume_text.GetWidth()) / 2, SCREEN_HEIGHT / 2 - 80);
            continue_button.Show(g_screen);
            home_button.Show(g_screen);
        } else if (game_state == GAME_OVER) {
            loadGameOver.Render(g_screen);
            std::string high_Score = "Time High Score: " + std::to_string(high_score);
            HighScore.SetText(high_Score);
            HighScore.LoadFromRenderText(font, g_screen);
            HighScore.RenderText(g_screen, 100, SCREEN_HEIGHT / 2 - 25);
            std::string Score_ = "Time: " + std::to_string(last_time_val);
            Score.SetText(Score_);
            Score.LoadFromRenderText(font, g_screen);
            Score.RenderText(g_screen, 100, SCREEN_HEIGHT / 2 + 5);
            char score_str[32];
            snprintf(score_str, sizeof(score_str), "Score: %d.000", p_player.GetMoneyCount());
            money_text.SetText(score_str);
            money_text.LoadFromRenderText(font, g_screen);
            money_text.RenderText(g_screen, SCREEN_WIDTH - money_text.GetWidth() - 100, SCREEN_HEIGHT / 2 - 25);
            char high_money_str[32];
            snprintf(high_money_str, sizeof(high_money_str), "High Score: %d.000", high_money);
            money_text.SetText(high_money_str);
            money_text.LoadFromRenderText(font, g_screen);
            money_text.RenderText(g_screen, SCREEN_WIDTH - money_text.GetWidth() - 100, SCREEN_HEIGHT / 2 + 5);
            home_button.Show(g_screen);
            restart_button.Show(g_screen);
        } else {
            Map map_data = loadmap.getMap();
            if (game_state == PLAYING) {
                p_player.HandleBullet(g_screen);
                p_player.setMapXY(map_data.start_x, map_data.start_y);
                p_player.DoPlayer(map_data);
                last_time_val = game_timer.get_ticks() / 1000;
                if (p_player.get_check_time_skill()) {
                    if (skill_activation_time == 0) {
                        skill_activation_time = last_time_val;
                    }
                    if (last_time_val >= skill_activation_time + TIME_SKILL_2) {
                        p_player.set_check_time_skill(false);
                        skill_activation_time = 0;
                    }
                }
                SpawnItem(items_list, map_data.start_x, map_data.start_y, map_data);
            }

            loadmap.setMap(map_data);
            loadmap.DrawMap(g_screen);
            p_player.show(g_screen);

            GeometricFormat rectangle_size(0, 0, SCREEN_WIDTH, 40);
            ColorData color_data(36, 36, 36);
            Geometric::RenderRectangle(rectangle_size, color_data, g_screen);
            GeometricFormat outlineSize(1, 1, SCREEN_WIDTH - 1, 38);
            ColorData color_data2(255, 255, 255);
            Geometric::RenderOutline(outlineSize, color_data2, g_screen);

            for (size_t i = 0; i < threats_list.size(); i++) {
                ThreatsObject* p_threat = threats_list.at(i);
                if (p_threat != NULL) {
                    if (p_threat->get_x_pos() < map_data.start_x - TILE_SIZE) {
                        if (p_threat->get_type_move() == ThreatsObject::FLYING_THREAT) {
                            max_x_pos_fly += 1000;
                            if (max_x_pos_fly < 20000 * TILE_SIZE) {
                                p_threat->set_x_pos(max_x_pos_fly);
                                p_threat->ResetBullet(g_screen);
                            } else {
                                p_threat->Free();
                                threats_list.erase(threats_list.begin() + i);
                                --i;
                                continue;
                            }
                        } else {
                            max_x_pos_dynamic += 1500;
                            if (max_x_pos_dynamic < 20000 * TILE_SIZE) {
                                p_threat->set_x_pos(max_x_pos_dynamic);
                                p_threat->ResetBullet(g_screen);
                            } else {
                                p_threat->Free();
                                threats_list.erase(threats_list.begin() + i);
                                --i;
                                continue;
                            }
                        }
                    }
                    p_threat->SetMapXY(map_data.start_x, map_data.start_y);
                    if (game_state == PLAYING) {
                        p_threat->ImpMoveType(g_screen);
                        p_threat->DoPlayer(map_data);
                        p_threat->MakeBullet(g_screen, SCREEN_WIDTH, SCREEN_HEIGHT);
                    }
                    p_threat->show(g_screen);

                    if (game_state == PLAYING) {
                        SDL_Rect rect_player = p_player.GetRectFrame();
                        bool bCol1 = false;
                        std::vector<BulletObject*> tBullet_list = p_threat->get_bullet_list();
                        for (size_t jj = 0; jj < tBullet_list.size(); jj++) {
                            BulletObject* pt_bullet = tBullet_list.at(jj);
                            if (pt_bullet != NULL) {
                                bCol1 = SDLCommonFunc::CheckCollisionPlayer(rect_player, pt_bullet->GetRect());
                                if (bCol1) {
                                    if (p_player.is_invulnerable() || p_player.is_dead()) {
                                    }
                                    p_threat->removeBullet(jj);
                                    break;
                                }
                            }
                        }

                        SDL_Rect rect_threat = p_threat->GetRectFrame();
                        bool bCol2 = SDLCommonFunc::CheckCollisionPlayer(rect_player, rect_threat);
                        if ((bCol1 || bCol2) && !p_player.is_invulnerable() && !p_player.is_dead()) {
                            p_player.decrease_life();
                            sound_manager.PlaySoundEffect("lose_life");
                            if (p_player.get_num_lives() > 0) {
                                if (p_player.get_status() == PlayerObject::IDLE_RIGHT || p_player.get_status() == PlayerObject::WALK_RIGHT) {
                                    p_player.set_status(PlayerObject::HIT_RIGHT);
                                    p_player.SwitchTexture("Hit_Right", MAX_FRAME_HIT);
                                } else {
                                    p_player.set_status(PlayerObject::HIT_LEFT);
                                    p_player.SwitchTexture("Hit_Left", MAX_FRAME_HIT);
                                }
                                p_player.set_invulnerable(true);
                                p_player.set_invulnerable_time(SDL_GetTicks());
                            } else {
                                if (p_player.get_status() == PlayerObject::IDLE_RIGHT || p_player.get_status() == PlayerObject::WALK_RIGHT ||
                                    p_player.get_status() == PlayerObject::HIT_RIGHT) {
                                    p_player.set_status(PlayerObject::DIE_RIGHT);
                                    p_player.SwitchTexture("Die_Right", MAX_FRAME_DIE);
                                } else {
                                    p_player.set_status(PlayerObject::DIE_LEFT);
                                    p_player.SwitchTexture("Die_Left", MAX_FRAME_DIE);
                                }
                                p_player.set_dead(true);
                                p_player.set_death_time(SDL_GetTicks());
                                Uint32 die_start = SDL_GetTicks();
                                while (SDL_GetTicks() - die_start < 1000) {
                                    SDL_RenderClear(g_screen);
                                    loadBackground.Render(g_screen);
                                    loadmap.DrawMap(g_screen);
                                    p_player.show(g_screen);
                                    for (size_t j = 0; j < threats_list.size(); j++) {
                                        threats_list[j]->show(g_screen);
                                    }
                                    for (size_t j = 0; j < items_list.size(); j++) {
                                        items_list[j]->Show(g_screen);
                                    }
                                    SDL_RenderPresent(g_screen);
                                    SDL_Delay(1000 / FRAME_PER_SECOND);
                                }
                                sound_manager.PlaySoundEffect("game_over");
                                sound_manager.StopMusic();
                                sound_manager.LoadMusic("audio//background_music.mp3");
                                if (music_state) sound_manager.PlayMusic(-1);
                                high_score = std::max(last_time_val, high_score);
                                high_money = std::max(p_player.GetMoneyCount(), high_money);
                                SaveHighScore();
                                game_state = GAME_OVER;
                                game_timer.stop();
                            }
                        }
                    }
                }
            }

            if (game_state == PLAYING) {
                std::vector<BulletObject*> bullet_arr = p_player.get_bullet_list();
                for (size_t bl = 0; bl < bullet_arr.size(); bl++) {
                    BulletObject* p_bullet = bullet_arr.at(bl);
                    if (p_bullet != NULL) {
                        for (size_t t = 0; t < threats_list.size(); t++) {
                            ThreatsObject* obj_threat = threats_list.at(t);
                            if (obj_threat != NULL) {
                                SDL_Rect tRect;
                                tRect.x = obj_threat->GetRect().x;
                                tRect.y = obj_threat->GetRect().y;
                                tRect.w = obj_threat->get_width_frame();
                                tRect.h = obj_threat->get_height_frame();
                                SDL_Rect bRect = p_bullet->GetRect();
                                bool bCol = SDLCommonFunc::CheckCollision(bRect, tRect);
                                if (bCol) {
                                    p_player.removeBullet(bl);
                                    if (obj_threat->get_type_move() == ThreatsObject::FLYING_THREAT) {
                                        max_x_pos_fly += 1000;
                                        if (max_x_pos_fly < 20000 * TILE_SIZE) {
                                            obj_threat->set_x_pos(max_x_pos_fly);
                                            obj_threat->ResetBullet(g_screen);
                                        } else {
                                            obj_threat->Free();
                                            threats_list.erase(threats_list.begin() + t);
                                            --t;
                                            break;
                                        }
                                    } else {
                                        max_x_pos_dynamic += 1500;
                                        if (max_x_pos_dynamic < 20000 * TILE_SIZE) {
                                            obj_threat->set_x_pos(max_x_pos_dynamic);
                                            obj_threat->ResetBullet(g_screen);
                                        } else {
                                            obj_threat->Free();
                                            threats_list.erase(threats_list.begin() + t);
                                            --t;
                                            break;
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }

                SDL_Rect rect_player = p_player.GetRectFrame();
                for (size_t i = 0; i < items_list.size();) {
                    ItemObject* item = items_list.at(i);
                    if (item != NULL && item->IsActive()) {
                        SDL_Rect item_rect = item->GetRectFrame();
                        if (SDLCommonFunc::CheckCollisionPlayer(rect_player, item_rect)) {
                            item->SetActive(false);
                            sound_manager.PlaySoundEffect("item_pickup");
                            if (item->GetItemType() == ItemObject::CHERRIES) {
                                p_player.IncreaseMoney(10);
                            } else if (item->GetItemType() == ItemObject::CUP) {
                                if (p_player.get_num_lives() < 3) {
                                    p_player.set_num_lives(p_player.get_num_lives() + 1);
                                }
                            }
                            items_list.erase(items_list.begin() + i);
                            delete item;
                            continue;
                        }
                        item->SetMapXY(map_data.start_x, map_data.start_y);
                        item->Show(g_screen);
                        ++i;
                    } else {
                        items_list.erase(items_list.begin() + i);
                        delete item;
                    }
                }
            }

            if (game_state == PLAYING || game_state == PAUSED) {
                RenderUI(g_screen, p_player, time_game, money_text, font, game_timer);
            }

            pause_button.Show(g_screen);

            if (game_state == PAUSED) {
                // Hiển thị buttons, không vẽ chữ "Paused"
                continue_button.Show(g_screen);
                home_button.Show(g_screen);
                setting_button.Show(g_screen);
                restart_button.Show(g_screen);
            }
        }

        SDL_RenderPresent(g_screen);

        int real_imp_time = fps_timer.get_ticks();
        int time_one_frame = 1000 / FRAME_PER_SECOND;
        if (real_imp_time < time_one_frame) {
            int delay_time = time_one_frame - real_imp_time;
            if (delay_time >= 0)
                SDL_Delay(delay_time);
        }
    }

    for (size_t i = 0; i < threats_list.size(); i++) {
        ThreatsObject* p_threat = threats_list.at(i);
        if (p_threat) {
            p_threat->Free();
            p_threat = NULL;
        }
    }
    threats_list.clear();

    for (size_t i = 0; i < items_list.size(); i++) {
        ItemObject* item = items_list.at(i);
        if (item) {
            item->Free();
            item = NULL;
        }
    }
    items_list.clear();

    sound_manager.Free();
    close();
    return 0;
}
