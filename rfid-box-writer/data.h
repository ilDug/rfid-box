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
// MIFARE CLASSIC BLOCKS TO USE
// ============================================================================
/**
 * @brief Autenticazione dei Blocchi MIFARE Classic
 *
 * @details
 * Le card MIFARE Classic utilizzano un meccanismo di autenticazione basato su settori per controllare l'accesso ai blocchi di dati.
 * Ogni settore è composto da più blocchi, inclusi i blocchi di dati e un trailer di settore.
 * Il trailer di settore contiene i bit di accesso e due chiavi (A e B).
 *
 * Processo di Autenticazione:
 * 1. **Selezione della Chiave:** Il lettore (es. MFRC522) tenta di autenticarsi a un settore utilizzando la Chiave A o la Chiave B.
 * 2. **Comando di Autenticazione:** Il lettore invia un comando di autenticazione alla card, specificando la chiave da utilizzare e il numero del settore.
 * 3. **Confronto Chiave:** La card confronta la chiave fornita con la chiave memorizzata nel trailer del settore.
 * 4. **Concessione/Negazione Accesso:** Se le chiavi corrispondono, la card concede l'accesso ai blocchi di dati del settore in base ai bit di accesso. Se non corrispondono, l'accesso viene negato.
 *
 * Bit di Accesso:
 * - I bit di accesso nel trailer del settore definiscono i permessi di lettura/scrittura per ciascun blocco nel settore.
 * - Determinano quale chiave (A o B) è richiesta per la lettura, la scrittura o entrambe.
 * - L'array `accessBits` in questo codice definisce una specifica configurazione di questi bit.
 *
 * Considerazioni Chiave:
 * - La chiave predefinita (0xFFFFFFFFFFFF) è una chiave ben nota e non sicura e dovrebbe essere modificata per qualsiasi applicazione reale.
 * - La `crypto_key` in questo codice rappresenta una chiave personalizzata che dovrebbe essere utilizzata per una maggiore sicurezza.
 * - La corretta gestione e protezione di queste chiavi sono fondamentali per mantenere la sicurezza del sistema MIFARE Classic.
 *
 * Nota: L'interpretazione specifica dei bit di accesso può essere complessa e dipende dalla versione della card MIFARE Classic. Fare riferimento al datasheet MIFARE Classic per informazioni dettagliate.
 */

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
 * @brief Standard access bits for data blocks with custom key protection
 * @details External declaration of access bits for MIFARE Classic card data blocks.
 *          These bits control read/write access to specific data blocks on the card.
 */
const byte accessBits[4] = {0xFF, 0x07, 0x80, 0x69};

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

