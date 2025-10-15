# Secrets Generator Utility

## Descrizione
Utility Python per generare passphrases e chiavi crittografiche sicure per il sistema RFID Box.

## Caratteristiche
- ✅ Generazione cryptographically secure con il modulo `secrets`
- ✅ Passphrase di 64 caratteri (lettere maiuscole, minuscole e cifre)
- ✅ Chiave crittografica MIFARE Classic a 6 byte
- ✅ Output formattato per C++/Arduino
- ✅ Type hints per migliore leggibilità del codice
- ✅ Funzioni ottimizzate e semplificate

## Utilizzo

### Esecuzione
```bash
./secrets-generator-util/dist/secrets_generator_util
```

### Output
L'utility genera:

1. **Passphrase casuale** (64 caratteri):
   ```
   Generated passphrase: A7n9mK2pL8qR3sT4uV5wX6yZ0bC1dE2fG3hI4jK5lM6nO7pQ8rS9tU0vW1xY2zA3
   ```

2. **Chiave crittografica** (6 byte):
   ```
   Generated crypto key: 1A 2B 3C 4D 5E 6F
   ```

3. **Codice C++ pronto per l'uso**:
   ```cpp
   // Per data.h - mainPassphrase
   const String mainPassphrase = "A7n9mK2pL8qR3sT4uV5wX6yZ0bC1dE2fG3hI4jK5lM6nO7pQ8rS9tU0vW1xY2zA3";
   
   // Per data.h - crypto_key
   const byte crypto_key[MFRC522::MF_KEY_SIZE] = {0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F};
   ```

## Workflow consigliato

### 1. Generazione
```bash
./secrets-generator-util/dist/secrets_generator_util
```

### 2. Aggiornamento dei file
- Copia la nuova `mainPassphrase` in `data.h`
- Copia la nuova `crypto_key` in `data.h`


## Sicurezza

### ⚠️ Note importanti
- **Backup sicuro**: Salva sempre le chiavi generate in un luogo sicuro
- **Confidenzialità**: Non condividere mai le chiavi generate

### 🔐 Vantaggi cryptographically secure
- Usa il modulo Python `secrets` (non `random`)
- Entropia elevata per passphrase e chiavi
- Adatto per applicazioni di sicurezza
- Type hints per migliore manutenibilità del codice
- Funzioni ottimizzate per performance migliori

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
✅ Generated crypto key: 1A 2B 3C 4D 5E 6F

==================================================

� SECURITY NOTES:
- Keep these values secure and private
- Update data.h with the new crypto_key and passphrase
- Re-program all existing cards with the new key
- Backup these values in a secure location

�📋 C++ CODE TO UPDATE:
==================================================
For data.h - mainPassphrase:
       const String mainPassphrase = "K8mN7pQ2rS9tU0vW1xY3zA4bC5dE6fG7hI8jK9lM0nO1pQ2rS3tU4vW5xY6zA7bC";
For data.h - crypto_key:
       const byte crypto_key[MFRC522::MF_KEY_SIZE] = {0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F};
```
