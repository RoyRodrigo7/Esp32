
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SDA_PIN 21
#define SCL_PIN 22

const char* ssid = "huawei";
const char* password = "e1s5p0e9r2a6n1t1o7";

Adafruit_PN532 nfc(SDA_PIN, SCL_PIN);

String urlEncode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  char code2;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encodedString += '+';
    } else if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      code2 = '\0';
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
    }
  }
  return encodedString;
}

void setup(void) {
  Serial.begin(115200);
  Serial.println("Hello!");

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to WiFi");

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

    Serial.print("UID Length: "); Serial.print(uid[7], DEC); Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i = 0; i < uid[7]; i++) {
      Serial.print(" 0x"); Serial.print(uid[i], HEX);
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
        String dataString = "";
        for (int i = 0; i < 16; i++) {
          dataString += (char)data[i];
        }
        Serial.println(dataString);

        // Enviar datos a IFTTT
        if (WiFi.status() == WL_CONNECTED) {
          HTTPClient http;
          String encodedDataString = urlEncode(dataString);
          String url = "https://maker.ifttt.com/trigger/Asistance_Dynavvy/with/key/dHbqb9OcPxqiBDHs8_he9nCoQtSdI0joTMSR7CtBrb0?value1=" + encodedDataString;
          Serial.println("Sending data to IFTTT: " + url);
          http.begin(url);
          int httpResponseCode = http.GET();
          if (httpResponseCode > 0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
          } else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
          }
          http.end();
        } else {
          Serial.println("WiFi not connected");
        }
      } else {
        Serial.println("Error reading data from card.");
      }
    } else {
      Serial.println("Authentication failed.");
    }
  }
  delay(1000);
}
