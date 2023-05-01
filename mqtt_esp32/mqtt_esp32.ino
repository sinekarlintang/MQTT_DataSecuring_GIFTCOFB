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
#include "api.h"
#include "cofb.h"
#include "crypto_aead.h"
#include "endian.h"
#include "giftb128.h"
#include "isi.h"
#include "key_schedule.h"
#include "PubSubClient.h"
}
/*
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "1sampai10" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/
unsigned char ad[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E};
unsigned char key[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
unsigned char nonce[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

const int steerPin = 34;
const int throttlePin = 35;
const float MAX_VOLTAGE = 3.3;
const float MAX_THROTTLE = 2047.0;


const char* ssid = "Adriel";
const char* password = "123456789";
const char* mqtt_server = "broker.mqtt-dashboard.com";
float besar = 10.4;
unsigned char message[MAX_MESSAGE_LENGTH];
int i;
int input_length;
int ret_var;
unsigned char ct[MAX_MESSAGE_LENGTH + CRYPTO_ABYTES];
unsigned char msg2[MAX_MESSAGE_LENGTH];
char msg1[96];
unsigned long long  clen, mlen2;
int func_ret, ret_val = KAT_SUCCESS;
char input_string[MAX_MESSAGE_LENGTH * 2 + 1];

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
unsigned char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
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
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe("testing");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
    // Konfigurasi ADC
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
}

void loop() {
  char encrypted[96];
  char decrypted[32];
  char message[14];

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
  sprintf(message,"%.3f,%.5f",steerAngle,throttlePosition);
  Serial.print("Steering Angle: ");
  Serial.print(steerAngle);
  Serial.println(" deegrees");
  Serial.println(throttlePosition);
  memcpy(msg,(unsigned char*)message, sizeof(message));
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

    unsigned long now = millis();
    if (now - lastMsg > 1000) {
      lastMsg = now;
      ++value;
      for (unsigned long long mlen = 0; (mlen <= MAX_MESSAGE_LENGTH) && (ret_val == KAT_SUCCESS); mlen++) {

        for (unsigned long long adlen = 0; adlen <= MAX_ASSOCIATED_DATA_LENGTH; adlen++) {

          if ((func_ret = crypto_aead_encrypt(ct, &clen, msg, mlen, ad, adlen, NULL, nonce, key)) != 0) {
            ret_val = KAT_CRYPTO_FAILURE;
            break;
          }
        }
      }
      for (int i = 0; i < 48; i++) {
        sprintf(&encrypted[i*2], "%02X", ct[i]);
      }
      snprintf (msg1, 100, "%s", encrypted);
      Serial.print("Publish message: ");
      Serial.print(msg1);
      client.publish("testing", msg1);
    }
  
}
