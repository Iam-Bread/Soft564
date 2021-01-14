#include <Wire.h>
//
////12c pins
//#define I2C_SDA 21
//#define I2C_SCL 22
//#define arduinoUno 5 //arduino address
//
//
//
//
//unsigned long delayTime;
//
//void setup() {
//  Serial.begin(115200);
//  //start i2c bus
//  Wire.begin(I2C_SDA, I2C_SCL, 100000);
//
//  bool status;
//
//  // check if arduino is connected
//  status = Wire.begin(arduinoUno);  
//  if (!status) {
//    Serial.println("Could not find Arduino Uno, check wiring!");
//    while (1);
//  }
//
//  Serial.println("-- Default Test --");
//  delayTime = 1000;
//
//  Serial.println();
//}
//
//void loop() { 
//  char c = 53;
//  Wire.beginTransmission(5);
//  Wire.write(c);
//  Wire.endTransmission();
//  Serial.println(c);
//  delay(delayTime);
//}



void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
}

byte x = 0;

void loop()
{
  Wire.beginTransmission(4); // transmit to device #4
  Wire.write("x is ");        // sends five bytes
  Wire.write(x);              // sends one byte  
  Wire.endTransmission();    // stop transmitting

  x++;
  delay(500);
}
