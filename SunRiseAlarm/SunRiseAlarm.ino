/*
 Sjors Platjouw
 Arduino ITTT
 Sunrise Alarm clock with LCD Display and Bluetooth app
 */
#include <Wire.h>               //De library voor I2C
#include <RTClib.h>             //De library voor de klok
#include <LiquidCrystal_I2C.h>  //De library voor het I2C scherm


LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <DS3231.h>  //Include the clock library

// Changable Vars
int fadeTime = 1;  // How long the light will fade to max
int uled = 8;

int firstHalf, secondHalf;
bool isAlarmOn = false;
bool backlightCur = false;
// Set up Vars
RTC_DS1307 RTC;  //Definieer de klok als DS1307

DS3231 rtc(SDA, SCL);
Time t;
void start();
void start() {
  digitalWrite(uled, 1);
}
void setup() {
  Wire.begin();
  pinMode(uled, OUTPUT);  // Set pinmode
  Serial.begin(9600);     // Match to serial monitor
  RTC.begin();
  rtc.begin();

  lcd.begin();
  // uncomment these lines to set time, then reupload again without them.
  //  rtc.setDOW(SATURDAY);     // Set Day-of-Week to SUNDAY
  //  rtc.setTime(16, 30, 50);     // Set the time to 12:00:00 (24hr format)
  //  rtc.setDate(16, 12, 2023);   // Set the date to January 1st, 2014
  lcd.noBacklight();
}

void loop() {

  if (Serial.available()) {
    String inputMode = "";
    while (Serial.available()) {
      inputMode += (char)Serial.read();
      delay(5);
    }
    Serial.println(inputMode);
    switch (inputMode.toInt()) {
      case 10 : setAlarm();
      break;
      case 11 : digitalWrite(uled, !digitalRead(uled));
      break;
      case 12: setTime();
      break;
      case 13 : setDate();
      break;
      case 14 : turnBacklight();
      break;
    }
    

  DateTime now = RTC.now();
  lcd.setCursor(13, 0);  // Set text at 3,1 on display
  char dateBuffer[12];   // Set databuffer
  t = rtc.getTime();     // Make a time class called 't'

  // Send Day-of-Week
  //  Serial.print(rtc.getDOWStr());
  lcd.print(rtc.getDOWStr());

  // Send date
  //  Serial.print(rtc.getDateStr());
  lcd.setCursor(0, 2);
  lcd.print(rtc.getDateStr());

  //sprintf(dateBuffer,"%02u-%02u-%02u ",now.day(),now.month(),now.year());

  // Send time
  //  Serial.println(rtc.getTimeStr());
  lcd.setCursor(0, 0);
  lcd.print(rtc.getTimeStr());

  delay(1000);  //Wacht 1 seconde (1000 milliseconden)

  lcd.setCursor(3, 0);


  if (isAlarmOn) {
    if (t.hour == firstHalf && t.min == secondHalf) {
      start();
    }
  }
}
}

// logic to seperate strings
String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


// func for set up alarm
void setAlarm() {
  String input = "";
  lcd.setCursor(0, 1);
  lcd.blink();
  delay(20);
  while (!Serial.available()) {}
  if (Serial.available()) {
    while (Serial.available()) {
      input += (char)Serial.read();
      delay(5);
    }
    firstHalf = getValue(input, ':', 0).toInt();   // check first input untill ":"
    secondHalf = getValue(input, ':', 1).toInt();  // check second input after ":"
    isAlarmOn = true;
    // check first 2 digits inputs, then check second 2 digits of input
  }
  lcd.noBlink();
  lcd.setCursor(0, 1);
  lcd.print(getValue(input, ':', 0) + ':' + getValue(input, ':', 1));
}

void setTime() {
  String input = "";
  delay(20);
  while (!Serial.available()) {
    delay(5);
  }
  if (Serial.available()) {
    while (Serial.available()) {
      input += (char)Serial.read();
      delay(5);
    }
    int hours = getValue(input, ':', 0).toInt();   // check first input untill ":"
    int minutes = getValue(input, ':', 1).toInt();  // check second input after ":"
    int seconds = getValue(input, ':', 2).toInt();  // check second input after ":"
    rtc.setTime(hours, minutes, seconds);
}
}

void setDate() {
  String input = "";
  delay(20);
  while (!Serial.available()) {
    delay(5);
  }
  if (Serial.available()) {
    while (Serial.available()) {
      input += (char)Serial.read();
      delay(5);
    }
    int day = getValue(input, '.', 0).toInt();   // check first input untill ":"
    int month = getValue(input, '.', 1).toInt();  // check second input after ":"
    int year = getValue(input, '.', 2).toInt();  // check second input after ":"
    rtc.setDate(day, month, year);
}
}

void turnBacklight() {
  if (!backlightCur) {
    lcd.backlight();
    backlightCur = true;
  } else {
    lcd.noBacklight();
    backlightCur = false;
  }
}