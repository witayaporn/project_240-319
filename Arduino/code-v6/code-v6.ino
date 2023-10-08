#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "SparkFun_VL53L1X.h"
#include <HX711_ADC.h>
#include <math.h>


#define buzzer 3
#define lcdAddress 0x27 // กำหนดขาต่อจอ LCD
#define DS1307_ADDRESS 0x68 // I2C address for DS1307 RTC
#define AVR
#include <EEPROM.h>

// กำหนดตัวแปรสำหรับเก็บค่าระยะทาง
int distance;
int sensorHeight = 202;

float weight;

unsigned long last_time = 0;
unsigned long period = 500;

unsigned long last_time_2 = 0;

unsigned long previousMillis = 0;
unsigned long interval = 250;

//pins:
const int HX711_dout = 4; //mcu > HX711 dout pin
const int HX711_sck = 5; //mcu > HX711 sck pin

byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
char date[11]; // Array to store date in the format "YYYY-MM-DD"
char time[9];  // Array to store time in the format "HH:MM:SS"

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

  // เริ่มต้นใช้งานจอ LCD
  lcd.begin();
  lcd.backlight();
  lcd.clear();

  pinMode(buzzer, OUTPUT);

  LoadCell.begin();
  float calibrationValue;
  calibrationValue = 207.49;

  unsigned long stabilizingtime = 2000;
  boolean _tare = true;
  LoadCell.start(stabilizingtime, _tare);
  LoadCell.setCalFactor(calibrationValue);

}


// ฟังก์ชันสำหรับวัดระยะทาง
int measureDistance() {
  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  while (!distanceSensor.checkForDataReady())
  {
    delay(1);
  }
  distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
  distanceSensor.clearInterrupt();
  distanceSensor.stopRanging();

  int distance_cm = distance * 0.1;
  int f_distance = sensorHeight - distance_cm;

  // คืนค่าระยะทาง
  return f_distance;
}

float measureWeight() {
  static boolean newDataReady = 0;
  const int serialPrintInterval = 0; //increase value to slow down serial print activity
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
  return ceilf(i * 10) / 1000;
}

// ฟังก์ชันสำหรับแสดงผลบนจอ LCD
void displayDistance(int distance, float weight) {
  // ไปที่ตำแหน่ง (0, 0) เพื่อเริ่มเขียนข้อความ
  lcd.setCursor(0, 0);
  lcd.print("Date: ");
  lcd.print(date);

  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  lcd.print(time);

  lcd.setCursor(0, 2);
  // เขียนข้อความ "ความสูง: "
  lcd.print("Hight  = ");
  // เขียนค่าระยะทาง
  lcd.print(distance);
  // เขียนหน่วยเป็นเซนติเมตร
  lcd.print(" cm");

  lcd.setCursor(0, 3);
  lcd.print("Weight = ");
  lcd.print(weight);
  lcd.print(" Kg");
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
  lcd.print("000");
  lcd.print(" cm");

  lcd.setCursor(0, 3);
  lcd.print("Weight = ");
  lcd.print("00.00");
  lcd.print(" Kg");
}

void display_notAvailable() {
  int row = 0;
  int totalColumns = 20;
  String message = "System Not Available";
  //unsigned long currentMillis = millis();
  //unsigned long last_time3;
  //if (currentMillis - last_time3 >= 2) {
  for (int i=0; i < totalColumns; i++) {
    message = " " + message;  
  } 
  message = message + " "; 
  for (int position = 0; position < message.length(); position++) {
      lcd.setCursor(0, row);
      lcd.print(message.substring(position, position + totalColumns));
      //delay(250);
  }
  //last_time3 = currentMillis;
  //}
}

void readDateTime() {
  // Request data from DS1307 RTC
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(0x00); // Start from register 0 (seconds)
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
  unsigned long currentMillis = millis(); // อ่านเวลาปัจจุบัน

  // เช็คว่าถึงเวลาที่ควรเปิดหรือปิดบัซเซอร์
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // อัพเดทเวลาที่ก่อนหน้า

    // สลับสถานะบัซเซอร์
    if (digitalRead(buzzer) == LOW) {
      //digitalWrite(buzzer, HIGH); // เปิดบัซเซอร์
      tone(buzzer, 5000, 100);
      x++;
    } else {
      digitalWrite(buzzer, LOW); // ปิดบัซเซอร์
    }
  }
}

bool check_system() {
  
}

void online() {
  //Serial.println("Start");
  unsigned long current_time = millis();
    weight = measureWeight();
    if (current_time - last_time >= period) {
      // วัดระยะทาง
      distance = measureDistance();
      //weight = measureWeight();
      // แสดงผลบนจอ LCD
      if (distance>=120) {
        //Serial.println("120");
        displayDistance(distance, weight);
        Serial.println(weight);
      }else {
        //Serial.println("0");
        displayDistance_NA();
      }


      
      Serial.println(distance);
      Serial.println(current_time);
      last_time = current_time;

      readDateTime(); // Call the function to read date and time

      // Print date and time
      Serial.print("Date: ");
      Serial.println(date);
      Serial.print("Time: ");
      Serial.println(time);
    }
    
    
    //Serial.println(distance);
    if (distance > 120) {
      int sum_distance = 0;
      int sum_weight = 0;
        for (int i=0;i<3;i++){
          while (i<3) {
            unsigned long current_time_2 = millis();
            weight = measureWeight();
            if (current_time_2 - last_time_2 >= 1000) {
              distance = measureDistance();
              sum_distance = sum_distance + distance;
              //weight = measureWeight();
              sum_weight = sum_weight + weight;
              Serial.println(i);
              Serial.println(sum_distance);
              last_time_2 = current_time_2;
              i++;
              
            }
          }
        }
      Serial.println(sum_distance/3);
      Serial.println(sum_weight/3);
      displayDistance(sum_distance/3, sum_weight/3);
      //tone(buzzer, 5000, 100);
      while (x<=2) {
        Buzzer();
        if (x==2) {
          x = 0;
          break;
        }
      }
    }
}

void offline() {
  display_notAvailable();
}

// ฟังก์ชันหลัก
void loop() {
  /*bool check = check_system();
  if (check) {
    Serial.println("มีอุปกรณ์ 6 ตัวที่เชื่อมต่อผ่าน I2C");
    //online();
  } else {
    Serial.println("มีอุปกรณ์ 5 ตัวที่เชื่อมต่อผ่าน I2C");
    //offline();
  }
  switch (check) {
    case true: online();	break;
    case false: offline();	break;
  }*/
  online();
}
