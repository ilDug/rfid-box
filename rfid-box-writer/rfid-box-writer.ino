/**
 * @file rfid-box-writer.ino
 * @brief RFID Box Writer - Secure Access Control System
 * @details This Arduino sketch implements a dual-mode RFID system that can both read and write
 *          MIFARE Classic cards for access control. The system supports two operational modes:
 *          - READ mode: Validates cards against stored passphrase for access control
 *          - WRITE mode: Programs new cards with the current passphrase
 *          Additionally, it supports SET mode for updating the master passphrase.
 * @author Dag
 */

// Required libraries for RFID, LCD, and system functionality
#include <SPI.h>        // SPI communication for RFID module
#include <MFRC522.h>    // RFID library - https://github.com/miguelbalboa/rfid
#include <Wire.h>       // I2C communication for LCD
#include <LCD_I2C.h>    // LCD display library
#include "dag-button.h" // Custom button library
#include "dag-timer.h"  // Custom timer library for periodic tasks
#include "def.h"        // Pin definitions, constants, and utility functions
#include "data.h"       // Data storage
#include "lcd.h"        // LCD display management functions

// ============================================================================
// HARDWARE INITIALIZATION
// ============================================================================

// BUTTON objects for user interaction
DagButton btnMode(BTN_MODE_PIN, PULLUP);   // Button to toggle between READ/WRITE modes
DagButton btnReset(BTN_RESET_PIN, PULLUP); // Button to reset system state and confirm operations

// TIMER for visual/audio feedback during SET mode
DagTimer blinkTimer; // Generates periodic signals to indicate SET mode is active

// RFID hardware components
MFRC522 rfid(SS_PIN, RST_PIN); // RFID reader instance using SPI communication
MFRC522::MIFARE_Key key;       // Cryptographic key for card authentication (read/write operations)

// LCD display for user feedback (16x2 character display)
LCD_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 columns, 2 rows (SDA=A4, SCL=A5)

// ============================================================================
// SYSTEM STATE VARIABLES
// ============================================================================

Mode MODE = MODE_READ;      // Current operational mode: READ (validate cards) or WRITE (program cards)
Job JOB = RUN;              // Current job type: RUN (normal operation) or SET (passphrase programming)
Agent AGENT = AGENT_WRITER; // Device role identifier (WRITER variant of the RFID box system)

// ============================================================================
// RUNTIME STATE FLAGS AND DATA
// ============================================================================

bool fired = false;     // Flag indicating a card has been detected and is being processed
String value;           // Temporary storage for data read from current card (up to 16 chars per block)
String passphrase = ""; // Master passphrase loaded from EEPROM for card validation
String uid;             // Unique identifier of the currently detected card
bool VALID = false;     // Flag indicating whether the current card contains valid passphrase

// Forward declarations of main FUNCTIONS
String readTag(int *blocksArray, int blocksCount);
bool writeTag(String data, int *blocksArray, int blocksCount);
void executeAction(bool valid);

/**
 * @brief System initialization and hardware setup
 * @details Initializes all hardware components, loads configuration from EEPROM,
 *          and prepares the system for normal operation. This function runs once
 *          at startup and sets up:
 *          - Serial communication for debugging
 *          - SPI bus and RFID reader
 *          - GPIO pins for outputs (action, alarm, error)
 *          - Timer for SET mode indication
 *          - RFID authentication key
 *          - Master passphrase from EEPROM
 */
void setup()
{
    // Initialize communication interfaces
    Serial.begin(9600);    // Start serial communication for debugging and status output
    SPI.begin();           // Initialize SPI bus for RFID module communication
    rfid.PCD_Init();       // Initialize the MFRC522 RFID reader
    blinkTimer.init(2000); // Setup periodic timer (2000ms intervals) for SET mode indication

    // Configure output pins for system feedback
    pinMode(ACTION_PIN, OUTPUT); // Main action output (e.g., relay control, lock mechanism)
    pinMode(ALARM_PIN, OUTPUT);  // Audio/visual alarm for status indication
    pinMode(ERROR_PIN, OUTPUT);  // Error state indicator

    // Display system information via serial
    Serial.print(F("RFID Box "));
    Serial.println(VERSION);
    Serial.println(F("Reader details:"));
    rfid.PCD_DumpVersionToSerial(); // Display RFID reader hardware information

    // Load master passphrase from persistent storage
    Serial.println(F("reading passphrase from eeprom..."));
    loadPayloadFromEEPROM(&passphrase);
    Serial.print(F("Passphrase: "));
    Serial.println(passphrase);
    Serial.println();

    // Initialize RFID authentication key
    // Using factory default key (FFFFFFFFFFFF) for MIFARE Classic cards
    // Alternative: use custom cryptographic key from def.h
    for (byte i = 0; i < MFRC522::MF_KEY_SIZE; i++)
        key.keyByte[i] = 0xFF;
    // key.keyByte[i] = cryptokey[i]; // Uncomment to use custom key

    // Initialize LCD display and show welcome message
    lcd_init(&lcd, VERSION);
    lcd_idle(&lcd, MODE, JOB);

    // Initialize system state
    executeAction(false); // Ensure all outputs are in safe/inactive state
}

/**
 * @brief Main program loop - handles user input and card detection
 * @details This function runs continuously and manages:
 *          - Button press detection and mode switching
 *          - RFID card detection and processing
 *          - System state management (RUN/SET, READ/WRITE modes)
 *          - Card authentication and data operations
 *          The loop implements a state machine that responds to user input
 *          and processes RFID cards according to the current operational mode.
 */
void loop()
{
    // ========================================================================
    // USER INPUT HANDLING
    // ========================================================================

    // Handle mode switching: short press toggles READ/WRITE mode
    btnMode.onPress(toggleMode);

    // Handle job switching: long press (3s) toggles RUN/SET mode
    btnMode.onLongPress(toggleJob, 3000);

    // Provide visual/audio feedback when in SET mode (passphrase programming)
    blinkTimer.run(blinkIfSetMode);

    // ========================================================================
    // RFID CARD DETECTION
    // ========================================================================

    // Check for presence of new RFID card - exit if none detected
    if (!rfid.PICC_IsNewCardPresent())
        return;

    // Attempt to read card serial number - exit if communication fails
    if (!rfid.PICC_ReadCardSerial())
    {
        Serial.println(F("Failed to read card serial."));
        lcd_uid_reading_error(&lcd);
        beep(3); // Triple beep indicates read error
        delay(1000);
        lcd_idle(&lcd, MODE, JOB);
        return;
    }

    // ========================================================================
    // CARD PROCESSING BEGINS
    // ========================================================================

    fired = true;                // Set flag indicating card processing is active
    uid = uidToString(rfid.uid); // Get the UID of the card as a string
    Serial.print(F("Card detected UID: "));
    Serial.println(uid); // Log card detection event
    Serial.println();

    // ========================================================================
    // READ MODE PROCESSING
    // ========================================================================
    if (MODE == MODE_READ)
    {
        // Verify card compatibility with MIFARE Classic standard
        if (!checkCompatibility())
        {
            beep(3);                       // Triple beep indicates compatibility error
            lcd_compatibility_error(&lcd); // Display compatibility error on LCD
            triggerErrorAndWaitForReset(&btnReset, &fired);
            lcd_idle(&lcd, MODE, JOB);
            return;
        }

        // Special debug feature: dump all card data when reset button is held
        if (btnReset.clicked())
        {
            rfid.PICC_DumpToSerial(&(rfid.uid)); // Output complete card structure to serial
            beep(1, 1000);                       // Long beep indicates dump completed
            lcd_show_uid(&lcd, uid);             // Display UID on LCD

            while (fired)
            {
                if (btnReset.pressed())
                {
                    fired = false; // Clear processing flag
                    lcd_idle(&lcd, MODE, JOB);
                    return;
                }
                delay(100);
            }
        }

        // ====================================================================
        // READ CARD DATA
        // ====================================================================

        value = "";                                           // Clear previous read data
        int blocksCount = sizeof(blocks) / sizeof(blocks[0]); // Calculate total blocks to read
        value = readTag(blocks, blocksCount);                 // Read passphrase from all configured blocks

        // Handle read operation results
        if (value == "")
        {
            // Reading failed - provide error feedback and wait for user reset
            beep(3);                                        // Triple beep indicates read error
            triggerErrorAndWaitForReset(&btnReset, &fired); // Enter error state
            lcd_idle(&lcd, MODE, JOB);
            return;
        }

        // ================================================================
        // SET MODE: PASSPHRASE PROGRAMMING
        // ================================================================
        if (JOB == SET)
        {
            // In SET mode, use the read passphrase to update the master passphrase
            bool saved = savePayloadToEEPROM(&value);

            if (!saved)
            {
                // EEPROM save failed
                beep(3); // Triple beep indicates error
                lcd_EEPROM_writing_error(&lcd);
                triggerErrorAndWaitForReset(&btnReset, &fired);
                JOB = RUN; // Return to normal operation mode
                lcd_idle(&lcd, MODE, JOB);
                return;
            }

            // Passphrase successfully saved - provide confirmation
            passphrase = value; // Update master passphrase with card data
            beep(1, 1000);      // Long success beep
            lcd_passphrase_set_success(&lcd);
            while (fired) // Wait for user acknowledgment via reset button
            {
                if (btnReset.pressed())
                {
                    fired = false; // Clear processing flag
                    JOB = RUN;     // Return to normal operation mode
                    lcd_idle(&lcd, MODE, JOB);
                    return; // ESCE dalla modalità SET dopo che ha settato la passphrase
                }
                delay(100);
            }
        }

        // ================================================================
        // RUN MODE: ACCESS VALIDATION
        // ================================================================
        else if (JOB == RUN)
        {
            // Compare read passphrase with stored master passphrase
            VALID = (value == passphrase);

            if (!VALID)
            {
                // Invalid passphrase - deny access
                beep(3); // Triple beep indicates invalid card
                lcd_invalid_passphrase(&lcd);
                triggerErrorAndWaitForReset(&btnReset, &fired);
                lcd_idle(&lcd, MODE, JOB);
                return;
            }
            else
            {
                // Valid passphrase - grant access
                beep(1, 600);        // Success confirmation beep
                executeAction(true); // Activate access control mechanism
                lcd_reading_success(&lcd);
                delay(3000);
                lcd_idle(&lcd, MODE, JOB);
            }
        }
    }

    // ========================================================================
    // WRITE MODE PROCESSING
    // ========================================================================
    else if (MODE == MODE_WRITE)
    {
        // Verify card compatibility before attempting write operations
        if (!checkCompatibility())
        {
            beep(3); // Triple beep indicates compatibility error
            lcd_compatibility_error(&lcd);
            triggerErrorAndWaitForReset(&btnReset, &fired);
            lcd_idle(&lcd, MODE, JOB);
            return;
        }

        // Write current master passphrase to all configured blocks on the card
        int blocksCount = sizeof(blocks) / sizeof(blocks[0]);
        bool result = writeTag(&passphrase, blocks, blocksCount);

        if (result)
        {
            Serial.println(F("Write operation completed successfully"));
            beep(1, 1000); // Long beep indicates write operation finished
            lcd_writing_success(&lcd);
        }
        else
        {
            Serial.println(F("Write operation failed"));
            beep(3); // Triple beep indicates write error
            // Note: LCD error are already displayed into writeTag function
        }

        // Enter waiting state until user acknowledges with reset button
        triggerErrorAndWaitForReset(&btnReset, &fired);
        lcd_idle(&lcd, MODE, JOB);
    }

    // ========================================================================
    // CLEANUP AND RESET
    // ========================================================================

    // Clear the card processing flag to allow detection of next card
    fired = false;
}

// ============================================================================
// SYSTEM CONTROL FUNCTIONS
// ============================================================================

/**
 * @brief Toggle between READ and WRITE operational modes
 * @details Switches the system between card validation (READ) and card programming (WRITE).
 *          In WRITE mode, the job is automatically forced to RUN to prevent accidental
 *          passphrase modification during card programming operations.
 */
void toggleMode()
{
    MODE = MODE == MODE_READ ? MODE_WRITE : MODE_READ;
    beep(1); // Single beep confirms mode change

    // Security measure: force RUN mode when switching to WRITE to prevent accidents
    if (MODE == MODE_WRITE)
        JOB = RUN;

    lcd_idle(&lcd, MODE, JOB);
    Serial.println(MODE == MODE_READ ? F("Read mode selected") : F("Write mode selected"));
}

/**
 * @brief Toggle between RUN and SET job modes
 * @details Switches between normal operation (RUN) and passphrase programming (SET).
 *          SET mode allows updating the master passphrase by reading it from a card.
 *          This function includes safety measures to prevent accidental passphrase changes.
 */
void toggleJob()
{
    JOB = JOB == RUN ? SET : RUN;

    // Security measure: only allow SET mode in READ mode to prevent write conflicts
    if (MODE == MODE_WRITE)
        JOB = RUN;
    else
        beep(5); // Multiple beeps confirm job mode change (only in READ mode)

    lcd_idle(&lcd, MODE, JOB);
    Serial.println(JOB == RUN ? F("Job: RUN") : F("Job: SET"));
}

/**
 * @brief Provide audio feedback when system is in SET mode
 * @details Called periodically by the blink timer to indicate that the system
 *          is in SET mode (passphrase programming mode). Generates short beeps
 *          to alert the user that the next card read will update the master passphrase.
 */
void blinkIfSetMode()
{
    if (JOB == RUN)
        return; // No indication needed in normal operation mode
    else if (JOB == SET)
        beep(1, 250, 50); // Short, quiet beep indicates SET mode is active

    lcd_idle(&lcd, MODE, JOB);
}

// ============================================================================
// RFID CARD OPERATIONS
// ============================================================================

/**
 * @brief Authenticate with RFID card using Key A
 * @details Performs MIFARE Classic authentication for a specific block using Key A.
 *          This is required before any read or write operation can be performed.
 * @param block The block number to authenticate (0-63 for MIFARE Classic 1K)
 * @return true if authentication successful, false if failed
 */
bool authenticateA(byte block)
{
    MFRC522::StatusCode status;
    status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(rfid.uid));

    if (status != MFRC522::STATUS_OK)
    {
        Serial.print(F("Authentication failed: "));
        Serial.println(rfid.GetStatusCodeName(status));
        return false;
    }
    else
        return true;
}

/**
 * @brief Verify RFID card compatibility with system requirements
 * @details Checks if the detected card is a MIFARE Classic type, which is required
 *          for this system. Other card types (MIFARE Ultralight, NTAG, etc.) are
 *          not supported due to different memory structures and authentication methods.
 * @return true if card is compatible (MIFARE Classic Mini/1K/4K), false otherwise
 */
bool checkCompatibility()
{
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
        piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
        piccType != MFRC522::PICC_TYPE_MIFARE_4K)
    {
        Serial.println(F("This device only works with MIFARE Classic cards."));
        return false;
    }
    else
        return true;
}

/**
 * @brief Read passphrase data from multiple RFID card blocks
 * @details Sequentially reads data from all specified blocks and concatenates them
 *          into a single passphrase string. The function handles authentication,
 *          error checking, and stops reading when empty blocks are encountered.
 *          Each block contains up to 16 bytes of data that are converted to ASCII.
 *
 * @param blocksArray Pointer to array of block numbers to read from
 * @param blocksCount Number of blocks in the array
 * @return Concatenated passphrase string from all blocks, or empty string if error occurred
 *
 * @note The function automatically handles RFID communication cleanup (halt and stop crypto)
 * @note Reading stops early if an empty block is encountered to avoid processing null data
 */
String readTag(int *blocksArray, int blocksCount)
{
    byte len = 18;          // Buffer size: 16 data bytes + 2 CRC bytes
    String finalValue = ""; // Accumulated passphrase from all blocks

    Serial.println(F("Reading data from all blocks..."));
    Serial.println();

    // Process each block in sequence
    for (int i = 0; i < blocksCount; i++)
    {
        byte currentBlock = blocksArray[i];
        byte buffer[len];       // Temporary buffer for block data
        String blockValue = ""; // ASCII content of current block

        // Authenticate before reading each block
        if (!authenticateA(currentBlock))
        {
            Serial.print(F("CRITICAL ERROR: Authentication failed for block "));
            Serial.println(currentBlock);
            Serial.println(F("Stopping read operation due to authentication failure."));
            Serial.println();
            lcd_authentication_error(&lcd);
            return ""; // Authentication failure is critical - abort entire operation
        }

        // Attempt to read data from the authenticated block
        MFRC522::StatusCode status = rfid.MIFARE_Read(currentBlock, buffer, &len);
        if (status != MFRC522::STATUS_OK)
        {
            Serial.print(F("CRITICAL ERROR: Reading failed on block "));
            Serial.print(currentBlock);
            Serial.print(F(": "));
            Serial.println(rfid.GetStatusCodeName(status));
            Serial.println(F("Stopping read operation due to read failure."));
            Serial.println();
            lcd_read_block_error(&lcd, currentBlock);
            return ""; // Read failure is critical - abort entire operation
        }
        else
        {
            // Successfully read block - process the data
            Serial.print(F("Data in block "));
            Serial.print(currentBlock);
            Serial.println(F(":"));
            dump_byte_array(buffer, len - 2); // Display raw hex data (excluding CRC)
            Serial.println();

            // Convert binary data to ASCII string
            blockValue = bufferToString(buffer, len - 2);
            blockValue.trim(); // Remove leading/trailing whitespace
            Serial.print(F("Block "));
            Serial.print(currentBlock);
            Serial.print(F(" content: "));
            Serial.println(blockValue);
            Serial.println();

            // Check for empty block - indicates end of passphrase data
            if (blockValue.length() == 0)
            {
                Serial.print(F("Found empty block "));
                Serial.print(currentBlock);
                Serial.println(F(", stopping read operation."));
                Serial.println();
                break; // Stop reading when empty block is found
            }

            // Append block content to final passphrase
            finalValue += blockValue;
        }
    }

    // Clean up the final result
    finalValue.trim();

    Serial.print(F("Final concatenated value: "));
    Serial.println(finalValue);
    Serial.println();

    // Properly terminate RFID communication
    rfid.PICC_HaltA();      // Put card to sleep
    rfid.PCD_StopCrypto1(); // Stop encryption on reader

    return finalValue;
}

/**
 * @brief Write passphrase data to multiple RFID card blocks
 * @details Distributes a passphrase string across multiple MIFARE Classic blocks,
 *          writing 16 bytes per block. The function handles authentication,
 *          data padding, and clearing of unused blocks. Remaining blocks are
 *          filled with null bytes to ensure clean card state.
 *
 * @param data Pointer to string containing passphrase to write to card
 * @param blocksArray Pointer to array of block numbers to write to
 * @param blocksCount Number of blocks available for writing
 * @return true if all blocks written successfully, false if any error occurred
 *
 * @note Each block holds exactly 16 bytes; longer passphrases span multiple blocks
 * @note Unused blocks are cleared with null bytes to prevent data leakage
 * @note Function automatically handles RFID communication cleanup
 */
bool writeTag(String *data, int *blocksArray, int blocksCount)
{
    int dataLength = data->length();
    int dataIndex = 0; // Current position in the source data string

    Serial.println(F("Writing data to all blocks..."));
    Serial.print(F("Data to write: "));
    Serial.println(*data);
    Serial.print(F("Data length: "));
    Serial.println(dataLength);
    Serial.println();

    // Process each block in sequence
    for (int i = 0; i < blocksCount; i++)
    {
        byte currentBlock = blocksArray[i];
        byte buffer[16]; // MIFARE Classic blocks are exactly 16 bytes

        // Initialize buffer with null bytes
        memset(buffer, 0x00, 16);

        // Fill buffer with data or leave as nulls if no more data
        for (int j = 0; j < 16; j++)
        {
            if (dataIndex < dataLength)
            {
                buffer[j] = (*data)[dataIndex];
                dataIndex++;
            }
            else
            {
                buffer[j] = 0x00; // Pad remaining space with null bytes
            }
        }

        // Authenticate before writing to each block
        if (!authenticateA(currentBlock))
        {
            Serial.print(F("CRITICAL ERROR: Authentication failed for block "));
            Serial.println(currentBlock);
            Serial.println(F("Stopping write operation due to authentication failure."));
            Serial.println();
            lcd_authentication_error(&lcd);
            return false; // Authentication failure is critical - abort operation
        }

        // Attempt to write data to the authenticated block
        MFRC522::StatusCode status = rfid.MIFARE_Write(currentBlock, buffer, 16);
        if (status != MFRC522::STATUS_OK)
        {
            Serial.print(F("CRITICAL ERROR: Writing failed on block "));
            Serial.print(currentBlock);
            Serial.print(F(": "));
            Serial.println(rfid.GetStatusCodeName(status));
            Serial.println(F("Stopping write operation due to write failure."));
            Serial.println();
            lcd_write_block_error(&lcd);
            return false; // Write failure is critical - abort operation
        }
        else
        {
            // Successfully wrote to block - log the operation
            Serial.print(F("Successfully wrote to block "));
            Serial.print(currentBlock);
            Serial.print(F(" - Data: "));

            // Display the ASCII content written to this block
            String blockData = "";
            for (int k = 0; k < 16; k++)
            {
                if (buffer[k] != 0x00)
                    blockData += (char)buffer[k];
            }
            Serial.println(blockData);
        }

        // Check if all data has been written
        if (dataIndex >= dataLength)
        {
            Serial.println(F("All data written successfully. Remaining blocks will be cleared."));
            Serial.println();

            // Clear any remaining blocks to ensure no old data remains
            for (int remainingBlock = i + 1; remainingBlock < blocksCount; remainingBlock++)
            {
                byte nullBuffer[16];
                memset(nullBuffer, 0x00, 16); // Create buffer of null bytes

                if (!authenticateA(blocksArray[remainingBlock]))
                {
                    Serial.print(F("CRITICAL ERROR: Authentication failed for clearing block "));
                    Serial.println(blocksArray[remainingBlock]);
                    Serial.println(F("Stopping write operation due to authentication failure."));
                    lcd_authentication_error(&lcd);
                    return false;
                }

                MFRC522::StatusCode clearStatus = rfid.MIFARE_Write(blocksArray[remainingBlock], nullBuffer, 16);
                if (clearStatus != MFRC522::STATUS_OK)
                {
                    Serial.print(F("CRITICAL ERROR: Failed to clear block "));
                    Serial.print(blocksArray[remainingBlock]);
                    Serial.print(F(": "));
                    Serial.println(rfid.GetStatusCodeName(clearStatus));
                    Serial.println(F("Stopping write operation due to write failure."));
                    lcd_write_block_error(&lcd);
                    return false;
                }
                else
                {
                    Serial.print(F("Cleared block "));
                    Serial.println(blocksArray[remainingBlock]);
                }
            }
            break; // Exit loop after clearing remaining blocks
        }
    }

    // Properly terminate RFID communication
    rfid.PICC_HaltA();      // Put card to sleep
    rfid.PCD_StopCrypto1(); // Stop encryption on reader

    Serial.println(F("Write operation completed successfully."));
    Serial.println();

    return true; // All operations completed successfully
}

// ============================================================================
// SYSTEM OUTPUT CONTROL
// ============================================================================

/**
 * @brief Execute access control action based on validation result
 * @details Controls the physical outputs of the system (relay, lock mechanism, etc.)
 *          based on whether a valid passphrase was detected. This function defines
 *          the actual security action taken when access is granted or denied.
 *
 * @param valid true = activate access control (grant access), false = deactivate (deny access)
 *
 * @note Current implementation provides 1-second pulse output for valid access
 * @note Customize this function based on specific hardware requirements (relay, servo, etc.)
 * @note Both ACTION_PIN and ALARM_PIN are controlled together for redundant signaling
 */
void executeAction(bool valid)
{
    if (valid)
    {
        // Grant access: activate outputs for 1 second
        digitalWrite(ACTION_PIN, HIGH); // Main action output (e.g., unlock relay)
        digitalWrite(ALARM_PIN, HIGH);  // Secondary confirmation signal
        delay(1000);                    // Hold active state for 1 second
        digitalWrite(ACTION_PIN, LOW);  // Return to inactive state
        digitalWrite(ALARM_PIN, LOW);
    }
    else
    {
        // Deny access: ensure all outputs are inactive
        digitalWrite(ACTION_PIN, LOW);
        digitalWrite(ALARM_PIN, LOW);
    }
}
