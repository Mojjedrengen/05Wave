#include <M5StickCPlus.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <FastLED.h>
// select the input pin for the potentiometer
#define sensorPin 33
// last variable to store the value coming from the sensor
int cur_sensorValue = 0;
int prev_sensorValue = 0;

const char* SSID = "Next-Guest";
const char* WIFI_PASS = "";

static const char* fingerprint PROGMEM = "DF 12 7C 16 41 3D 8C 87 0D 8A 40 DE FD 08 2E 7F 8D 3D 08 3B";

const char* MQTT_SERVER = "mqtt.nextservices.dk";
const uint16_t MQTT_PORT = 8883; 
const char* MQTT_CLIENT_ID = "FAMSBtn2";

WiFiClientSecure wifiClient;
PubSubClient pubSubClient(wifiClient);

void setup_wifi() {
  Serial.printf("Connecting to %s", SSID);
  WiFi.begin(SSID, WIFI_PASS);

  // Wait until WiFi is connected.
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.printf("\nSuccessfully connected to %s!\n", SSID);
  wifiClient.setInsecure();
}

void setup_mqtt() {
  pubSubClient.setServer(MQTT_SERVER, MQTT_PORT);
  pubSubClient.setCallback(mqtt_callback);
}

void reconnect_mqtt() {
  while (!pubSubClient.connected()) {
    Serial.printf("Attempting to reconnect to MQTT Broker: %s\n", MQTT_SERVER);
    if (pubSubClient.connect(MQTT_CLIENT_ID)) {
      Serial.printf("Connected!\n");
      pubSubClient.publish("DDU4/FAMS", "Connected!");
    } else {
      Serial.printf("failed to connect, rc=%d\n", pubSubClient.state());
      delay(5000);
    }
  }
}

bool btnAPressed = false;
bool btnBPressed = false;
bool btnCPressed = false;

void setup() {
  M5.begin();
  setup_wifi();
  setup_mqtt();
  pinMode(sensorPin, INPUT);
  pinMode(26, INPUT_PULLDOWN);
  pinMode(25, INPUT_PULLDOWN);
  pinMode(36, INPUT_PULLDOWN);
  pinMode(0, INPUT_PULLUP);
}


void loop() {
  if (!pubSubClient.connected()) {
    Serial.println("Connection lost to MQTT Broker!");
    reconnect_mqtt();
  }
  pubSubClient.loop();
  if (digitalRead(26) == HIGH && !btnAPressed) {
    btnAPressed = true;
    pubSubClient.publish("DDU4/FAMS/05Wave", "NW");
    Serial.printf("A Down\n");
  }
  if (digitalRead(26) != HIGH && btnAPressed) {
    btnAPressed = false;
    pubSubClient.publish("DDU4/FAMS/05Wave", "NWUP");
    Serial.printf("A UP\n");
  }
  if (digitalRead(0) == LOW && !btnBPressed) {
    btnBPressed = true;
    pubSubClient.publish("DDU4/FAMS/05Wave", "C");
    Serial.printf("B Down\n");
  }
  if (digitalRead(0) != LOW && btnBPressed) {
    btnBPressed = false;
    pubSubClient.publish("DDU4/FAMS/05Wave", "CUP");
    Serial.printf("B UP\n");
  }
  if (digitalRead(36) == HIGH && !btnCPressed) {
    btnCPressed = true;
    pubSubClient.publish("DDU4/FAMS/05Wave", "SW");
    Serial.printf("C Down\n");
  }
  if (digitalRead(36) != HIGH && btnCPressed) {
    btnCPressed = false;
    pubSubClient.publish("DDU4/FAMS/05Wave", "SWUP");
    Serial.printf("C UP\n");
  }
 
  delay(250);
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    Serial.printf("Message arrived [%s]:", topic);
    // String str = byteArrayToString(payload, sizeof(payload));
    //Serial.println(str);
    char* data = new char[length + 1];
    memcpy(data, payload, length);
    data[length] = '\0';
    Serial.printf("byte: %s\n", data);
    for (int i = 0; i < length; i++) {
        M5.Lcd.print((char)payload[i]);
    }
}