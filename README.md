# rfid-box

Arduino project for read/write rfid tags

## How it works

### AGENT: WRITER

`FORCE BUTTON`: contatto diretto al relay senza passare dal controllore.

`MODE BUTTON`: se premuto cambia il **MODE** tra _READ_ e _WRITE_. 

`RESET BUTTON`: 
-   Quando si legge/scrive un tag, il programma si blocca. Per eseguire una nuova operazione, premere questo pulsante. 

- **Se si tiene premuto** durante la lettura di una carta, stampa sul serial monitor tutti i blocchi del tag.


### AGENT: READER

`FORCE BUTTON`: contatto diretto al relay senza passare dal controllore.

`MODE BUTTON`: se premuto per più di 3 secondi cambia il **MODE** tra _RUN_ e _SET_.

`RESET BUTTON`: non utilizzato.
