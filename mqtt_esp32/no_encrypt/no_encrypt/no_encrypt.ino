#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <WiFiGeneric.h>
#include <WiFiMulti.h>
#include <WiFiScan.h>
#include <WiFiServer.h>
#include <WiFiSTA.h>
#include <WiFiType.h>
#include <WiFiUdp.h>

extern "C"{
#include "PubSubClient.h"
}
const int steerPin = 34;
const int throttlePin = 35;
const float MAX_VOLTAGE = 3.3;
const float MAX_THROTTLE = 2047.0;

const char* ssid = "The Chosen One";
const char* password = "sekarsinekar";
const char* mqtt_server = "broker.mqtt-dashboard.com";
char msg1[96];

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
unsigned char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  randomSeed(micros());
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  char new_char[32];
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      // Once connected, publish an announcement...
      client.subscribe("testing");
    } else {
      delay(5000);
    }
  }
}

void setup() {
     unsigned long startTime = millis();
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
    // Konfigurasi ADC
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
      unsigned long executionTime = millis() - startTime;
    
    // Print the execution time
    Serial.print("Program execution time: ");
    Serial.print(executionTime);
    Serial.println(" milliseconds");
}

void loop() {
  char encrypted[96];
  char decrypted[32];
  char message[32];

  // Membaca nilai potensiometer untuk steering
  int steerValue = analogRead(steerPin);
  float steerVoltage = steerValue * (MAX_VOLTAGE / 4095.0);

  // Menghitung sudut steering angle
  float maxSteerValue = (MAX_VOLTAGE / 2.0);
  float steerAngle = 0;
  
  if (steerVoltage <= maxSteerValue) {
    steerAngle = ((steerVoltage - maxSteerValue) * (90.0 / maxSteerValue));
  } else {
    steerAngle = ((steerVoltage - maxSteerValue) * (90.0 / maxSteerValue));
  }

  // Membaca nilai potensiometer untuk throttle
  int throttleValue = analogRead(throttlePin);

  // Memastikan bahwa nilai throttle tidak melebihi nilai maksimum
  if (throttleValue > MAX_THROTTLE) {
    throttleValue = MAX_THROTTLE;
  }

  // Menghitung posisi throttle
  float throttlePosition = 0;
  if (throttleValue > MAX_THROTTLE / 2) {
    throttlePosition = ((throttleValue - MAX_THROTTLE / 2) * (100.0 / (MAX_THROTTLE / 2)));
  }
  sprintf(msg1,"%.13f,%.14f",steerAngle,throttlePosition);
  memcpy(msg,(unsigned char*)message, sizeof(message));
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

   Serial.print("Publish message: ");
      Serial.print(msg1);
      client.publish("testing", msg1);
   
uint32_t freeHeapBytes = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
uint32_t totalHeapBytes = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
float percentageHeapFree = freeHeapBytes * 100.0f / (float)totalHeapBytes;
// Print to serial
Serial.printf("[Memory] %.1f%% free - %d of %d bytes free\n", percentageHeapFree, freeHeapBytes, totalHeapBytes);
  
}
