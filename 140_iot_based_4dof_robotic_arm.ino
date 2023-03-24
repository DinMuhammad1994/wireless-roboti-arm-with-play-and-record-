

#include <ESP8266WiFi.h>
#include <Servo.h>

Servo myservo1;  // create servo object to control a servo
Servo myservo2;
Servo myservo3;
Servo myservo4;


// twelve servo objects can be created on most boards

// GPIO the servo is attached to
static const int servo1Pin = D3;    //JAW
static const int servo2Pin = D4;    // Wrist
static const int servo3Pin = D5;    // Elbow
static const int servo4Pin = D6;    // Base

// Replace with your network credentials
const char* ssid     = "Projexels";
const char* password = "qwerty123";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Decode HTTP GET value
String valueString1 = String(0);
String valueString2 = String(90);
String valueString3 = String(90);
String valueString4 = String(90);


int pos1 = 0;
int pos2 = 0;


// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);

  myservo1.attach(servo1Pin);  // attaches the servo on the servo1Pin to the servo object
  myservo2.attach(servo2Pin);
  myservo3.attach(servo3Pin);
  myservo4.attach(servo4Pin);
  myservo1.write(100);  // to stop the base at initial position centre
  myservo2.write(90);  // to stop the base at initial position centre
  myservo3.write(90);  // to stop the base at initial position centre
  myservo4.write(90);  // to stop the base at initial position centre

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

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
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

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto;}");
            client.println(".slider { width: 300px; }</style>");
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");

              client.println("<style> .button { border: none; color: white; padding: 16px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; transition-duration: 0.4s; cursor: pointer; }");
             client.println(".button1 { background-color: white; color: black; border: 2px solid #4CAF50; } .button1:hover { background-color: #4CAF50; color: white; } .button2 { background-color: white; color: black; border: 2px solid #008CBA; } .button2:hover { background-color: #008CBA; color: white; } .button3 { background-color: white; color: black; border: 2px solid #FF0000; } .button3:hover { background-color: #FF0000; color: white; } .button4 { background-color: white; color: black; border: 2px solid #991E03; } .button4:hover { background-color: #991E03; color: white; } </style>");

             client.println("</head><body><h1>IOT Based 4 DOF Robotic Arm</h1>");
             
             client.println("</head> <body> <button class=\"button button1\">RECORD</button> <button class=\"button button2\">PLAY</button>");
             

            // Web Page
            
            //sliders
            client.println("<p>Position of GRIPPER: <span id=\"servoPos1\"></span></p>");
            client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider1\" onchange=\"servo1(this.value)\" value=\"" + valueString1 + "\"/>");
            client.println("<p>Position of WRIST LEFT/RIGHT: <span id=\"servoPos2\"></span></p>");
            client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider2\" onchange=\"servo2(this.value)\" value=\"" + valueString2 + "\"/>");
            client.println("<p>Position of ELBOW UP/DOWN: <span id=\"servoPos3\"></span></p>");
            client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider3\" onchange=\"servo3(this.value)\" value=\"" + valueString3 + "\"/>");
            client.println("<p>Position of BASE: <span id=\"servoPos4\"></span></p>");
            client.println("<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"servoSlider4\" onchange=\"servo4(this.value)\" value=\"" + valueString4 + "\"/>");
            client.println("<p>Reset all servos at initial position</p>");
            client.println("<p><a href=\"/Reset\"><button class=\"button button3\">RESET</button></a></p>");
             client.println("<p>Clear all records</p>");
            client.println("<p><a href=\"/clear\"><button class=\"button button4\">CLEAR</button></a></p>");
             

          


            //script
            client.println("<script>");
            client.println("var slider1 = document.getElementById(\"servoSlider1\");");
            client.println("var slider2 = document.getElementById(\"servoSlider2\");");
            client.println("var slider3 = document.getElementById(\"servoSlider3\");");
            client.println("var slider4 = document.getElementById(\"servoSlider4\");");

            client.println("var servoP1 = document.getElementById(\"servoPos1\");");
            client.println("var servoP2 = document.getElementById(\"servoPos2\");");
            client.println("var servoP3 = document.getElementById(\"servoPos3\");");
            client.println("var servoP4 = document.getElementById(\"servoPos4\");");

            client.println("servoP1.innerHTML = slider1.value;");
            client.println("servoP2.innerHTML = slider2.value;");
            client.println("servoP3.innerHTML = slider3.value;");
            client.println("servoP4.innerHTML = slider4.value;");

            client.println("slider1.oninput = function() { slider1.value = this.value; servoP1.innerHTML = this.value; }");
            client.println("slider2.oninput = function() { slider2.value = this.value; servoP2.innerHTML = this.value; }");
            client.println("slider3.oninput = function() { slider3.value = this.value; servoP3.innerHTML = this.value; }");
            client.println("slider4.oninput = function() { slider4.value = this.value; servoP4.innerHTML = this.value; }");

            client.println("$.ajaxSetup({timeout:1000});");
            client.println("function servo1(pos) { $.get(\"/?value1=\" + pos + \"&\");{Connection: close};}");
            client.println("function servo2(pos) { $.get(\"/?value2=\" + pos + \"&\");{Connection: close};}");
            client.println("function servo3(pos) { $.get(\"/?value3=\" + pos + \"&\");{Connection: close};}");
            client.println("function servo4(pos) { $.get(\"/?value4=\" + pos + \"&\");{Connection: close};}");
            client.println("</script>");


            client.println("</body></html>");
            // for servo 1
            //GET /?value=180& HTTP/1.1
            if (header.indexOf("GET /?value1=") >= 0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString1 = header.substring(pos1 + 1, pos2);
              int jaw = valueString1.toInt();
              jaw = map(jaw, 0, 180, 100, 30);
               
              //Rotate the servo
              myservo1.write(jaw);
              Serial.print("Servo 1 Angle");
              Serial.println(valueString1);
            }
            // for servo 2

            //GET /?value=180& HTTP/1.1
            if (header.indexOf("GET /?value2=") >= 0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString2 = header.substring(pos1 + 1, pos2);

               int wrist = valueString2.toInt();
              wrist = map(wrist, 0, 180, 180, 0);
               
              //Rotate the servo
              myservo2.write(wrist);
              Serial.print("Servo 2 Angle");
              Serial.println(valueString2);
            }

            // for servo 3

            //GET /?value=180& HTTP/1.1
            if (header.indexOf("GET /?value3=") >= 0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString3 = header.substring(pos1 + 1, pos2);

              //Rotate the servo
              myservo3.write(valueString3.toInt());
              Serial.print("Servo 3 Angle");
              Serial.println(valueString3);
            }
            // for servo 4

            //GET /?value=180& HTTP/1.1
            if (header.indexOf("GET /?value4=") >= 0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString4 = header.substring(pos1 + 1, pos2);

             int elbow = valueString4.toInt();
              elbow = map(elbow, 0, 160, 180, 0);
               
              //Rotate the servo
              myservo4.write(elbow);
              Serial.print("Servo 4 Angle");
              Serial.println(valueString4);

           
            }

            if (header.indexOf("GET /Reset") >= 0) {
              Serial.println("Reset.....");

              myservo1.write(100);
              delay(200);
              myservo2.write(90);
              delay(200);
              myservo3.write(90);
              delay(200);
              myservo4.write(90);
              delay(200);


              valueString1 = String(0);
              valueString2 = String(90);
              valueString3 = String(90);
              valueString4 = String(90);
            }



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
