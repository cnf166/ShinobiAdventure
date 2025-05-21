
#ifndef TIME_H

#define TIME_H


class Time{
public:
    Time();
    ~Time();

    void start();
    void stop();

    void paused();
    void  unpaused();
    int get_ticks();
    bool is_started_();
    bool is_paused_();

private:
    int start_tick;
    int paused_tick;

    bool is_paused;
    bool is_started;


};


#endif // TIME_H
