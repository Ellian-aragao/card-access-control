#ifndef _CARD_
#define _CARD_

#include <EEPROM.h>  // We are going to read and write PICC's UIDs from/to EEPROM
#include <SPI.h>     // RC522 Module uses SPI protocol
#include <MFRC522.h> // Library for Mifare RC522 Devices

void granted(uint16_t setDelay);
void denied();
uint8_t getID(MFRC522 *mfrc522, byte readCard[]);
void ShowReaderDetails(MFRC522 *mfrc522);
void cycleLeds();
void normalModeOn();
void writeID(byte newCard[]);
void deleteID(byte deleteCard[]);
bool checkTwo(byte a[], byte b[]);
uint8_t findIDSLOT(byte find[]);
bool findID(byte storedCard[]);
void successWrite();
void failedWrite();
void successDelete();
bool isMaster(byte test[], byte masterCard[]);
bool monitorWipeButton(uint32_t interval);

#endif