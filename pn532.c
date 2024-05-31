# para leer datos 

#include <Wire.h>
#include <Adafruit_PN532.h>

#define SDA_PIN 21
#define SCL_PIN 22

Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

void setup(void) {
  Serial.begin(115200);
  Serial.println("Hello!");

  nfc.begin();
  
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Configurar el PN532 para que funcione con tarjetas MIFARE
  nfc.SAMConfig();
  
  Serial.println("Waiting for an NFC card...");
}

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Almacena el UID leído

  // Esperar hasta que se detecte una tarjeta NFC
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uid[7]);

  if (success) {
    Serial.println("Found an NFC card!");

    Serial.print("UID Length: ");Serial.print(uid[7], DEC);Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i=0; i < uid[7]; i++) {
      Serial.print(" 0x");Serial.print(uid[i], HEX);
    }
    Serial.println("");

    // Realizar autenticación
    uint8_t key[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; // Clave de autenticación por defecto para tarjetas MIFARE Classic
    success = nfc.mifareclassic_AuthenticateBlock(uid, uid[7], 4, 0, key);
    if (success) {
      Serial.println("Authentication successful!");
      
      // Leer datos de la tarjeta NFC
      uint8_t data[16]; // Almacena los datos leídos
      success = nfc.mifareclassic_ReadDataBlock(4, data);
      if (success) {
        Serial.println("Data read from card:");
        Serial.println((char*)data);
      } else {
        Serial.println("Error reading data from card.");
      }
    } else {
      Serial.println("Authentication failed.");
    }
  }
  delay(1000);
}

# Para escribir los datos


#include <Wire.h>
#include <Adafruit_PN532.h>

#define SDA_PIN 21
#define SCL_PIN 22

Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

void setup(void) {
  Serial.begin(115200);
  Serial.println("Hello!");

  nfc.begin();
  
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Configurar el PN532 para que funcione con tarjetas MIFARE
  nfc.SAMConfig();
  
  Serial.println("Waiting for an NFC card...");
}

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Almacena el UID leído

  // Esperar hasta que se detecte una tarjeta NFC
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uid[7]);

  if (success) {
    Serial.println("Found an NFC card!");

    Serial.print("UID Length: ");Serial.print(uid[7], DEC);Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i=0; i < uid[7]; i++) {
      Serial.print(" 0x");Serial.print(uid[i], HEX);
    }
    Serial.println("");

    // Realizar autenticación
    uint8_t key[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; // Clave de autenticación por defecto para tarjetas MIFARE Classic
    success = nfc.mifareclassic_AuthenticateBlock(uid, uid[7], 4, 0, key);
    if (success) {
      Serial.println("Authentication successful!");
      
      // Convertir la cadena de caracteres a un array de bytes
      char message[] = "SIS15092611";
      uint8_t data[16]; // Almacena los datos a escribir
      strncpy((char*)data, message, sizeof(data));

      // Escribir datos en la tarjeta NFC
      success = nfc.mifareclassic_WriteDataBlock(4, data);
      if (success) {
        Serial.println("Data written to card successfully!");
      } else {
        Serial.println("Error writing data to card.");
      }
    } else {
      Serial.println("Authentication failed.");
    }
  }
  delay(1000);
}
