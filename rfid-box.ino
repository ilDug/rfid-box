/**
 * @file rfid-box.ino
 * @brief RFID Box
 * @details This is the main file for the RFID Box project. It includes the setup and loop functions.
 *
 *
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

#include <MFRC522.h> // https://github.com/miguelbalboa/rfid
#include <SPI.h>
#include "dag-button.h"


// PINS
const int SS_PIN = 10; // Slave Select Pin
const int RST_PIN = 9; // Reset Pin
const int BZR_PIN = 0; // Buzzer Pin
const int BTN_MODE_PIN = 0; // Mode Button Pin
const int BTN_SELECT_PIN = 0; // Select Button Pin

// BUTTONS
DagButton btnMode(BTN_MODE_PIN, PULLUP);
DagButton btnSelect(BTN_SELECT_PIN, PULLUP);

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;       // Key for the read and write

enum  Mode {
    MODE_READ,
    MODE_WRITE
};
uint8_t mode = MODE_READ;

String version = "v1.0.0"; // Version of the program

void setup()
{
    Serial.begin(9600); // Initialize serial communications with the PC
    SPI.begin();        // Init SPI bus
    rfid.PCD_Init();    // Init MFRC522

    Serial.println("RFID Box " + version);

}

void loop()
{
    btnMode.onPress(toggleMode); // Switch between read and write mode when button is pressed
    
}


/**
 * switch between modes
*/
void toggleMode()
{
    mode = mode == MODE_READ ? MODE_WRITE : MODE_READ;
}


/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}
