#ifndef RFID_DATA_H
#define RFID_DATA_H

#include <MFRC522.h>
#include "def.h"

// A struct used for passing data to the reader
typedef struct
{
    byte  block;  // block number.
    String  data; // data to write
} DB;

/************************************************/
/*** INSERIRE QUI I DATI DA SCRIVERE */
/*** max 16 caratteri per blocco      */
/************************************************/

DB PAYLOAD[] = {
    {4, "Ciao Mondo"},
    {5, "Hello World"},
    {6, "Hola Mundo"},
};

int PAYLOAD_SIZE = 3;
/************************************************/

#endif

