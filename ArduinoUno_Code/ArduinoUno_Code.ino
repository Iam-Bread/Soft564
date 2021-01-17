#include <Wire.h>
#include "DHT.h"

#define uno 5

#define DHTPIN A0
#define DHTTYPE DHT11

#define motor_stop 0xF0
#define motor_forward 0xF2
#define get_sensData 0xA0

DHT dht(DHTPIN, DHTTYPE);

byte command = 0;
void setup() {
   dht.begin(); 

  // Start the I2C Bus as Slave on address 9
  Wire.begin(uno); 
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.begin(115200);   
}
void receiveEvent(int bytes) {

  while(Wire.available()) // loop through all but the last
  {
     command = Wire.read();    // read one character from the I2C
     Serial.println(command,HEX);  
  }

}

void requestEvent() {
  if(command == motor_stop){
    Wire.write("stopp "); // respond with message of 6 bytes
    Serial.println("motor stop");
  }else if(command == motor_forward){
    Wire.write("forwa ");
    Serial.println("motor forward");
  }else if(command == get_sensData){
   // Wire.write("forwa ");
   // Serial.println("motor forward");
    getDHT();
  }
  // as expected by master
}

void loop() {
  //If value received is 0 blink LED for 200 ms
  delay(500);
  //Serial.print(x);  

}

void getDHT(){
  
    //read humidity
  float h = dht.readHumidity();
  //read temperature
  float t = dht.readTemperature();


  // Check if any reads failed
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }


//print temperature and humidity
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));

  }
