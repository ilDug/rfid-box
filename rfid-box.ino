/**
 * @file rfid-box.ino
 * @brief RFID Box
 * @details This is the main file for the RFID Box project. It includes the setup and loop functions.
 * @author Dag
 */

#include <SPI.h>
#include <MFRC522.h> // https://github.com/miguelbalboa/rfid
#include <Wire.h>
#include <LCD_I2C.h>
#include "dag-button.h"
#include "dag-timer.h"
#include "def.h"
#include "lcd.h"

// BUTTONS
DagButton btnMode(BTN_MODE_PIN, PULLUP);
DagButton btnSelect(BTN_SELECT_PIN, PULLUP);

// RFID
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;       // Key for the read and write
Mode mode = MODE_READ;

// LCD
LCD_I2C lcd(0x27, 16, 2);  // SDA => A4: SCL => A5
String version = "v1.0.0"; // Version of the program

void setup()
{
    Serial.begin(9600); // Initialize serial communications with the PC
    SPI.begin();        // Init SPI bus
    rfid.PCD_Init();    // Init MFRC522

    Serial.println("RFID Box " + version);
    lcd_init(&lcd, version); // Initialize LCD
    lcd_idle(&lcd, mode);    // Show idle message

    // Prepare the key (used both for read and write) using FFFFFFFFFFFF hex value
    // for each byte of the key (6 bytes) [Factory default]
    for (byte i = 0; i < 6; i++)
        key.keyByte[i] = 0xFF;
}

void loop()
{
    btnMode.onPress(toggleMode); // Switch between read and write mode when button is pressed

    // Look for new cards, else do nothing
    if (!rfid.PICC_IsNewCardPresent())
        return;

    // Select one of the cards, else do nothing
    if (!rfid.PICC_ReadCardSerial())
        return;

    // if a new card is present, read or write data
    switch (mode)
    {
    case MODE_READ:
        lcd_reading(&lcd);
        break;

        
    case MODE_WRITE:
        break;
    }
}

/**
 * switch between modes
 */
void toggleMode()
{
    mode = mode == MODE_READ ? MODE_WRITE : MODE_READ;
    beep(1);
    lcd_idle(&lcd, mode);
}


/****************************************************************************/

void readCard()
{

    // // Dump debug info about the card. PICC_HaltA() is automatically called.
    // rfid.PICC_DumpToSerial(&(rfid.uid));
}


/****************************************************************************/

void writeCard()
{
    // Look for new cards
    if (!rfid.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if (!rfid.PICC_ReadCardSerial())
        return;

    // // Write data to the card
    // byte block = 1;
    // byte data[16] = {
    //     0x01, 0x02, 0x03, 0x04,
    //     0x05, 0x06, 0x07, 0x08,
    //     0x08, 0x07, 0x06, 0x05,
    //     0x04, 0x03, 0x02, 0x01
    // };
    // MFRC522::StatusCode status = rfid.MIFARE_Write(block, data, 16);
    // if (status != MFRC522::STATUS_OK)
    // {
    //     Serial.print(F("MIFARE_Write() failed: "));
    //     Serial.println(rfid.GetStatusCodeName(status));
    // }
    // else
    // {
    //     Serial.println(F("Write data to the card success"));
    // }
}
