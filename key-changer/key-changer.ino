/**
 * @file key-changer.ino
 * @brief MIFARE Classic Factory Key Changer Utility
 * @details Standalone utility to change the factory default key (FFFFFFFFFFFF) 
 *          on new MIFARE Classic cards to a custom security key. This tool is
 *          designed to prepare blank cards for use with the RFID Box system.
 * @author Dag
 * @version 1.0
 * @date 2025-10-10
 */

#include <SPI.h>
#include <MFRC522.h>

// ============================================================================
// HARDWARE CONFIGURATION
// ============================================================================

const int SS_PIN = 10;   // RFID Slave Select Pin
const int RST_PIN = 9;   // RFID Reset Pin
const int LED_PIN = 13;  // Built-in LED for status indication
const int BUZZER_PIN = 6; // Optional buzzer for audio feedback

// ============================================================================
// RFID CONFIGURATION
// ============================================================================

MFRC522 mfrc522(SS_PIN, RST_PIN);

// Factory default key (all cards come with this key)
byte factoryKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Your custom security key (MUST MATCH crypto_key in data.cpp!)
byte customKey[6] = {0x01, 0x02, 0x13, 0x51, 0x09, 0x0F};

// Standard access bits for data blocks with custom key protection
byte accessBits[4] = {0xFF, 0x07, 0x80, 0x69};

// ============================================================================
// STATUS INDICATORS
// ============================================================================

/**
 * @brief Visual and audio feedback for different operations
 */
void signalSuccess() {
  digitalWrite(LED_PIN, HIGH);
  tone(BUZZER_PIN, 1000, 200);
  delay(200);
  tone(BUZZER_PIN, 1500, 200);
  delay(200);
  digitalWrite(LED_PIN, LOW);
}

void signalError() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 500, 150);
    delay(150);
    digitalWrite(LED_PIN, LOW);
    delay(150);
  }
}

void signalWaiting() {
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
  delay(100);
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Display byte array in hex format
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
  Serial.println();
}

/**
 * @brief Display card UID
 */
void printUID() {
  Serial.print("Card UID:");
  printHex(mfrc522.uid.uidByte, mfrc522.uid.size);
}

// ============================================================================
// KEY CHANGING FUNCTIONS
// ============================================================================

/**
 * @brief Change factory key to custom key for a specific sector
 * @param sector Sector number (0-15 for MIFARE Classic 1K)
 * @return true if key change was successful, false otherwise
 */
bool changeSectorKey(byte sector) {
  byte trailerBlock = sector * 4 + 3; // Calculate trailer block number
  byte trailerBuffer[16];
  
  Serial.print("Processing sector ");
  Serial.print(sector);
  Serial.print(" (trailer block ");
  Serial.print(trailerBlock);
  Serial.println(")...");
  
  // Step 1: Authenticate with factory default key
  MFRC522::StatusCode status = mfrc522.PCD_Authenticate(
    MFRC522::PICC_CMD_MF_AUTH_KEY_A, 
    trailerBlock, 
    &factoryKey, 
    &(mfrc522.uid)
  );
  
  if (status != MFRC522::STATUS_OK) {
    Serial.print("  Authentication failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    
    // Try to authenticate with custom key (card might already be programmed)
    status = mfrc522.PCD_Authenticate(
      MFRC522::PICC_CMD_MF_AUTH_KEY_A, 
      trailerBlock, 
      &customKey, 
      &(mfrc522.uid)
    );
    
    if (status == MFRC522::STATUS_OK) {
      Serial.println("  Card already programmed with custom key - SKIPPING");
      return true; // Not an error, just already programmed
    }
    
    return false;
  }
  
  Serial.println("  Factory key authentication successful");
  
  // Step 2: Prepare new trailer block data
  // Structure: [Key A - 6 bytes][Access Bits - 4 bytes][Key B - 6 bytes]
  
  // Copy custom key A
  for (byte i = 0; i < 6; i++) {
    trailerBuffer[i] = customKey[i];
  }
  
  // Copy access bits
  for (byte i = 0; i < 4; i++) {
    trailerBuffer[6 + i] = accessBits[i];
  }
  
  // Copy custom key B (same as key A for simplicity)
  for (byte i = 0; i < 6; i++) {
    trailerBuffer[10 + i] = customKey[i];
  }
  
  // Step 3: Write new trailer block
  status = mfrc522.MIFARE_Write(trailerBlock, trailerBuffer, 16);
  
  if (status != MFRC522::STATUS_OK) {
    Serial.print("  Write failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return false;
  }
  
  Serial.println("  Key change successful!");
  return true;
}

/**
 * @brief Change keys for all sectors on the card
 * @return true if all sectors were processed successfully
 */
bool changeAllKeys() {
  bool allSuccessful = true;
  int successCount = 0;
  int totalSectors = 16; // MIFARE Classic 1K has 16 sectors
  
  Serial.println("\n=== Starting key change process ===");
  Serial.print("Target key: ");
  printHex(customKey, 6);
  Serial.println();
  
  for (byte sector = 0; sector < totalSectors; sector++) {
    if (changeSectorKey(sector)) {
      successCount++;
    } else {
      allSuccessful = false;
      Serial.print("FAILED to change key for sector ");
      Serial.println(sector);
    }
    
    delay(100); // Small delay between sectors
  }
  
  Serial.println("\n=== Process completed ===");
  Serial.print("Successfully changed: ");
  Serial.print(successCount);
  Serial.print("/");
  Serial.print(totalSectors);
  Serial.println(" sectors");
  
  return allSuccessful;
}

// ============================================================================
// MAIN FUNCTIONS
// ============================================================================

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for serial port to connect
  
  // Initialize pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Initialize SPI and MFRC522
  SPI.begin();
  mfrc522.PCD_Init();
  
  // Verify MFRC522 communication
  mfrc522.PCD_DumpVersionToSerial();
  
  Serial.println();
  Serial.println("========================================");
  Serial.println("   MIFARE Classic Key Changer v1.0    ");
  Serial.println("========================================");
  Serial.println();
  Serial.println("This utility changes factory default keys");
  Serial.println("to custom security keys on MIFARE Classic cards.");
  Serial.println();
  Serial.print("Factory key: ");
  printHex(factoryKey, 6);
  Serial.print("Custom key:  ");
  printHex(customKey, 6);
  Serial.println();
  Serial.println("⚠️  WARNING: This operation is irreversible!");
  Serial.println("⚠️  Test on expendable cards first!");
  Serial.println();
  Serial.println("Place a card near the reader to begin...");
  Serial.println("----------------------------------------");
}

void loop() {
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    signalWaiting();
    return;
  }
  
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  Serial.println("\n🔍 Card detected!");
  printUID();
  
  // Check if it's a MIFARE Classic card
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.print("Card type: ");
  Serial.println(mfrc522.PICC_GetTypeName(piccType));
  
  if (piccType != MFRC522::PICC_TYPE_MIFARE_1K && 
      piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println("❌ This is not a MIFARE Classic card!");
    signalError();
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return;
  }
  
  // Perform key change
  bool success = changeAllKeys();
  
  if (success) {
    Serial.println("\n✅ Key change completed successfully!");
    Serial.println("Card is now ready for RFID Box system.");
    signalSuccess();
  } else {
    Serial.println("\n❌ Key change failed or partially completed!");
    Serial.println("Check serial output for details.");
    signalError();
  }
  
  // Halt PICC and stop encryption
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  
  Serial.println("\nRemove card and place next card to continue...");
  Serial.println("============================================");
  
  delay(2000); // Prevent rapid re-reading of the same card
}