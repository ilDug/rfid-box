# Secrets Generator Utility

## Descrizione
Utility Python per generare passphrases e chiavi crittografiche sicure per il sistema RFID Box.

## Caratteristiche
- ✅ Generazione cryptographically secure con il modulo `secrets`
- ✅ Passphrase di 64 caratteri (lettere maiuscole, minuscole e cifre)
- ✅ Chiave crittografica MIFARE Classic a 6 byte
- ✅ Output formattato per C++/Arduino
- ✅ Generazione completa del file `data.h`
- ✅ Opzione di salvataggio automatico

## Utilizzo

### Esecuzione
```bash
cd secrets-generator-util
python main.py
```

### Output
L'utility genera:

1. **Passphrase casuale** (64 caratteri):
   ```
   Generated passphrase: A7n9mK2pL8qR3sT4uV5wX6yZ0bC1dE2fG3hI4jK5lM6nO7pQ8rS9tU0vW1xY2zA3
   ```

2. **Chiave crittografica** (6 byte):
   ```
   Generated crypto key: {0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F}
   Hex representation: 1A 2B 3C 4D 5E 6F
   ```

3. **Codice C++ pronto per l'uso**:
   ```cpp
   // Per data.h
   const String mainPassphrase = "A7n9mK2pL8qR3sT4uV5wX6yZ0bC1dE2fG3hI4jK5lM6nO7pQ8rS9tU0vW1xY2zA3";
   const byte crypto_key[MFRC522::MF_KEY_SIZE] = {0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F};
   
   // Per key-changer.ino
   byte customKey[6] = {0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F};
   ```

4. **File `data.h` completo** (opzionale salvataggio)

## Workflow consigliato

### 1. Generazione
```bash
python main.py
```

### 2. Aggiornamento dei file
- Copia la nuova `mainPassphrase` in `data.h`
- Copia la nuova `crypto_key` in `data.h`
- Aggiorna `customKey` in `key-changer.ino`

### 3. Re-programmazione
- Usa `key-changer` per programmare tutte le card con la nuova chiave
- Testa il sistema con le nuove configurazioni

## Sicurezza

### ⚠️ Note importanti
- **Backup sicuro**: Salva sempre le chiavi generate in un luogo sicuro
- **Coerenza**: Assicurati che `crypto_key` e `customKey` siano identici
- **Re-programmazione**: Tutte le card esistenti dovranno essere riprogrammate
- **Confidenzialità**: Non condividere mai le chiavi generate

### 🔐 Vantaggi cryptographically secure
- Usa il modulo Python `secrets` (non `random`)
- Entropia elevata per passphrase e chiavi
- Adatto per applicazioni di sicurezza

## Requisiti
- Python 3.6+
- Nessuna dipendenza esterna (usa solo librerie standard)

## Esempi di output

```
🔐 RFID Box Secrets Generator
==================================================
🎲 Generating new 64-character passphrase...
✅ Generated passphrase: K8mN7pQ2rS9tU0vW1xY3zA4bC5dE6fG7hI8jK9lM0nO1pQ2rS3tU4vW5xY6zA7bC

🔑 Generating new 6-byte crypto key...
✅ Generated crypto key: {0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F}
   Hex representation: 1A 2B 3C 4D 5E 6F

==================================================
📋 C++ CODE TO UPDATE:
==================================================

1️⃣  For data.h - mainPassphrase:
const String mainPassphrase = "K8mN7pQ2rS9tU0vW1xY3zA4bC5dE6fG7hI8jK9lM0nO1pQ2rS3tU4vW5xY6zA7bC";

2️⃣  For data.h - crypto_key:
const byte crypto_key[MFRC522::MF_KEY_SIZE] = {0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F};

3️⃣  For key-changer.ino - customKey:
byte customKey[6] = {0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F};
```
