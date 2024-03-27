#ifndef DAG_CONSTANTS_H
#define DAG_CONSTANTS_H

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

// fa suonare il Buzzer per 200 ms e attende 200 ms per un numero di volte passato come argomento
// @param n numero di volte che il buzzer suona
// @param duration durata del suono
// @param pause pausa tra un suono e l'altro
void beep(int n);
void beep(int n, int duration);
void beep(int n, int duration, int pause)
{
    if(!duration) duration = 300;
    if(!pause) pause = duration;
    for (int i = 0; i < n; i++)
    {
        digitalWrite(BZR_PIN, HIGH);
        delay(duration);
        digitalWrite(BZR_PIN, LOW);
        delay(pause);
    }
}

#endif