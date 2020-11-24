#include <DHT.h>
#include <NewPing.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define TRIGGER_PIN 32
#define ECHO_PIN 35
#define DHT_PIN 33
#define LDR_PIN 34
#define BUZZ_PIN 14
#define RGB_PIN 26

int ldrValue;
float tempValue;
float humValue;
float distance;

#define DHTTYPE DHT11
NewPing sonar(TRIGGER_PIN, ECHO_PIN, 200); 
DHT dht(DHT_PIN, DHTTYPE);

#define LED_COUNT 10
Adafruit_NeoPixel strip(LED_COUNT, RGB_PIN, NEO_GRB + NEO_KHZ800);

unsigned long now = millis();
unsigned long lastMeasure = 0;

char* ssid = "TuePixel";
char* pass = "e70fd27eeaf1Z";

const char* mqtt_server = "192.168.224.194";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, pass);
  WiFi.mode(WIFI_STA); 
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  pinMode(BUZZ_PIN, OUTPUT);
  dht.begin();
  strip.begin();           
  strip.show();             
  strip.setBrightness(150); 

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  now = millis();
  // Publish every 30 seconds
  if (now - lastMeasure > 30000) {
    lastMeasure = now;

    char tempMsg[50];
    snprintf (tempMsg, 50, "%f", readTemp());
    char humMsg[50];
    snprintf (humMsg, 50, "%f", readHum());
    char luxMsg[50];
    snprintf (luxMsg, 50, "%f", readLDR());
    
    client.publish("esp32/temperature", tempMsg); 
    client.publish("esp32/humidity", humMsg);
    client.publish("esp32/light", luxMsg);

    if (readDistance() <= 8) {
        client.publish("esp32/sonar", "OBJECT DETECTED!"); 
    } else {
        client.publish("esp32/sonar", "NO OBJECT DETECTED!"); 
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
  
  Serial.println();
  
  if (topic == "esp32/led") { 
    if (messageTemp == "1") {
      blinkWhite();
    }
  }

  if (topic == "esp32/buzzer") { 
    if (messageTemp == "1") {
      buzz();
    }
  }
  
  Serial.println(); 
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("esp32/led");
      client.subscribe("esp32/buzzer");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

int readLDR() {
  delay(500);  
  ldrValue = analogRead(LDR_PIN);
  return ldrValue;
}

int readTemp() {
  delay(500);  
  tempValue = dht.readTemperature();
  return tempValue;  
}

int readHum() {
  delay(500);  
  humValue = dht.readHumidity();  
  return humValue;
}

int readDistance() {
  delay(500);
  distance = sonar.ping_cm();
  return distance; 
}

void buzz() {
  digitalWrite(BUZZ_PIN, HIGH);   
  delay(500);                       
  digitalWrite(BUZZ_PIN, LOW);    
  delay(500);
}

void blinkWhite() {
  colorWipe(strip.Color(255, 255, 255), 500);
}

void colorWipe(uint32_t color, int wait) {
  for(int i = 0; i < strip.numPixels(); i++) { 
    strip.setPixelColor(i, color);         
    strip.show();                          
    delay(wait);                          
  }
}
