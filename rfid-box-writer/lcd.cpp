/**
 * @file lcd.cpp
 * @brief Implementazione delle funzioni per gestione display LCD I2C
 * @details Questo file contiene l'implementazione di tutte le funzioni per
 *          la gestione del display LCD 16x2 collegato tramite interfaccia I2C.
 * @author Dag
 * @version 1.0.0
 */

#include "lcd.h"

// ============================================================================
// INIZIALIZZAZIONE E CONFIGURAZIONE LCD
// ============================================================================

void lcd_init(LCD_I2C *lcd, const String &version)
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
}

// ============================================================================
// VISUALIZZAZIONE STATI SISTEMA
// ============================================================================

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