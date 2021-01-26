#include <Wire.h>   //used for i2c
#include <Servo.h>  //servo control library 

#include <IRLib.h>  // library for IR Remote
#include <HCSR04.h> //library for ultrasonic
#include "DHT.h"  //library for temp/humidity sensor

#include <L298NX2.h> //motor driver library

#define uno 5 //slave address

#define DHTPIN A2   //pin for DHT sensor
#define DHTTYPE DHT11   //Type of DHT sensor used for library 
#define servoHeader A3   //signal pin of servo
#define trig A0          //pin for triger pin of ultrasonic
#define echo A1           //echo pin of ultrasonic

#define PROTOCOL NEC    //protocol used by IR Remote

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

IRrecv reciever(11);  //create reciever object on pin 11
IRdecode decoder;     //create decoder object

int servoPos = 90;  //defualt servo position
bool servoMoved = false;    //flag
bool dhtRetrieved = false;   //flag
byte command = 0;           //holds i2c command recieved from master

//motor directions
bool motorForward = false;
bool motorBackward = false;
bool motorLeft = false;
bool motorRight = false;

//variables for sensor data to be stored in
char dhtemperature[6];        //holds temperature from sensor
char dhthumidity[6];          //holds humidity value from sensor
char usDistance[6];           //holds distance value from ultrasonic sensor
byte buf[3];                  //temp buffer used when recieving i2c commands

void setup() {
  dht.begin();    //start DHT sensor
  servoBuggy.attach(servoHeader); //attach servo object onto gpio
  getDHT();   //Run sensor once to update current values
  getUS();    //Run ultrasonic once to update current values
  // Start the I2C Bus as Slave on address 9
  Wire.begin(uno);      //connect to i2c bus
  Wire.onReceive(receiveEvent);       // Attach a function to run when data is received on i2c
  Wire.onRequest(requestEvent);       // Attach a function to run when data is requested on i2c
  Serial.begin(115200);       //set baud rate of serial
  //starts IR Remote Interupt
  reciever.No_Output();
  reciever.enableIRIn();
}
void receiveEvent(int bytes) {

  int n = Wire.available();   //if data is available in i2c buffer
  Wire.readBytes(buf, n);     //read bytes from i2c
  command = buf[0];    // read one character from the I2C
  Serial.println(command, HEX);


  //what to do with set commands
  switch (command) {
    case move_servo:
      servoPos = buf[1];    //get servo position from i2c to move to
      servoMoved = false;   //set flag
      break;
    case get_sensData:
      dhtRetrieved = false;  //set flag
      break;
    case motor_stop:         //set motor flags
      motorForward = false;
      motorBackward = false;
      motorLeft = false;
      motorRight = false;
      break;
    case motor_forward:       //set motor flags
      motorBackward = false;
      motorForward = true;
      motorLeft = false;
      motorRight = false;
      break;
    case motor_backward:       //set motor flags
      motorBackward = true;
      motorForward = false;
      motorLeft = false;
      motorRight = false;
      break;
    case motor_left:          //set motor flags
      motorBackward = false;
      motorForward = false;
      motorLeft = true;
      motorRight = false;
      break;
    case motor_right:           //set motor flags
      motorBackward = false;
      motorForward = false;
      motorLeft = false;
      motorRight = true;
      break;
    case motor_forwardStop:    //set motor flags
      motorForward = false;
      break;
    case motor_backwardStop:
      motorBackward = false;
      break;
    case motor_leftStop:       //set motor flags
      motorLeft = false;
      break;
    case motor_rightStop:       //set motor flags
      motorRight = false;
      break;
  }

}

void requestEvent() {
  if (command == get_sensData) {      //Check was previous command sent was if sens data
   // Serial.println("sending dht");
    Wire.write(dhtemperature);      // send data from dht sensor
    Wire.write(dhthumidity);
  } else if (command == move_servo) {   //if previous command was move servo
    Wire.write(usDistance);       //send ultrasonic distance
  }
}



void loop() {

  if (command == get_sensData && dhtRetrieved == false) { // if get data command  and data retieved flag not set
    Serial.println(dhtemperature);
    Serial.println(dhthumidity);
    getDHT();       //get dht data
    dhtRetrieved = true;  // set flag to true
  } else if (command == move_servo && servoMoved == false) {    // if command is move servo and flag not set
    servoBuggy.write(servoPos);     //move servo to position set
    getUS();      //get ultrasonic value
    Serial.println(servoPos);
    servoMoved = true;    //set flag to true
  }

  //code to move motors on set direction
  if (!motorForward && !motorLeft && !motorRight && !motorBackward) {   //stop motors
    motors.stop();
    //Serial.println("stop");
  } else if (motorForward && !motorLeft || motorForward && !motorRight) {  //go forwards
    motors.forward();
    // Serial.println("forwards");
  } else if (motorBackward && !motorLeft || motorBackward && !motorRight) {   //go backwards
    motors.backward();
    // Serial.println("backwards");
  } else if (motorRight && !motorForward || motorRight && !motorBackward) {    //turn right
    motors.forwardA();
    motors.backwardB();
    // Serial.println("right");
  } else if (motorLeft && !motorForward || motorLeft && !motorBackward) {   //turn left
    motors.forwardB();
    motors.backwardA();
    // Serial.println("left");
  }

  if (reciever.GetResults(&decoder)) {
    decoder.decode();
    if (decoder.decode_type == PROTOCOL && decoder.value != 0xFFFFFFFF) {
      Serial.println(decoder.value, HEX);
       switch (decoder.value) {
        case   0xFF38C7: //ok
          motorForward = false;
          motorBackward = false;
          motorLeft = false;
          motorRight = false;
          //Serial.println("stop");
          break;
        case 0xFF18E7: //^
          motorForward = true;
          motorBackward = false;
          motorLeft = false;
          motorRight = false;
         // Serial.println("forwards");
          break;
        case  0xFF10EF:  //>
          motorForward = false;
          motorBackward = false;
          motorLeft = true;
          motorRight = false;
         // Serial.println("left");
          break;
        case  0XFF5AA5:  //<
          motorForward = false;
          motorBackward = false;
          motorLeft = false;
          motorRight = true;
         // Serial.println("right");
          break;
        case  0xFF4AB5:  //V
          motorForward = false;
          motorBackward = true;
          motorLeft = false;
          motorRight = false;
         // Serial.println("backwards");
          break;
      }
    }
      reciever.resume();
    }
}
  
//ultrasonic read
void getUS() {
  float distance = ultraSonic.dist();   //get distance
  if (distance > 99.99) {   //if distance is greater than 100cm
    distance = 99.99;     //set distance to 99.99
  }
  dtostrf(distance, 5, 2, usDistance);    //convert float to char array
  Serial.println(distance);
}

//DHT sensor read
void getDHT() {
  //read humidity
  float h = dht.readHumidity();   //get humidity from sensor
  //read temperature
  float t = dht.readTemperature();//get temperature from sensor

  //store values in char array
  dtostrf(t, 5, 2, dhtemperature);  //converts from float to chars
  dtostrf(h, 5, 2, dhthumidity);

  // Check if any reads failed
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

}
