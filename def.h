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

#endif