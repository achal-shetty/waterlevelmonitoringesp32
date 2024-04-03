#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "LAPTOPAC";
const char* password = "X4x15@45";
const char* scriptURL = "https://script.google.com/macros/s/AKfycbyszqewK2-bg-rh5ASfMxBMSaa_IwA5VY0S3lIi-RuoYGUi6wwuxZy0mlQE2I3ms1U/exec";

const int trigPin = 32;
const int echoPin = 33;

#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

const unsigned long interval = 30000; // 30 seconds
const unsigned long resetInterval = 86400000; // 24 hours in milliseconds

unsigned long previousMillis = 0;
unsigned long previousResetMillis = 0;

long duration;
float distanceCm;
float distanceInch;

void setup() {
  Serial.begin(19200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    duration = pulseIn(echoPin, HIGH);
    distanceCm = duration * SOUND_SPEED/2;
    distanceInch = distanceCm * CM_TO_INCH;
    
    Serial.print("Distance (cm): ");
    Serial.println(distanceCm);
    Serial.print("Distance (inch): ");
    Serial.println(distanceInch);
  
    sendToGoogleSheets(distanceCm, distanceInch);
  }

  if (currentMillis - previousResetMillis >= resetInterval) {
    previousResetMillis = currentMillis;
    resetGoogleSheet();
  }
}

void sendToGoogleSheets(float distanceCm, float distanceInch) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(scriptURL) + "?distanceCm=" + String(distanceCm) + "&distanceInch=" + String(distanceInch);
    
    Serial.print("Sending data to Google Sheets: ");
    Serial.println(url);
    
    http.begin(url);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
}

void resetGoogleSheet() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(scriptURL) + "?reset=true";
    
    Serial.print("Resetting Google Sheet: ");
    Serial.println(url);
    
    http.begin(url);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
}
