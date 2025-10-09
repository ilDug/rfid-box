# RFID Box Writer - Quick Guide

## Panoramica
Sistema Arduino per lettura/scrittura tessere RFID MIFARE Classic con autenticazione tramite passphrase.

## Controlli

### Pulsante MODE (Pin 5)
- **Click**: Cambia modalità (Lettura ↔ Scrittura)
- **Hold 3s**: Cambia stato (RUN ↔ SET) *solo in lettura*

### Pulsante RESET (Pin 4)
- **Click**: Reset errori / Conferma operazioni
- **Hold durante lettura**: Debug tessera (dump completo)

## Modalità Operative

### 🔍 LETTURA (Default)
**Stato RUN**: Verifica tessere
- Avvicina tessera → Verifica passphrase → Accesso autorizzato/negato

**Stato SET**: Programma nuova passphrase
- Lampeggia ogni 2s → Avvicina tessera master → Salva passphrase → RESET per confermare

### ✏️ SCRITTURA
**Solo stato RUN**: Crea nuove tessere
- Avvicina tessera vuota → Scrive passphrase → Conferma scrittura → RESET

## Feedback Audio

| Suono | Significato |
|-------|-------------|
| 1 beep breve | Cambio modalità |
| 5 beep | Attivazione SET |
| 1 beep lungo (600ms) | Accesso autorizzato |
| 1 beep lungo (1000ms) | Operazione completata |
| 3 beep | Errore |
| Beep ogni 2s | Modalità SET attiva |

## LED di Stato

| LED | Stato |
|-----|-------|
| ACTION (Pin 2) | Accesso autorizzato (1s) |
| ALARM (Pin 6) | Accompagna audio |
| ERROR (Pin 3) | Errore (fino a RESET) |

## Uso Rapido

### Prima configurazione
1. Avvia sistema
2. MODE hold 3s → SET mode
3. Avvicina tessera master
4. RESET → Passphrase salvata

### Verifica accesso
1. Modalità LETTURA (default)
2. Avvicina tessera
3. ✅ Autorizzato: ACTION LED + beep
4. ❌ Negato: ERROR LED + 3 beep → RESET

### Crea nuova tessera
1. MODE click → Modalità SCRITTURA
2. Avvicina tessera vuota
3. Beep conferma → RESET
4. MODE click → Torna a LETTURA

## Risoluzione Problemi

| Problema | Soluzione |
|----------|-----------|
| Tessera non rilevata | Verifica MIFARE Classic |
| Errore autenticazione | RESET + riprova |
| ERROR LED fisso | Premere RESET |
| Nessun feedback | Controlla connessioni |

## Hardware

- **RFID**: MFRC522 (SS=10, RST=9)
- **Pulsanti**: MODE=5, RESET=4 (pull-up)
- **Output**: ACTION=2, ALARM=6, ERROR=3
- **LCD**: I2C 0x27 (opzionale)

## Note Tecniche
- **Tessere supportate**: Solo MIFARE Classic
- **Capacità**: 720 bytes (45 blocchi)
- **Memoria**: Passphrase in EEPROM
- **Debug**: Monitor seriale 9600 baud