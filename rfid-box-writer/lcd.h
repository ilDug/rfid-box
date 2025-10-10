/**
 * @file lcd.h
 * @brief Gestione display LCD I2C per RFID Box Writer
 * Questo file contiene le funzioni per l'inizializzazione e la gestione
 *          del display LCD 16x2 collegato tramite interfaccia I2C. Fornisce
 *          funzioni per mostrare messaggi di stato, errori e informazioni
 *          durante il funzionamento del sistema RFID.
 * @author Dag
 * @version 1.0.0
 */

#ifndef LCD_MANAGER_H
#define LCD_MANAGER_H

#include "Arduino.h"
#include <Wire.h>
#include <LCD_I2C.h>

// ============================================================================
// INIZIALIZZAZIONE E CONFIGURAZIONE LCD
// ============================================================================

/**
 * @brief Inizializza il display LCD I2C
 * Questa funzione configura e prepara il display LCD per l'uso.
 *          Esegue tutti i passaggi necessari per l'avvio e mostra un messaggio
 *          di benvenuto con la versione del firmware.
 *
 * @param version Stringa contenente la versione del programma da visualizzare
 * @return true se l'inizializzazione è completata con successo, false altrimenti
 *
 */
bool lcd_init(LCD_I2C *lcd, const String &version)
{
    // Passo 1: Inizializzazione hardware del display LCD
    lcd->begin();
    // Passo 2: Attivazione della retroilluminazione per migliorare la visibilità
    lcd->backlight();
    // Passo 3: Posizionamento del cursore in posizione home (prima riga, prima colonna)
    lcd->home();
    // Passo 4: Visualizzazione del messaggio di benvenuto principale
    lcd->print(F("RFID BOX "));
    // Passo 5: Spostamento del cursore alla seconda riga (riga 1, colonna 0)
    lcd->setCursor(0, 1);
    // Passo 6: Visualizzazione della versione del firmware
    lcd->print(F("Version "));
    lcd->print(version);
    // Passo 7: Pausa di 2 secondi per permettere all'utente di leggere il messaggio
    delay(2000);
    // Passo 8: Pulizia completa dello schermo per prepararlo all'uso normale
    lcd->clear();
    return true; // Inizializzazione completata con successo
}

// ============================================================================
// VISUALIZZAZIONE STATI SISTEMA
// ============================================================================

/**
 * @brief Mostra lo stato di attesa (idle) del sistema
 * Questa funzione visualizza sul display LCD lo stato corrente del sistema
 *          quando è in attesa di una carta RFID. Mostra la modalità operativa
 *          attiva e il messaggio di attesa carta.
 * @param mode Modalità operativa corrente (MODE_READ o MODE_WRITE)
 * @param job Tipo di lavoro corrente (RUN o SET)
 *
 */
void lcd_idle(LCD_I2C *lcd, Mode mode, Job job)
{
    // Passo 1: Determinazione del messaggio di modalità basato sui parametri di stato
    const __FlashStringHelper *modeStr;
    if (job == SET)
    {
        // Modalità SET ha priorità: permette di aggiornare la passphrase master
        modeStr = F("SETTING mode.");
    }
    else if (mode == MODE_READ)
    {
        // Modalità lettura: validazione delle carte contro la passphrase memorizzata
        modeStr = F("READING mode.");
    }
    else if (mode == MODE_WRITE)
    {
        // Modalità scrittura: programmazione di nuove carte con la passphrase corrente
        modeStr = F("WRITING mode.");
    }

    // Passo 2: Pulizia completa del display per rimuovere contenuto precedente
    lcd->clear();
    // Passo 3: Posizionamento del cursore in posizione home (prima riga, prima colonna)
    lcd->home();
    // Passo 4: Visualizzazione della modalità operativa corrente
    lcd->print(modeStr);
    // Passo 5: Spostamento del cursore alla seconda riga (riga 1, colonna 0)
    lcd->setCursor(0, 1);
    // Passo 6: Visualizzazione del messaggio di attesa carta
    lcd->print(F("Waiting card..."));
    // Passo 7: Output aggiuntivo su Serial Monitor per scopi di debug e monitoraggio
    Serial.print(modeStr);
    Serial.print(F(" "));
    Serial.println(F("Waiting card..."));
    Serial.println(); // Riga vuota per migliorare la leggibilità del log
}

/**
 * @brief Visualizza un messaggio di errore per carta non compatibile
 * Mostra un messaggio quando viene rilevata una carta RFID non supportata dal sistema
 */
void lcd_compatibility_error(LCD_I2C *lcd)
{
    // Passo 1: Pulizia completa del display per rimuovere contenuto precedente
    lcd->clear();
    // Passo 2: Posizionamento del cursore in posizione home (prima riga, prima colonna)
    lcd->home();
    // Passo 3: Visualizzazione del messaggio di errore di compatibilità
    lcd->print(F("Incompatible"));
    // Passo 4: Spostamento alla seconda riga
    lcd->setCursor(0, 1);
    // Passo 5: Visualizzazione del dettaglio dell'errore
    lcd->print(F("card type!"));
}

/**
 * @brief Visualizza l'UID della carta RFID rilevata
 * Mostra l'identificativo univoco della carta con scorrimento automatico per UID lunghi
 * @param uid Stringa contenente l'UID della carta da visualizzare
 */
void lcd_show_uid(LCD_I2C *lcd, const String &uid)
{
    // Passo 1: Pulizia completa del display per rimuovere contenuto precedente
    lcd->clear();
    // Passo 2: Posizionamento del cursore in posizione home (prima riga, prima colonna)
    lcd->home();
    // Passo 3: Visualizzazione dell'etichetta per l'UID
    lcd->print(F("Card UID:"));
    // Passo 4: Spostamento alla seconda riga
    lcd->setCursor(0, 1);
    // Passo 5: Attivazione dello scorrimento automatico per UID lunghi
    lcd->autoscroll();
    // Passo 6: Visualizzazione dell'UID della carta
    lcd->print(uid);
    // Passo 7: Disattivazione dello scorrimento automatico
    lcd->noAutoscroll();
}

/**
 * @brief Visualizza errore di autenticazione della carta RFID
 * Mostra un messaggio di errore quando fallisce l'autenticazione con la carta
 */
void lcd_authentication_error(LCD_I2C *lcd)
{
    // Passo 1: Pulizia completa del display per rimuovere contenuto precedente
    lcd->clear();
    // Passo 2: Posizionamento del cursore in posizione home (prima riga, prima colonna)
    lcd->home();
    // Passo 3: Visualizzazione dell'intestazione di errore
    lcd->print(F("ERROR!!!"));
    // Passo 4: Spostamento alla seconda riga
    lcd->setCursor(0, 1);
    // Passo 5: Visualizzazione del dettaglio dell'errore di autenticazione
    lcd->print(F("auth failure!"));
}

/**
 * @brief Visualizza errore di lettura di un blocco specifico
 * Mostra quale blocco della carta ha causato un errore durante la lettura
 * @param block Numero del blocco che ha causato l'errore
 */
void lcd_read_block_error(LCD_I2C *lcd, byte block)
{
    // Passo 1: Pulizia completa del display per rimuovere contenuto precedente
    lcd->clear();
    // Passo 2: Posizionamento del cursore in posizione home (prima riga, prima colonna)
    lcd->home();
    // Passo 3: Visualizzazione del messaggio di errore di lettura
    lcd->print(F("Read error on"));
    // Passo 4: Spostamento alla seconda riga
    lcd->setCursor(0, 1);
    // Passo 5: Visualizzazione del numero del blocco con errore
    lcd->print(F("block "));
    lcd->print(block);
}

/**
 * @brief Visualizza errore di passphrase non valida
 * Mostra un messaggio quando la passphrase letta dalla carta non corrisponde a quella memorizzata
 */
void lcd_invalid_passphrase(LCD_I2C *lcd)
{
    // Passo 1: Pulizia completa del display per rimuovere contenuto precedente
    lcd->clear();
    // Passo 2: Posizionamento del cursore in posizione home (prima riga, prima colonna)
    lcd->home();
    // Passo 3: Visualizzazione del messaggio di passphrase non valida
    lcd->print(F("INVALID"));
    // Passo 4: Spostamento alla seconda riga
    lcd->setCursor(0, 1);
    // Passo 5: Visualizzazione del dettaglio dell'errore
    lcd->print(F("passphrase!"));
}

/**
 * @brief Visualizza errore di scrittura EEPROM
 * Mostra un messaggio quando fallisce il salvataggio della passphrase in memoria EEPROM
 */
void lcd_EEPROM_writing_error(LCD_I2C *lcd)
{
    // Passo 1: Pulizia completa del display per rimuovere contenuto precedente
    lcd->clear();
    // Passo 2: Posizionamento del cursore in posizione home (prima riga, prima colonna)
    lcd->home();
    // Passo 3: Visualizzazione dell'intestazione di errore
    lcd->print(F("ERROR!!!"));
    // Passo 4: Spostamento alla seconda riga
    lcd->setCursor(0, 1);
    // Passo 5: Visualizzazione del dettaglio dell'errore EEPROM
    lcd->print(F("EEPROM write!"));
}

/**
 * @brief Visualizza errore di lettura UID della carta
 * Mostra un messaggio quando fallisce la lettura dell'identificativo univoco della carta
 */
void lcd_uid_reading_error(LCD_I2C *lcd)
{
    // Passo 1: Pulizia completa del display per rimuovere contenuto precedente
    lcd->clear();
    // Passo 2: Posizionamento del cursore in posizione home (prima riga, prima colonna)
    lcd->home();
    // Passo 3: Visualizzazione dell'intestazione di errore
    lcd->print(F("ERROR!!!"));
    // Passo 4: Spostamento alla seconda riga
    lcd->setCursor(0, 1);
    // Passo 5: Visualizzazione del dettaglio dell'errore di lettura UID
    lcd->print(F("reading uid!"));
}

/**
 * @brief Visualizza conferma di impostazione passphrase master
 * Mostra messaggio di successo quando la nuova passphrase è stata salvata correttamente
 */
void lcd_passphrase_set_success(LCD_I2C *lcd)
{
    // Passo 1: Pulizia completa del display per rimuovere contenuto precedente
    lcd->clear();
    // Passo 2: Posizionamento del cursore in posizione home (prima riga, prima colonna)
    lcd->home();
    // Passo 3: Visualizzazione del messaggio di successo
    lcd->print(F("SUCCESS!!!"));
    // Passo 4: Spostamento alla seconda riga
    lcd->setCursor(0, 1);
    // Passo 5: Visualizzazione della conferma di impostazione passphrase
    lcd->print(F("Passphrase set"));
}

/**
 * @brief Visualizza messaggio di successo per lettura carta valida
 * Mostra conferma quando la carta è stata letta con successo e l'accesso è garantito
 */
void lcd_reading_success(LCD_I2C *lcd)
{
    // Passo 1: Pulizia completa del display per rimuovere contenuto precedente
    lcd->clear();
    // Passo 2: Posizionamento del cursore in posizione home (prima riga, prima colonna)
    lcd->home();
    // Passo 3: Visualizzazione del messaggio di successo
    lcd->print(F("reading success"));
    // Passo 4: Spostamento alla seconda riga
    lcd->setCursor(0, 1);
    // Passo 5: Visualizzazione del messaggio di accesso garantito
    lcd->print(F("APRITI SESAMO !"));
}

/**
 * @brief Visualizza messaggio di successo per scrittura carta
 * Mostra conferma quando la carta è stata programmata con successo
 */
void lcd_writing_success(LCD_I2C *lcd)
{
    // Passo 1: Pulizia completa del display per rimuovere contenuto precedente
    lcd->clear();
    // Passo 2: Posizionamento del cursore in posizione home (prima riga, prima colonna)
    lcd->home();
    // Passo 3: Visualizzazione del messaggio di successo
    lcd->print(F("writing success"));
    // Passo 4: Spostamento alla seconda riga
    lcd->setCursor(0, 1);
    // Passo 5: Visualizzazione del messaggio di conferma scrittura
    lcd->print(F("Card programmed"));
}

void lcd_write_block_error(LCD_I2C *lcd)
{
    // Passo 1: Pulizia completa del display per rimuovere contenuto precedente
    lcd->clear();
    // Passo 2: Posizionamento del cursore in posizione home (prima riga, prima colonna)
    lcd->home();
    // Passo 3: Visualizzazione del messaggio di errore di scrittura
    lcd->print(F("Writing ERROR!"));
    // Passo 4: Spostamento alla seconda riga
    lcd->setCursor(0, 1);
    // Passo 5: Visualizzazione del numero del blocco con errore
    lcd->print(F("remove card!"));
}

#endif // LCD_MANAGER_H