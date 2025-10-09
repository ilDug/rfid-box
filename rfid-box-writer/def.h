/**
 * @file def.h
 * @brief System definitions, constants, and utility functions for RFID Box Writer
 * @details This header file contains all hardware pin definitions, system enumerations,
 *          MIFARE Classic memory layout mappings, and utility functions used throughout
 *          the RFID Box Writer system. It serves as the central configuration file
 *          for hardware abstraction and system constants.
 * @author Dag
 * @version 1.0.0
 */

#ifndef DAG_CONSTANTS_H
#define DAG_CONSTANTS_H

#include <MFRC522.h>
#include <EEPROM.h>

// ============================================================================
// HARDWARE PIN DEFINITIONS
// ============================================================================

/**
 * @brief MFRC522 RFID Reader Pin Configuration
 * @details Pin mapping for MFRC522 RFID reader using Arduino's Hardware SPI interface.
 *          MOSI (11), MISO (12), and SCK (13) are fixed by SPI hardware.
 *          Only SS (Slave Select) and RST (Reset) pins are configurable.
 *
 * Pin Mapping Table:
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
 * -----------------------------------------------------------------------------------------
 */
const int SS_PIN = 10; // RFID Slave Select Pin - Controls RFID module SPI communication
const int RST_PIN = 9; // RFID Reset Pin - Hardware reset line for RFID module

/**
 * @brief User Interface Pin Definitions
 * @details Physical buttons for user interaction and system control
 */
const int BTN_MODE_PIN = 5;  // Mode Toggle Button - Short press: READ/WRITE, Long press: RUN/SET
const int BTN_RESET_PIN = 4; // Reset/Confirm Button - Resets error states and confirms operations

/**
 * @brief System Output Pin Definitions
 * @details Output pins for controlling external devices and providing user feedback
 */
const int ACTION_PIN = 2; // Primary Action Output - Controls main access mechanism (relay, lock, etc.)
const int ALARM_PIN = 6;  // Audio/Visual Alarm - Provides beep feedback and status indication
const int ERROR_PIN = 3;  // Error State Indicator - Signals system errors and failures

/**
 * @brief Additional Signal Pins (Currently Unused)
 * @details Reserved pins for future expansion or additional functionality
 */
const int SIGNAL1_PIN = A1; // Reserved Signal Pin 1 - Available for future features
const int SIGNAL2_PIN = A2; // Reserved Signal Pin 2 - Available for future features

/**
 * @brief MIFARE Classic Authentication Key
 * @details Custom 6-byte key for MIFARE Classic card authentication.
 *          Alternative to factory default key (FFFFFFFFFFFF).
 *          Used for enhanced security when reading/writing card data.
 */
byte cryptokey[MFRC522::MF_KEY_SIZE] = {0x01, 0x02, 0x13, 0x51, 0x09, 0x0F};

// ============================================================================
// SYSTEM STATE ENUMERATIONS
// ============================================================================

/**
 * @brief Operational Mode Enumeration
 * @details Defines the primary operational modes of the RFID system
 */
enum Mode
{
    MODE_READ, // Card Validation Mode - Reads cards and validates against stored passphrase
    MODE_WRITE // Card Programming Mode - Writes current passphrase to blank cards
};

/**
 * @brief Job Type Enumeration
 * @details Defines the job context within each operational mode
 */
enum Job
{
    RUN, // Normal Operation - Standard read/write operations using stored passphrase
    SET  // Configuration Mode - Updates the master passphrase (READ mode only)
};

/**
 * @brief Agent Type Enumeration
 * @details Identifies the specific device variant in multi-device systems
 */
enum Agent
{
    AGENT_READER, // Reader-only device variant (if implemented)
    AGENT_WRITER  // Writer-capable device variant (current implementation)
};

// ============================================================================
// MIFARE CLASSIC MEMORY LAYOUT CONFIGURATION
// ============================================================================

/**
 * @brief Primary Block Array for Data Storage
 * @details List of MIFARE Classic 1K blocks available for passphrase storage.
 *          Each block contains 16 bytes of data, totaling 720 bytes (45 blocks × 16 bytes).
 *          This provides storage for approximately 720 ASCII characters.
 *
 * Memory Structure:
 * - MIFARE Classic 1K has 64 blocks total (0-63)
 * - Blocks 0-3: Manufacturer data and UID (read-only)
 * - Blocks 4-62: User data (configured below)
 * - Block 63: Additional manufacturer data
 * - Every 4th block (7, 11, 15, etc.) contains sector keys and access bits
 *
 * Security Note: Blocks containing sector trailers (keys/access bits) are
 * excluded from this array to prevent accidental key overwriting.
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
 *          Each sector contains 3 data blocks (48 bytes total per sector).
 *          These arrays are available for sector-specific operations but
 *          are currently unused in favor of the main blocks[] array.
 *
 * @note Each sector's 4th block (sector trailer) is intentionally excluded
 *       as it contains authentication keys and access control bits.
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
// AUDIO FEEDBACK SYSTEM
// ============================================================================

/**
 * @brief Generate audio feedback using alarm pin
 * @details Produces a sequence of beeps through the ALARM_PIN for user feedback.
 *          Used to indicate system states, confirmations, and error conditions.
 *
 * @param n Number of beeps to generate (1-10 recommended)
 * @param duration Duration of each beep in milliseconds (default: 300ms)
 * @param pause Pause between beeps in milliseconds (default: same as duration)
 *
 * Common Usage Patterns:
 * - beep(1): Single confirmation beep
 * - beep(3): Error indication
 * - beep(5): Mode change confirmation
 * - beep(1, 1000): Long success confirmation
 * - beep(1, 250, 50): Short SET mode indicator
 */
void beep(int n, int duration = 300, int pause = 300)
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
// DATA CONVERSION UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Convert RFID card UID to readable string format
 * @details Transforms the binary UID data into a human-readable hexadecimal string
 *          with proper spacing and zero-padding for consistent formatting.
 *
 * @param uid MFRC522::Uid structure containing the card's unique identifier
 * @return String formatted UID (e.g., " 04 A1 B2 C3" for a 4-byte UID)
 *
 * @note Output format includes leading spaces and zero-padding for values < 0x10
 */
String uidToString(MFRC522::Uid uid)
{
    String str = "";
    for (byte i = 0; i < uid.size; i++)
    {
        // Add leading space and zero-pad single-digit hex values
        str += (uid.uidByte[i] < 0x10 ? " 0" : " ");
        str += uid.uidByte[i];
    }
    return str;
}

/**
 * @brief Convert ASCII string to byte array for RFID writing
 * @details Converts a String object into a byte array suitable for writing
 *          to MIFARE Classic blocks. Each character becomes one byte.
 *
 * @param str Source string to convert (ASCII characters only)
 * @param buffer Destination byte array (must be pre-allocated with sufficient size)
 *
 * @warning Buffer must be large enough to hold str.length() bytes
 * @note Non-ASCII characters may not convert correctly
 */
void stringToBuffer(String str, byte *buffer)
{
    byte bufferSize = str.length();

    for (byte i = 0; i < bufferSize; i++)
    {
        buffer[i] = str[i]; // Direct ASCII to byte conversion
    }
}

/**
 * @brief Convert byte array back to ASCII string
 * @details Reconstructs a String from binary data read from RFID cards.
 *          Automatically filters out null bytes (0x00) which indicate
 *          end of data or unused space in MIFARE blocks.
 *
 * @param buffer Source byte array containing ASCII data
 * @param bufferSize Number of bytes to process from the buffer
 * @return String containing only non-null ASCII characters
 *
 * @note Null bytes (0x00) are skipped to avoid string termination issues
 */
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

/**
 * @brief Display byte array in hexadecimal format for debugging
 * @details Outputs binary data to Serial Monitor in formatted hex representation.
 *          Useful for analyzing raw RFID data and debugging read/write operations.
 *
 * @param buffer Byte array to display
 * @param bufferSize Number of bytes to display from the buffer
 *
 * @note Output format: " 04 A1 B2 C3 FF 00" (space-separated hex bytes)
 * @note Single-digit hex values are zero-padded for consistent formatting
 */
void dump_byte_array(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++)
    {
        // Add leading space and zero-pad single-digit hex values
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

// ============================================================================
// BLOCK NAVIGATION UTILITY (CURRENTLY UNUSED)
// ============================================================================

/**
 * @brief Get next block number in sequence from blocks array
 * @details Utility function to navigate through the blocks array sequentially.
 *          Currently unused in the main implementation but available for
 *          future enhancements requiring block-by-block processing.
 *
 * @param block Current block number to find in the array
 * @param limit Maximum number of blocks to search (default: 64)
 * @return Next block number in sequence, or first block if at end/not found
 *
 * @note Returns blocks[0] if current block is the last one or not found
 * @note Useful for implementing circular block navigation if needed
 */
int nextBlock(int block, int limit = 64)
{
    int len = sizeof(blocks) / sizeof(blocks[0]);
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
// EEPROM PERSISTENT STORAGE MANAGEMENT
// ============================================================================

/**
 * @brief Save passphrase data to Arduino's EEPROM memory
 * @details Stores passphrase string in non-volatile EEPROM for persistence across
 *          power cycles. Includes safety checks, data validation, and null termination.
 *          Clears entire EEPROM before writing to ensure clean state.
 *
 * @param payload Pointer to String containing passphrase to save
 * @return true if save operation succeeded, false if failed
 *
 * Safety Features:
 * - Null pointer validation
 * - Size limit checking (prevents EEPROM overflow)
 * - ASCII character validation (printable characters only)
 * - Automatic null termination
 * - Complete EEPROM clearing before write
 *
 * @warning Clears entire EEPROM memory before writing new data
 * @note Maximum storage depends on Arduino model (typically 512-4096 bytes)
 */
bool savePayloadToEEPROM(String *payload)
{
    // Validate input parameter
    if (payload == nullptr)
    {
        Serial.println("Error: Null payload pointer");
        return false;
    }

    // Check available EEPROM space
    int dataLength = payload->length();
    int maxEEPROMSize = EEPROM.length();

    if (dataLength >= maxEEPROMSize)
    {
        Serial.println("Error: Payload too large for EEPROM (" + String(dataLength) + " bytes, max " + String(maxEEPROMSize - 1) + ")");
        return false;
    }

    Serial.println("Saving " + String(dataLength) + " bytes to EEPROM...");

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
            Serial.println("Warning: Non-printable character at position " + String(i) + ", replacing with '?'");
            EEPROM.write(i, '?'); // Replace invalid characters with placeholder
        }
    }

    // Ensure proper null termination
    if (dataLength < maxEEPROMSize)
    {
        EEPROM.write(dataLength, 0);
    }

    // Note: EEPROM.commit() may be required on ESP32/ESP8266 platforms

    Serial.println("Successfully saved " + String(dataLength) + " bytes to EEPROM");
    return true;
}

/**
 * @brief Load passphrase data from Arduino's EEPROM memory
 * @details Reads stored passphrase from EEPROM with multiple safety mechanisms
 *          to prevent corruption and buffer overflows. Stops reading at null
 *          terminator or when invalid data is encountered.
 *
 * @param payload Pointer to String where loaded data will be stored
 *
 * Safety Features:
 * - Maximum read length limiting (512 bytes)
 * - Non-printable character detection and rejection
 * - Automatic string length limiting (500 characters max)
 * - Null terminator detection
 * - Corruption detection and early termination
 *
 * @note Automatically clears target string before loading
 * @note Stops reading if corrupted/non-printable data is found
 */
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
            Serial.println("Warning: Non-printable character found in EEPROM at position " + String(i));
            break;
        }

        i++;

        // Additional safety: prevent excessive memory usage
        if (payload->length() > 500)
        {
            Serial.println("Warning: EEPROM data too long, truncating at 500 characters");
            break;
        }
    }

    // Report loading status
    Serial.println("Loaded " + String(payload->length()) + " characters from EEPROM");
}

// ============================================================================
// ERROR HANDLING AND USER INTERACTION
// ============================================================================

/**
 * @brief Enter error state and wait for user acknowledgment
 * @details Activates the error indicator and enters a blocking loop until the user
 *          presses the reset button to acknowledge the error condition. This provides
 *          a fail-safe mechanism that prevents the system from continuing operation
 *          when errors occur.
 *
 * @param btn Pointer to DagButton object that will trigger the reset/acknowledgment
 * @param fired Pointer to boolean flag indicating error state is active
 *
 * Behavior:
 * 1. Sets ERROR_PIN to HIGH (activates error indicator LED/buzzer)
 * 2. Enters blocking loop while error flag is true
 * 3. Continuously polls reset button for user acknowledgment
 * 4. When button pressed: clears error flag and deactivates error indicator
 * 5. Returns control to main program after user acknowledgment
 *
 * @note This is a blocking function - system will not proceed until user input
 * @note Used for critical errors that require explicit user acknowledgment
 * @note 100ms delay prevents excessive CPU usage during button polling
 */
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

    // Note: LCD idle display could be restored here if implemented
    // lcd_idle(&lcd, mode, block);
}

#endif // DAG_CONSTANTS_H