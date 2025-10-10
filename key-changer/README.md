# MIFARE Classic Key Changer Utility

## Descrizione
Utility standalone per cambiare la chiave factory default (`FF FF FF FF FF FF`) delle card MIFARE Classic nuove con una chiave di sicurezza personalizzata.

## Caratteristiche
- ✅ Rilevamento automatico card MIFARE Classic
- ✅ Cambio chiave per tutti i 16 settori (MIFARE 1K)
- ✅ Verifica se la card è già stata programmata
- ✅ Feedback visivo e audio per le operazioni
- ✅ Log dettagliato delle operazioni
- ✅ Gestione errori completa

## Hardware richiesto
- Arduino Uno/Nano/Pro Mini
- Modulo MFRC522 RFID Reader
- LED (opzionale, usa quello built-in)
- Buzzer (opzionale)

## Collegamento MFRC522
```
MFRC522    Arduino
------------------------
VCC    --> 3.3V
RST    --> Pin 9
GND    --> GND
MISO   --> Pin 12
MOSI   --> Pin 11
SCK    --> Pin 13
SS     --> Pin 10
```

## Configurazione

### Personalizzazione della chiave
Modifica la chiave personalizzata nel file `key-changer.ino`:

```cpp
// La tua chiave personalizzata (CAMBIA QUESTO VALORE!)
byte customKey[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};
```

**IMPORTANTE**: Assicurati che questa chiave sia identica a `cryptokey` nel progetto principale RFID Box!

## Utilizzo

1. **Compila e carica** il codice su Arduino
2. **Apri il Serial Monitor** (9600 baud)
3. **Posiziona una card** vicino al lettore
4. **Attendi il completamento** dell'operazione
5. **Rimuovi la card** programmata
6. **Ripeti** per altre card

## Output del Serial Monitor

```
========================================
   MIFARE Classic Key Changer v1.0    
========================================

🔍 Card detected!
Card UID: 04 A1 B2 C3
Card type: MIFARE 1K

=== Starting key change process ===
Target key:  12 34 56 78 9A BC

Processing sector 0 (trailer block 3)...
  Factory key authentication successful
  Key change successful!

Processing sector 1 (trailer block 7)...
  Factory key authentication successful
  Key change successful!

[... continua per tutti i settori ...]

=== Process completed ===
Successfully changed: 16/16 sectors

✅ Key change completed successfully!
Card is now ready for RFID Box system.
```

## Feedback LED e Buzzer

- **LED lampeggiante**: In attesa di card
- **LED fisso + beep doppio**: Operazione riuscita
- **LED triplo + beep basso**: Errore

## Gestione Errori

### Card già programmata
Se una card è già stata programmata, lo strumento la riconosce:
```
Authentication failed: MF_AUTH_FAILED
Card already programmed with custom key - SKIPPING
```

### Card non supportata
```
❌ This is not a MIFARE Classic card!
```

### Errore di scrittura
```
Write failed: MF_WRITE_FAILED
```

## Sicurezza

⚠️ **ATTENZIONE**: L'operazione è **irreversibile**!

- Testa sempre su card di prova
- Annota la chiave personalizzata in luogo sicuro
- Verifica che la chiave sia identica nel progetto principale

## Integrazione con RFID Box

Dopo aver programmato le card con questo utility:

1. Le card saranno protette con `customKey`
2. Il sistema RFID Box potrà accedervi usando `cryptokey`
3. **Assicurati che `customKey` = `cryptokey`**

## Troubleshooting

### Errore di compilazione
- Installa la libreria **MFRC522** dal Library Manager
- Verifica che **SPI** sia disponibile (inclusa nell'IDE)

### Card non rilevata
- Verifica i collegamenti hardware
- Controlla l'alimentazione (3.3V per MFRC522)
- Posiziona la card più vicina al lettore

### Autenticazione fallita
- La card potrebbe essere già programmata
- La card potrebbe essere danneggiata
- Verifica che sia una MIFARE Classic genuina