#include <Wire.h>

// const String strId = "DARK";
const String strId = "DARK,IF,FLASH";
// const String strId = "PROX,IF,BACK";
// const String strId = "HOT,UNTIL,FIRE";

const byte I2C_MASTER_ADDR = 0x20;
const byte I2C_SLAVE_ADDR = 0x20;
//const byte I2C_NEXT_SLAVE_ADDR = 0x20;

String dataReceived = "";
String dataSent = "";

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SLAVE_ADDR); 
  // Wire.begin(I2C_SDA, I2C_SCL); 
  blinkLED(); 
  Wire.onReceive(receiveEvent);  
  Wire.onRequest(requestEvent);  
}

void loop() {
  delay(100);
}

void receiveEvent(int byteCount) {
  /*if (I2C_NEXT_SLAVE_ADDR != NULL) {
    Wire.requestFrom(I2C_NEXT_SLAVE_ADDR, 15);
    while (Wire.available())
    {
     dataReceived += (char)Wire.read();
     Serial.print("Received:");
     Serial.println(dataReceived);
     dataSent += dataReceived;
     dataSent += ",";
    }
  }*/  
  dataSent += strId;
  Serial.println(dataSent);
  dataReceived = "";
}

void requestEvent() {
  Wire.beginTransmission(I2C_MASTER_ADDR);
  Serial.print("To send:");
  Serial.println(dataSent);
  Wire.write(dataSent.c_str());
  Wire.endTransmission();
  dataSent = "";
}

// debugging
void blinkLED() {
  digitalWrite(13, LOW);   
  delay(500);                     
  digitalWrite(13, HIGH);  
  delay(500);
}
