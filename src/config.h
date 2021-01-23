#ifndef CONFIG_H
#define CONFIG_H

#define COMMON_ANODE

#ifdef COMMON_ANODE
  #define LED_ON LOW
  #define LED_OFF HIGH
#else
  #define LED_ON HIGH
  #define LED_OFF LOW
#endif

// Set Led Pins
#define redLed 7 
#define greenLed 6
#define blueLed 5

#define relay 4 // Set Relay Pin
#define wipeB 3 // Button pin for WipeMode

// Set MFRC522 Pins
#define SS_PIN 10
#define RST_PIN 9

#define TAM_EEPROM 16

#endif