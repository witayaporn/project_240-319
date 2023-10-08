#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Ultrasonic.h>

// กำหนดขาต่อเซ็นเซอร์ ultrasonic
#define trigPin A0
#define echoPin A1


#define lcdAddress 0x27 // กำหนดขาต่อจอ LCD
#define DS1307_ADDRESS 0x68 // I2C address for DS1307 RTC

#define INTERVAL_MESSAGE1 1500 //1วินาที
#define INTERVAL_MESSAGE2 1500 //1วินาที
#define INTERVAL_MESSAGE3 1500 //1วินาที
 
unsigned long time_1 = 0;
unsigned long time_2 = 0;
unsigned long time_3 = 0;

// กำหนดตัวแปรสำหรับเก็บค่าระยะทาง
int distance;
int sensorHeight = 214;
int d1 = 1;
int d2 = 2;
int d3 = 3;

unsigned long last_time = 0;
unsigned long period = 1000;

byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
char date[11]; // Array to store date in the format "YYYY-MM-DD"
char time[9];  // Array to store time in the format "HH:MM:SS"

LiquidCrystal_I2C lcd(lcdAddress, 20, 4);

// ฟังก์ชันสำหรับเริ่มต้นใช้งานเซ็นเซอร์ ultrasonic
void setup() {
  Wire.begin();
  Serial.begin(115200);
  // กำหนดขาต่อเซ็นเซอร์ ultrasonic เป็นเอาต์พุต
  pinMode(trigPin, OUTPUT);
  // กำหนดขาต่อเซ็นเซอร์ ultrasonic เป็นอินพุต
  pinMode(echoPin, INPUT);

  // เริ่มต้นใช้งานจอ LCD
  lcd.begin();
  lcd.backlight();
}


// ฟังก์ชันสำหรับวัดระยะทาง
int measureDistance() {
  // ปล่อยสัญญาณ ultrasonic ออกไป
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // รอรับสัญญาณสะท้อนกลับมา
  long duration = pulseIn(echoPin, HIGH);

  // คำนวณระยะทาง
  distance = sensorHeight-(duration * 0.0177);
  //Serial.println(duration * 0.03548 / 2);

  // คืนค่าระยะทาง
  return distance;
}

// ฟังก์ชันสำหรับแสดงผลบนจอ LCD
void displayDistance() {
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

// ฟังก์ชันหลัก
void loop() {
  unsigned long current_time = millis();
  if (current_time - last_time >= period) {
    // วัดระยะทาง
    distance = measureDistance();
    // แสดงผลบนจอ LCD
    if (distance>=0) {
      displayDistance();
    }else {
      displayDistance_NA();
    }
    if(millis() - time_1 > INTERVAL_MESSAGE1){
      time_1 = millis();
      d1 = distance;
    }
    if(millis() - time_2 > INTERVAL_MESSAGE2){
      time_2 = millis();
      d2 = distance;
    }
    if(millis() - time_3 > INTERVAL_MESSAGE3){
      time_3 = millis();
      d3 = distance;
    }

    if(d1 == d2 && d2 == d3){
      Serial.println("=====");
      d1 = 1;
      d2 = 2;
      d3 = 3;

    }
    else if (d1 != d2){
      d1 = 1;
      d2 = 2;
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
}
