#ifndef DAG_CONSTANTS_H
#define DAG_CONSTANTS_H

#include <MFRC522.h>
#include <EEPROM.h>

/**
 * Pin definitions for the MFRC522
 * Using Hardware SPI of Arduino
 *  MOSI (11), MISO (12) and SCK (13) are fixed
 *  configure SS and RST Pins only
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
const int SS_PIN = 10; // RFID Slave Select Pin
const int RST_PIN = 9; // RFID Reset Pin

const int BTN_MODE_PIN = 5;  // Mode Button Pin
const int BTN_RESET_PIN = 4; // Reset Button Pin

const int ACTION_PIN = 2; // Action Pin
const int ALARM_PIN = 6;  // Alarm Pin
const int ERROR_PIN = 3;  // Error Pin

const int SIGNAL1_PIN = A1; // Signal1 Pin
const int SIGNAL2_PIN = A2; // Signal2 Pin

byte cryptokey[MFRC522::MF_KEY_SIZE] = {0x01, 0x02, 0x13, 0x51, 0x09, 0x0F}; // Key for reading and writing data to the card

// RFID MODES
enum Mode
{
    MODE_READ,
    MODE_WRITE
};

enum Agent
{
    AGENT_READER,
    AGENT_WRITER
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

// @brief Generates a series of beeps using the ALARM_PIN.
// The function produces a sequence of beeps by toggling the ALARM_PIN between HIGH and LOW.
// The number of beeps, duration of each beep, and pause between beeps can be customized.
// @param n The number of beeps to generate.
// @param duration The duration of each beep in milliseconds. Defaults to 300ms.
// @param pause The duration of the pause between beeps in milliseconds. Defaults to the same value as duration.
void beep(int n, int duration = 300, int pause = 300)
{
    if (!duration)
        duration = 300;
    if (!pause)
        pause = duration;
    for (int i = 0; i < n; i++)
    {
        digitalWrite(ALARM_PIN, HIGH);
        delay(duration);
        digitalWrite(ALARM_PIN, LOW);
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

// Helper routine to dump a byte array to serial monitor
void dump_byte_array(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

/****************************************************************************/
// Get the next block number from the blocks array
// @param block current block number
// @param limit limit the search to the first 'limit' blocks
// @return next block number
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

    return (i >= len - 1 || i >= limit) ? blocks[0] : blocks[i + 1];
}

/****************************************************************************/

/**
 * save PAYLOAD to EEPROM
 * @param payload pointer to String containing the data to save
 * @return true if save was successful, false if data was too large
 */
bool savePayloadToEEPROM(String *payload)
{
    // Check if payload is null
    if (payload == nullptr)
    {
        Serial.println("Error: Null payload pointer");
        return false;
    }

    // Check if data fits in EEPROM
    int dataLength = payload->length();
    int maxEEPROMSize = EEPROM.length();

    if (dataLength >= maxEEPROMSize)
    {
        Serial.println("Error: Payload too large for EEPROM (" + String(dataLength) + " bytes, max " + String(maxEEPROMSize - 1) + ")");
        return false;
    }

    Serial.println("Saving " + String(dataLength) + " bytes to EEPROM...");

    // Clear all EEPROM (optional optimization: only clear what we need)
    for (int i = 0; i < maxEEPROMSize; i++)
    {
        EEPROM.write(i, 0);
    }

    // Write new data directly - much more efficient
    for (int i = 0; i < dataLength; i++)
    {
        char c = payload->charAt(i);

        // Optional: validate character (only save printable ASCII)
        if (c >= 32 && c <= 126)
        {
            EEPROM.write(i, c);
        }
        else
        {
            Serial.println("Warning: Non-printable character at position " + String(i) + ", skipping");
            EEPROM.write(i, '?'); // Replace with placeholder
        }
    }

    // Ensure null termination
    if (dataLength < maxEEPROMSize)
    {
        EEPROM.write(dataLength, 0);
    }

    // Optional: commit changes if using ESP32/ESP8266
    // EEPROM.commit();

    Serial.println("Successfully saved " + String(dataLength) + " bytes to EEPROM");
    return true;
}

/****************************************************************************/

/**
 * load PAYLOAD from EEPROM
 * @param payload pointer to String where the loaded data will be stored
 */
void loadPayloadFromEEPROM(String *payload)
{
    // Clear the payload string first
    *payload = "";

    // Read data from EEPROM with safety limits
    int i = 0;
    int maxLength = min(EEPROM.length(), 512); // Limit to reasonable size (512 bytes max)

    while (i < maxLength)
    {
        char c = EEPROM.read(i);

        // Stop if null character is found
        if (c == 0)
            break;

        // Only add printable ASCII characters (32-126) to avoid corrupted data
        if (c >= 32 && c <= 126)
        {
            (*payload) += c;
        }
        else
        {
            // If we encounter non-printable character, it might be corrupted data
            // Stop reading to avoid garbage
            Serial.println("Warning: Non-printable character found in EEPROM at position " + String(i));
            break;
        }

        i++;

        // Additional safety: stop if string gets too long
        if (payload->length() > 500)
        {
            Serial.println("Warning: EEPROM data too long, truncating at 500 characters");
            break;
        }
    }

    // Debug output
    Serial.println("Loaded " + String(payload->length()) + " characters from EEPROM");
}

/****************************************************************************/

// @brief Activates or deactivates action and alarm pins based on validity, with an optional duration.
// If the input 'valid' is true, this function activates both the action and alarm pins (HIGH).
// If a 'duration' is specified (greater than 0), the pins remain active for that duration (in milliseconds)
// before being deactivated (LOW). If 'valid' is false, both action and alarm pins are immediately deactivated.
// @param valid Boolean indicating whether to activate the pins (true) or deactivate them (false).
// @param duration Optional duration (in milliseconds) for which the pins should remain active. Default is 500ms. If 0, the pins will remain active until manually deactivated.
// @note ACTION_PIN and ALARM_PIN must be defined elsewhere in the code.
void openSesame(bool valid, int duration = 500)
{
    if (valid)
    {
        digitalWrite(ACTION_PIN, HIGH); // Attiva il pin di azione
        digitalWrite(ALARM_PIN, HIGH);  // Attiva il pin di allarme

        // Se è specificata una durata, attiva il pin per quel periodo
        if (duration > 0)
        {
            delay(duration);
            digitalWrite(ACTION_PIN, LOW); // Disattiva il pin di azione
            digitalWrite(ALARM_PIN, LOW);  // Disattiva il pin di allarme
        }
    }
    else // disattiva entrambi i pin quando non valido
    {
        digitalWrite(ACTION_PIN, LOW);
        digitalWrite(ALARM_PIN, LOW);
    }
}

/****************************************************************************/

// @brief Triggers an error state by setting the ERROR_PIN high and waits for a button press to reset.
// This function sets the ERROR_PIN to HIGH, indicating an error state. It then enters a loop,
// waiting for a button press. When the specified button is pressed, the error state is cleared
// by setting the ERROR_PIN to LOW, and the loop terminates. Additionally, it calls openSesame(false)
// to perform any necessary actions related to closing or deactivating a mechanism.
// @param btn A pointer to the DagButton object that triggers the reset.
// @param fired A boolean flag indicating whether the error has been initially triggered. The loop continues as long as this flag is true.
void triggerErrorAndWaitForReset(DagButton *btn, bool fired)
{
    digitalWrite(ERROR_PIN, HIGH); // set the error pin to HIGH
    while (fired)
    {
        if (btn->pressed())
        {
            fired = false;                // reset the flag
            digitalWrite(ERROR_PIN, LOW); // reset the error pin
        }
        delay(100);
    }
    // lcd_idle(&lcd, mode, block);
    openSesame(false);
}

#endif