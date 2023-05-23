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
unsigned char ad[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E};
unsigned char key[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
unsigned char nonce[32] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

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
unsigned long long  clen, mlen2 =32;
int func_ret, ret_val = KAT_SUCCESS;
char input_string[MAX_MESSAGE_LENGTH * 2 + 1];

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
unsigned char msg[MSG_BUFFER_SIZE] = "10.4";
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

  char decrypted[32];
  char received[length];
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
    for (int i = 0; i < length; i++) {
      received[i] = (char)payload[i];
      Serial.print((char)payload[i]);
    }
    Serial.print("\n");
    for (int i = 0; i < length; i += 2) {
      unsigned int hex_value;
      sscanf(&received[i], "%2x", &hex_value);
      ct[i/2] = (unsigned char)hex_value;
    }
    if ((func_ret = crypto_aead_decrypt(msg2, &mlen2, NULL, ct, 48, ad, 32, nonce, key)) != 0) {
      ret_val = KAT_CRYPTO_FAILURE;
    }
    for (int i = 0; i < 32; i++) {
        //Serial.print((char)msg2[i]);
        decrypted[i] = (char) msg2[i];
    }
    print_to_csv(decrypted);
    Serial.print(decrypted);
    Serial.println();
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
      // ... and resubscribe
      client.subscribe("testing");
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
     unsigned long startTime = millis();
  Serial.begin(115200);
  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
      unsigned long executionTime = millis() - startTime;
    
    // Print the execution time
    Serial.print("Program execution time: ");
    Serial.print(executionTime);
    Serial.println(" milliseconds");
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
uint32_t freeHeapBytes = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
uint32_t totalHeapBytes = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
float percentageHeapFree = freeHeapBytes * 100.0f / (float)totalHeapBytes;
// Print to serial
Serial.printf("[Memory] %.1f%% free - %d of %d bytes free\n", percentageHeapFree, freeHeapBytes, totalHeapBytes);
}
