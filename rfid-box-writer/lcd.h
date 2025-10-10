/**
 * @file lcd.h
 * @brief Dichiarazioni per la gestione display LCD I2C per RFID Box Writer
 * @details Questo file contiene le dichiarazioni delle funzioni per
 *          la gestione del display LCD 16x2 collegato tramite interfaccia I2C.
 *          Le implementazioni si trovano in lcd.cpp.
 * @author Dag
 * @version 1.0.0
 */

#ifndef LCD_MANAGER_H
#define LCD_MANAGER_H

#include "Arduino.h"
#include <Wire.h>
#include <LCD_I2C.h>
#include "def.h"

// ============================================================================
// DICHIARAZIONI FUNZIONI LCD
// ============================================================================

/**
 * @brief Inizializza il display LCD I2C
 * @param lcd Puntatore all'oggetto LCD_I2C
 * @param version Stringa contenente la versione del programma da visualizzare
 * @return true se l'inizializzazione è completata con successo, false altrimenti
 */
bool lcd_init(LCD_I2C *lcd, const String &version);

/**
 * @brief Mostra lo stato di attesa (idle) del sistema
 * @param lcd Puntatore all'oggetto LCD_I2C
 * @param mode Modalità operativa corrente (MODE_READ o MODE_WRITE)
 * @param job Tipo di lavoro corrente (RUN o SET)
 */
void lcd_idle(LCD_I2C *lcd, Mode mode, Job job);

/**
 * @brief Visualizza un messaggio di errore per carta non compatibile
 * @param lcd Puntatore all'oggetto LCD_I2C
 */
void lcd_compatibility_error(LCD_I2C *lcd);

/**
 * @brief Visualizza l'UID della carta RFID rilevata
 * @param lcd Puntatore all'oggetto LCD_I2C
 * @param uid Stringa contenente l'UID della carta da visualizzare
 */
void lcd_show_uid(LCD_I2C *lcd, const String &uid);

/**
 * @brief Visualizza errore di autenticazione della carta RFID
 * @param lcd Puntatore all'oggetto LCD_I2C
 */
void lcd_authentication_error(LCD_I2C *lcd);

/**
 * @brief Visualizza errore di lettura di un blocco specifico
 * @param lcd Puntatore all'oggetto LCD_I2C
 * @param block Numero del blocco che ha causato l'errore
 */
void lcd_read_block_error(LCD_I2C *lcd, byte block);

/**
 * @brief Visualizza errore di passphrase non valida
 * @param lcd Puntatore all'oggetto LCD_I2C
 */
void lcd_invalid_passphrase(LCD_I2C *lcd);

/**
 * @brief Visualizza errore di scrittura EEPROM
 * @param lcd Puntatore all'oggetto LCD_I2C
 */
void lcd_EEPROM_writing_error(LCD_I2C *lcd);

/**
 * @brief Visualizza errore di lettura UID della carta
 * @param lcd Puntatore all'oggetto LCD_I2C
 */
void lcd_uid_reading_error(LCD_I2C *lcd);

/**
 * @brief Visualizza conferma di impostazione passphrase master
 * @param lcd Puntatore all'oggetto LCD_I2C
 */
void lcd_passphrase_set_success(LCD_I2C *lcd);

/**
 * @brief Visualizza messaggio di successo per lettura carta valida
 * @param lcd Puntatore all'oggetto LCD_I2C
 */
void lcd_reading_success(LCD_I2C *lcd);

/**
 * @brief Visualizza messaggio di successo per scrittura carta
 * @param lcd Puntatore all'oggetto LCD_I2C
 */
void lcd_writing_success(LCD_I2C *lcd);

/**
 * @brief Visualizza errore di scrittura blocco
 * @param lcd Puntatore all'oggetto LCD_I2C
 */
void lcd_write_block_error(LCD_I2C *lcd);

#endif // LCD_MANAGER_H