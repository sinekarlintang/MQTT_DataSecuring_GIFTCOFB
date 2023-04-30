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

#define BUILTIN_LED 2
unsigned char ad[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E};
unsigned char key[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
unsigned char nonce[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

// Update these with values suitable for your network.

const char* ssid = "The Chosen One";
const char* password = "sekarsinekar";
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
unsigned char msg[MSG_BUFFER_SIZE] = "10.4";
int value = 0;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
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
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("testing", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  char encrypted[96];
char decrypted[32];
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

    unsigned long now = millis();
    if (now - lastMsg > 2000) {
      lastMsg = now;
      ++value;
      for (unsigned long long mlen = 0; (mlen <= MAX_MESSAGE_LENGTH) && (ret_val == KAT_SUCCESS); mlen++) {

        for (unsigned long long adlen = 0; adlen <= MAX_ASSOCIATED_DATA_LENGTH; adlen++) {

          if ((func_ret = crypto_aead_encrypt(ct, &clen, msg, mlen, ad, adlen, NULL, nonce, key)) != 0) {
            ret_val = KAT_CRYPTO_FAILURE;
            break;
          }
          if ((func_ret = crypto_aead_decrypt(msg2, &mlen2, NULL, ct, clen, ad, adlen, nonce, key)) != 0) {
            ret_val = KAT_CRYPTO_FAILURE;
            break;
			    }
        }
      }
      for (int i = 0; i < 32; i++) {
        decrypted[i] = (char) msg2[i];
      }
      for (int i = 0; i < 48; i++) {
        sprintf(&encrypted[i*2], "%02X", ct[i]);
      }
      snprintf (msg1, 160, "%s %s", encrypted, decrypted);
        Serial.print("Publish message: ");
        Serial.print(msg1);
        client.publish("testing", msg1);
    }
  
}
