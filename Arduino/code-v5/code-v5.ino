#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "SparkFun_VL53L1X.h"


#define buzzer 3
#define lcdAddress 0x27 // กำหนดขาต่อจอ LCD
#define DS1307_ADDRESS 0x68 // I2C address for DS1307 RTC

// กำหนดตัวแปรสำหรับเก็บค่าระยะทาง
int distance;
int sensorHeight = 209;

unsigned long last_time = 0;
unsigned long period = 1000;

unsigned long last_time_2 = 0;

unsigned long previousMillis = 0;
unsigned long interval = 250;

byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
char date[11]; // Array to store date in the format "YYYY-MM-DD"
char time[9];  // Array to store time in the format "HH:MM:SS"

LiquidCrystal_I2C lcd(lcdAddress, 20, 4);
SFEVL53L1X distanceSensor;

// ฟังก์ชันสำหรับเริ่มต้นใช้งานเซ็นเซอร์ ultrasonic
void setup() {
  Wire.begin();
  Serial.begin(115200);

  // เริ่มต้นใช้งานจอ LCD
  lcd.begin();
  lcd.backlight();
  lcd.clear();

  pinMode(buzzer, OUTPUT);

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

// ฟังก์ชันสำหรับแสดงผลบนจอ LCD
void displayDistance(int distance) {
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
  lcd.print("N/A");
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

  lcd.setCursor(0, 3);
  lcd.print("Weight = ");
  lcd.print("N/A");
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
  unsigned long current_time = millis();
    if (current_time - last_time >= period) {
      // วัดระยะทาง
      distance = measureDistance();
      // แสดงผลบนจอ LCD
      if (distance>=120) {
        displayDistance(distance);
      }else {
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
        for (int i=0;i<3;i++){
          while (i<3) {
            unsigned long current_time_2 = millis();
            if (current_time_2 - last_time_2 >= 1000) {
              distance = measureDistance();
              sum_distance = sum_distance + distance;
              Serial.println(i);
              Serial.println(sum_distance);
              last_time_2 = current_time_2;
              i++;
              
            }
          }
        }
      Serial.println(sum_distance/3);
      displayDistance(sum_distance/3);
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
  ///display_notAvailable();
}

// ฟังก์ชันหลัก
void loop() {
  bool check = check_system();
  if (check) {
    Serial.println("มีอุปกรณ์ 6 ตัวที่เชื่อมต่อผ่าน I2C");
    //online();
  } else {
    Serial.println("มีอุปกรณ์ 5 ตัวที่เชื่อมต่อผ่าน I2C");
    //offline();
  }
  /*switch (check) {
    case true: online();	break;
    case false: offline();	break;
  }*/
}
