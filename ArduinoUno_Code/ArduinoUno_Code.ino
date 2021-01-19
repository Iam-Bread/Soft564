#include <Wire.h>
#include "DHT.h"

#define uno 5

#define DHTPIN A0
#define DHTTYPE DHT11

//commands to slave
#define motor_stop 0xF0
#define motor_forward 0xF1
#define motor_backward 0xF2
#define motor_left 0xF3
#define motor_right 0xF4

#define motor_forwardStop 0xF5
#define motor_backwardStop 0xF6
#define motor_leftStop 0xF7
#define motor_rightStop 0xF8

#define get_sensData 0xA0
#define move_server 0xB0

DHT dht(DHTPIN, DHTTYPE);

byte command = 0;
bool sensGot = false;

char dhtemperature[6];
char dhthumidity[6];


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
  if(command == get_sensData){
    sensGot = false;
   // Serial.println("BOOL SET FALSE");




  }

}

void requestEvent() {
//  if(command == motor_stop){
//    Wire.write("stopp "); // respond with message of 6 bytes
//    Serial.println("motor stop");
//  }else if(command == motor_forward){
//    Wire.write("forwa ");
//    Serial.println("motor forward");
//  }else 

if(command == get_sensData){
       Serial.println("sending dht");  

    Wire.write(dhtemperature);
    Wire.write(dhthumidity);
  }
  // as expected by master
}

void loop() {

  delay(500);

   if(command == get_sensData && sensGot == false){  //checks if data has already been collected otherwise would get stuck

    getDHT();
    sensGot = true;
  Serial.println(dhtemperature);
  Serial.println(dhthumidity);
  


  }

}

//void getDHT(char *allSensData){
  //void getDHT(char *tempd, char *humd){
  void getDHT(){
   //read humidity
  float h = dht.readHumidity();
  //read temperature
  float t = dht.readTemperature();
  

//  char sensDataH[5] = "";           //create char arrays to store temp and humidity
//  char sensDataT[5] = "";
//  allSensData[11] = "";
 // Serial.println(t);
    dtostrf(t, 5, 2, dhtemperature);
    dtostrf(h, 5, 2, dhthumidity);
 // Serial.println(dhtemperature);
    
//  dtostrf(t, 5, 2, sensDataT);  //convert float to char array
//  strcpy(allSensData,sensDataT); //copy array into larger array
//  strcat(allSensData," ");      //concatante space
//  dtostrf(h, 5, 2, sensDataH);  //convert float to char array
//  strcat(allSensData,sensDataH);  //concatanate array onto end of bigger array
//  strcat(allSensData,'\0');
//  //allSensData[11] = '\0';         //terminate array  to avoid unwanted characters
//  Serial.println(allSensData);


  // Check if any reads failed
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

//return allSensData;
////print temperature and humidity
//  Serial.print(F("Humidity: "));
//  Serial.print(h);
//  Serial.print(F("%  Temperature: "));
//  Serial.print(t);
//  Serial.println(F("°C "));

  }
