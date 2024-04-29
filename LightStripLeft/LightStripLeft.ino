#include <M5StickCPlus.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <FastLED.h>

#define Neopixel_PIN 32
#define NUM_LEDS     60

int brightness = 255;

CRGB leds[NUM_LEDS];

const char* SSID = "Next-Guest";
const char* WIFI_PASS = "";

static const char* fingerprint PROGMEM = "DF 12 7C 16 41 3D 8C 87 0D 8A 40 DE FD 08 2E 7F 8D 3D 08 3B";

const char* MQTT_SERVER = "mqtt.nextservices.dk";
const uint16_t MQTT_PORT = 8883; 
const char* MQTT_CLIENT_ID = "FAMSLightL";

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

#define DEFAULTCOLOR CRGB( 15, 0, 151);
#define WAVECOLOR CRGB( 151, 0, 151);

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
    if (strcmp(data, "LSWave") == 0) {
      SWave();
    }

    if (strcmp(data, "LLWave") == 0) {
      LWave();
    }
    if (strcmp(data, "LBWave") == 0) {
      BWave();
    }
    if (strcmp(data, "LBSWave") == 0) {
      BSWave();
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
  for(int i = 0; i < NUM_LEDS; i++) {
    Serial.printf("Light nr: %d\n", i);
    leds[i] = WAVECOLOR;
    FastLED.show();
    delay(25);
  }
  delay(500);
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = DEFAULTCOLOR;
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

// LORTET VIRKER IKKE HJÆLP
void BSWave() {
  for (int i = 0; i < (NUM_LEDS/2)+5; i++) {
    Serial.printf("Light nr: %d & %d\n", i, NUM_LEDS-1-i);
    if (i < NUM_LEDS/2){
      leds[i] = WAVECOLOR;
      leds[NUM_LEDS-1-i] = WAVECOLOR;
    }
    if (i < 5){
      leds[i-5] = DEFAULTCOLOR;
      leds[NUM_LEDS-1-i+5] = DEFAULTCOLOR;
    }
    FastLED.show();
    delay(25);
  }
  delay(100);
  for (int i = (NUM_LEDS/2)-1+5; i >= 0; i--) {
    Serial.printf("Light nr: %d & %d\n", i, NUM_LEDS-1-i);
    if (i > (NUM_LEDS/2)-1){
      leds[i-5] = WAVECOLOR;
      leds[NUM_LEDS-1-i-5] = WAVECOLOR;
    }
    if (i < NUM_LEDS/2) {
      leds[i] = DEFAULTCOLOR;
      leds[NUM_LEDS-1-i] = DEFAULTCOLOR;
    }
    FastLED.show();
    delay(25);
  }
}