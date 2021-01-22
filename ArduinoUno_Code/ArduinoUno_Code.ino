#include <Wire.h>   //used for i2c
#include <Servo.h>  //servo control library 

#include <HCSR04.h> //library for ultrasonic
#include "DHT.h"  //library for temp/humidity sensor

#include <L298NX2.h> //motor driver library

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

// Pins for motor driver
const unsigned int IN1_A = 4;
const unsigned int IN2_A = 5;

const unsigned int IN1_B = 6;
const unsigned int IN2_B = 7;

DHT dht(DHTPIN, DHTTYPE); //create dht object
Servo servoBuggy; // create servo object
HCSR04 ultraSonic(trig, echo); //create ultrasonic object
L298NX2 motors(IN1_A, IN2_A, IN1_B, IN2_B); // create motor driver object

int servoPos = 90;  //defualt servo position
bool servoMoved = false;
bool dhtRetrieved = false;
byte command = 0;

//motor directions
bool motorForward = false;
bool motorBackward = false;
bool motorLeft = false;
bool motorRight = false;

//variables for sensor data to be stored in
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
  Wire.onReceive(receiveEvent);       // Attach a function to run when data is received on i2c
  Wire.onRequest(requestEvent);       // Attach a function to run when data is requested on i2c
  Serial.begin(115200);
}
void receiveEvent(int bytes) {

  int n = Wire.available();
  Wire.readBytes(buf, n);
  command = buf[0];    // read one character from the I2C
  Serial.println(command, HEX);

  switch (command) {
    case move_servo:
      servoPos = buf[1];
      servoMoved = false;
      break;
    case get_sensData:
      dhtRetrieved = false;
      break;
    case motor_stop:
      motorForward = false;
      motorBackward = false;
      motorLeft = false;
      motorRight = false;
      break;
    case motor_forward:
      motorBackward = false;
      motorForward = true;
      motorLeft = false;
      motorRight = false;
      break;
    case motor_backward:
      motorBackward = true;
      motorForward = false;
      motorLeft = false;
      motorRight = false;
      break;
    case motor_left:
      motorBackward = false;
      motorForward = false;
      motorLeft = true;
      motorRight = false;
      break;
    case motor_right:
      motorBackward = false;
      motorForward = false;
      motorLeft = false;
      motorRight = true;
      break;
    case motor_forwardStop:
      motorForward = false;
      break;
    case motor_backwardStop:
      motorBackward = false;
      break;
    case motor_leftStop:
      motorLeft = false;
      break;
    case motor_rightStop:
      motorRight = false;
      break;
  }
}

void requestEvent() {
  if (command == get_sensData) {
    Serial.println("sending dht");
    Wire.write(dhtemperature);      // sernd data from dht sensor
    Wire.write(dhthumidity);
  } else if (command == move_servo) {
    Wire.write(usDistance);       //send ultrasonic distance
  }
}

void loop() {

  if (command == get_sensData && dhtRetrieved == false) { // if get data command
    Serial.println(dhtemperature);
    Serial.println(dhthumidity);
    getDHT();       //get dht data
    dhtRetrieved = true;  // set flag to true
  } else if (command == move_servo && servoMoved == false) {
    servoBuggy.write(servoPos);     //move servo to position set
    getUS();      //get ultrasonic value
    Serial.println(servoPos);
    servoMoved = true;    //set flag to true
  }

  //code to more motors on set direction
  if (!motorForward && !motorLeft && !motorRight && !motorBackward) {
    motors.stop();
    Serial.println("stop");
  } else if (motorForward && !motorLeft || motorForward && !motorRight) {
    motors.forward();
    Serial.println("forwards");
  } else if (motorBackward && !motorLeft || motorBackward && !motorRight) {
    motors.backward();
    Serial.println("backwards");
  } else if (motorRight && !motorForward || motorRight && !motorBackward) {
    motors.forwardA();
    motors.backwardB();
    Serial.println("right");
  } else if (motorLeft && !motorForward || motorLeft && !motorBackward) {
    motors.forwardB();
    motors.backwardA();
    Serial.println("left");
  } 
  
}

void getUS() {
  float distance = ultraSonic.dist();   //get distance
  if (distance > 99.99) {   //if distance is greater than 100cm
    distance = 99.99;     //set distance to 99.99
  }
  dtostrf(distance, 5, 2, usDistance);    //convert float to char array
  Serial.println(distance);
}

void getDHT() {
  //read humidity
  float h = dht.readHumidity();   //get humidity from sensor
  //read temperature
  float t = dht.readTemperature();//get temperature from sensor

  //store values in char array
  dtostrf(t, 5, 2, dhtemperature);
  dtostrf(h, 5, 2, dhthumidity);

  // Check if any reads failed
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

}
