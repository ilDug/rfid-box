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
DagButton btnReset(BTN_SELECT_PIN, PULLUP);

// RFID
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;       // Key for the read and write
Mode mode = MODE_READ;

// LCD
LCD_I2C lcd(0x27, 16, 2);  // SDA => A4: SCL => A5
String version = "v1.0.0"; // Version of the program
bool fired = false;        //  flag to mark when a card is detected

// First block of sector 1 (block 4) is used to store the user data up to 16 bytes
byte block = 4; // Block number to read/write (0-63 for MIFARE Classic 1K card)
byte len = 18;  // Length of the buffer to store the data read from the card (16 bytes + 2 bytes for CRC)
String value;   // Value read from the card as a string (ASCII) up to 16 bytes (16 characters)
String data;    // Data to write to the card
String uid;     // UID of the card


//Functions
String readCard(byte block, byte len = 18);
bool writeCard(byte block, String data);
bool checkCompatibility();
bool authenticateA(byte block);
void toggleMode();
void waitForReset();
void beep(int n);
void beep(int n, int duration);
void beep(int n, int duration, int pause);

/****************************************************************************/

void setup()
{
    Serial.begin(9600); // Initialize serial communications with the PC
    SPI.begin();        // Init SPI bus
    rfid.PCD_Init();    // Init MFRC522

    Serial.println("RFID Box " + version);
    Serial.println("Reader details:");
    rfid.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details

    // Prepare the key (used both for read and write) using FFFFFFFFFFFF hex value
    // for each byte of the key (6 bytes) [Factory default]
    for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++)
        key.keyByte[i] = 0xFF;

    lcd_init(&lcd, version); // Initialize LCD
    lcd_idle(&lcd, mode);    // Show idle message
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

    /***********************************************************************/

    fired = true;                               // mark that a card is detected
    Serial.print(F("Card detected:"));          // Show some details of the PICC (that is: the tag/card)
    rfid.PICC_DumpDetailsToSerial(&(rfid.uid)); // dump some details about the card
    Serial.println();

    // check card compatibility
    if (!checkCompatibility())
    {
        waitForReset();
        return;
    }

    // Read data from the card
    switch (mode)
    {
    case MODE_READ:
        lcd_reading(&lcd);

        // Authenticate using key A
        if (!authenticateA(block))
        {
            waitForReset();
            return;
        }

        // Get the UID of the card
        uid = uidToString(rfid.uid);

        // Read data from the card
        value = readCard(block, len);

        // Show the UID and the value read from the card
        if (value != "")
            lcd_reading_result(&lcd, uid, value);

        waitForReset();
        break;

    case MODE_WRITE:
        lcd_writing(&lcd);

        // Authenticate using key A
        if (!authenticateA(block))
        {
            waitForReset();
            return;
        }

        bool result = writeCard(block, "Hello, World!");
        if (result)
            lcd_reading_result(&lcd, "Write success", "Hello, World!");
        waitForReset();
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

// Reset the flag when the reset button is pressed
void waitForReset()
{
    while (fired)
    {
        if (btnReset.pressed())
            fired = false;
        delay(100);
    }
    lcd_idle(&lcd, mode);
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

/**
 * Read data from the card
 * @param block block number to read (0-63 for MIFARE Classic 1K card)
 * @param sector sector number to read  (0-15 for MIFARE Classic 1K card)
 * @param len length of the buffer to store the data read
 * @return value read from the card as a string
 */
String readCard(byte block, byte len = 18)
{
    byte buffer[len]; // buffer to store the data read from the card (16 bytes + 2 bytes for CRC)
    String value;     // value read from the card

    MFRC522::StatusCode status = rfid.MIFARE_Read(block, buffer, &len);
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("Reading failed: "));
        Serial.println(rfid.GetStatusCodeName(status));
        lcd_reading_error(&lcd, rfid.GetStatusCodeName(status));
        value = "";
    }

    Serial.print(F("Data in block "));
    Serial.print(block); //  print the block number to the serial monitor
    Serial.println(F(":"));
    dump_byte_array(buffer, len - 2); // dump the data read from the card to the serial monitor
    Serial.println();

    value = bufferToString(buffer, len - 2); // convert the byte reading array to a string (ASCII)
    Serial.println(value);
    Serial.println();

    // Halt PICC
    rfid.PICC_HaltA();
    // Stop encryption on PCD
    rfid.PCD_StopCrypto1();

    beep(1);
    return value;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/**
 * Write data to the card up to 16 bytes (16 characters) in a block (0-63 for MIFARE Classic 1K card)
 * @param block block number to write (0-63 for MIFARE Classic 1K card)
 * @param data string to write to the card
 * @return void
 * */
bool writeCard(byte block, String data)
{
    bool success = true;
    byte l = data.length(); // length of the data to write

    if (l > 16) // check if the data is more than 16 bytes
    {
        Serial.println(F("Data too long. Maximum length is 16 bytes"));
        lcd_reading_error(&lcd, "Data too long. Maximum length is 16 bytes");
        beep(3);
        success = false;
    }
    else
    {
        for (byte i = l; i < 16; i++)
            data += " ";              // pad the buffer with spaces if the data is less than 16 bytes
        byte buffer[16];              // buffer to store the data to write to the card
        stringToBuffer(data, buffer); // convert the string to a byte array (ASCII)

        // Write data to the card
        MFRC522::StatusCode status = rfid.MIFARE_Write(block, buffer, 16);
        if (status != MFRC522::STATUS_OK)
        {
            Serial.print(F("MIFARE_Write() failed: "));
            Serial.println(rfid.GetStatusCodeName(status));
            lcd_reading_error(&lcd, rfid.GetStatusCodeName(status));
            beep(3);
            success = false;
        }
        else
        {
            Serial.println(F("Write data to the card success"));
            success = true;
            beep(1);
        }
    }

    // Halt PICC
    rfid.PICC_HaltA();
    // Stop encryption on PCD
    rfid.PCD_StopCrypto1();

    return success;
}


/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

// Check for card  compatibility
bool checkCompatibility()
{
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K)
    {
        Serial.println(F("This device only works with MIFARE Classic cards."));
        lcd_reading_error(&lcd, "Card not supported");
        beep(3);
        return false;
    }
    else
        return true;
}



/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

// Authenticate using key A
bool authenticateA(byte block)
{
    MFRC522::StatusCode status;
    status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(rfid.uid));
    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("Authentication failed: "));
        Serial.println(rfid.GetStatusCodeName(status));
        lcd_reading_error(&lcd, rfid.GetStatusCodeName(status));
        beep(3);
        return false;
    }
    else
        return true;
}



