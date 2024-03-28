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
const int BTN_MODE_PIN = 0;   // Mode Button Pin
const int BTN_SELECT_PIN = 0; // Select Button Pin

// RFID MODES
enum Mode
{
    MODE_READ,
    MODE_WRITE
};

// fa suonare il Buzzer per 200 ms e attende 200 ms per un numero di volte passato come argomento
// @param n numero di volte che il buzzer suona
// @param duration durata del suono
// @param pause pausa tra un suono e l'altro
void beep(int n);
void beep(int n, int duration);
void beep(int n, int duration, int pause)
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


#endif