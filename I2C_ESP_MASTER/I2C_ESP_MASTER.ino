#include <Wire.h>

//12c pins
#define I2C_SDA 21
#define I2C_SCL 22
#define arduinoUno 5
//create i2c bus
TwoWire I2C = TwoWire(0);


unsigned long delayTime;

void setup() {
  Serial.begin(115200);
  //start i2c bus
  I2C.begin(I2C_SDA, I2C_SCL, 100000);

  bool status;

  // default settings
  status = I2C.begin(arduinoUno);  
  if (!status) {
    Serial.println("Could not find Arduino Uno, check wiring!");
    while (1);
  }

  Serial.println("-- Default Test --");
  delayTime = 1000;

  Serial.println();
}

void loop() { 
  
  delay(delayTime);
}
