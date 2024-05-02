#include <M5StickCPlus.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <FastLED.h>

#define Neopixel_PIN 32
#define NUM_LEDS     60

int brightness = 127;

CRGB leds[NUM_LEDS];

const char* SSID = "Next-Guest";
const char* WIFI_PASS = "";

static const char* fingerprint PROGMEM = "DF 12 7C 16 41 3D 8C 87 0D 8A 40 DE FD 08 2E 7F 8D 3D 08 3B";

const char* MQTT_SERVER = "mqtt.nextservices.dk";
const uint16_t MQTT_PORT = 8883; 
const char* MQTT_CLIENT_ID = "FAMSLightR";

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
      pubSubClient.subscribe("DDU4/FAMS/Light");
    } else {
      Serial.printf("failed to connect, rc=%d\n", pubSubClient.state());
      delay(5000);
    }
  }
}

#define DEFAULTCOLOR CRGB( 16, 0, 151);
#define WAVECOLOR    CRGB( 160, 0, 151);
#define WAVEGRADIAN1 CRGB( 36, 0, 151);
#define WAVEGRADIAN2 CRGB( 60, 0, 151);
#define WAVEGRADIAN3 CRGB( 90, 0, 151);
#define WAVEGRADIAN4 CRGB( 130, 0, 151);
#define NUM_GRADIAN  5

void setup() {
  // put your setup code here, to run once:
  M5.begin();
  setup_wifi();
  setup_mqtt();
  M5.Lcd.fillScreen(WHITE);
  //M5.Lcd.fillScreen(BLUE);
  M5.update();
  //display();

  // Neopixel initialization
    FastLED.addLeds<WS2811, Neopixel_PIN, GRB>(leds, NUM_LEDS)
        .setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(brightness);

    for (int i = 0; i < NUM_LEDS; i++) {
      //leds[i].red = 0;
      //leds[i].green = 0;
      //leds[i].blue = 151;

      leds[i] = DEFAULTCOLOR;
    }
  FastLED.show();  // must be executed for neopixel becoming effectiv
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!pubSubClient.connected()) {
    Serial.println("Connection lost to MQTT Broker!");
    reconnect_mqtt();
  }
  pubSubClient.loop();
  M5.update();
  delay(250);
/*
  for (int i = 0; i < NUM_LEDS; i++){
    brightness -= 4;
    FastLED.setBrightness(brightness);
    leds[i] = CRGB::White;
    FastLED.show();
    M5.Lcd.fillScreen(WHITE);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.print(brightness);
    M5.update();
    delay(250);
  }
  for (int i = 0; i < NUM_LEDS; i++){
    brightness += 4;
    FastLED.setBrightness(brightness);
    leds[i] = CRGB::Black;
    FastLED.show();
    M5.Lcd.fillScreen(WHITE);
    M5.Lcd.setCursor(0, 0);
    M5.Lcd.print(brightness);
    M5.update();
    delay(250);
  }*/
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
    if (strcmp(data, "SWave") == 0) {
      SWave();
    }

    if (strcmp(data, "LWave") == 0) {
      LWave();
    }
    if (strcmp(data, "BWave") == 0) {
      BWave();
    }
    if (strcmp(data, "BSWave") == 0) {
      BSWave();
    }
    if (strcmp(data, "BDWave") == 0) {
      BDWave();
    }
}

void SWave(){
  for(int i = 0; i < NUM_LEDS+5; i++) {
    Serial.printf("Light nr: %d\n", i);
    if (i < NUM_LEDS){
      leds[i] = WAVECOLOR;
    }
    leds[i-5] = DEFAULTCOLOR;
    FastLED.show();
    delay(25);
  }
}

void LWave(){
  for(int i = 0; i < NUM_LEDS+NUM_GRADIAN; i++) {
    Serial.printf("Light nr: %d\n", i);
    if (i >= 1 && i < NUM_LEDS+1)           leds[i-1] = WAVEGRADIAN1;
    if (i >= 2 && i < NUM_LEDS+2)           leds[i-2] = WAVEGRADIAN2;
    if (i >= 3 && i < NUM_LEDS+3)           leds[i-3] = WAVEGRADIAN3;
    if (i >= 4 && i < NUM_LEDS+4)           leds[i-4] = WAVEGRADIAN4;
    if (i >= NUM_GRADIAN && i < NUM_LEDS+5) leds[i-NUM_GRADIAN] = WAVECOLOR;
    Serial.printf(" G1 on %d\n G2 on %d\n G3 on %d\n G4 on %d\n WC on %d\n _______ \n", i-1, i-2, i-3, i-4, i-5);
    FastLED.show();
    delay(25);
  }
  delay(500);
  for(int i = 0; i < NUM_LEDS+NUM_GRADIAN; i++) {
    if (i >= 1 && i < NUM_LEDS+1)           leds[i-1] = WAVECOLOR;
    if (i >= 2 && i < NUM_LEDS+2)           leds[i-2] = WAVEGRADIAN4;
    if (i >= 3 && i < NUM_LEDS+3)           leds[i-3] = WAVEGRADIAN3;
    if (i >= 4 && i < NUM_LEDS+4)           leds[i-4] = WAVEGRADIAN2;
    if (i >= NUM_GRADIAN && i < NUM_LEDS+5) leds[i-NUM_GRADIAN] = WAVEGRADIAN1;
    FastLED.show();
    delay(25);
  }
}

void BWave(){
  for (int i = 0; i < NUM_LEDS/2; i++) {
    Serial.printf("Light nr: %d & %d\n", i, NUM_LEDS-1-i);
    leds[i] = WAVECOLOR;
    leds[NUM_LEDS-1-i] = WAVECOLOR;
    FastLED.show();
    delay(25);
  }
  delay(100);
  for (int i = (NUM_LEDS/2)-1; i >= 0; i--) {
    Serial.printf("Light nr: %d & %d\n", i, NUM_LEDS-1-i);
    leds[i] = DEFAULTCOLOR;
    leds[NUM_LEDS-1-i] = DEFAULTCOLOR;
    FastLED.show();
    delay(25);
  }
}

void BSWave () {
  int vs = 10;
  for (int i = 0; i < NUM_LEDS+vs*2; i++) {
    if (i < NUM_LEDS/2){
      Serial.printf("Light On nr: %d & %d\n", i, NUM_LEDS-1-i);
      leds[i] = WAVECOLOR;
      leds[NUM_LEDS-1-i] = WAVECOLOR;
    }
    if (i > vs-1 && i < NUM_LEDS/2+vs){
      Serial.printf("Light Off nr: %d & %d\n", i-vs, NUM_LEDS-1-i+vs);
      leds[i-vs] = DEFAULTCOLOR;
      leds[NUM_LEDS-1-i+vs] = DEFAULTCOLOR;
    }
    if (i > (NUM_LEDS/2)+vs && i < NUM_LEDS+vs) {
      Serial.printf("Light On nr: %d & %d\n", i-vs, NUM_LEDS-1-i+vs);
      leds[i-vs] = WAVECOLOR;
      leds[NUM_LEDS-1-i+vs] = WAVECOLOR;
    }
    if (i > (NUM_LEDS/2)+vs*2) {
      Serial.printf("Light Off nr: %d & %d\n", i-vs*2, NUM_LEDS-1-i+vs*2);
      leds[i-vs*2] = DEFAULTCOLOR;
      leds[NUM_LEDS-1-i+vs*2] = DEFAULTCOLOR;
    }
    FastLED.show();
    delay(25);
  }
}

void BDWave(){
  for (int i = 0; i < NUM_LEDS/2; i++) {
    Serial.printf("Light nr: %d & %d\n", i, NUM_LEDS-1-i);
    leds[i] = WAVECOLOR;
    leds[NUM_LEDS-1-i] = WAVECOLOR;
    FastLED.show();
    delay(25);
  }
  delay(100);
  for (int i = 0; i < NUM_LEDS/2; i++) {
    Serial.printf("Light nr: %d & %d\n", i, NUM_LEDS-1-i);
    leds[i] = DEFAULTCOLOR;
    leds[NUM_LEDS-1-i] = DEFAULTCOLOR;
    FastLED.show();
    delay(25);
  }
}