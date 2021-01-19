// Load Wi-Fi library
#include <WiFi.h>

#include <Wire.h>


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
#define move_server 0xB0

unsigned long delayTime;




// Replace with your network credentials
const char* ssid = "Linksys15538";
const char* password = "u75c00bxle";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";


bool forwardState = false;
bool backwardState = false;
bool leftState = false;
bool rightState = false;
bool getSensState = false;


char dhtdata[10];
char temperature[6];
char humidity[6];



// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  // Start the I2C Bus
  Wire.begin(); 
  Serial.begin(115200);
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
webpage();

}

void webpage(){
    WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
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
            


            if (header.indexOf("GET /forward/true") >= 0) {
              Serial.println("Move forwards");
              forwardState = true;  
              backwardState = false;   
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
//            if (forwardState == false && backwardState == false && leftState == false && rightState == false){
//                Wire.beginTransmission(uno); // transmit to device #4
//                Wire.write(byte(motor_stop));              // sends one byte  
//                Wire.endTransmission();    // stop transmitting
//             }


           if (header.indexOf("GET /getSens/true") >= 0) {
              Serial.println("Get sens");
              getSensState = true;   
            } else if (header.indexOf("GET /getSens/false") >= 0) {
              Serial.println("stop get sens");
              getSensState = false;      
            } 


            if(getSensState == true){
                Wire.beginTransmission(uno); // transmit to device 
                Wire.write(byte(get_sensData));              // sends one byte  
                Wire.endTransmission();    // stop transmitting    
                Wire.requestFrom(uno, 10);    // request 10 bytes from slave device
  
            //get all data from slave
            for(int i=0; i <10; i++){
                dhtdata[i] = Wire.read();
              }
                  Serial.println(dhtdata);
              //split data up into temperature and humidity
               for(int i =0; i<5; i++) { 
                  temperature[i] = dhtdata[i]; // receive a byte as character    
                        
                }
               for(int i =0; i<5; i++) { 
                  humidity[i] = dhtdata[i+5]; // receive a byte as character      
                    
                }
                Serial.println(temperature); 
               Serial.println(humidity); 
               
                getSensState = false;                  
              }

            
            // Display the HTML
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");

            client.println("<style>");
            
            client.println(".button2 {background-color: #4CAF50; color: white; padding: 16px 40px;}");
            client.println(".button {background-color: #555555; color: white; padding: 16px 40px;}");
              
             client.println("</style>");
                         

            
            client.println("</head>");

            // Web Page Heading
            client.println("<body><h1 style='text-align:center;'>Soft564 Robot Control</h1>");


            client.println("<p style='text-align:center;'>");
            if(!forwardState){
                client.println("<a href=\"/forward/true\"><button class=\"button\">&#x2191</button></a>");
            }else{
              client.println("<a href=\"/forward/false\"><button class=\"button2\">&#x2191</button></a>");
            }
            client.println("</p>");
            
            client.println("<p style='text-align:center;'>");
            
            if(!leftState){
                client.println("<a href=\"/left/true\"><button class=\"button\">&#x2190</button></a>");
            }else{
              client.println("<a href=\"/left/false\"><button class=\"button2\">&#x2190</button></a>");
            }  
             if(!backwardState){
                client.println("<a href=\"/backward/true\"><button class=\"button\">&#x2193</button></a>");
            }else{
              client.println("<a href=\"/backward/false\"><button class=\"button2\">&#x2193</button></a>");
            }
            if(!rightState){
                client.println("<a href=\"/right/true\"><button class=\"button\">&#x2192</button></a>");
            }else{
              client.println("<a href=\"/right/false\"><button class=\"button2\">&#x2192</button></a>");
            }
          
           client.println("</p>");

           
           client.println("<p style='text-align:center;'>");
             if(!getSensState){
                client.println("<a href=\"/getSens/true\"><button class=\"button\">Get Sensor Data</button></a>");
                client.println("</p>");
                client.println("<pstyle='text-align:center;'> Temperature: ");
                client.println(temperature);
                client.println(" Humidity: ");
                client.println(humidity);
            }else{
              client.println("<a href=\"/getSens/false\"><button class=\"button2\">Get Sensor Data</button></a>");
              client.println("</p>");
            }

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
