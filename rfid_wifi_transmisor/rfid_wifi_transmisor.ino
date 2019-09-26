//RST           D9
//SDA(SS)      D10
//MOSI         D11
//MISO         D12
//SCK          D13
 
#include <SPI.h>
#include <MFRC522.h>
#include "RF24.h" 
#include <nRF24L01.h>

int msg[1];
 
const int RST_PIN = 9;            // Pin 9 para el reset del RC522
const int SS_PIN = 10;            // Pin 10 para el SS (SDA) del RC522
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Crear instancia del MFRC522

//7 and 8 are a digital pin numbers to which signals CE and CSN are connected.
RF24 radio(7,8); 
int radioss = 8;

//the address of the modem, that will receive data from Arduino.
const uint64_t pipe = 0xE8E8F0F0E1LL;

bool blockMfrc522;
byte RFID_Data[4]; 


void printArray(byte *buffer, byte bufferSize) {
   for (byte i = 0; i < bufferSize; i++) {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], HEX);
   }
}
 
void setup()
{
   Serial.begin(9600);      //Inicializa la velocidad de Serial
   SPI.begin();         //Función que inicializa SPI
   mfrc522.PCD_Init();     //Función  que inicializa RFID
   RFID_Data[0] = 0;
   RFID_Data[1] = 0;
   RFID_Data[2] = 0;
   RFID_Data[3] = 0;

   // Setup and configure radio
   radio.begin();                      //it activates the modem.
   radio.openWritingPipe(pipe);        //sets the address of the receiver to which the program will send data.
   blockMfrc522 = false;
   digitalWrite(SS_PIN, LOW);
   digitalWrite(radioss,HIGH);
}
 
void loop()
{
   // Read the RFID tag... when not transmitting
   if (!blockMfrc522) getRFID();      

   if (RFID_Data[0] == 0 && RFID_Data[1] == 0 && RFID_Data[2] == 0 && RFID_Data[3] == 0){
   } else {
    blockMfrc522 = true;
    digitalWrite(SS_PIN, HIGH);
    digitalWrite(radioss,LOW);
    send_rfid();
    RFID_Data[0] = 0;
    RFID_Data[1] = 0;
    RFID_Data[2] = 0;
    RFID_Data[3] = 0;
    blockMfrc522 = false;
    digitalWrite(SS_PIN, LOW);
    digitalWrite(radioss,HIGH);
   }
   
}

void send_rfid(){
  Serial.println("comienza a emitir");
  for(int i=0; i<127; i++){
    radio.write(&RFID_Data, sizeof(RFID_Data));
    //Serial.println("emitiendo");
  }
  Serial.println("termina de emitir");
}

void getRFID(){
  // Detectar tarjeta
   if (mfrc522.PICC_IsNewCardPresent())
   {
      if (mfrc522.PICC_ReadCardSerial())
      {

         for (int i = 0; i < mfrc522.uid.size; i++) {
           RFID_Data[i] = mfrc522.uid.uidByte[i];
          }
         
         Serial.print(F("Card UID:"));
         printArray(mfrc522.uid.uidByte, mfrc522.uid.size);
         Serial.println();
 
         // Finalizar lectura actual
         mfrc522.PICC_HaltA();
      }
   }
  // delay(250);
}
