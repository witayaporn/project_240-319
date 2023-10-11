#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "SparkFun_VL53L1X.h"
#include <HX711_ADC.h>
#include <math.h>
#include <avr/io.h>
#include <Keypad_I2C.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

#define buzzer 3
#define lcdAddress 0x27      // กำหนดขาต่อจอ LCD
#define DS1307_ADDRESS 0x68  // I2C address for DS1307 RTC
#define AVR
#define I2CADDR 0x21

const byte rxPin = 8;
const byte txPin = 9;

// Set up a new SoftwareSerial object
SoftwareSerial ser (rxPin, txPin);

const byte ROWS = 4;  // กำหนดจำนวนของ Rows
const byte COLS = 4;  // กำหนดจำนวนของ Columns

// กำหนด Key ที่ใช้งาน (4x4)
char keys[COLS][ROWS] = {
  { 'D', 'C', 'B', 'A' },
  { '#', '9', '6', '3' },
  { '0', '8', '5', '2' },
  { '*', '7', '4', '1' }
};

// กำหนด Pin ที่ใช้งาน (4x4)
byte rowPins[ROWS] = { 0, 1, 2, 3 };  // เชื่อมต่อกับ Pin แถวของปุ่มกด
byte colPins[COLS] = { 4, 5, 6, 7 };  // เชื่อมต่อกับ Pin คอลัมน์ของปุ่มกด

// makeKeymap(keys) : กำหนด Keymap
// rowPins : กำหนด Pin แถวของปุ่มกด
// colPins : กำหนด Pin คอลัมน์ของปุ่มกด
// ROWS : กำหนดจำนวนของ Rows
// COLS : กำหนดจำนวนของ Columns
// I2CADDR : กำหนด Address ขอ i2C
// PCF8574 : กำหนดเบอร์ IC
Keypad_I2C keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR, PCF8574);


// กำหนดตัวแปรสำหรับเก็บค่าระยะทาง
int distance;
int sensorHeight = 202;

float weight;

unsigned long last_time = 0;
unsigned long period = 1000;

unsigned long last_time_2 = 0;

unsigned long previousMillis = 0;
unsigned long interval = 250;

//pins:
const int HX711_dout = 4;  //mcu > HX711 dout pin
const int HX711_sck = 5;   //mcu > HX711 sck pin

byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
char date[11];  // Array to store date in the format "YYYY-MM-DD"
char time[9];   // Array to store time in the format "HH:MM:SS"

LiquidCrystal_I2C lcd(lcdAddress, 20, 4);
SFEVL53L1X distanceSensor;

//HX711 constructor:
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
unsigned long t = 0;

// ฟังก์ชันสำหรับเริ่มต้นใช้งานเซ็นเซอร์ ultrasonic
void setup() {
  Wire.begin();
  Serial.begin(115200);
  keypad.begin(makeKeymap(keys));

  // เริ่มต้นใช้งานจอ LCD
  lcd.begin();
  lcd.backlight();
  lcd.clear();

  pinMode(buzzer, OUTPUT);
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  ser.begin(115200);

  distanceSensor.begin();

  LoadCell.begin();
  float calibrationValue;
  calibrationValue = 227.2;

  unsigned long stabilizingtime = 2000;
  boolean _tare = true;
  LoadCell.start(stabilizingtime, _tare);
  LoadCell.setCalFactor(calibrationValue);
}


// ฟังก์ชันสำหรับวัดระยะทาง
int measureDistance() {
  distanceSensor.startRanging();  //Write configuration bytes to initiate measurement
  while (!distanceSensor.checkForDataReady()) {
    delay(1);
  }
  distance = distanceSensor.getDistance();  //Get the result of the measurement from the sensor
  distanceSensor.clearInterrupt();
  distanceSensor.stopRanging();

  int distance_cm = distance * 0.1;
  int f_distance = sensorHeight - distance_cm;

  // คืนค่าระยะทาง
  return f_distance;
}

float measureWeight() {
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0;  //increase value to slow down serial print activity
  float i;

  // check for new data/start next conversion:
  //if (LoadCell.update()) newDataReady = true;
  LoadCell.update();

  // get smoothed value from the dataset:
  //if (newDataReady) {
  //if (millis() > t + serialPrintInterval) {
  i = LoadCell.getData();
  //Serial.print("Load_cell output val: ");
  //Serial.println(i);
  //newDataReady = 0;
  //t = millis();
  //}
  //}

  // receive command from serial terminal, send 't' to initiate tare operation:
  /*if (Serial.available() > 0) {
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }*/

  // check if last tare operation is complete:
  //if (LoadCell.getTareStatus() == true) {
  //Serial.println("Tare complete");
  //}
  //Serial.println(i);
  return ceilf(i * 10) / 1000;
}

// ฟังก์ชันสำหรับแสดงผลบนจอ LCD
void displayDistance(int distance, float weight) {
  // ไปที่ตำแหน่ง (0, 0) เพื่อเริ่มเขียนข้อความ
  char buff[15];
  char chrweight[10];
  dtostrf(weight, 5, 1, chrweight);

  lcd.setCursor(0, 0);
  lcd.print("Date: ");
  lcd.print(date);

  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  lcd.print(time);

  lcd.setCursor(0, 2);
  // เขียนข้อความ "ความสูง: "
  lcd.print("Hight  = ");
  sprintf(buff, "%3d cm", distance);
  lcd.setCursor(11, 2);  // เขียนค่าระยะทาง
  lcd.print(buff);
  // เขียนหน่วยเป็นเซนติเมตร
  //lcd.print(" cm");

  lcd.setCursor(0, 3);
  lcd.print("Weight = ");
  sprintf(buff, "%s Kg", chrweight);
  lcd.setCursor(9, 3);
  //Serial.println(buff);
  //lcd.print(buff);
  lcd.print(buff);
  //lcd.print(" Kg");
}

void displayDistance_NA() {
  // ไปที่ตำแหน่ง (0, 0) เพื่อเริ่มเขียนข้อความ
  lcd.setCursor(0, 0);
  lcd.print("Date: ");
  lcd.print(date);

  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  lcd.print(time);

  lcd.setCursor(0, 2);
  lcd.print("Hight  = ");
  lcd.print("N/A");
  lcd.print(" N/A");

  lcd.setCursor(0, 3);
  lcd.print("Weight = ");
  lcd.print("N/A");
  lcd.print("N/A");
}

void displayConfirm(int distance, float weight, char sid, int nums) {
  char buff[15];
  char chrweight[10];
  dtostrf(weight, 5, 1, chrweight);
  if (nums == 0) {
    lcd.clear();
    // ไปที่ตำแหน่ง (0, 0) เพื่อเริ่มเขียนข้อความ
    lcd.setCursor(0, 0);
    lcd.print("Hight  = ");
    sprintf(buff, "%3d cm", distance);
    lcd.setCursor(11, 0);  // เขียนค่าระยะทาง
    lcd.print(buff);

    lcd.setCursor(0, 1);
    lcd.print("Weight = ");
    sprintf(buff, "%s Kg", chrweight);
    lcd.setCursor(9, 1);
    //Serial.println(buff);
    //lcd.print(buff);
    lcd.print(buff);

    lcd.setCursor(0, 2);
    lcd.print("Press Enter ID");

    lcd.setCursor(0, 3);
    lcd.print("==>> ");
    //lcd.print(sid);
  } else {
    lcd.print(sid);
  }
}

void displayAck(int distance, float weight, char res, int nums) {
  char buff[15];
  char chrweight[10];
  dtostrf(weight, 5, 1, chrweight);
  if (nums == 0) {
    lcd.clear();
    // ไปที่ตำแหน่ง (0, 0) เพื่อเริ่มเขียนข้อความ
    lcd.setCursor(0, 0);
    lcd.print("Height  = ");
    sprintf(buff, "%3d cm", distance);
    lcd.setCursor(11, 0);  // เขียนค่าระยะทาง
    lcd.print(buff);

    lcd.setCursor(0, 1);
    lcd.print("Weight= ");
    sprintf(buff, "%s Kg", chrweight);
    lcd.setCursor(9, 1);
    //Serial.println(buff);
    //lcd.print(buff);
    lcd.print(buff);

    lcd.setCursor(0, 2);
    lcd.print("Save Information ?");

    lcd.setCursor(0, 3);
    lcd.print("==>> ");
    //lcd.print(sid);
  } else {
    lcd.print(res);
  }
}

void display_sysnotAvailable() {
  int row = 0;
  int totalColumns = 20;
  String message = "System Not Available";
  for (int i = 0; i < totalColumns; i++) {
    message = " " + message;
  }
  message = message + " ";
  for (int position = 0; position < message.length(); position++) {
    lcd.setCursor(0, row);
    lcd.print(message.substring(position, position + totalColumns));
    lcd.setCursor(0, 2);
    lcd.print(message.substring(position, position + totalColumns));
    delay(300);
  }
}

void readDateTime() {
  // Request data from DS1307 RTC
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(0x00);  // Start from register 0 (seconds)
  Wire.endTransmission();

  // Read data from DS1307 RTC
  Wire.requestFrom(DS1307_ADDRESS, 7);

  second = bcdToDec(Wire.read());
  minute = bcdToDec(Wire.read());
  hour = bcdToDec(Wire.read());
  dayOfWeek = bcdToDec(Wire.read());
  dayOfMonth = bcdToDec(Wire.read());
  month = bcdToDec(Wire.read());
  year = bcdToDec(Wire.read());

  // Format date and time strings
  snprintf(date, sizeof(date), "%04d-%02d-%02d", 2000 + year, month, dayOfMonth);
  snprintf(time, sizeof(time), "%02d:%02d:%02d", hour, minute, second);
}

byte bcdToDec(byte value) {
  return ((value / 16) * 10 + (value % 16));
}

int x = 0;
void Buzzer() {
  unsigned long currentMillis = millis();  // อ่านเวลาปัจจุบัน

  // เช็คว่าถึงเวลาที่ควรเปิดหรือปิดบัซเซอร์
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // อัพเดทเวลาที่ก่อนหน้า

    // สลับสถานะบัซเซอร์
    if (digitalRead(buzzer) == LOW) {
      //digitalWrite(buzzer, HIGH); // เปิดบัซเซอร์
      tone(buzzer, 5000, 100);
      x++;
    } else {
      digitalWrite(buzzer, LOW);  // ปิดบัซเซอร์
    }
  }
}

bool scanI2CAddress(byte address) {
  Wire.beginTransmission(address);
  byte error = Wire.endTransmission();

  if (error == 0) {
    //Serial.print("I2C device found at address 0x");
    //if (address < 16)
    //Serial.print("0");
    //Serial.print(address, HEX);
    //Serial.println("  !");
    return true;
  } /*else if (error == 4) {
    Serial.print("Unknown error at address 0x");
    if (address < 16)
      Serial.print("0");
    Serial.println(address, HEX);
  }*/
  return false;
}

bool check_system() {
  bool foundAll = true;

  foundAll &= scanI2CAddress(0x21);
  foundAll &= scanI2CAddress(0x27);
  foundAll &= scanI2CAddress(0x29);
  foundAll &= scanI2CAddress(0x50);
  foundAll &= scanI2CAddress(0x58);
  foundAll &= scanI2CAddress(0x68);

  if (foundAll) {
    //Serial.println("พบทุก I2C devices\n");
    return true;
  } else {
    //Serial.println("ไม่พบทุก I2C devices\n");
    return false;
  }
}

bool input_sid(int avg_distance, float avg_weight) {
  int i = 0;
  int charCount = 0;
  String sid = "";
  while (i < 11) {
    if (i == 0) {
      displayConfirm(avg_distance, avg_weight, ' ', 0);
      i++;
      continue;
    }
    char key = keypad.getKey();
    if (key != NO_KEY) {
      if (key == 'C') {
        displayConfirm(avg_distance, avg_weight, ' ', 0);
        displayConfirm(avg_distance, avg_weight, 'C', 1);
        displayConfirm(avg_distance, avg_weight, 'a', 1);
        displayConfirm(avg_distance, avg_weight, 'n', 1);
        displayConfirm(avg_distance, avg_weight, 'c', 1);
        displayConfirm(avg_distance, avg_weight, 'e', 1);
        displayConfirm(avg_distance, avg_weight, 'l', 1);
        tone(buzzer, 5200, 100);
        delay(700);
        digitalWrite(buzzer, LOW);

        return true;
      }
      Serial.println(key);
      displayConfirm(avg_distance, avg_weight, key, 1);
      tone(buzzer, 4000, 200);
      delay(10);
      digitalWrite(buzzer, LOW);
      sid += key;
      i++;
    }
  }
  charCount = sid.length();
  while (charCount == 10) {
    char key = keypad.getKey();
    if (key == '#') {
      displayConfirm(avg_distance, avg_weight, ' ', 0);
      displayConfirm(avg_distance, avg_weight, 'C', 1);
      displayConfirm(avg_distance, avg_weight, 'o', 1);
      displayConfirm(avg_distance, avg_weight, 'n', 1);
      displayConfirm(avg_distance, avg_weight, 'f', 1);
      displayConfirm(avg_distance, avg_weight, 'i', 1);
      displayConfirm(avg_distance, avg_weight, 'r', 1);
      displayConfirm(avg_distance, avg_weight, 'm', 1);
      ser.println(sid);
      ser.println(avg_weight);
      ser.println(avg_distance);
      tone(buzzer, 4700, 200);
      delay(150);
      digitalWrite(buzzer, LOW);
      delay(50);
      tone(buzzer, 4700, 200);
      delay(150);
      digitalWrite(buzzer, LOW);
      delay(50);
      tone(buzzer, 5200, 100);
      delay(250);
      digitalWrite(buzzer, LOW);
      delay(3000);
      return true;
    }
  }
  return false;
}


void online() {
  readDateTime();  // Call the function to read date and time
  //displayDistance(0, 0);
  unsigned long current_time = millis();
  weight = measureWeight();
  if (current_time - last_time >= period) {
    // วัดระยะทาง
    distance = measureDistance();
    //weight = measureWeight();
    // แสดงผลบนจอ LCD
    if (distance >= 120) {
      //Serial.println("120");
      displayDistance(distance, weight);
      //Serial.println(weight);
    } else {
      //Serial.println("0");
      //displayDistance_NA();
      displayDistance(0, 0);
    }
    //Serial.println(distance);
    //Serial.println(current_time);
    last_time = current_time;

    // Print date and time
    Serial.print("Date: ");
    Serial.println(date);
    Serial.print("Time: ");
    Serial.println(time);
  }


  //Serial.println(distance);
  if (distance > 120) {
    delay(3000);
    int sum_distance = 0;
    int sum_weight = 0;
    for (int i = 0; i < 5; i++) {
      while (i < 5) {
        unsigned long current_time_2 = millis();
        weight = measureWeight();
        if (current_time_2 - last_time_2 >= 1000) {
          distance = measureDistance();
          displayDistance(distance, weight);
          sum_distance = sum_distance + distance;
          //weight = measureWeight();
          sum_weight = sum_weight + weight;
          //Serial.println(i);
          //Serial.println(sum_distance);
          last_time_2 = current_time_2;
          i++;
        }
      }
    }
    //Serial.println(sum_distance / 5);
    //Serial.println(sum_weight / 5);
    //displayDistance(sum_distance / 5, sum_weight / 5);
    //tone(buzzer, 5000, 100);
    while (x <= 2) {
      Buzzer();
      if (x == 2) {
        x = 0;
        int i = 0;
        while (1) {
          if (i == 0) {
            displayAck(sum_distance / 5, sum_weight / 5, ' ', 0);
            i++;
            continue;
          }
          char key = keypad.getKey();
          if (key != NO_KEY) {
            if (key == 'A') {
              displayAck(sum_distance / 5, sum_weight / 5, 'Y', 1);
              displayAck(sum_distance / 5, sum_weight / 5, 'E', 1);
              displayAck(sum_distance / 5, sum_weight / 5, 'S', 1);
              tone(buzzer, 4000, 200);
              delay(10);
              digitalWrite(buzzer, LOW);
              delay(1000);
              while (!input_sid(sum_distance / 5, sum_weight / 5))
                ;
              lcd.clear();
              break;
            } else {
              displayAck(sum_distance / 5, sum_weight / 5, 'N', 1);
              displayAck(sum_distance / 5, sum_weight / 5, 'O', 1);
              tone(buzzer, 4000, 200);
              delay(10);
              digitalWrite(buzzer, LOW);
              delay(1000);
              lcd.clear();
              break;
            }
          }
        }
        break;
      }
    }
  }
}

void offline() {
  display_sysnotAvailable();
}

// ฟังก์ชันหลัก
void loop() {
  //offline();
  //online();
  switch (check_system()) {
    case true: /*Serial.println("online");*/ online(); break;
    case false: /*Serial.println("offline");*/ offline(); break;
  }
}
