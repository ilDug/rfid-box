#include "dag-timer.h"

void timerNoop() {} // funzione segnaposto che non fa niente// esegue periodo del timer e le call back

DagTimer::DagTimer()
{
    FIRED = 0;
    LOOP = true;
    call_back = timerNoop;
}

void DagTimer::init(unsigned long time_duration)
{
    init(time_duration, true);
}

void DagTimer::init(unsigned long time_duration, bool repeat)
{
    duration = time_duration;
    bookmark = millis();
    FIRED = 0;
    LOOP = repeat;
}

void DagTimer::run(void (*fun)(void))
{
    if (clock())
    {
        call_back = fun;
        call_back();
    }
}

bool DagTimer::clock()
{
    unsigned long dt = (millis() - bookmark);
    bool res = false;

    if ((dt > duration) && !FIRED)
    {
        FIRED = 1;
        res = true;

        if (LOOP)
        {
            bookmark = millis();
            FIRED = 0;
        }
    }
    else
        res = false;

    return res;
}

bool DagTimer::exhausted()
{
    unsigned long dt = (millis() - bookmark);
    return (dt > duration);
}