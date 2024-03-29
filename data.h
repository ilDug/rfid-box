#ifndef RFID_DATA_H
#define RFID_DATA_H

#include <MFRC522.h>

// available block (16 bytes each) for writing
// 16 bytes * 45 blocks = 720 bytes (~ 135 words)
int blocks[] = {
    4, 5, 6,    // sector 1
    8, 9, 10,   // sector 2
    12, 13, 14, // sector 3
    16, 17, 18, // sector 4
    20, 21, 22, // sector 5
    24, 25, 26, // sector 6
    28, 29, 30, // sector 7
    32, 33, 34, // sector 8
    36, 37, 38, // sector 9
    40, 41, 42, // sector 10
    44, 45, 46, // sector 11
    48, 49, 50, // sector 12
    52, 53, 54, // sector 13
    56, 57, 58, // sector 14
    60, 61, 62 // sector 15
};

//available sectors for writing (3 blocks each)
// 16 bytes * 3 blocks = 48 bytes
int sector1[] = {4, 5, 6};
int sector2[] = {8, 9, 10};
int sector3[] = {12, 13, 14};
int sector4[] = {16, 17, 18};
int sector5[] = {20, 21, 22};
int sector6[] = {24, 25, 26};
int sector7[] = {28, 29, 30};
int sector8[] = {32, 33, 34};
int sector9[] = {36, 37, 38};
int sector10[] = {40, 41, 42};
int sector11[] = {44, 45, 46};
int sector12[] = {48, 49, 50};
int sector13[] = {52, 53, 54};
int sector14[] = {56, 57, 58};
int sector15[] = {60, 61, 62};

// valid uids
MFRC522::Uid valid_uids[] = {
    {0x04, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F}, // da verificare
    {0x05, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F}, // inserire quelli del cancello
};





#endif