/**
 * @file def.h
 * @brief System definitions, constants, and utility functions for RFID Box Writer
 * @details This header file contains all hardware pin definitions, system enumerations,
 *          MIFARE Classic memory layout mappings, and utility functions used throughout
 *          the RFID Box Writer system. It serves as the central configuration file
 *          for hardware abstraction and system constants.
 * @author Dag
 */

#ifndef DAG_CONSTANTS_H
#define DAG_CONSTANTS_H

#include <MFRC522.h>
#include <EEPROM.h>

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

// Forward declaration for DagButton class to avoid circular includes
class DagButton;

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
 * @brief Primary Block Array for Data Storage - Declaration
 * @details External declaration of the main blocks array defined in def.cpp
 */
extern int blocks[];

/**
 * @brief Trailer Blocks Array - Declaration
 * @details External declaration of the trailerBlocks array defined in def.cpp
 */
extern int trailerBlocks[];

/**
 * @brief Number of blocks in the main blocks array
 * @details Total number of data blocks available for passphrase storage
 *          This constant allows for proper array size calculation in other files
 */
const int BLOCKS_COUNT = 45; // 15 sectors × 3 data blocks per sector

/**
 * @brief Number of trailer blocks in the trailerBlocks array
 * @details Total number of sector trailer blocks used for key and access bit storage
 */
const int TRAILER_BLOCKS_COUNT = 15; // 15 sectors

// ============================================================================
// FUNCTION DECLARATIONS
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
void beep(int n, int duration = 300, int pause = 300);

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
String uidToString(MFRC522::Uid uid);

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
void stringToBuffer(String str, byte *buffer);

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
String bufferToString(byte *buffer, byte bufferSize);

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
void dump_byte_array(byte *buffer, byte bufferSize);

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
int nextBlock(int block, int limit = 64);

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
bool savePayloadToEEPROM(String *payload);

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
void loadPayloadFromEEPROM(String *payload);

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
void triggerErrorAndWaitForReset(DagButton *btn, bool *fired);

// +===========================================================================
// KEY MANAGEMENT FUNCTION DECLARATIONS
// +===========================================================================

/**
 * Function to change the sector key for a specific MIFARE Classic sector
 * @param trailerBlock The trailer block number of the sector to change
 * @param mfrc522 Reference to the MFRC522 instance used for communication
 * @param newKey Pointer to the new 6-byte key to set for the sector
 * @param oldKey Pointer to the current 6-byte key used for authentication
 * @param accessBits Pointer to the 4-byte access bits array to set for the sector
 * @return true if the key change was successful, false otherwise
 */
bool changeSectorKey(byte trailerBlock, MFRC522 &mfrc522, byte *newKey, byte *oldKey, byte *accessBits);

#endif // DAG_CONSTANTS_H