#include <ESP8266HTTPClient.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>

#define RX_PIN 3
#define TX_PIN 1

#define SCHOOL_TOKEN 103353055527

#define OnCmd 1
#define OffCmd 0


WiFiClient wifiClient;

//https://safety-schools.com/php_action/updateState.php?trigger=1&station_token=103353031809

const char *host = "safety-schools.com";
const char *updater = "/php_action/updateState.php";
const char *station_token = "station_token=";  // don't forget to change!!
bool state = false;
unsigned long previousMillis;
unsigned long previousMillis2;
unsigned long millisDeviceReady = 0;
const long interval = 3000;

const int togglePin = RX_PIN;
const int siren = 12;  //relay
const int status = 13;
const int buzzer = TX_PIN;
const int LedDeviceReady = 2;

int lastButtonState;
int currentButtonState;
int countState;
int lossWifiCount;

WiFiManager wifiManager;

void setup() {
  Serial.begin(9600);
  pinMode(togglePin, INPUT_PULLUP);
  pinMode(siren, OUTPUT);
  pinMode(status, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(LedDeviceReady, OUTPUT);


  digitalWrite(LedDeviceReady, LOW);
  digitalWrite(buzzer, HIGH);
  digitalWrite(status, OffCmd);
  digitalWrite(siren, LOW);

  wifiManager.setTimeout(300);
  wifiManager.autoConnect("Smart Safety School System");
  Serial.println("Connected to Wi-Fi!");
  Serial.println(WiFi.localIP());
  digitalWrite(status, OnCmd);
  digitalWrite(buzzer, HIGH);
  buzzeron();
}

void loop() {
  blinkLedDeviceStandby();

  if (millis() - previousMillis >= 60000 & WiFi.status() != WL_CONNECTED) {
    lossWifiCount = 0;
    digitalWrite(buzzer, HIGH);
    digitalWrite(status, !digitalRead(status));
    delay(150);
    ESP.restart();
  }
  if (WiFi.status() != WL_CONNECTED) {
    lossWifiCount++;
    if (lossWifiCount == 1) {
      previousMillis = millis();
    }
    // Serial.println("WiFi connection lost. Resetting ESP8285...");
    digitalWrite(buzzer, HIGH);
    digitalWrite(status, OffCmd);
    delay(150);
    digitalWrite(buzzer, LOW);
    digitalWrite(status, OnCmd);
    delay(150);
    return;
  } else if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(buzzer, HIGH);
    digitalWrite(status, OnCmd);
  }
  //Serial.println(stateReader());
  //delay(3000);
  lastButtonState = currentButtonState;
  currentButtonState = digitalRead(togglePin);
  //Serial.println(currentButtonState);
  if (lastButtonState == 0) {
    countState += 1;
    Serial.println(countState);
  }
  if (countState >= 65) {
    toggleState();
    if (state == 1) {
      digitalWrite(siren, HIGH);
    } else if (state == 0) {
      digitalWrite(siren, LOW);
    }
    // Serial.println("command send!");
    while (!digitalRead(togglePin)){delay(100);}
    // delay(3500);
    countState = 0;
  }
  if (state == 1) {
    if (millis() - previousMillis2 >= 3000) {
      digitalWrite(siren, LOW);
      if (millis() - previousMillis2 >= 4000) {
        previousMillis2 = millis();
        digitalWrite(siren, HIGH);
      }
    }
  }
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
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

  http.begin(wifiClient, String("http://") + host + updater + "?" + station_token + SCHOOL_TOKEN);
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

  http.begin(wifiClient, String("http://") + host + updater + "?" + station_token + SCHOOL_TOKEN + "&trigger=1");
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

void blinkLedDeviceStandby() {
  if (millis() - millisDeviceReady >= 6150) {
    millisDeviceReady = millis();
    // digitalWrite(LedBuiltIn, LOW);
    digitalWrite(LedDeviceReady, LOW);
    // digitalWrite(LedStatusPin, LOW);
  } else if (millis() - millisDeviceReady >= 6050) {
    // digitalWrite(LedBuiltIn, HIGH);
    digitalWrite(LedDeviceReady, HIGH);
    // digitalWrite(LedStatusPin, HIGH);
  } else if (millis() - millisDeviceReady >= 5950) {
    // digitalWrite(LedBuiltIn, LOW);
    digitalWrite(LedDeviceReady, LOW);
    // digitalWrite(LedStatusPin, LOW);
  } else if (millis() - millisDeviceReady >= 5850) {
    // digitalWrite(LedBuiltIn, HIGH);
    digitalWrite(LedDeviceReady, HIGH);
    // digitalWrite(LedStatusPin, HIGH);
  } else if (millis() - millisDeviceReady >= 5750) {
    // digitalWrite(LedBuiltIn, LOW);
    digitalWrite(LedDeviceReady, LOW);
    // digitalWrite(LedStatusPin, LOW);
  } else if (millis() - millisDeviceReady >= 5650) {
    // digitalWrite(LedBuiltIn, HIGH);
    digitalWrite(LedDeviceReady, HIGH);
    // digitalWrite(LedStatusPin, HIGH);
  } else if (millis() - millisDeviceReady >= 5550) {
    // digitalWrite(LedBuiltIn, LOW);
    digitalWrite(LedDeviceReady, LOW);
    // digitalWrite(LedStatusPin, LOW);
  } else if (millis() - millisDeviceReady >= 5450) {
    // digitalWrite(LedBuiltIn, HIGH);
    digitalWrite(LedDeviceReady, HIGH);
    // digitalWrite(LedStatusPin, HIGH);
  }


  else if (millis() - millisDeviceReady >= 4300) {
    // digitalWrite(LedBuiltIn, LOW);
    digitalWrite(LedDeviceReady, LOW);
    // digitalWrite(LedStatusPin, LOW);
  } else if (millis() - millisDeviceReady >= 4200) {
    // digitalWrite(LedBuiltIn, HIGH);
    digitalWrite(LedDeviceReady, HIGH);
    // digitalWrite(LedStatusPin, HIGH);
  } else if (millis() - millisDeviceReady >= 4100) {
    // digitalWrite(LedBuiltIn, LOW);
    digitalWrite(LedDeviceReady, LOW);
    // digitalWrite(LedStatusPin, LOW);
  } else if (millis() - millisDeviceReady >= 4000) {
    // digitalWrite(LedBuiltIn, HIGH);
    digitalWrite(LedDeviceReady, HIGH);
    // digitalWrite(LedStatusPin, HIGH);
  }


  else if (millis() - millisDeviceReady >= 3250) {
    // digitalWrite(LedBuiltIn, LOW);
    digitalWrite(LedDeviceReady, LOW);
    // digitalWrite(LedStatusPin, LOW);
  } else if (millis() - millisDeviceReady >= 3150) {
    // digitalWrite(LedBuiltIn, HIGH);
    digitalWrite(LedDeviceReady, HIGH);
    // digitalWrite(LedStatusPin, HIGH);
  } else if (millis() - millisDeviceReady >= 3050) {
    // digitalWrite(LedBuiltIn, LOW);
    digitalWrite(LedDeviceReady, LOW);
    // digitalWrite(LedStatusPin, LOW);
  } else if (millis() - millisDeviceReady >= 2950) {
    // digitalWrite(LedBuiltIn, HIGH);
    digitalWrite(LedDeviceReady, HIGH);
    // digitalWrite(LedStatusPin, HIGH);
  } else if (millis() - millisDeviceReady >= 2850) {
    // digitalWrite(LedBuiltIn, LOW);
    digitalWrite(LedDeviceReady, LOW);
    // digitalWrite(LedStatusPin, LOW);
  } else if (millis() - millisDeviceReady >= 2750) {
    // digitalWrite(LedBuiltIn, HIGH);
    digitalWrite(LedDeviceReady, HIGH);
    // digitalWrite(LedStatusPin, HIGH);
  }


  else if (millis() - millisDeviceReady >= 1550) {
    // digitalWrite(LedBuiltIn, LOW);
    digitalWrite(LedDeviceReady, LOW);
    // digitalWrite(LedStatusPin, LOW);
  } else if (millis() - millisDeviceReady >= 1450) {
    // digitalWrite(LedBuiltIn, HIGH);
    digitalWrite(LedDeviceReady, HIGH);
    // digitalWrite(LedStatusPin, HIGH);
  } else if (millis() - millisDeviceReady >= 1350) {
    // digitalWrite(LedBuiltIn, LOW);
    digitalWrite(LedDeviceReady, LOW);
    // digitalWrite(LedStatusPin, LOW);
  } else if (millis() - millisDeviceReady >= 1250) {
    // digitalWrite(LedBuiltIn, HIGH);
    digitalWrite(LedDeviceReady, HIGH);
    // digitalWrite(LedStatusPin, HIGH);
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