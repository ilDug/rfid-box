#ifndef DAG_CONSTANTS_H
#define DAG_CONSTANTS_H

#include <MFRC522.h>

/**
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */
const int SS_PIN = 10;        // Slave Select Pin
const int RST_PIN = 9;        // Reset Pin
const int BZR_PIN = 0;        // Buzzer Pin
const int BTN_MODE_PIN = 2;   // Mode Button Pin
const int BTN_RESET_PIN = 4;  // Select Button Pin
const int BTN_SELECT_PIN = 3; // Select Button Pin
const int VALID_PIN = 5;      // Valid Pin


byte cryptokey[MFRC522::MF_KEY_SIZE] = {0x01, 0x02, 0x13, 0x51, 0x09, 0x0F};// Key for reading and writing data to the card


// RFID MODES
enum Mode
{
    MODE_READ,
    MODE_WRITE
};

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
    60, 61, 62  // sector 15
};

// available sectors for writing (3 blocks each)
//  16 bytes * 3 blocks = 48 bytes
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

/****************************************************************************/

// fa suonare il Buzzer per 200 ms e attende 200 ms per un numero di volte passato come argomento
// @param n numero di volte che il buzzer suona
// @param duration durata del suono
// @param pause pausa tra un suono e l'altro
void beep(int n, int duration = 300, int pause = 300)
{
    if (!duration)
        duration = 300;
    if (!pause)
        pause = duration;
    for (int i = 0; i < n; i++)
    {
        digitalWrite(BZR_PIN, HIGH);
        delay(duration);
        digitalWrite(BZR_PIN, LOW);
        delay(pause);
    }
}

/****************************************************************************/

// Helper routine to convert UID to a string
String uidToString(MFRC522::Uid uid)
{
    String str = "";
    for (byte i = 0; i < uid.size; i++)
    {
        str += (uid.uidByte[i] < 0x10 ? " 0" : " ");
        str += uid.uidByte[i];
    }
    return str;
}

/****************************************************************************/

//  Helper routine to convert a string to a  byte array (ASCII)
void stringToBuffer(String str, byte *buffer)
{
    // get the string length
    byte bufferSize = str.length();

    for (byte i = 0; i < bufferSize; i++)
    {
        buffer[i] = str[i];
    }
}

/****************************************************************************/

// Helper routine to convert a byte array (ASCII) to a string
String bufferToString(byte *buffer, byte bufferSize)
{
    String str = "";
    for (byte i = 0; i < bufferSize; i++)
    {
        if (buffer[i] != 0x00)
            str += (char)buffer[i];
    }
    return str;
}

/****************************************************************************/

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

/****************************************************************************/
/**
 * Select block to write data
 */
int nextBlock(int block, int limit = 64)
{
    int len = sizeof(blocks) / sizeof(blocks[0]);
    int i = 0;
    while (block != blocks[i])
    {
        i++;
        if (i >= len || i >= limit)
            break;
    }

    return i >= len - 1 ? blocks[0] : blocks[i + 1];
}

/****************************************************************************/


/**
 * Check if th UID is contained into the list of valid UIDs
*/
bool isValidUid(String uid, MFRC522::Uid valid_uids[], int list_len)
{
    for (int i = 0; i < list_len; i++)
    {
        if (uid == uidToString(valid_uids[i]))
            return true;
    }
    return false;
}



#endif