// Password Vault
// Firmware for the Arduino UNO
// Distributed under the MIT License
// © Copyright Maxim Bortnikov 2021
// For more information please visit
// https://github.com/Northstrix/Password_Vault
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(5, 4); // RX, TX
#include "GBUS.h"
GBUS bus(&mySerial, 5, 20);
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
struct myStruct {
  byte dt[4];
};
void setup() 
{
  Serial.begin(115200);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  pinMode(8,OUTPUT);
  mySerial.begin(9600);
}
void loop() 
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  myStruct data;
  data.dt[0] = mfrc522.uid.uidByte[0];
  data.dt[1] = mfrc522.uid.uidByte[1];
  data.dt[2] = mfrc522.uid.uidByte[2];
  data.dt[3] = mfrc522.uid.uidByte[3];
  Serial.println();
  digitalWrite(8,HIGH);
  bus.sendData(3, data);
  delay(750);
  digitalWrite(8,LOW);

} 
