/**
 * @file def.cpp
 * @brief Implementation of utility functions and global variables for RFID Box Writer
 * @details This file contains the implementations of all utility functions and definitions
 *          of global variables declared in def.h. This separation follows C++ best practices
 *          for better compilation performance and code organization.
 * @author Dag
 */

#include "def.h"
#include "dag-button.h"

// ============================================================================
// GLOBAL VARIABLE DEFINITIONS
// ============================================================================

/**
 * @brief MIFARE Classic Authentication Key
 * @details Custom 6-byte key for MIFARE Classic card authentication.
 *          Alternative to factory default key (FFFFFFFFFFFF).
 */
byte cryptokey[MFRC522::MF_KEY_SIZE] = {0x01, 0x02, 0x13, 0x51, 0x09, 0x0F};

/**
 * @brief Primary Block Array for Data Storage
 * @details List of MIFARE Classic 1K blocks available for passphrase storage.
 *          Each block contains 16 bytes of data, totaling 720 bytes (45 blocks × 16 bytes).
 */
int blocks[] = {
    4, 5, 6,    // Sector 1 - Data blocks (Block 7 is sector trailer)
    8, 9, 10,   // Sector 2 - Data blocks (Block 11 is sector trailer)
    12, 13, 14, // Sector 3 - Data blocks (Block 15 is sector trailer)
    16, 17, 18, // Sector 4 - Data blocks (Block 19 is sector trailer)
    20, 21, 22, // Sector 5 - Data blocks (Block 23 is sector trailer)
    24, 25, 26, // Sector 6 - Data blocks (Block 27 is sector trailer)
    28, 29, 30, // Sector 7 - Data blocks (Block 31 is sector trailer)
    32, 33, 34, // Sector 8 - Data blocks (Block 35 is sector trailer)
    36, 37, 38, // Sector 9 - Data blocks (Block 39 is sector trailer)
    40, 41, 42, // Sector 10 - Data blocks (Block 43 is sector trailer)
    44, 45, 46, // Sector 11 - Data blocks (Block 47 is sector trailer)
    48, 49, 50, // Sector 12 - Data blocks (Block 51 is sector trailer)
    52, 53, 54, // Sector 13 - Data blocks (Block 55 is sector trailer)
    56, 57, 58, // Sector 14 - Data blocks (Block 59 is sector trailer)
    60, 61, 62  // Sector 15 - Data blocks (Block 63 is sector trailer)
};

/**
 * @brief Individual Sector Block Arrays
 * @details Pre-defined arrays for accessing individual sectors if needed.
 */
int sector1[] = {4, 5, 6};     // Sector 1 data blocks (48 bytes capacity)
int sector2[] = {8, 9, 10};    // Sector 2 data blocks (48 bytes capacity)
int sector3[] = {12, 13, 14};  // Sector 3 data blocks (48 bytes capacity)
int sector4[] = {16, 17, 18};  // Sector 4 data blocks (48 bytes capacity)
int sector5[] = {20, 21, 22};  // Sector 5 data blocks (48 bytes capacity)
int sector6[] = {24, 25, 26};  // Sector 6 data blocks (48 bytes capacity)
int sector7[] = {28, 29, 30};  // Sector 7 data blocks (48 bytes capacity)
int sector8[] = {32, 33, 34};  // Sector 8 data blocks (48 bytes capacity)
int sector9[] = {36, 37, 38};  // Sector 9 data blocks (48 bytes capacity)
int sector10[] = {40, 41, 42}; // Sector 10 data blocks (48 bytes capacity)
int sector11[] = {44, 45, 46}; // Sector 11 data blocks (48 bytes capacity)
int sector12[] = {48, 49, 50}; // Sector 12 data blocks (48 bytes capacity)
int sector13[] = {52, 53, 54}; // Sector 13 data blocks (48 bytes capacity)
int sector14[] = {56, 57, 58}; // Sector 14 data blocks (48 bytes capacity)
int sector15[] = {60, 61, 62}; // Sector 15 data blocks (48 bytes capacity)

// ============================================================================
// AUDIO FEEDBACK SYSTEM IMPLEMENTATION
// ============================================================================

void beep(int n, int duration, int pause)
{
    // Ensure minimum valid duration values
    if (!duration)
        duration = 300;
    if (!pause)
        pause = duration;

    // Generate the requested number of beeps
    for (int i = 0; i < n; i++)
    {
        digitalWrite(ALARM_PIN, HIGH); // Activate alarm/buzzer
        delay(duration);               // Hold active state
        digitalWrite(ALARM_PIN, LOW);  // Deactivate alarm/buzzer
        delay(pause);                  // Pause between beeps
    }
}

// ============================================================================
// DATA CONVERSION UTILITY FUNCTIONS IMPLEMENTATION
// ============================================================================

String uidToString(MFRC522::Uid uid)
{
    String str = "";
    for (byte i = 0; i < uid.size; i++)
    {
        // Add leading space and zero-pad single-digit hex values
        str += (uid.uidByte[i] < 0x10 ? F(" 0") : F(" "));
        str += String(uid.uidByte[i], HEX);
    }
    return str;
}

void stringToBuffer(String str, byte *buffer)
{
    byte bufferSize = str.length();

    for (byte i = 0; i < bufferSize; i++)
    {
        buffer[i] = str[i]; // Direct ASCII to byte conversion
    }
}

String bufferToString(byte *buffer, byte bufferSize)
{
    String str = "";
    for (byte i = 0; i < bufferSize; i++)
    {
        if (buffer[i] != 0x00) // Skip null bytes (end markers or padding)
            str += (char)buffer[i];
    }
    return str;
}

void dump_byte_array(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        // Add leading space and zero-pad single-digit hex values
        Serial.print(buffer[i] < 0x10 ? F(" 0") : F(" "));
        Serial.print(buffer[i], HEX);
    }
}

// ============================================================================
// BLOCK NAVIGATION UTILITY IMPLEMENTATION
// ============================================================================

int nextBlock(int block, int limit)
{
    int len = BLOCKS_COUNT; // Use the constant instead of sizeof
    int i = 0;

    // Search for current block in the array
    while (block != blocks[i])
    {
        i++;
        if (i >= len || i >= limit)
            break;
    }

    // Return next block or wrap to beginning
    return (i >= len - 1 || i >= limit) ? blocks[0] : blocks[i + 1];
}

// ============================================================================
// EEPROM PERSISTENT STORAGE MANAGEMENT IMPLEMENTATION
// ============================================================================

bool savePayloadToEEPROM(String *payload)
{
    // Validate input parameter
    if (payload == nullptr)
    {
        Serial.println(F("Error: Null payload pointer"));
        return false;
    }

    // Check available EEPROM space
    int dataLength = payload->length();
    int maxEEPROMSize = EEPROM.length();

    if (dataLength >= maxEEPROMSize)
    {
        Serial.print(F("Error: Payload too large for EEPROM ("));
        Serial.print(dataLength);
        Serial.print(F(" bytes, max "));
        Serial.print(maxEEPROMSize - 1);
        Serial.println(F(")"));
        return false;
    }

    Serial.print(F("Saving "));
    Serial.print(dataLength);
    Serial.println(F(" bytes to EEPROM..."));

    // Clear entire EEPROM to ensure clean state
    for (int i = 0; i < maxEEPROMSize; i++)
    {
        EEPROM.write(i, 0);
    }

    // Write payload data with character validation
    for (int i = 0; i < dataLength; i++)
    {
        char c = payload->charAt(i);

        // Validate printable ASCII characters only (security measure)
        if (c >= 32 && c <= 126)
        {
            EEPROM.write(i, c);
        }
        else
        {
            Serial.print(F("Warning: Non-printable character at position "));
            Serial.print(i);
            Serial.println(F(", replacing with '?'"));
            EEPROM.write(i, '?'); // Replace invalid characters with placeholder
        }
    }

    // Ensure proper null termination
    if (dataLength < maxEEPROMSize)
    {
        EEPROM.write(dataLength, 0);
    }

    Serial.print(F("Successfully saved "));
    Serial.print(dataLength);
    Serial.println(F(" bytes to EEPROM"));
    return true;
}

void loadPayloadFromEEPROM(String *payload)
{
    // Initialize output string
    *payload = "";

    // Read EEPROM data with safety limits
    int i = 0;
    int maxLength = min(EEPROM.length(), 512); // Reasonable limit to prevent memory issues

    while (i < maxLength)
    {
        char c = EEPROM.read(i);

        // Stop at null terminator (proper end of data)
        if (c == 0)
            break;

        // Validate printable ASCII characters (corruption detection)
        if (c >= 32 && c <= 126)
        {
            (*payload) += c;
        }
        else
        {
            // Invalid character suggests corruption - stop reading
            Serial.print(F("Warning: Non-printable character found in EEPROM at position "));
            Serial.println(i);
            break;
        }

        i++;

        // Additional safety: prevent excessive memory usage
        if (payload->length() > 500)
        {
            Serial.println(F("Warning: EEPROM data too long, truncating at 500 characters"));
            break;
        }
    }

    // Report loading status
    Serial.print(F("Loaded "));
    Serial.print(payload->length());
    Serial.println(F(" characters from EEPROM"));
}

// ============================================================================
// ERROR HANDLING AND USER INTERACTION IMPLEMENTATION
// ============================================================================

void triggerErrorAndWaitForReset(DagButton *btn, bool *fired)
{
    digitalWrite(ERROR_PIN, HIGH); // Activate error state indicator

    // Block execution until user acknowledges error
    while (*fired)
    {
        if (btn->pressed())
        {
            *fired = false;               // Clear error state flag
            digitalWrite(ERROR_PIN, LOW); // Deactivate error indicator
        }
        delay(100); // Prevent excessive polling and reduce power consumption
    }
}