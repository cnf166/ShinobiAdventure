
#include "Time.h"
#include "CommonFunc.h"

Time :: Time()
{
    start_tick = 0;
    paused_tick = 0;
    is_paused = false;
    is_started = false;

}


Time :: ~Time()
{


}


void Time :: start()
{

    is_started = true;
    is_paused = false;

    start_tick = SDL_GetTicks();
}

void Time :: stop()
{
    is_paused = false;
    is_started = false;
}


void Time :: paused()
{

    if(is_started == true && is_paused == false)
    {
        is_paused = true;
        paused_tick = SDL_GetTicks() - start_tick;

    }
}


void Time :: unpaused()
{
    if(is_paused == true)
    {
        is_paused =false;
        start_tick = SDL_GetTicks() - paused_tick;
        paused_tick = 0;
    }

}


int Time :: get_ticks()
{

    if(is_started == true)
    {
        if(is_paused == true)
        {
            return paused_tick;
        }
        else
        {
            return SDL_GetTicks() - start_tick;
        }
    }

    return 0;
}

bool Time ::is_started_()
{

    return is_started;
}

bool Time :: is_paused_(){
    return is_paused;
}
