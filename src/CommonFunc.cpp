#include "CommonFunc.h"

void logErrorAndExit(const char* msg, const char* error)
{
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "%s: %s", msg, error);
    SDL_Quit();
}

bool SDLCommonFunc::CheckCollisionPlayer(const SDL_Rect& a, const SDL_Rect& b)
{
    if (a.x + a.w - PLAYER_COLLISION_OFFSET_X <= b.x) return false;
    if (a.x + PLAYER_COLLISION_OFFSET_X >= b.x + b.w) return false;
    if (a.y + a.h - PLAYER_COLLISION_OFFSET_Y <= b.y) return false;
    if (a.y + PLAYER_COLLISION_OFFSET_Y >= b.y + b.h) return false;
    return true;
}

bool SDLCommonFunc::CheckCollision(const SDL_Rect& a, const SDL_Rect& b)
{
    if (a.x + a.w <= b.x) return false;
    if (a.x >= b.x + b.w) return false;
    if (a.y + a.h <= b.y) return false;
    if (a.y >= b.y + b.h) return false;
    return true;
}
