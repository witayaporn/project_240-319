#include <Keypad_I2C.h>
#include <Keypad.h>
#include <Wire.h>

#define I2CADDR 0x21  // กำหนด Address ของ I2C
                      // วิธีการหา Address ของ I2C สามารถดูได้จากลิงค์ข้างล่าง 
                      // https://www.myarduino.net/article/98


const byte ROWS = 4;  // กำหนดจำนวนของ Rows
const byte COLS = 4;  // กำหนดจำนวนของ Columns

// กำหนด Key ที่ใช้งาน (4x4)
char keys[COLS][ROWS] = {
  {'D','C','B','A'},
  {'#','9','6','3'},
  {'0','8','5','2'},
  {'*','7','4','1'}
};

// กำหนด Pin ที่ใช้งาน (4x4)
byte rowPins[ROWS] = {0, 1, 2, 3}; // เชื่อมต่อกับ Pin แถวของปุ่มกด
byte colPins[COLS] = {4, 5, 6, 7}; // เชื่อมต่อกับ Pin คอลัมน์ของปุ่มกด

// makeKeymap(keys) : กำหนด Keymap
// rowPins : กำหนด Pin แถวของปุ่มกด
// colPins : กำหนด Pin คอลัมน์ของปุ่มกด
// ROWS : กำหนดจำนวนของ Rows
// COLS : กำหนดจำนวนของ Columns
// I2CADDR : กำหนด Address ขอ i2C
// PCF8574 : กำหนดเบอร์ IC
Keypad_I2C keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS, I2CADDR, PCF8574 );

void setup(){
  Wire.begin();  // เรียกการเชื่อมต่อ Wire
  keypad.begin( makeKeymap(keys) );  // เรียกกาเชื่อมต่อ
  Serial.begin(9600);


}
void loop(){
 
  char key = keypad.getKey();  // สร้างตัวแปรชื่อ key ชนิด char เพื่อเก็บตัวอักขระที่กด
 
  if (key){  // ถ้าหากตัวแปร key มีอักขระ
    Serial.println(key);
  }
}
