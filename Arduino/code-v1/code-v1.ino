#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int pingPin = 13;
int inPin = 12;

LiquidCrystal_I2C lcd(0x27, 20, 4);


void setup() {
Serial.begin(9600);
lcd.begin();
 
// Turn on the blacklight and print a message.
lcd.backlight();
lcd.setCursor(5, 0); // ไปที่ตัวอักษรที่ 0 แถวที่ 1
lcd.print("Welcome To");
 
lcd.setCursor(4, 1); // ไปที่ตัวอักษรที่ 6 แถวที่ 2
lcd.print("COE  Project");

lcd.setCursor(5, 2); // ไปที่ตัวอักษรที่ 0 แถวที่ 1
lcd.print("240 -- 319");

}

void loop()
{
long duration, cm ;
int hcm;
hcm = 211;

pinMode(pingPin, OUTPUT);


digitalWrite(pingPin, LOW);
delayMicroseconds(2);
digitalWrite(pingPin, HIGH);
delayMicroseconds(5);
digitalWrite(pingPin, LOW);
pinMode(inPin, INPUT);
duration = pulseIn(inPin, HIGH);

cm = microsecondsToCentimeters(duration);

cm = hcm-cm;


Serial.print(cm);
Serial.print("cm");
Serial.println();


lcd.setCursor(1, 3);
lcd.print(cm);

delay(100);

}

long microsecondsToCentimeters(long microseconds)
{
// The speed of sound is 340 m/s or 29 microseconds per centimeter.
// The ping travels out and back, so to find the distance of the
// object we take half of the distance travelled.
return microseconds / 29 / 2;
}
