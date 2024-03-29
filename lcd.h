#include "Arduino.h"
#include <Wire.h>
#include <LCD_I2C.h>
#include "def.h"

/**
 * Inizializza il display LCD
 *
 * @param lcd puntatore all'oggetto LCD_I2C
 * @param version versione del programma
 * @return true se l'inizializzazione è andata a buon fine
 */
bool lcd_init(LCD_I2C *lcd, String version)
{
    lcd->begin();                     // inizializza LCD
    lcd->backlight();                 // accende lo sfondo
    lcd->home();                      // si posrta in prima riga
    lcd->print("RFID BOX ");          // messaggio di benvenuto
    lcd->setCursor(0, 1);             // passa alla seconda riga
    lcd->print("Version " + version); // stampa
    delay(2000);                      // pausa
    lcd->clear();                     // pulisce tutto lo schermo
    return true;
}

void lcd_idle(LCD_I2C *lcd, Mode mode, int block)
{
    String modeStr = mode == MODE_READ ? "READ" : "WRITE";
    lcd->clear();
    lcd->home();
    lcd->print(modeStr + " Block " + block);
    lcd->setCursor(0, 1);
    lcd->print("Waiting card...");
    
    Serial.println(modeStr + " Block " + block + ". Waiting card...");
    Serial.println();
}


void lcd_reading(LCD_I2C *lcd)
{
    lcd->clear();
    lcd->home();
    lcd->print("Card Detected!");
    lcd->setCursor(0, 1);
    lcd->print("Reading card...");
}

void lcd_writing(LCD_I2C *lcd)
{
    lcd->clear();
    lcd->home();
    lcd->print("Card Detected!");
    lcd->setCursor(0, 1);
    lcd->print("Writing card...");
}

void lcd_reading_result(LCD_I2C *lcd, String uid, String value)
{
    lcd->clear();
    lcd->home();
    lcd->print(uid);
    lcd->setCursor(0, 1);
    lcd->print(value);
}

void lcd_reading_error(LCD_I2C *lcd, String error)
{
    lcd->clear();
    lcd->home();
    lcd->print("ERROR");
    lcd->setCursor(0, 1);
    lcd->autoscroll();
    lcd->print(error);
    lcd->noAutoscroll();
}


