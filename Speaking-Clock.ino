// Configuration
#include "DHT.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define SCREEN_WIDTH 128  // OLED display in pixels
#define SCREEN_HEIGHT 64

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin Diagram
const int speakBtn = 1;
const int modeBtn = 2;
const int incBtn = 3;
const int decBtn = 4;
const int setBtn = 5;

// Initial variables

int alarmHrs = 10;
int alarmMins = 20;
bool isAlarmMode = false;

void setup() {
  Serial.begin(921600);
  Serial.println("Started");
  // Pin Configuration
  pinMode(speakBtn, INPUT);
  pinMode(modeBtn, INPUT);
  pinMode(incBtn, INPUT);
  pinMode(decBtn, INPUT);
  pinMode(setBtn, INPUT);

  // Sensor Configuartion
  dht.begin();                                       //DHT11
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // OLED
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  String time = String(getHours()) + ":" + String(getMinutes()) + ":" + String(getSeconds());
  int temp = readTemp();
  // Mode toggle
  static bool prevModeState = LOW;
  bool currentModeState = digitalRead(modeBtn);
  if (currentModeState == HIGH && prevModeState == LOW) {
    isAlarmMode = !isAlarmMode;
    delay(200);
  }
  prevModeState = currentModeState;

  // Speak time
  if (digitalRead(speakBtn)) {
    speak(time);
    delay(200);
  }

  // Display or set alarm
  if (isAlarmMode) {
    displayAlarm();
    if (digitalRead(setBtn)) {
      setAlarm();
    }
  } else {
    display(time, temp);
  }
}

int getHours() {
  return 12;
}
int getMinutes() {
  return 32;
}
int getSeconds() {
  return 1;
}

void display(String time, int temp) {
    display.setTextColor(WHITE); 
  display.setTextSize(2);     
  display.setCursor(0, 0);
  display.write("Time : ")
    display.write(time)// Start at top-left corner
  Serial.println("Time : ", time);
  Serial.println("Temperature : ", temp);
}

void speak(String text) {
  Serial.println("speak");
  Serial.println(text);
  Serial.println("speak");
}

void displayAlarm() {
  String alarmTime = String(alarmHrs) + ":" + String(alarmMins);
  display(alarmTime);
}

void setAlarm() {
  static bool isHourChange = true;

  if (digitalRead(setBtn)) {
    isHourChange = !isHourChange;
    delay(200);
  }

  if (digitalRead(incBtn)) {
    if (isHourChange) alarmHrs++;
    else alarmMins++;
    delay(200);
  }

  if (digitalRead(decBtn)) {
    if (isHourChange) alarmHrs--;
    else alarmMins--;
    delay(200);
  }
}
int readTemp() {
  float t = dht.readTemperature();
  if (isnan(t)) {
    Serial.println("Failed reception");
    return;
  }
  return t;
}
