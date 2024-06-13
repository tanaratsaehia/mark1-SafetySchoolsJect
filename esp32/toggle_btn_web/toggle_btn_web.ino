#include <HTTPClient.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>

const char *host = "safety-schools.com/php_action";
const char *updater = "/updateState.php";
const char *station_token = "station_token=103353028743"; //don't forget to change!!
// safety-schools.com/php_action/updateState.php

bool state = false;
unsigned long previousMillis;
unsigned long previousMillis2;
const long interval = 3000;
const int togglePin = 13;
const int siren = 26;          //relay 1
const int LEDstatusWifi = 25;  //relay2
const int buzzer = 33;

int lastButtonState;
int currentButtonState;
int countState;
int lossWifiCount;
int playing;
int tonefeq = 3000;

WiFiManager wifiManager;

void setup() {
  Serial.begin(115200);
  pinMode(togglePin, INPUT_PULLUP);
  pinMode(2, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(LEDstatusWifi, OUTPUT);
  pinMode(siren, OUTPUT);

  digitalWrite(2, LOW);
  digitalWrite(buzzer, HIGH);
  digitalWrite(LEDstatusWifi, LOW);
  digitalWrite(siren, LOW);

  delay(2000);
  holdToOffline();

  Serial.println(station_token);
  // wifiManager.autoConnect("SafetySchoolConfig");
  wifiManager.setTimeout(300);  // set timeout to 5 minutes
  wifiManager.autoConnect("Smart Safety School System");

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi connection failed, resetting ESP32...");
    delay(2500);
    ESP.restart();
  }

  Serial.println("Connected to Wi-Fi!");
  Serial.println(WiFi.localIP());
  digitalWrite(2, HIGH);
  digitalWrite(LEDstatusWifi, HIGH);
  buzzeron();
  // buzzeronTone();
}


void loop() {
  if (millis() - previousMillis >= 60000 & WiFi.status() != WL_CONNECTED) {
    lossWifiCount = 0;
    digitalWrite(buzzer, HIGH);
    digitalWrite(LEDstatusWifi, LOW);
    delay(300);
    ESP.restart();
  }
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(buzzer, HIGH);
    digitalWrite(LEDstatusWifi, HIGH);
  } else if (WiFi.status() != WL_CONNECTED) {
    lossWifiCount++;
    if (lossWifiCount == 1) {
      previousMillis = millis();
    }
    // Serial.println("WiFi connection lost. Resetting ESP8285...");
    digitalWrite(buzzer, HIGH);
    delay(150);
    digitalWrite(buzzer, LOW);
    digitalWrite(LEDstatusWifi, LOW);
    delay(150);
    return;
  }

  lastButtonState = currentButtonState;
  currentButtonState = digitalRead(togglePin);
  Serial.println(currentButtonState);
  if (lastButtonState == 1) {
    countState += 1;
  }
  if (countState == 1500) {
    toggleState();
    if (state == 1) {
      digitalWrite(siren, HIGH);
    } else if (state == 0) {
      digitalWrite(siren, LOW);
    }
    delay(3500);
    countState = 0;
  }

  if (state == 1) {
    if (millis() - previousMillis2 >= 1000 & millis() - previousMillis2 <= 1100) {
      updateState();
    } else if (millis() - previousMillis2 >= 3000) {
      digitalWrite(siren, LOW);
      if (millis() - previousMillis2 >= 4000) {
        previousMillis2 = millis();
        digitalWrite(siren, HIGH);
      }
    }
    return;
  }
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 3000) {
    previousMillis = currentMillis;
    updateState();
    if (state == 1) {
      digitalWrite(siren, HIGH);
    } else if (state == 0) {
      digitalWrite(siren, LOW);
    }
  }
}

void updateState() {
  HTTPClient http;
  http.begin(String("http://") + host + updater + "?" + station_token);
  int httpCode = http.GET();
  if (httpCode == 200) {
    String body = http.getString();
    Serial.println(body);

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, body);
    JsonObject obj = doc.as<JsonObject>();
    String txtState = obj[String("newState")];

    Serial.println(txtState);

    if (txtState == "On") {
      state = 1;
    } else if (txtState == "Off") {
      state = 0;
    }

  } else {
    Serial.println("Error getting state: " + String(httpCode));
  }
  http.end();
  Serial.println("State: " + String(state));
}

void toggleState() {
  HTTPClient http;
  http.begin(String("http://") + host + updater + "?" + station_token + "&trigger=1");
  int httpCode = http.GET();
  if (httpCode == 200) {
    String body = http.getString();
    Serial.println(body);

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, body);
    JsonObject obj = doc.as<JsonObject>();
    String txtState = obj[String("newState")];

    Serial.println(txtState);

    if (txtState == "On") {
      state = 1;
    } else if (txtState == "Off") {
      state = 0;
    }
    previousMillis2 = millis();
  } else {
    Serial.println("Error getting state: " + String(httpCode));
  }
  http.end();
  Serial.println("State: " + String(state));
}

void holdToOffline() {
  if (digitalRead(togglePin) == 1) {
    for (int i=1; i<= 3; i++){
      digitalWrite(buzzer, LOW);
      delay(70);
      digitalWrite(buzzer, HIGH);
      delay(70);
    }

    while (digitalRead(togglePin) == 1)
      ;

    int count;
    unsigned long premillis;

    while (true) {
      if (digitalRead(togglePin) == 1) {
        count++;
      }
      if (count >= 1500 & digitalRead(togglePin) == 0) {
        digitalWrite(LEDstatusWifi, !digitalRead(LEDstatusWifi));
        count = 0;
      }
      if (digitalRead(togglePin) == 0 & millis() - premillis >= 100){
        premillis = millis();
        count = 0;
      }
    }
  }
}

void tone(byte pin, int freq) {
  ledcSetup(0, 2000, 8);   // setup beeper
  ledcAttachPin(pin, 0);   // attach beeper
  ledcWriteTone(0, freq);  // play tone
}
void noTone() {
  tone(playing, 0);
}

void buzzeronTone() {
  Serial.println("Buzzer On!");
  for(int i=1; i<=5; i++){
  tone(buzzer, tonefeq);
  delay(70);
  noTone();
  delay(70);
  tone(buzzer, tonefeq);
  delay(200);
  noTone();
  delay(70);
  tone(buzzer, tonefeq);
  delay(70);
  noTone();
  delay(70);
  tone(buzzer, tonefeq);
  delay(70);
  noTone();
  delay(70);
  tone(buzzer, tonefeq);
  delay(200);
  noTone();
  delay(70);
  tone(buzzer, tonefeq);
  delay(70);
  noTone();
  delay(70);
  tone(buzzer, tonefeq);
  delay(70);
  noTone();
  delay(600);
  }
}

void buzzeron() {
  Serial.println("Buzzer On!");
  for (int i=1; i<=5; i++){
  digitalWrite(buzzer, LOW);
  delay(70);
  digitalWrite(buzzer, HIGH);
  delay(70);
  digitalWrite(buzzer, LOW);
  delay(200);
  digitalWrite(buzzer, HIGH);
  delay(70);
  digitalWrite(buzzer, LOW);
  delay(70);
  digitalWrite(buzzer, HIGH);
  delay(70);
  digitalWrite(buzzer, LOW);
  delay(70);
  digitalWrite(buzzer, HIGH);
  delay(70);
  digitalWrite(buzzer, LOW);
  delay(200);
  digitalWrite(buzzer, HIGH);
  delay(70);
  digitalWrite(buzzer, LOW);
  delay(70);
  digitalWrite(buzzer, HIGH);
  delay(70);
  digitalWrite(buzzer, LOW);
  delay(70);
  digitalWrite(buzzer, HIGH);
  delay(600);
  }
}