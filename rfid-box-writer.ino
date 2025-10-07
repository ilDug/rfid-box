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
#include "data.h"

// BUTTONS
DagButton btnMode(BTN_MODE_PIN, PULLUP);
DagButton btnReset(BTN_RESET_PIN, PULLUP);

// RFID
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;       // Key for the read and write
Mode mode = MODE_READ;          // Current mode (MODE_READ or MODE_WRITE)

// LCD
LCD_I2C lcd(0x27, 16, 2);  // SDA => A4: SCL => A5
String version = "v1.0.0"; // Version of the program
bool fired = false;        //  flag to mark when a card is detected

String value;   // Value read from the card as a string (ASCII) up to 16 bytes (16 characters)
String passphrase = ""; // Passphrase stored to eeprom
String uid;     // UID of the card

// Function prototypes
String readTag(int *blocksArray, int blocksCount);
bool writeTag(String data, int *blocksArray, int blocksCount);

void setup()
{
    Serial.begin(9600); // Initialize serial communications with the PC
    SPI.begin();        // Init SPI bus for MFRC522
    rfid.PCD_Init();    // Init MFRC522

    // pinmode
    pinMode(ACTION_PIN, OUTPUT);
    pinMode(ALARM_PIN, OUTPUT);
    pinMode(ERROR_PIN, OUTPUT);

    Serial.println("RFID Box " + version);
    Serial.println("Reader details:");
    rfid.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
    Serial.println("reading passphrase from eeprom...");
    loadPayloadFromEEPROM(&passphrase);
    Serial.println("Passphrase: " + passphrase);
    Serial.println();

    // Prepare the key (used both for read and write) using FFFFFFFFFFFF hex value
    // for each byte of the key (6 bytes) [Factory default]
    for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++)
        key.keyByte[i] = 0xFF;
    // key.keyByte[i] = cryptokey[i];

    //lcd_init(&lcd, version);     // Initialize LCD
    //lcd_idle(&lcd, mode, block); // Show idle message
    openSesame(false); // Set the valid output to LOW as default
}

void loop()
{
    btnMode.onPress(toggleMode);   // Switch between read and write mode when button is pressed

    // Look for new cards, else do nothing
    if (!rfid.PICC_IsNewCardPresent())
        return;

    // Select one of the cards, else do nothing
    if (!rfid.PICC_ReadCardSerial())
        return;

    /***********************************************************************/
    // quando una carta è stata letta con successo

    fired = true;                               // mark that a card is detected
    Serial.println(F("Card detected:"));        // Show some details of the PICC (that is: the tag/card)
    Serial.println();

    // ------------------------------------------------------------------------
    // Read data from the card
    if (mode == MODE_READ)
    {
        // lcd_reading(&lcd);

        // quando si tiene premuto il pulsante di reset durante la lettura, stampa al Serial monitor i dati di tutti i blocchi
        if (btnReset.clicked())
        {
            rfid.PICC_DumpToSerial(&(rfid.uid)); // dump some details about the card
            return;
        }

        // reset the value
        value = "";
        // Read data from all blocks in the blocks array
        int blocksCount = sizeof(blocks) / sizeof(blocks[0]);
        value = readTag(blocks, blocksCount);

        // Handle beep signals based on readTag result
        if (value == "")
        {
            beep(3); // error beep if reading failed or no data was read
        }
        else
        {
            beep(1); // success beep if data was read successfully
        }

        // Get the UID of the card as a string
        uid = uidToString(rfid.uid);

        // Halt PICC
        // rfid.PICC_HaltA();
        // Stop encryption on PCD
        // rfid.PCD_StopCrypto1();

        // Show the UID and the value read from the card
        // if (value != "")
        //     lcd_reading_result(&lcd, uid, value);
    }

    // ------------------------------------------------------------------------
    else if (mode == MODE_WRITE)
    {
        // check card compatibility
        if (!checkCompatibility())
        {
            if (mode == MODE_WRITE)
                triggerErrorAndWaitForReset(btnReset, fired); // wait until the reset button is pressed only in write mode
            return;
        }

        // lcd_writing(&lcd);
        Serial.println("Write mode switched on");

        // Write all data using the new writeTag function
        int blocksCount = sizeof(blocks) / sizeof(blocks[0]);
        bool result = writeTag(&passphrase, blocks, blocksCount);
        if (result)
        {
            // lcd_reading_result(&lcd, "Write success", "All blocks written");
            Serial.println("Write operation completed successfully");
        }
        else
        {
            // lcd_reading_error(&lcd, "Write failed");
            Serial.println("Write operation failed");
        }

        // Halt PICC
        // rfid.PICC_HaltA();
        // Stop encryption on PCD
        // rfid.PCD_StopCrypto1();

        beep(1, 1000);

        triggerErrorAndWaitForReset(btnReset, fired); // wait until the reset button is pressed
    }
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

/**
 * switch between modes
 */
void toggleMode()
{
    mode = mode == MODE_READ ? MODE_WRITE : MODE_READ;
    beep(1);
    //lcd_idle(&lcd, mode, block);
    Serial.println(mode == MODE_READ ? "Read mode selected" : "Write mode selected");
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
        // lcd_reading_error(&lcd, rfid.GetStatusCodeName(status));
        return false;
    }
    else
        return true;
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
        // lcd_reading_error(&lcd, "Card not supported");
        return false;
    }
    else
        return true;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

/**
 * Read data from all blocks defined in the blocks array
 * @param blocksArray pointer to the array of blocks to read
 * @param blocksCount number of blocks in the array
 * @return concatenated value read from all blocks as a string
 */
String readTag(int *blocksArray, int blocksCount)
{
    byte len = 18;          // Length of the buffer to store the data read from the card (16 bytes + 2 bytes for CRC)
    String finalValue = ""; // final concatenated value from all blocks

    Serial.println(F("Reading data from all blocks..."));
    Serial.println();

    for (int i = 0; i < blocksCount; i++)
    {
        byte currentBlock = blocksArray[i];
        byte buffer[len];       // buffer to store the data read from the card (16 bytes + 2 bytes for CRC)
        String blockValue = ""; // value read from current block

        // Authenticate for each block
        if (!authenticateA(currentBlock))
        {
            Serial.println("CRITICAL ERROR: Authentication failed for block " + String(currentBlock));
            Serial.println("Stopping read operation due to authentication failure.");
            Serial.println();
            return ""; // return empty string to indicate failure
        }

        MFRC522::StatusCode status = rfid.MIFARE_Read(currentBlock, buffer, &len);
        if (status != MFRC522::STATUS_OK)
        {
            Serial.print(F("CRITICAL ERROR: Reading failed on block "));
            Serial.print(currentBlock);
            Serial.print(F(": "));
            Serial.println(rfid.GetStatusCodeName(status));
            Serial.println("Stopping read operation due to read failure.");
            Serial.println();
            // lcd_reading_error(&lcd, rfid.GetStatusCodeName(status));
            return ""; // return empty string to indicate failure
        }
        else
        {
            Serial.print(F("Data in block "));
            Serial.print(currentBlock);
            Serial.println(F(":"));
            dump_byte_array(buffer, len - 2); // dump the data read from the card to the serial monitor
            Serial.println();

            blockValue = bufferToString(buffer, len - 2); // convert the byte reading array to a string (ASCII)
            Serial.println("Block " + String(currentBlock) + " content: " + blockValue);
            Serial.println();

            // Check if the block value is empty/null - if so, stop reading
            if (blockValue.length() == 0 || blockValue.trim().length() == 0)
            {
                Serial.println("Found empty block " + String(currentBlock) + ", stopping read operation.");
                Serial.println();
                break; // terminate the reading loop
            }

            // Concatenate the block value to the final string
            finalValue += blockValue;
        }
    }

    // Trim whitespace from the beginning and end of finalValue
    finalValue.trim();

    Serial.println("Final concatenated value: " + finalValue);
    Serial.println();

    // Halt PICC
    rfid.PICC_HaltA();
    // Stop encryption on PCD
    rfid.PCD_StopCrypto1();

    return finalValue;
}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

/**
 * Write data to all blocks defined in the blocks array
 * @param data String to write to the tag (will be distributed across multiple blocks)
 * @param blocksArray pointer to the array of blocks to write
 * @param blocksCount number of blocks in the array
 * @return true if all blocks were written successfully, false if any error occurred
 */
bool writeTag(String *data, int *blocksArray, int blocksCount)
{
    int dataLength = data->length();
    int dataIndex = 0;

    Serial.println(F("Writing data to all blocks..."));
    Serial.println("Data to write: " + *data);
    Serial.println("Data length: " + String(dataLength));
    Serial.println();

    for (int i = 0; i < blocksCount; i++)
    {
        byte currentBlock = blocksArray[i];
        byte buffer[16]; // buffer to store the data to write to the card (16 bytes per block)

        // Clear the buffer
        memset(buffer, 0x00, 16);

        // Fill buffer with data or nulls
        for (int j = 0; j < 16; j++)
        {
            if (dataIndex < dataLength)
            {
                buffer[j] = (*data)[dataIndex];
                dataIndex++;
            }
            else
            {
                buffer[j] = 0x00; // null bytes for remaining space
            }
        }

        // Authenticate for each block
        if (!authenticateA(currentBlock))
        {
            Serial.println("CRITICAL ERROR: Authentication failed for block " + String(currentBlock));
            Serial.println("Stopping write operation due to authentication failure.");
            Serial.println();
            return false; // return false to indicate failure
        }

        // Write data to the card
        MFRC522::StatusCode status = rfid.MIFARE_Write(currentBlock, buffer, 16);
        if (status != MFRC522::STATUS_OK)
        {
            Serial.print(F("CRITICAL ERROR: Writing failed on block "));
            Serial.print(currentBlock);
            Serial.print(F(": "));
            Serial.println(rfid.GetStatusCodeName(status));
            Serial.println("Stopping write operation due to write failure.");
            Serial.println();
            return false; // return false to indicate failure
        }
        else
        {
            Serial.print(F("Successfully wrote to block "));
            Serial.print(currentBlock);
            Serial.print(F(" - Data: "));

            // Print the data written to this block
            String blockData = "";
            for (int k = 0; k < 16; k++)
            {
                if (buffer[k] != 0x00)
                    blockData += (char)buffer[k];
            }
            Serial.println(blockData);
        }

        // If we've written all the data and the rest would be nulls, we can stop
        if (dataIndex >= dataLength)
        {
            Serial.println("All data written successfully. Remaining blocks will be cleared.");
            Serial.println();

            // Continue to clear remaining blocks with null values
            for (int remainingBlock = i + 1; remainingBlock < blocksCount; remainingBlock++)
            {
                byte nullBuffer[16];
                memset(nullBuffer, 0x00, 16);

                if (!authenticateA(blocksArray[remainingBlock]))
                {
                    Serial.println("CRITICAL ERROR: Authentication failed for clearing block " + String(blocksArray[remainingBlock]));
                    return false;
                }

                MFRC522::StatusCode clearStatus = rfid.MIFARE_Write(blocksArray[remainingBlock], nullBuffer, 16);
                if (clearStatus != MFRC522::STATUS_OK)
                {
                    Serial.print(F("CRITICAL ERROR: Failed to clear block "));
                    Serial.print(blocksArray[remainingBlock]);
                    Serial.print(F(": "));
                    Serial.println(rfid.GetStatusCodeName(clearStatus));
                    return false;
                }
                else
                {
                    Serial.println("Cleared block " + String(blocksArray[remainingBlock]));
                }
            }
            break;
        }
    }

    // Halt PICC
    rfid.PICC_HaltA();
    // Stop encryption on PCD
    rfid.PCD_StopCrypto1();

    Serial.println("Write operation completed successfully.");
    Serial.println();

    return true; // return true to indicate success
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
// bool writeTagObsolete(byte block, String data)
// {
//     bool success = true;
//     byte l = data.length(); // length of the data to write

//     if (l > 16) // check if the data is more than 16 bytes
//     {
//         Serial.println(F("Data too long. Maximum length is 16 bytes"));
//         lcd_reading_error(&lcd, "Data too long. Maximum length is 16 bytes");
//         beep(3);
//         success = false;
//     }
//     else
//     {
//         for (byte i = l; i < 16; i++)
//             data += " ";              // pad the buffer with spaces if the data is less than 16 bytes
//         byte buffer[16];              // buffer to store the data to write to the card
//         stringToBuffer(data, buffer); // convert the string to a byte array (ASCII)

//         // Write data to the card
//         MFRC522::StatusCode status = rfid.MIFARE_Write(block, buffer, 16);
//         if (status != MFRC522::STATUS_OK)
//         {
//             Serial.print(F("MIFARE_Write() failed: "));
//             Serial.println(rfid.GetStatusCodeName(status));
//             lcd_reading_error(&lcd, rfid.GetStatusCodeName(status));
//             beep(3);
//             success = false;
//         }
//         else
//         {
//             Serial.println(F("Write data to the card success"));
//             success = true;
//         }
//     }

//     // // Halt PICC
//     // rfid.PICC_HaltA();
//     // // Stop encryption on PCD
//     // rfid.PCD_StopCrypto1();

//     return success;
// }
