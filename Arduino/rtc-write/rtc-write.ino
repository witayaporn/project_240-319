#include <Wire.h>
#include <RTClib.h>

RTC_DS1307 rtc;

void setup() {
  Wire.begin();
  rtc.begin();
  Serial.begin(115200);

  // Uncomment the line below to set the DS1307 time initially.
   rtc.adjust(DateTime(__DATE__, __TIME__));
}

void loop() {
  DateTime now = rtc.now();

  // Print the current date and time
  Serial.print("Current Date and Time: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  delay(1000); // Update once per second
}
