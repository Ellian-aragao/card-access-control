#include <Arduino.h>

// Configurations of Constants for loop and card library
#include "config.h"
#include "../../lib/card/card.h"

/*
   --------------------------------------------------------------------------------------------------------------------
   Example sketch/program showing An Arduino Door Access Control featuring RFID, EEPROM, Relay
   --------------------------------------------------------------------------------------------------------------------
   This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid

   This example showing a complete Door Access Control System

  Simple Work Flow (not limited to):
                                    +--------------+
  +----------------------------------->READ TAGS+^------------------------------------------+
  |                              +--------------------+                                     |
  |                              |                    |                                     |
  |                              |                    |                                     |
  |                         +----v-----+        +-----v----+                                |
  |                         |MASTER TAG|        |OTHER TAGS|                                |
  |                         +--+-------+        ++-------------+                            |
  |                            |                 |             |                            |
  |                            |                 |             |                            |
  |                      +-----v---+        +----v----+   +----v------+                     |
  |         +------------+READ TAGS+---+    |KNOWN TAG|   |UNKNOWN TAG|                     |
  |         |            +-+-------+   |    +-----------+ +------------------+              |
  |         |              |           |                |                    |              |
  |    +----v-----+   +----v----+   +--v--------+     +-v----------+  +------v----+         |
  |    |MASTER TAG|   |KNOWN TAG|   |UNKNOWN TAG|     |GRANT ACCESS|  |DENY ACCESS|         |
  |    +----------+   +---+-----+   +-----+-----+     +-----+------+  +-----+-----+         |
  |                       |               |                 |               |               |
  |       +----+     +----v------+     +--v---+             |               +--------------->
  +-------+EXIT|     |DELETE FROM|     |ADD TO|             |                               |
          +----+     |  EEPROM   |     |EEPROM|             |                               |
                     +-----------+     +------+             +-------------------------------+


   Use a Master Card which is act as Programmer then you can able to choose card holders who will granted access or not

 * **Easy User Interface**

   Just one RFID tag needed whether Delete or Add Tags. You can choose to use Leds for output or Serial LCD module to inform users.

 * **Stores Information on EEPROM**

   Information stored on non volatile Arduino's EEPROM memory to preserve Users' tag and Master Card. No Information lost
   if power lost. EEPROM has unlimited Read cycle but roughly 100,000 limited Write cycle.

 * **Security**
   To keep it simple we are going to use Tag's Unique IDs. It's simple and not hacker proof.

   @license Released into the public domain.

   Typical pin layout used:
   -----------------------------------------------------------------------------------------
               MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
               Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
   Signal      Pin          Pin           Pin       Pin        Pin              Pin
   -----------------------------------------------------------------------------------------
   RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
   SPI SS      SDA(SS)      10            53        D10        10               10
   SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
   SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
   SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
*/

/*
  Instead of a Relay you may want to use a servo. Servos can lock and unlock door locks too
  Relay will be used by default
*/

// #include <Servo.h>

/*
  For visualizing whats going on hardware we need some leds and to control door lock a relay and a wipe button
  (or some other hardware) Used common anode led,digitalWriting HIGH turns OFF led Mind that if you are going
  to use common cathode led or just separate leds, simply comment out #define COMMON_ANODE,
*/

bool programMode = false; // initialize programming mode to false

uint8_t successRead; // Variable integer to keep if we have Successful Read from Reader

byte storedCard[4]; // Stores an ID read from EEPROM
byte readCard[4];   // Stores scanned ID read from RFID Module
byte masterCard[4]; // Stores master card's ID read from EEPROM

byte eeprom[TAM_EEPROM];

// Create MFRC522 instance.
MFRC522 mfrc522(SS_PIN, RST_PIN);

///////////////////////////////////////// Setup ///////////////////////////////////
void setup()
{
  //Arduino Pin Configuration
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(wipeB, INPUT_PULLUP); // Enable pin's pull up resistor
  pinMode(relay, OUTPUT);

  //Be careful how relay circuit behave on while resetting or power-cycling your Arduino
  digitalWrite(relay, HIGH);       // Make sure door is locked
  digitalWrite(redLed, LED_OFF);   // Make sure led is off
  digitalWrite(greenLed, LED_OFF); // Make sure led is off
  digitalWrite(blueLed, LED_OFF);  // Make sure led is off

  //Protocol Configuration
  Serial.begin(9600l); // Initialize serial communications with PC
  SPI.begin();        // MFRC522 Hardware uses SPI protocol
  mfrc522.PCD_Init(); // Initialize MFRC522 Hardware

  //If you set Antenna Gain to Max it will increase reading distance
  //mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);

  Serial.println(F("Access Control Example v0.1")); // For debugging purposes
  ShowReaderDetails(&mfrc522);                      // Show details of PCD - MFRC522 Card Reader details

  // Check if master card defined, if not let user choose a master card
  // This also useful to just redefine the Master Card
  // You can keep other EEPROM records just write other than 143 to EEPROM address 1
  // EEPROM address 1 should hold magical number which is '143'
  // if (EEPROM.read(1) != 143)
  // {
    Serial.println(F("No Master Card Defined"));
    Serial.println(F("Scan A PICC to Define as Master Card"));
    do
    {
      successRead = getID(&mfrc522, readCard); // sets successRead to 1 when we get read from reader otherwise 0
      digitalWrite(blueLed, LED_ON);           // Visualize Master Card need to be defined
      delay(200);
      digitalWrite(blueLed, LED_OFF);
      delay(200);
    } while (!successRead); // Program will not go further while you not get a successful read
    for (uint8_t j = 0; j < 4; j++)
    {                                   // Loop 4 times
      eeprom[j + 2] = readCard[j]; // EEPROM.write(2 + j, readCard[j]); // Write scanned PICC's UID to EEPROM, start from address 3
    }
    eeprom[1] = 143; // EEPROM.write(1, 143); // Write to EEPROM we defined Master Card.
    Serial.println(F("Master Card Defined"));
  // }
  Serial.println(F("-------------------"));
  Serial.println(F("Master Card's UID"));
  for (uint8_t i = 0; i < 4; i++)
  {                                     // Read Master Card's UID from EEPROM
    masterCard[i] = eeprom[i + 2]; //EEPROM.read(2 + i); // Write it to masterCard
    Serial.print(masterCard[i], HEX);
  }
  Serial.println("");
  Serial.println(F("-------------------"));
  Serial.println(F("Everything is ready"));
  Serial.println(F("Waiting PICCs to be scanned"));
  cycleLeds(); // Everything ready lets give user some feedback by cycling leds
}

///////////////////////////////////////// Main Loop ///////////////////////////////////
void loop()
{
  do
  {
    successRead = getID(&mfrc522, readCard); // sets successRead to 1 when we get read from reader otherwise 0
    // When device is in use if wipe button pressed for 10 seconds initialize Master Card wiping
    // if (digitalRead(wipeB) == LOW)
    // { // Check if button is pressed
    //   // Visualize normal operation is iterrupted by pressing wipe button Red is like more Warning to user
    //   digitalWrite(redLed, LED_ON);    // Make sure led is off
    //   digitalWrite(greenLed, LED_OFF); // Make sure led is off
    //   digitalWrite(blueLed, LED_OFF);  // Make sure led is off
    //   // Give some feedback
    //   Serial.println(F("Wipe Button Pressed"));
    //   Serial.println(F("Master Card will be Erased! in 10 seconds"));
    //   bool buttonState = monitorWipeButton(10000); // Give user enough time to cancel operation
    //   if (buttonState == true && digitalRead(wipeB) == LOW)
    //   {                     // If button still be pressed, wipe EEPROM
    //     EEPROM.write(1, 0); // Reset Magic Number.
    //     Serial.println(F("Master Card Erased from device"));
    //     Serial.println(F("Please reset to re-program Master Card"));
    //     while (1);
    //   }
    //   Serial.println(F("Master Card Erase Cancelled"));
    // }
    if (programMode)
    {
      cycleLeds(); // Program Mode cycles through Red Green Blue waiting to read a new card
    }
    else
    {
      normalModeOn(); // Normal mode, blue Power LED is on, all others are off
    }
  } while (!successRead); //the program will not go further while you are not getting a successful read
  if (programMode)
  {
    if (isMaster(readCard, masterCard))
    { //When in program mode check First If master card scanned again to exit program mode
      Serial.println(F("Master Card Scanned"));
      Serial.println(F("Exiting Program Mode"));
      Serial.println(F("-----------------------------"));
      programMode = false;
      return;
    }
    else if (findID(readCard))
    { // If scanned card is known delete it
      Serial.println(F("I know this PICC, removing..."));
      deleteID(readCard);
      Serial.println("-----------------------------");
      Serial.println(F("Scan a PICC to ADD or REMOVE to EEPROM"));
    }
    else
    { // If scanned card is not known add it
      Serial.println(F("I do not know this PICC, adding..."));
      writeID(readCard);
      Serial.println(F("-----------------------------"));
      Serial.println(F("Scan a PICC to ADD or REMOVE to EEPROM"));
    }
  }
  else if (isMaster(readCard, masterCard))
  { // If scanned card's ID matches Master Card's ID - enter program mode
    programMode = true;
    Serial.println(F("Hello Master - Entered Program Mode"));
    uint8_t count = eeprom[0]; // EEPROM[0]; // EEPROM.read(0); // Read the first Byte of EEPROM that
    Serial.print(F("I have "));     // stores the number of ID's in EEPROM
    Serial.print(count);
    Serial.print(F(" record(s) on EEPROM"));
    Serial.println("");
    Serial.println(F("Scan a PICC to ADD or REMOVE to EEPROM"));
    Serial.println(F("Scan Master Card again to Exit Program Mode"));
    Serial.println(F("-----------------------------"));
  }
  else if (findID(readCard))
  { // If not, see if the card is in the EEPROM
    Serial.println(F("Welcome, You shall pass"));
    granted(300); // Open the door lock for 300 ms
  }
  else
  { // If not, show that the ID was not valid
    Serial.println(F("You shall not pass"));
    denied();
  }
}
