#include "dag-button.h"

void dagBtnNoop() {} // funzione segnaposto che non fa niente

DagButton::DagButton(int pin) : DagButton(pin, PULLDOWN)
{
    // chiamata al costruttore con due parametri
    // se non viene specificato il secondo parametro, il default è PULLDOWN
}

DagButton::DagButton(int pin, DagBtnTriggerMode triggeredBy = PULLDOWN)
{
    call_back = dagBtnNoop;
    PIN = pin;
    this->triggeredBy = triggeredBy;

    if (triggeredBy == PULLDOWN)
    {
        // modalità PULLDOWN, ricordarsi di mettere una resistenza di pull_down
        // il pulsante esegue la callback quando lo stato del pin è HIGH
        pinMode(PIN, INPUT);
    }

    if (triggeredBy == PULLUP)
    {
        // modalità PULLUP. Usa la pullup interna di arduino
        // il pulsante esegue la callback quando lo stato del pin è LOW
        pinMode(PIN, INPUT_PULLUP);
    }
}

bool DagButton::clicked()
{
    STATE = digitalRead(PIN);
    return STATE == triggeredBy;
}

bool DagButton::pressed()
{
    if (clicked() == prevState)
        return false;
    else
    {
        // imposta il valore per il prossimo loop
        prevState = clicked();
        return true;
    }
}

void DagButton::onPress(void (*fun)(void))
{
    if (!clicked())
    {
        executed = false;
        return;
    };

    if (executed)
        return;

    call_back = fun;
    call_back();
    executed = true;
}

void DagButton::onLongPress(void (*fun)(void), int trigger_time)
{

    if (!clicked())
    {
        pressTime = millis();
        return;
    };

    unsigned long dt = millis() - pressTime;
    boolean isStillPressed = clicked() && dt >= trigger_time;

    if (isStillPressed)
    {
        call_back = fun;
        call_back();
        pressTime = millis();
    }
}

void DagButton::onRelease(void (*fun)(void))
{
    if (!clicked())
        return;

    while (clicked())
    {
        delay(10);
        // attende fino a quando non viene rilasciato
    }
    call_back = fun;
    call_back();
}