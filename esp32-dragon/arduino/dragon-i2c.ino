#include <DHT.h>
#include <NewPing.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <stdio.h>
#include <string.h>

#define BUILTIN_LED 13 
#define TRIGGER_PIN 32
#define ECHO_PIN 35
#define DHT_PIN 33
#define LDR_PIN 34
#define BUZZ_PIN 14
#define RGB_PIN 26
#define MOTORA_PIN1 4
#define MOTORA_PIN2 16 
#define MOTORB_PIN1 17 
#define MOTORB_PIN2 5 

int ldrValue;
float tempValue;
float humValue;
float distance;

#define DHTTYPE DHT11
NewPing sonar(TRIGGER_PIN, ECHO_PIN, 200); 
DHT dht(DHT_PIN, DHTTYPE);

#define LED_COUNT 10
Adafruit_NeoPixel strip(LED_COUNT, RGB_PIN, NEO_GRB + NEO_KHZ800);

String inputId = "";
String outputId = "";
String logicId = "";
String data = "BUZZ,IF,DARK";
//const byte I2C_MASTER_ADDR = 0x20;
//const byte I2C_SLAVE_ADDR = 0x20;

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(BUZZ_PIN, OUTPUT);
  pinMode(MOTORA_PIN1, OUTPUT);
  pinMode(MOTORA_PIN2, OUTPUT);
  pinMode(MOTORB_PIN1, OUTPUT);
  pinMode(MOTORB_PIN2, OUTPUT); 

  Serial.begin(115200);
  //Wire.begin(I2C_MASTER_ADDR);
  //Wire.begin(I2C_SDA, I2C_SCL);
  
  dht.begin();
  strip.begin();           
  strip.show();             
  strip.setBrightness(150); 
}

void loop() {
  delay(100);
  //Wire.onReceive(receiveEvent);
  splitIds();
  
  if (logicId == "IF") { // if input then output 
    if (inputId == "LIGHT") {
        if (readLDR() >= 60) {
          executeOutput(outputId);
        }
      } else if (inputId == "DARK") {
        if (readLDR() < 60) {
          executeOutput(outputId);
        }
      } else if (inputId == "HOT") {
        if (readTemp() >= 30) {
          executeOutput(outputId);
        }
      } else if (inputId == "COLD") {
        if (readTemp() <= 15) {
          executeOutput(outputId);
        }
      } else if (inputId == "PROX") {
        if (readDistance() <= 8) {
          executeOutput(outputId);
        }
      } else {
        Serial.println("Error input!");
        blinkLED();
      } 
  }
  else if (logicId == "UNTIL") { // until input keep output 
     if (inputId == "LIGHT") {
        while (readLDR() < 60) {
          executeOutput(outputId);
        }
      } else if (inputId == "DARK") {
        while (readLDR() > 60) {
          executeOutput(outputId);
        }
      } else if (inputId == "HOT") {
        while (readTemp() < 30) {
          executeOutput(outputId);
        }
      } else if (inputId == "COLD") {
        while (readTemp() > 15) {
          executeOutput(outputId);
        }
      } else if (inputId == "PROX") {
        while (readDistance() > 8) {
          executeOutput(outputId);
        }
      } else {
        Serial.println("Error input!");
        blinkLED();
      } 
  }
  else {
    Serial.println("Error logic!");
    blinkLED(); 
  }
}

void executeOutput(String id) {
  if (id == "FIRE") {
    blinkRed();
  } else if (id == "FLASH") {
    blinkWhite();
  } else if (id == "BUZZ") {
    buzz();
  } else if (id == "FOR") {
    moveForward();
    stopDC();
  } else if (id == "BACK") {
    moveBackward();  
    stopDC();
  } else {
    Serial.println("Error output!");
    blinkLED(); 
  } 
}

// check IDs
/*void receiveEvent(int byteCount) {
  Wire.requestFrom(I2C_SLAVE_ADDR, 20);
  while (Wire.available())
  {
     data += (char)Wire.read();
     Serial.println(data);
     data = "";
  }
}*/ 

void splitIds() {
  int commaIndex = data.indexOf(',');
  int secondCommaIndex = data.indexOf(',', commaIndex + 1);
  outputId = data.substring(0, commaIndex);
  logicId = data.substring(commaIndex + 1, secondCommaIndex);
  inputId = data.substring(secondCommaIndex + 1);
  Serial.println(inputId + " - " + logicId + " - " + outputId);
}

// debugging
void blinkLED() {
  digitalWrite(BUILTIN_LED, LOW);   
  delay(500);                     
  digitalWrite(BUILTIN_LED, HIGH);  
  delay(500);
}

// read sensors
int readLDR() {
  delay(500);  
  ldrValue = analogRead(LDR_PIN);
  Serial.print(ldrValue); 
  Serial.println(" \n"); 
  return ldrValue;
}

int readTemp() {
  delay(500);  
  tempValue = dht.readTemperature();
  Serial.print(tempValue);
  Serial.print("°C");
  Serial.println(" \n"); 
  return tempValue;  
}

int readHum() {
  delay(500);  
  humValue = dht.readHumidity();  
  Serial.print(humValue);
  Serial.println(" \n");
  return humValue;
}

int readDistance() {
  delay(500);
  distance = sonar.ping_cm();
  Serial.print(distance); 
  Serial.print("cm");
  Serial.println(" \n"); 
  return distance; 
}

// control buzzer
void buzz() {
  Serial.println("Buzz!");
  digitalWrite(BUZZ_PIN, HIGH);   
  delay(500);                       
  digitalWrite(BUZZ_PIN, LOW);    
  delay(500);
}

// control LED strip
void blinkRed() {
  Serial.println("Fire!");
  colorWipe(strip.Color(255, 0, 0), 500);
}

void blinkWhite() {
  Serial.println("Flash!");
  colorWipe(strip.Color(255, 255, 255), 500);
}

void colorWipe(uint32_t color, int wait) {
  for(int i = 0; i < strip.numPixels(); i++) { 
    strip.setPixelColor(i, color);         
    strip.show();                          
    delay(wait);                          
  }
}

// control DC motor
void stopDC() {
  Serial.println("Motors stop!"); 
  digitalWrite(MOTORA_PIN1, LOW);
  digitalWrite(MOTORA_PIN2, LOW);
  digitalWrite(MOTORB_PIN1, LOW);
  digitalWrite(MOTORB_PIN2, LOW);
  delay(1000);
}

void moveForward() {
  Serial.println("Motors move forward!");
  digitalWrite(MOTORA_PIN1, LOW);
  digitalWrite(MOTORA_PIN2, HIGH); 
  digitalWrite(MOTORB_PIN1, LOW);
  digitalWrite(MOTORB_PIN2, HIGH); 
  delay(5000);
}

void moveBackward() {
  Serial.println("Motors move backward!");
  digitalWrite(MOTORA_PIN1, HIGH);
  digitalWrite(MOTORA_PIN2, LOW); 
  digitalWrite(MOTORB_PIN1, HIGH);
  digitalWrite(MOTORB_PIN2, LOW); 
  delay(5000);
}
