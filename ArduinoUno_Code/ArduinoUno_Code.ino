//#include <Wire.h>
//#include "DHT.h"
//
//#define DHTPIN A0
//#define DHTTYPE DHT11
//
//DHT dht(DHTPIN, DHTTYPE);
//
//
//void setup()
//{
//  dht.begin(); // start dht 
//  Wire.begin(5);                // join i2c bus with address
//  Wire.onReceive(receiveEvent); // register event
//  Serial.begin(9600);           // start serial for output
//}
//
//void loop()
//{
//  delay(500);
//}
//
//// executes whenever data is received from the esp32 master
//void receiveEvent(int howMany)
//{
//  while(1 < Wire.available()) // loop through all but the last
//  {
//    char c = Wire.read(); // receive byte as a character
//    Serial.print(c);         // print the character
//  }
//  int x = Wire.read();    // receive byte as an integer
//  Serial.println(x);         // print the integer
//}
//
//void getDHT(){
//  
//    //read humidity
//  float h = dht.readHumidity();
//  //read temperature
//  float t = dht.readTemperature();
//
//
//  // Check if any reads failed
//  if (isnan(h) || isnan(t)) {
//    Serial.println(F("Failed to read from DHT sensor!"));
//    return;
//  }
//
//
////print temperature and humidity
//  Serial.print(F("Humidity: "));
//  Serial.print(h);
//  Serial.print(F("%  Temperature: "));
//  Serial.print(t);
//  Serial.print(F("°C "));
//
//  }


#include <Wire.h>

void setup()
{
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
}

void loop()
{
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  while(1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}
