#include <Wire.h>

#define uno 5

#define motor_stop 0xF0
#define motor_forward 0xF2
#define motor_left 0xF3
#define motor_light 0xF4
#define get_sensData 0xA0
#define move_server 0xB0


void setup() {
  // Start the I2C Bus as Master
  Wire.begin(); 
  Serial.begin(115200);  
}
char inputBuffer[1];
byte number = 0;

void loop() {


  if (Serial.available() > 0) {

    Serial.readBytesUntil('\n', inputBuffer, 1);
    number = atoi(inputBuffer);
    Serial.print("Number to display: ");
    Serial.println(inputBuffer);
  }

  
  Wire.beginTransmission(uno); // transmit to device #4
  Wire.write(byte(motor_stop));              // sends one byte  
  Wire.endTransmission();    // stop transmitting
  
  Wire.requestFrom(uno, 6);    // request 6 bytes from slave device #8
  
  while (Wire.available()) { // slave may send less than requested
    char c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
  }
  delay(500);

//
//   Wire.beginTransmission(uno); // transmit to device #9
//  Wire.write(byte(motor_forward));              // sends x 
//  Wire.endTransmission();    // stop transmitting
//
//  Wire.requestFrom(uno, 6);    // request 6 bytes from slave device #8
//  
//  while (Wire.available()) { // slave may send less than requested
//    char c = Wire.read(); // receive a byte as character
//    Serial.print(c);         // print the character
//  }
//
//  delay(500);
//
//  Wire.beginTransmission(uno); // transmit to device #9
//  Wire.write(byte(get_sensData));              // sends x 
//  Wire.endTransmission();    // stop transmitting

//  Wire.requestFrom(uno, 6);    // request 6 bytes from slave device #8
//  
//  while (Wire.available()) { // slave may send less than requested
//    char c = Wire.read(); // receive a byte as character
//    Serial.print(c);         // print the character
//  }

  delay(500);
}
