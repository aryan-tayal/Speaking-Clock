// Configuration
#include "DHT.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "RTClib.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"


#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
RTC_DS1307 rtc;


#define SCREEN_WIDTH 128  // OLED display in pixels
#define SCREEN_HEIGHT 64
#define OLED_MOSI   23
#define OLED_CLK    18
#define OLED_DC     16
#define OLED_CS     5
#define OLED_RESET  17

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

static const uint8_t PIN_MP3_TX = 26; 
static const uint8_t PIN_MP3_RX = 27;  
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);
DFRobotDFPlayerMini player;

// Pin Diagram
const int speakBtn = 1;
const int modeBtn = 2;
const int incBtn = 3;
const int decBtn = 4;
const int setBtn = 5;

// Initial variables

int alarmHrs = 10;
int alarmMins = 20;
int currentTime[3];
bool isAlarmMode = false;
char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

void setup() {
  Serial.begin(921600);
  softwareSerial.begin(921600);
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
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.display();
  delay(2000);
  display.clearDisplay();
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  if (player.begin(softwareSerial)) {
    Serial.println("OK");
    player.volume(20);  // max 30
  } else {
    Serial.println("Connecting to DFPlayer Mini failed!");
  }
  if (!rtc.isrunning()) {
    Serial.println("RTC is NOT running, setting time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}

void loop() {
  String time = formatTime();
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
    speak();
    delay(200);
  }

  // Display or set alarm
  if (isAlarmMode) {
    displayAlarm();
    if (digitalRead(setBtn)) {
      setAlarm();
    }
  } else {
    useDisplay(time);
  }
}
void getTime() {
  DateTime now = rtc.now();
  currentTime[0] = now.hour();
  currentTime[1]=  now.minute();
  currentTime[2]= now.second();
  
}
String formatTime() {
  String hourStr = (currentTime[0]< 10 ? "0" : "") + String(currentTime[0], DEC);
  String minuteStr = (currentTime[1] < 10 ? "0" : "") + String(currentTime[1], DEC);
  String secondStr = (currentTime[2] < 10 ? "0" : "") + String(currentTime[2], DEC);
  String formattedTime = hourStr + ":" + minuteStr + ":" + secondStr;
  Serial.println(formattedTime);
  return formattedTime;
}

void useDisplay(String time) {
  int temp = readTemp();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.write("Time : ");
  display.write(time.c_str());
  display.setCursor(0,1);
  display.write(temp);
}

void speak() {
  player.playFolder(1, currentTime[0]);
  player.playFolder(2, currentTime[1]);
}

void displayAlarm() {
  String alarmTime = String(alarmHrs) + ":" + String(alarmMins);
  useDisplay(alarmTime);
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
    return 200;
  }
  return t;
}
