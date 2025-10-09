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
    duration = time_duration;   // imposta la durata del timer
    bookmark = millis();        // inizializza il bookmark con il millis corrente
    FIRED = 0;                  // inizializza lo stato del timer
    LOOP = repeat;              // imposta se il timer deve essere ripetitivo o meno
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
    unsigned long dt = (millis() - bookmark);   // calcola il delta time
    bool res = false;                           // inizializza il valore di ritorno

    if ((dt > duration) && !FIRED) // se è passato il periodo e il timer non è già scattato
    {
        FIRED = 1;  // imposta lo stato del timer a scattato
        res = true; // imposta il valore di ritorno a TRUE

        if (LOOP) // se il timer è ripetitivo, reimposta il bookmark e lo stato del timer
        {
            bookmark = millis();
            FIRED = 0; // resetta lo stato del timer a non scattato per il prossimo loop in modo da ripetere l'operazione
        }
    }
    else // se non è passato il periodo oppure il timer è già scattato imposta il valore di ritorno a FALSE
        res = false;

    return res;
}

bool DagTimer::exhausted()
{
    unsigned long dt = (millis() - bookmark);  // calcola il delta time
    return (dt > duration); // restituisce TRUE se è passato il periodo, FALSE altrimenti
}