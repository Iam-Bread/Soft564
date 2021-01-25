#include <WiFi.h> //wifi libray for server and connecting to socket
#include "BluetoothSerial.h"  //bluetooth serial library
#include <Wire.h>   //wire library used for i2c
#include <LiquidCrystal_I2C.h>    //lcd library for sending commands over i2c 

//12c pins
#define I2C_SDA 21
#define I2C_SCL 22
#define uno 5

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

unsigned long delayTime;

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  //create lcd object

// Replace with your network credentials
const char* ssid = "Linksys15538";
const char* password = "u75c00bxle";

// Set web server port number to 80
WiFiServer server(80);
BluetoothSerial BlueTooth; //bluetooth object

// Variable to store the HTTP request
String header;


//state flags
bool forwardState = false;
bool backwardState = false;
bool leftState = false;
bool rightState = false;
bool getSensState = false;
bool servoState = false;


String servoPosition = String(90);
int pos1 = 0;
int pos2 = 0;
char ultraSonicDist[6] = "00000";

char dhtdata[10];
char temperature[6];
char humidity[6];

char bluetoothData[20]; //incoming bluetooth data

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds
const long timeoutTime = 10000;

void bluetooth();
void webpage();
void servoMove(int servoPos);
void getDHT_Data();

void setup() {
  // Start the I2C Bus
  Wire.begin();
  Serial.begin(115200);
  lcd.init();// initialize LCD
  lcd.backlight();// turn on LCD backlight
  lcd.setCursor(0, 0);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  lcd.print("Connecting-Wifi");
  uint32_t moment = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - moment < 8000)) { //timeout if it cant coonect to wifi
    delay(500);
    Serial.print(".");
  }
  lcd.clear();
  if (WiFi.status() == WL_CONNECTED) {            //if connected to wifi
    // Print IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
    lcd.print(WiFi.localIP());
  } else {       //if not connected to wifi
    Serial.println("Cant connect to WiFi");
    lcd.print("   Connect to   ");
    lcd.setCursor(0, 1);
    lcd.print("    Bluetooth   ");
    BlueTooth.begin("Soft564 Robot");
    Serial.println("Connect via BlueTooth");
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {  //if connected to wifi
    webpage();
  } else {                              //if not connected to wifi
    bluetooth();        

  }
}

void webpage() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client is connected and hasnt timed out
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();


            //ARROW KEY DIRECTIONS
            if (header.indexOf("GET /forward/true") >= 0) {
              Serial.println("Move forwards");
              forwardState = true;
              backwardState = false;
              leftState = false;
              rightState = false;
              Wire.beginTransmission(uno); // transmit to device #4
              Wire.write(byte(motor_forward));              // sends one byte
              Wire.endTransmission();    // stop transmitting
            } else if (header.indexOf("GET /forward/false") >= 0) {
              Serial.println("Stop moving forwards");
              forwardState = false;
              Wire.beginTransmission(uno); // transmit to device #4
              Wire.write(byte(motor_forwardStop));              // sends one byte
              Wire.endTransmission();    // stop transmitting
            }
            if (header.indexOf("GET /left/true") >= 0) {
              Serial.println("turn left");
              leftState = true;
              rightState = false;
              forwardState = false;
              backwardState = false;
              Wire.beginTransmission(uno); // transmit to device #4
              Wire.write(byte(motor_left));              // sends one byte
              Wire.endTransmission();    // stop transmitting
            } else if (header.indexOf("GET /left/false") >= 0) {
              Serial.println("Stop turning left");
              leftState = false;
              Wire.beginTransmission(uno); // transmit to device #4
              Wire.write(byte(motor_leftStop));              // sends one byte
              Wire.endTransmission();    // stop transmitting
            }
            if (header.indexOf("GET /backward/true") >= 0) {
              Serial.println("Move backwards");
              backwardState = true;
              forwardState = false;
              leftState = false;
              rightState = false;
              Wire.beginTransmission(uno); // transmit to device #4
              Wire.write(byte(motor_backward));              // sends one byte
              Wire.endTransmission();    // stop transmitting
            } else if (header.indexOf("GET /backward/false") >= 0) {
              Serial.println("Stop moving backwards");
              backwardState = false;
              Wire.beginTransmission(uno); // transmit to device #4
              Wire.write(byte(motor_backwardStop));              // sends one byte
              Wire.endTransmission();    // stop transmitting
            }
            if (header.indexOf("GET /right/true") >= 0) {
              Serial.println("turn right");
              rightState = true;
              leftState = false;
              forwardState = false;
              backwardState = false;
              Wire.beginTransmission(uno); // transmit to device #4
              Wire.write(byte(motor_right));              // sends one byte
              Wire.endTransmission();    // stop transmitting
            } else if (header.indexOf("GET /right/false") >= 0) {
              Serial.println("Stop turning right");
              rightState = false;
              Wire.beginTransmission(uno); // transmit to device #4
              Wire.write(byte(motor_rightStop));              // sends one byte
              Wire.endTransmission();    // stop transmitting
            }


            //DHT SENSOR DATA UPDATE STATE IF BUTTON PRESSED
            if (header.indexOf("GET /getSens/true") >= 0) {
              Serial.println("Get sens");
              getSensState = true;
            } else if (header.indexOf("GET /getSens/false") >= 0) {
              Serial.println("stop get sens");
              getSensState = false;
            }

            //SEND SLAVE COMMAND THEN REQUEST DATA BACK
            if (getSensState == true) {
              getDHT_Data();
              getSensState = false;     //Set state back to false so it doesnt run continuosly
            }

            //GET /?value=180& HTTP/1.1
            if (header.indexOf("GET /?value=") >= 0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              servoPosition = header.substring(pos1 + 1, pos2);
              Serial.println(servoPosition);
              int servoPosInt = servoPosition.toInt();  //convert slider position to int
              servoMove(servoPosInt);


            }


            // Display the HTML
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");

            client.println("<style>");

            client.println(".button2 {background-color: #008B8B; color: white; padding: 16px 40px;}");    //button style
            client.println(".button {background-color: #555555; color: white; padding: 16px 40px;}");

            client.println("</style>");

            client.println("</head>");

            // Web Page Heading
            client.println("<body><h1 style='text-align:center;'>Soft564 Robot Control</h1>");

            //ARROW KEY CODE
            client.println("<p style='text-align:center;'>");
            if (!forwardState) {
              client.println("<a href=\"/forward/true\"><button class=\"button\">&#x2191</button></a>");
            } else {
              client.println("<a href=\"/forward/false\"><button class=\"button2\">&#x2191</button></a>");
            }
            client.println("</p>");

            client.println("<p style='text-align:center;'>");

            if (!leftState) {
              client.println("<a href=\"/left/true\"><button class=\"button\">&#x2190</button></a>");
            } else {
              client.println("<a href=\"/left/false\"><button class=\"button2\">&#x2190</button></a>");
            }
            if (!backwardState) {
              client.println("<a href=\"/backward/true\"><button class=\"button\">&#x2193</button></a>");
            } else {
              client.println("<a href=\"/backward/false\"><button class=\"button2\">&#x2193</button></a>");
            }
            if (!rightState) {
              client.println("<a href=\"/right/true\"><button class=\"button\">&#x2192</button></a>");
            } else {
              client.println("<a href=\"/right/false\"><button class=\"button2\">&#x2192</button></a>");
            }

            client.println("</p>");

            //DHT SENSOR CODE

            if (!getSensState) {
              client.println("<p style='text-align:center;'>");
              client.println("<a href=\"/getSens/true\"><button class=\"button2\">Get Sensor Data</button></a>");
              client.println("</p>");
              client.println("<p style='text-align:center;'> Temperature: ");
              client.println(temperature);
              client.println("'C");
              client.println(" Humidity: ");
              client.println(humidity);
              client.println("%");
              client.println("</p>");
            } else {
              client.println("<p style='text-align:center;'>");
              client.println("<a href=\"/getSens/false\"><button class=\"button\">Get Sensor Data</button></a>");
              client.println("</p>");
            }

            //SLIDER CODE
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");   //ajax google api
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto;}");
            client.println(".slider { width: 300px; }</style>");
            client.println("<p>Servo Position: <span id=\"servoPos\"></span></p>");
            client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider\" onchange=\"servo(this.value)\" value=\"" + servoPosition + "\"/>");   //slider input type position set to servo pos

            //displays ultrasonic distance
            client.println("<div id=\"dis\">"); //div gets updated by
            client.println("<p>Distance: ");
            client.print(ultraSonicDist);
            client.print("</p>");
            client.println("</div >");

            client.println("<script>var slider = document.getElementById(\"servoSlider\");");
            client.println("var servoP = document.getElementById(\"servoPos\"); servoP.innerHTML = slider.value;");
            client.println("slider.oninput = function() { slider.value = this.value; servoP.innerHTML = this.value;}");
            client.println("$.ajaxSetup({timeout:1000}); function servo(pos) { ");
            client.println("$.get(\"/?value=\" + pos + \"&\"); $( \"#dis\" ).load(window.location.href + \" #dis\" ); {Connection: close};}</script>");   //gets value of slider and refreshes  distance div to load the

            client.println("</body></html>");


            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
void servoMove(int servoPos) {
  Wire.beginTransmission(uno); // transmit to device #4
  Wire.write(byte(move_servo));              // sends one byte
  Wire.write(servoPos); //send servo position to slave
  Wire.endTransmission();    // stop transmitting
  delay(100);   //gives time to slave to get data ready
  Wire.requestFrom(uno, 6);    // request 10 bytes from slave device

  //get all data from slave
  for (int i = 0; i < 5; i++) {
    ultraSonicDist[i] = Wire.read();
  }
  Serial.println(ultraSonicDist);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dist: ");
  lcd.print(ultraSonicDist);
}
void getDHT_Data() {
  Wire.beginTransmission(uno); // transmit to device
  Wire.write(byte(get_sensData));              // sends one byte
  Wire.endTransmission();    // stop transmitting

  Wire.requestFrom(uno, 10);    // request 10 bytes from slave device

  //get all data from slave
  for (int i = 0; i < 10; i++) {
    dhtdata[i] = Wire.read();
  }
  Serial.println(dhtdata);
  //split data up into temperature and humidity
  for (int i = 0; i < 5; i++) {
    temperature[i] = dhtdata[i]; // receive a byte as character
  }
  for (int i = 0; i < 5; i++) {
    humidity[i] = dhtdata[i + 5]; // receive a byte as character
  }
  Serial.println(temperature);
  Serial.println(humidity);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T: ");
  lcd.print(temperature);
  lcd.setCursor(0, 1);
  lcd.print("H: ");
  lcd.print(humidity);
}

//bluetooth code
void bluetooth() {
  if (BlueTooth.available()) //Check if we receive anything from Bluetooth
  {

    //int n = ESP_BT.available();
    BlueTooth.readBytes(bluetoothData, 20); //Read whats recevived and store in char array
    String temp = bluetoothData;      //convert char array to string
    Serial.print("Received:");
    Serial.println(temp);
    Serial.println(temp.length());
    lcd.clear();      //print to lcd
    lcd.print("Recieved Data:");
    lcd.setCursor(0, 1);
    lcd.print(temp);

    if (temp == "help") {
      BlueTooth.println('\n');
      BlueTooth.println("Commands: ");
      BlueTooth.println(" help ");
      BlueTooth.println(" get dht ");
      BlueTooth.println(" pan servo ");
      BlueTooth.println(" stop ");
      BlueTooth.println(" forwards ");
      BlueTooth.println(" backwards ");
      BlueTooth.println(" left ");
      BlueTooth.println(" right ");
    } else if (temp == "get dht") {   //request dht data
      getDHT_Data();
      BlueTooth.println('\n');
      BlueTooth.println(" Sensor Data: ");
      BlueTooth.println(" Temperature: ");
      BlueTooth.println(temperature);
      BlueTooth.println("Humidity:");
      BlueTooth.println(humidity);
    } else if (temp == "pan servo") {       //move servo and get distance
      BlueTooth.println('\n');
      BlueTooth.println(" Servo Moving");
      for (int i = 0; i < 180; i = i + 5) {
        servoMove(i);   //moves servo and gets distances
        BlueTooth.println(" Distance: ");
        BlueTooth.println(ultraSonicDist);
      }
      } else if (temp == "stop") {                  //stop motors
        Wire.beginTransmission(uno);                // transmit to device #4
        Wire.write(byte(motor_stop));               // sends one byte
        Wire.endTransmission();                     // stop transmitting
        BlueTooth.println(" Stopping ");
      } else if (temp == "forwards") {              //move motors forwards
        Wire.beginTransmission(uno);                // transmit to device #4
        Wire.write(byte(motor_forward));               // sends one byte
        Wire.endTransmission();                     // stop transmitting
        BlueTooth.println(" Going Forwards ");
      } else if (temp == "backwards") {             //move motors backwards
        Wire.beginTransmission(uno);                // transmit to device #4
        Wire.write(byte(motor_backward));               // sends one byte
        Wire.endTransmission();                     // stop transmitting
        BlueTooth.println(" Going Backwards ");
      } else if (temp == "left") {                  //move motors left
        Wire.beginTransmission(uno);                // transmit to device #4
        Wire.write(byte(motor_left));               // sends one byte
        Wire.endTransmission();                     // stop transmitting
        BlueTooth.println(" Turning Left ");
      } else if (temp == "right") {                 //move motors righ
        Wire.beginTransmission(uno);                // transmit to device #4
        Wire.write(byte(motor_right));               // sends one byte
        Wire.endTransmission();                     // stop transmitting
        BlueTooth.println(" Turning Right ");
      } else {
        BlueTooth.println('\n');
        BlueTooth.println("Invalid Command");    //if invalid comman entered
      }
      memset(bluetoothData, 0, sizeof bluetoothData);   // empty data buffer
    }

  }
