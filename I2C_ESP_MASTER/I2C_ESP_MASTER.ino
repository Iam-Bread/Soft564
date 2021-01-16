#include <Wire.h>
//
//#define arduinoUno 4
//
//void setup() {
//  Serial.begin(115200);
//  //start i2c bus
//  Wire.begin(); //i2c start with default pins 21 and 22
//  bool status;
//
//  //check if arduino is connected
//  status = Wire.begin(arduinoUno);  
//  if (!status) {
//    Serial.println("Could not find Arduino Uno, check wiring!");
//    while (1);
//  }
//  Serial.println("-- Default Test --");
//}
//
//byte x = 0;
//
//void loop()
//{
//  Wire.beginTransmission(4); // transmit to device #4
//  Wire.write(x);              // sends one byte  
//  Wire.endTransmission();    // stop transmitting
// Serial.println(x);
//  x++;
//  delay(500);
//}
int x = 0;
void setup() {
  // Start the I2C Bus as Master
  Wire.begin(); 
}
void loop() {
  Wire.beginTransmission(9); // transmit to device #9
  Wire.write(x);              // sends x 
  Wire.endTransmission();    // stop transmitting
  x++; // Increment x
  if (x > 5) x = 0; // `reset x once it gets 6
  delay(500);
}
