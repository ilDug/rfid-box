# Guida RFID Box Writer v1.0.0

## Descrizione Generale

Il **RFID Box Writer** è un sistema basato su microcontrollore Arduino che gestisce la lettura e scrittura di card RFID MIFARE Classic. Il dispositivo può operare in due modalità principali (lettura e scrittura) e due stati di funzionamento (RUN e SET), offrendo un sistema completo per la gestione di tessere RFID con autenticazione tramite passphrase.

## Componenti Hardware

### Lettore RFID
- **Modulo**: MFRC522
- **Protocollo**: SPI
- **Pin di connessione**:
  - SS (Slave Select): Pin 10
  - RST (Reset): Pin 9
  - MOSI: Pin 11
  - MISO: Pin 12
  - SCK: Pin 13

### Pulsanti di Controllo
- **Pulsante MODE**: Pin 5 (con pull-up interno)
- **Pulsante RESET**: Pin 4 (con pull-up interno)

### Pin di Output
- **ACTION_PIN**: Pin 2 - Segnale di attivazione principale
- **ALARM_PIN**: Pin 6 - Segnale acustico e di allarme
- **ERROR_PIN**: Pin 3 - Segnale di errore

### Display LCD (Opzionale)
- **Tipo**: LCD I2C 16x2
- **Indirizzo**: 0x27
- **Connessioni**: SDA (A4), SCL (A5)

## Modalità di Funzionamento

### 1. Modalità LETTURA (MODE_READ)
Nella modalità di lettura, il dispositivo:
- Legge i dati dalle card RFID
- Confronta i dati letti con la passphrase memorizzata in EEPROM
- Attiva l'output se la passphrase corrisponde

### 2. Modalità SCRITTURA (MODE_WRITE)
Nella modalità di scrittura, il dispositivo:
- Scrive la passphrase memorizzata sulla card RFID
- Distribuisce i dati su più blocchi della card
- Conferma il successo dell'operazione

## Stati di Funzionamento

### 1. Stato RUN
- **Funzione**: Operazione normale
- **Lettura**: Verifica la passphrase e attiva l'output se valida
- **Scrittura**: Scrive la passphrase sulla card

### 2. Stato SET (Solo in modalità lettura)
- **Funzione**: Programmazione della passphrase
- **Operazione**: Legge una card "master" e salva la passphrase in EEPROM
- **Indicazione**: LED/buzzer lampeggia ogni 2 secondi

## Controlli e Pulsanti

### Pulsante MODE (Pin 5)

#### Pressione Breve
- **Funzione**: Cambio modalità
- **Azione**: Alterna tra modalità LETTURA e SCRITTURA
- **Feedback**: 1 beep di conferma
- **Note**: Se si passa alla modalità SCRITTURA, lo stato viene automaticamente impostato su RUN

#### Pressione Lunga (3 secondi)
- **Funzione**: Cambio stato di funzionamento
- **Azione**: Alterna tra stato RUN e SET
- **Feedback**: 5 beep di conferma
- **Limitazioni**: 
  - Disponibile solo in modalità LETTURA
  - In modalità SCRITTURA rimane sempre RUN

### Pulsante RESET (Pin 4)

#### Durante Operazione Normale
- **Funzione**: Reset dello stato di errore
- **Azione**: Resetta il flag `fired` e torna allo stato normale
- **Utilizzo**: Premere quando il LED di errore è acceso

#### Durante Lettura Card (Tenuto Premuto)
- **Funzione**: Debug della card
- **Azione**: Stampa tutti i dati della card sul monitor seriale
- **Utilizzo**: Per diagnostica e debugging

#### Dopo Operazione SET
- **Funzione**: Conferma e ritorno a RUN
- **Azione**: Conferma il salvataggio della nuova passphrase e torna in stato RUN

## Sequenze Operative

### Scenario 1: Lettura Card in Stato RUN
1. Avvicinare una card al lettore
2. Il sistema legge i dati dai blocchi configurati
3. Confronta con la passphrase memorizzata
4. **Se valida**:
   - 1 beep lungo (600ms)
   - ACTION_PIN e ALARM_PIN HIGH per 1 secondo
   - Ritorno allo stato normale
5. **Se non valida**:
   - 3 beep di errore
   - ERROR_PIN HIGH
   - Attesa pressione RESET

### Scenario 2: Programmazione Nuova Passphrase (Stato SET)
1. Premere pulsante MODE per 3 secondi (5 beep)
2. Sistema entra in stato SET (beep ogni 2 secondi)
3. Avvicinare card "master" con nuova passphrase
4. Sistema legge e salva la passphrase in EEPROM
5. 1 beep lungo di conferma (1000ms)
6. Premere RESET per tornare in stato RUN

### Scenario 3: Scrittura Card
1. Premere brevemente pulsante MODE per passare in modalità SCRITTURA
2. Avvicinare card vuota al lettore
3. Sistema scrive la passphrase sui blocchi della card
4. 1 beep lungo di conferma (1000ms)
5. Premere RESET per continuare

## Gestione Errori

### Errori di Compatibilità
- **Causa**: Card non compatibile (solo MIFARE Classic supportate)
- **Segnale**: ERROR_PIN HIGH
- **Risoluzione**: Premere RESET e usare card compatibile

### Errori di Autenticazione
- **Causa**: Impossibile autenticare con la card
- **Segnale**: ERROR_PIN HIGH, messaggio su seriale
- **Risoluzione**: Verificare la card e premere RESET

### Errori di Lettura/Scrittura
- **Causa**: Operazione fallita sui blocchi RFID
- **Segnale**: ERROR_PIN HIGH, messaggio dettagliato su seriale
- **Risoluzione**: Riprovare l'operazione o sostituire la card

## Feedback Audio e Visivo

### Segnali Acustici
- **1 beep breve**: Conferma cambio modalità
- **5 beep**: Conferma cambio stato (RUN ↔ SET)
- **1 beep lungo (600ms)**: Lettura valida
- **1 beep lungo (1000ms)**: Operazione completata con successo
- **3 beep**: Errore di lettura
- **Beep ogni 2 secondi**: Modalità SET attiva

### Segnali LED
- **ACTION_PIN HIGH**: Accesso autorizzato (1 secondo)
- **ALARM_PIN HIGH**: Accompagna ACTION_PIN e segnali audio
- **ERROR_PIN HIGH**: Stato di errore (fino a RESET)

## Memoria e Persistenza

### EEPROM
- **Contenuto**: Passphrase di riferimento
- **Gestione**: Caricamento all'avvio, salvataggio in modalità SET
- **Limiti**: Massimo 500 caratteri per sicurezza, solo caratteri ASCII stampabili (32-126)
- **Sicurezza**: Validazione caratteri, pulizia completa prima della scrittura

### Card RFID
- **Settori utilizzati**: 15 settori (dal settore 1 al 15, settore 0 escluso per sicurezza)
- **Blocchi per settore**: 3 blocchi dati (45 blocchi totali)
- **Capacità fisica**: 720 bytes (45 blocchi × 16 bytes)
- **Limite passphrase**: 500 caratteri (limite EEPROM di sicurezza)
- **Distribuzione**: Dati distribuiti sequenzialmente sui blocchi dati (esclusi i blocchi di controllo)

## Monitor Seriale

### Informazioni di Debug
- Versione del firmware
- Dettagli del lettore RFID
- Passphrase caricata dalla EEPROM
- Stato delle operazioni di lettura/scrittura
- Errori dettagliati con codici di stato

### Comandi di Debug
- **Dump card**: Tenere premuto RESET durante lettura

## Configurazione

### Passphrase Predefinita
La passphrase principale è definita in `data.h`:
```cpp
String mainPassphrase = "64char_passphrase_example_1234567890abcdefghij";
```

### Blocchi RFID Utilizzati
I blocchi sono definiti in `def.h` e includono 3 blocchi dati per ogni settore (dal settore 1 al 15), evitando deliberatamente:
- **Settore 0**: Riservato per informazioni del produttore e UID
- **Blocchi di controllo**: Un blocco ogni 4 (blocchi 3, 7, 11, 15, ecc.) utilizzati per chiavi di accesso

La configurazione utilizza quindi i blocchi: 4-6, 8-10, 12-14, 16-18, 20-22, 24-26, 28-30, 32-34, 36-38, 40-42, 44-46, 48-50, 52-54, 56-58, 60-62.

## Utilizzo Tipico

### Setup Iniziale
1. Caricare il firmware sul microcontrollore
2. Avviare il sistema (carica passphrase da EEPROM)
3. Se necessario, programmare nuova passphrase:
   - Tenere premuto MODE per 3 secondi
   - Avvicinare card master
   - Premere RESET per confermare

### Uso Quotidiano
1. Modalità LETTURA, stato RUN (default)
2. Avvicinare card per verifica accesso
3. Se autorizzata: ACTION_PIN attivo per 1 secondo
4. Se non autorizzata: segnale di errore

### Creazione Nuove Card
1. Premere MODE per passare in modalità SCRITTURA
2. Avvicinare card vuota
3. Sistema scrive passphrase
4. Premere RESET per continuare
5. Tornare in modalità LETTURA con MODE

## Note di Sicurezza

- Le card utilizzano chiave factory default (FF FF FF FF FF FF)
- La passphrase è memorizzata in chiaro in EEPROM
- Il sistema supporta solo card MIFARE Classic
- Autenticazione richiesta per ogni operazione sui blocchi

## Troubleshooting

### La card non viene rilevata
- Verificare connessioni MFRC522
- Controllare alimentazione
- Verificare compatibilità card (solo MIFARE Classic)

### Errori di autenticazione persistenti
- Verificare che la card non sia protetta
- Controllare integrità dei dati sulla card
- Riprovare con card nuova

### EEPROM corrotta
- Il sistema rileva automaticamente caratteri non stampabili
- In caso di problemi, riprogrammare la passphrase in modalità SET