#ifndef RFID_DATA_H
#define RFID_DATA_H

#include <MFRC522.h>
#include "def.h"

// A struct used for passing data to the reader
typedef struct
{
    byte block;  // block number.
    String data; // data to write
} DB;

/************************************************/
/*** VALID UIDS    */
/************************************************/
MFRC522::Uid valid_uids[] = {
    {4, {0xF7, 0xC2, 0x1D, 0x3C}},
    {10, {0x4D, 0x1D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D}},
    {7, {0x4D, 0x1D, 0x3D, 0x3D, 0x3D, 0x3D, 0x3D}}
};

int valid_uids_len = 3;

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