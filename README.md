# rfid-box

Arduino project for read/write rfid tags

## How it works

`MODE BUTTON`: se premuto cambia il **MODE** tra _read_ e _write_.

`RESET BUTTON`: Quando si lgge/scrive un tag, il programma si blocca. Per eseguire una nuova operazione, premere questo pulsante. **Se si tiene premuto** durante la lettura di una carta, stampa sul serial monitor tutti i blocchi del tag.

`SELECT BUTTON`: seleziona il blocco da leggere/scrivere. incrementa di uno ad ogni pressione. Ci potrebbe essere un limite impostato oltre il quale non si possono accedere a blocchi superiori.
