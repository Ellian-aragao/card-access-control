#include "../../src/config.h"
#include "card.h"

/////////////////////////////////////////  Access Granted    ///////////////////////////////////
void granted(uint16_t setDelay)
{
  digitalWrite(blueLed, LED_OFF); // Turn off blue LED
  digitalWrite(redLed, LED_OFF);  // Turn off red LED
  digitalWrite(greenLed, LED_ON); // Turn on green LED
  digitalWrite(relay, LOW);       // Unlock door!
  delay(setDelay);                // Hold door lock open for given seconds
  digitalWrite(relay, HIGH);      // Relock door
  delay(1000);                    // Hold green LED on for a second
}

/////////////////////////////////////////  Access Denied  ///////////////////////////////////
void denied()
{
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  digitalWrite(blueLed, LED_OFF);  // Make sure blue LED is off
  digitalWrite(redLed, LED_ON);    // Turn on red LED
  delay(1000);
}

/////////////////////////////////////////  Get PICC's UID ///////////////////////////////////
uint8_t getID(MFRC522 *mfrc522, byte readCard[])
{
  // Getting ready for Reading PICCs
  if (!mfrc522->PICC_IsNewCardPresent())
  { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if (!mfrc522->PICC_ReadCardSerial())
  { //Since a PICC placed get Serial and continue
    return 0;
  }
  // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
  // I think we should assume every PICC as they have 4 byte UID
  // Until we support 7 byte PICCs
  Serial.println(F("Scanned PICC's UID:"));
  for (uint8_t i = 0; i < 4; i++)
  {
    readCard[i] = mfrc522->uid.uidByte[i];
    Serial.print(readCard[i], HEX);
  }
  Serial.println("");
  mfrc522->PICC_HaltA(); // Stop reading
  return 1;
}

void ShowReaderDetails(MFRC522 *mfrc522)
{
  // Get the MFRC522 software version
  byte v = mfrc522->PCD_ReadRegister(mfrc522->VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown),probably a chinese clone?"));
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF))
  {
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
    Serial.println(F("SYSTEM HALTED: Check connections."));
    // Visualize system is halted
    digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
    digitalWrite(blueLed, LED_OFF);  // Make sure blue LED is off
    digitalWrite(redLed, LED_ON);    // Turn on red LED
    while (true)
      ; // do not go further
  }
}

/////////////////////////////////////////  Cycle Leds (Program Mode) ///////////////////////////////////
void cycleLeds()
{
  digitalWrite(redLed, LED_OFF);  // Make sure red LED is off
  digitalWrite(greenLed, LED_ON); // Make sure green LED is on
  digitalWrite(blueLed, LED_OFF); // Make sure blue LED is off
  delay(200);
  digitalWrite(redLed, LED_OFF);   // Make sure red LED is off
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  digitalWrite(blueLed, LED_ON);   // Make sure blue LED is on
  delay(200);
  digitalWrite(redLed, LED_ON);    // Make sure red LED is on
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  digitalWrite(blueLed, LED_OFF);  // Make sure blue LED is off
  delay(200);
}

/////////////////////////////////////////  Normal Mode Led  ///////////////////////////////////
void normalModeOn()
{
  digitalWrite(blueLed, LED_ON);   // Blue LED ON and ready to read card
  digitalWrite(redLed, LED_OFF);   // Make sure Red LED is off
  digitalWrite(greenLed, LED_OFF); // Make sure Green LED is off
  digitalWrite(relay, HIGH);       // Make sure Door is Locked
}

/////////////////////////////////////////  Read an ID from EEPROM //////////////////////////////
void readID(uint8_t number, byte storedCard[])
{
  uint8_t start = (number * 4) + 2; // Figure out starting position
  for (uint8_t i = 0; i < 4; i++)
  {                                         // Loop 4 times to get the 4 Bytes
    storedCard[i] = EEPROM.read(start + i); // Assign values read from EEPROM to array
  }
}

/////////////////////////////////////////  Add ID to EEPROM   ///////////////////////////////////
void writeID(byte storedCard[])
{
  if (!findID(storedCard))
  {                                  // Before we write to the EEPROM, check to see if we have seen this card before!
    uint8_t num = EEPROM.read(0);    // Get the numer of used spaces, position 0 stores the number of ID cards
    uint8_t start = (num * 4) + 6;   // Figure out where the next slot starts
    num++;                           // Increment the counter by one
    EEPROM.write(0, num);            // Write the new count to the counter
    for (uint8_t j = 0; j < 4; j++)
    {                                         // Loop 4 times
      EEPROM.write(start + j, storedCard[j]); // Write the array values to EEPROM in the right position
    }
    successWrite();
    Serial.println(F("Successfully added ID record to EEPROM"));
  }
  else
  {
    failedWrite();
    Serial.println(F("Failed! There is something wrong with ID or bad EEPROM"));
  }
}

/////////////////////////////////////////  Remove ID from EEPROM   ///////////////////////////////////
void deleteID(byte deleteCard[])
{
  if (!findID(deleteCard))
  {                // Before we delete from the EEPROM, check to see if we have this card!
    failedWrite(); // If not
    Serial.println(F("Failed! There is something wrong with ID or bad EEPROM"));
  }
  else
  {
    uint8_t num = EEPROM.read(0); // Get the numer of used spaces, position 0 stores the number of ID cards
    uint8_t slot;                 // Figure out the slot number of the card
    uint8_t start;                // = ( num * 4 ) + 6; // Figure out where the next slot starts
    uint8_t looping;              // The number of times the loop repeats
    uint8_t j;
    // uint8_t count = EEPROM.read(0); // Read the first Byte of EEPROM that stores number of cards
    slot = findIDSLOT(deleteCard); // Figure out the slot number of the card to delete
    start = (slot * 4) + 2;
    looping = ((num - slot) * 4);
    num--;                // Decrement the counter by one
    EEPROM.write(0, num); // Write the new count to the counter
    for (j = 0; j < looping; j++)
    {                                                      // Loop the card shift times
      EEPROM.write(start + j, EEPROM.read(start + 4 + j)); // Shift the array values to 4 places earlier in the EEPROM
    }
    for (uint8_t k = 0; k < 4; k++)
    { // Shifting loop
      EEPROM.write(start + j + k, 0);
    }
    successDelete();
    Serial.println(F("Successfully removed ID record from EEPROM"));
  }
}

/////////////////////////////////////////  Check Bytes   ///////////////////////////////////
bool checkTwo(byte a[], byte b[])
{
  for (uint8_t k = 0; k < 4; k++)
  { // Loop 4 times
    if (a[k] != b[k])
    { // IF a != b then false, because: one fails, all fail
      return false;
    }
  }
  return true;
}

/////////////////////////////////////////  Find Slot   ///////////////////////////////////
uint8_t findIDSLOT(byte find[])
{
  byte storedCard[4];
  uint8_t count = EEPROM.read(0); // Read the first Byte of EEPROM that
  for (uint8_t i = 1; i <= count; i++)
  {                        // Loop once for each EEPROM entry
    readID(i, storedCard); // Read an ID from EEPROM, it is stored in storedCard[4]
    if (checkTwo(find, storedCard))
    { // Check to see if the storedCard read from EEPROM is the same as the find[] ID card passed
      return i; // The slot number of the card
    }
  }
  return 0;
}

/////////////////////////////////////////  Find ID From EEPROM   ///////////////////////////////////
bool findID(byte find[])
{
  byte storedCard[4];
  uint8_t count = EEPROM.read(0); // Read the first Byte of EEPROM that
  for (uint8_t i = 1; i < count; i++)
  {                        // Loop once for each EEPROM entry
    readID(i, storedCard); // Read an ID from EEPROM, it is stored in storedCard[4]
    if (checkTwo(find, storedCard))
    { // Check to see if the storedCard read from EEPROM
      return true;
    }
  }
  return false;
}

/////////////////////////////////////////  Write Success to EEPROM   ///////////////////////////////////
// Flashes the green LED 3 times to indicate a successful write to EEPROM
void successWrite()
{
  digitalWrite(blueLed, LED_OFF);  // Make sure blue LED is off
  digitalWrite(redLed, LED_OFF);   // Make sure red LED is off
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is on
  delay(200);
  digitalWrite(greenLed, LED_ON); // Make sure green LED is on
  delay(200);
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  delay(200);
  digitalWrite(greenLed, LED_ON); // Make sure green LED is on
  delay(200);
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  delay(200);
  digitalWrite(greenLed, LED_ON); // Make sure green LED is on
  delay(200);
}

/////////////////////////////////////////  Write Failed to EEPROM   ///////////////////////////////////
// Flashes the red LED 3 times to indicate a failed write to EEPROM
void failedWrite()
{
  digitalWrite(blueLed, LED_OFF);  // Make sure blue LED is off
  digitalWrite(redLed, LED_OFF);   // Make sure red LED is off
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  delay(200);
  digitalWrite(redLed, LED_ON); // Make sure red LED is on
  delay(200);
  digitalWrite(redLed, LED_OFF); // Make sure red LED is off
  delay(200);
  digitalWrite(redLed, LED_ON); // Make sure red LED is on
  delay(200);
  digitalWrite(redLed, LED_OFF); // Make sure red LED is off
  delay(200);
  digitalWrite(redLed, LED_ON); // Make sure red LED is on
  delay(200);
}

/////////////////////////////////////////  Success Remove UID From EEPROM  ///////////////////////////////////
// Flashes the blue LED 3 times to indicate a success delete to EEPROM
void successDelete()
{
  digitalWrite(blueLed, LED_OFF);  // Make sure blue LED is off
  digitalWrite(redLed, LED_OFF);   // Make sure red LED is off
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  delay(200);
  digitalWrite(blueLed, LED_ON); // Make sure blue LED is on
  delay(200);
  digitalWrite(blueLed, LED_OFF); // Make sure blue LED is off
  delay(200);
  digitalWrite(blueLed, LED_ON); // Make sure blue LED is on
  delay(200);
  digitalWrite(blueLed, LED_OFF); // Make sure blue LED is off
  delay(200);
  digitalWrite(blueLed, LED_ON); // Make sure blue LED is on
  delay(200);
}

////////////////////// Check readCard IF is masterCard   ///////////////////////////////////
// Check to see if the ID passed is the master programing card
bool isMaster(byte test[], byte masterCard[])
{
  return checkTwo(test, masterCard);
}

bool monitorWipeButton(uint32_t interval)
{
  uint32_t now = (uint32_t)millis();
  while ((uint32_t)millis() - now < interval)
  {
    // check on every half a second
    if (((uint32_t)millis() % 500) == 0)
    {
      if (digitalRead(wipeB) != LOW)
        return false;
    }
  }
  return true;
}
