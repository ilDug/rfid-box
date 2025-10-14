#ifndef RFID_DATA_H
#define RFID_DATA_H

#include <MFRC522.h>

/************************************************/
/*** INSERIRE QUI I DATI DA SCRIVERE */
/************************************************/

// Firmware version for display and serial output
const String VERSION = "v1.0.0";

// main passphrase (max 135 characters)
const String mainPassphrase = "soJ0ijae1yee2yohChoop6ieshohrex5eeVuk7Cooghiu5She5OhpaingaicheG5";

// ============================================================================
// CRYPTOGRAPHIC KEYS DECLARATIONS
// ============================================================================

/**
 * @brief MIFARE Classic Authentication Key (Constant Declaration)
 * @details External declaration of factory default 6-byte key for MIFARE Classic card authentication.
 *          This key is used for initial access to cards before any custom keys are set.
 *          Factory default key is widely known (FFFFFFFFFFFF) and should be changed for security.
 */
const byte default_key[MFRC522::MF_KEY_SIZE] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/**
 * @brief MIFARE Classic Authentication Key (Constant Declaration)
 * @details External declaration of custom 6-byte key for MIFARE Classic card authentication.
 *          Alternative to factory default key (FFFFFFFFFFFF).
 *          Used for enhanced security when reading/writing card data.
 *          This key must match the one programmed on the cards using the key-changer utility.
 */
const byte crypto_key[MFRC522::MF_KEY_SIZE] = {0x01, 0x02, 0x13, 0x51, 0x09, 0x0F};

/************************************************/

#endif

