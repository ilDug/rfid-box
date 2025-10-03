#ifndef DAG_BUTTON_H
#define DAG_BUTTON_H

#include "Arduino.h"

// modalità di innesco del bottone. PULLUP --> LOW | PULLDOWN --> HIGH
enum DagBtnTriggerMode
{
    PULLUP = LOW,
    PULLDOWN = HIGH
};

class DagButton
{
private:
    /** pin di lettura del bottone  */
    int PIN;

    /** variabile per la lettura edllo stato del pin collegato al bottone (LOW or HIGH) */
    int STATE;

    /*
        stato del pin che fa eseguire la callback: PULLUP --> LOW | PULLDOWN --> HIGH

        modalità PULLDOWN, ricordarsi di mettere una resistenza di pull_down
        il pulsante esegue la callback quando lo stato del pin è HIGH.

        modalità PULLUP. Usa la pullup interna di arduino
        il pulsante esegue la callback quando lo stato del pin è LOW
      */
    int triggeredBy;

    /** callback function */
    void (*call_back)(void);

    /** registra il momento in cui viene premuto*/
    unsigned long pressTime;

    /** registra l'esecuzione del callback */
    bool executed;

    /** salva l'ultimo stato del bottone */
    bool prevState = false;

public:
    /**
     * CONSTRUCTOR che prende come argomento il pin di lettura del bottone
     * (PULLUP --> LOW | PULLDOWN --> HIGH ). DEFAULT MODE= HIGH (PULLDOWN)
     *
     * - modalità PULLDOWN, ricordarsi di mettere una resistenza di pull_down
     * il pulsante esegue la callback quando lo stato del pin è HIGH
     *
     * - modalità PULLUP. Usa la pullup interna di arduino
     * il pulsante esegue la callback quando lo stato del pin è LOW
     * @param pin Il pin che legge il segnale del button
     */
    DagButton(int pin);

    /**
     * CONSTRUCTOR che prende come argomento il pin di lettura del bottone e la modalità
     * (PULLUP --> LOW | PULLDOWN --> HIGH ). DEFAULT MODE= HIGH (PULLDOWN)
     *
     * Esegua in autonomia il PINMODE del pin di lettura del bottone. non c'è bisogno di eseguirlo nel setup
     *
     * - modalità PULLDOWN, ricordarsi di mettere una resistenza di pull_down
     * il pulsante esegue la callback quando lo stato del pin è HIGH
     *
     * - modalità PULLUP. Usa la pullup interna di arduino
     * il pulsante esegue la callback quando lo stato del pin è LOW
     *
     * @param pin Il pin che legge il segnale del button
     * @param triggeredBy: PULLUP o PULLDOWN. Stato del pin che fa eseguire la callback: PULLUP --> LOW | PULLDOWN --> HIGH
     */
    DagButton(int pin, DagBtnTriggerMode triggeredBy);

    /** VALORE ISTANTANEO. restituisce se il pulsante è stato premuto */
    bool clicked(void);

    // VALORE PONDERATO, non BLOCCANTE.  restituisce se il bottone è stato premuto,
    // impedendo che si legga due volte lo stesso valore in caso di pressioni prolungate
    bool pressed(void);

    /** esegue la callback quando il pulsante viene premuto (da mettere nel loop). NON BLOCCANTE */
    void onPress(void (*fun)(void));

    /** esegue la funzione quando premuto per un numero di millisecondi. riceve anche la callback, NON BLOCCANTE*/
    void onLongPress(void (*fun)(void), int trigger_time);

    /** esegue quando il pulsante viene rilasciato. BLOCCANTE  */
    void onRelease(void (*fun)(void));
};

#endif