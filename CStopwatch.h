#pragma once

#include <Windows.h>

class CStopwatch
{
    struct Timer
    {
        LARGE_INTEGER start;
        LARGE_INTEGER stop;
    };

public:
    CStopwatch();
    inline void Start();
    inline double Stop();
    inline double GetElapsedTime();

private:
    inline double toSeconds(const LARGE_INTEGER &v);

private:
    Timer timer;
    LARGE_INTEGER frequency;
};

CStopwatch::CStopwatch()
{
    timer.start.QuadPart = 0;
    timer.stop.QuadPart  = 0;
    QueryPerformanceFrequency(&frequency);
}

void CStopwatch::Start()
{
    QueryPerformanceCounter(&timer.start);
}

double CStopwatch::Stop()
{
    QueryPerformanceCounter(&timer.stop);

    return GetElapsedTime();
}

double CStopwatch::GetElapsedTime()
{
    LARGE_INTEGER time;
    time.QuadPart = timer.stop.QuadPart - timer.start.QuadPart;
    return toSeconds(time);
}

double CStopwatch::toSeconds(const LARGE_INTEGER &v)
{
    return ((double)v.QuadPart/(double)frequency.QuadPart);
}
