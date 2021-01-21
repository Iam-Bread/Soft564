#include <Wire.h>   //used for i2c
#include <Servo.h>  //servo control library 

#include <HCSR04.h> //library for ultrasonic
#include "DHT.h"  //library for temp/humidity sensor

#define uno 5

#define DHTPIN A2
#define DHTTYPE DHT11
#define servoHeader A3
#define trig A0
#define echo A1

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
#define move_servo 0xB0

DHT dht(DHTPIN, DHTTYPE); //create dht object
Servo servoBuggy; // create servo object
HCSR04 ultraSonic(trig, echo); //create ultrasonic object

int servoPos = 90;  //defualt servo position
bool servoMoved = false;
byte command = 0;


char dhtemperature[6];
char dhthumidity[6];
char usDistance[6];
byte buf[3];


void setup() {
  dht.begin();
  servoBuggy.attach(servoHeader); //attach servo object onto gpio
  getDHT();
  // Start the I2C Bus as Slave on address 9
  Wire.begin(uno);
  // Attach a function to trigger when something is received.
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.begin(115200);
}
void receiveEvent(int bytes) {

  int n = Wire.available();
  Wire.readBytes(buf, n);
  command = buf[0];    // read one character from the I2C
  Serial.println(command, HEX);

 if (command == move_servo) {
    servoPos = buf[1];
     servoMoved = false;
  }

}

void requestEvent() {
  if (command == get_sensData) {
    Serial.println("sending dht");
    Wire.write(dhtemperature);
    Wire.write(dhthumidity);
  }
  
}

void loop() {
  if (command == get_sensData) {
    getDHT();
    Serial.println(dhtemperature);
    Serial.println(dhthumidity);
  }else if(command == move_servo && servoMoved == false){
    servoBuggy.write(servoPos);
    getUS();
   
    Serial.println(servoPos);
    servoMoved = true;
   }

}

void getUS() {
  float distance = ultraSonic.dist();
  if (distance > 99.99) {
    distance = 99.99;
  }
  dtostrf(distance, 5, 2, usDistance);
   Serial.println(distance);
}

void getDHT() {
  //read humidity
  float h = dht.readHumidity();
  //read temperature
  float t = dht.readTemperature();

  //store values in char array
  dtostrf(t, 5, 2, dhtemperature);
  dtostrf(h, 5, 2, dhthumidity);



  // Check if any reads failed
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

}
