#include <ESP8266WiFi.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
Servo myservo1;  // create servo object to control a servo
Servo myservo2;
Servo myservo3;
Servo myservo4;
const int del = 20; // delay during play
//define the buttons
const int button1 = 13;  //Record-Active High
const int button2 = 12; //Play-Active High
int adj;
//define variable for values of the button
int button1Pressed = 0;
boolean button2Pressed = false;

//define variable for angles of the potentiometer
int pot1Angle = 90;
int pot2Angle = 90;
int pot3Angle = 90;
int pot4Angle = 90;

//define variable for saved position of the servos
int servo1PosSave[5]; //It is an Array containing five desired positions
int servo2PosSave[5]; //It is an Array containing five desired positions
int servo3PosSave[5]; //It is an Array containing five desired positions
int servo4PosSave[5]; //It is an Array containing five desired positions
//define variable for current position of the servos
int servo1PosCurrent[6]; //It is an Array containing five Current positions
int servo2PosCurrent[6]; //It is an Array containing five Current positions
int servo3PosCurrent[6]; //It is an Array containing five Current positions
int servo4PosCurrent[6]; //It is an Array containing five Current positions

// twelve servo objects can be created on most boards

// GPIO the servo is attached to
static const int servo1Pin = D4;   //JAW
static const int servo2Pin = D3;    // Wrist
static const int servo3Pin = D5;     // Elbow
static const int servo4Pin = D6;     // Base

// Replace with your network credentials
const char* ssid     = "Robotic Arm";
const char* password = "12345678";



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
  Wire.begin(D2, D1);   //Use predefined PINS consts
  lcd.begin();      // The begin call takes the width and height. This
  lcd.backlight();      // Turn on the backlight.
  lcd.clear();
  lcd.setCursor(0, 0);

  lcd.print(" HAMDARD UNIVERSITY ");
  lcd.setCursor(0, 2);
  lcd.print("   BE ELECTRONICS   ");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);  // Move the cursor at origin
  lcd.print("IOT BASED 4DOF");
  lcd.setCursor(0, 1);
  lcd.print("     ROBOTIC ARM");
  lcd.setCursor(0, 2);
  lcd.print("WITH RECORD & PLAY");
  lcd.setCursor(0, 3);
  lcd.print("     FUNTIONALITY  ");

  delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("AGHA M.DILAWAR");
  lcd.setCursor(0, 1);
  lcd.print("       BEES/S15/0209");
  lcd.setCursor(0, 2);
  lcd.print("RIZWAN ALI");
  lcd.setCursor(0, 3);
  lcd.print("       BEES/S15/0193");
  delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);  // Move the cursor at origin
  lcd.print("Servo at initial position...");
  lcd.setCursor(0, 0);

  myservo1.attach(servo1Pin);  // attaches the servo on the servo1Pin to the servo object
  myservo2.attach(servo2Pin);
  myservo3.attach(servo3Pin);
  myservo4.attach(servo4Pin);
  myservo1.write(90);  // to stop the base at initial position centre
  myservo2.write(90);  // to stop the base at initial position centre
  myservo3.write(90);  // to stop the base at initial position centre
  myservo4.write(90);  // to stop the base at initial position centre

  //clear all records and make it at initial position
  for (int i = 0; i < 5; i++)
  { servo1PosSave[i] = 90 ;
    servo2PosSave[i] = 90;
    servo3PosSave[i] = 90;
    servo4PosSave[i] = 90;
    Serial.println("Clear all Records");
    Serial.println(servo1PosSave[i]);
    Serial.println(servo2PosSave[i]);
    Serial.println(servo3PosSave[i]);
    Serial.println(servo4PosSave[i]);
  }
  delay(1000);
  lcd.clear();
  lcd.print("clear all recrods");
  delay(1000);
  lcd.clear();

  delay(10);
  Serial.println("Connecting to ");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print("WIFI:");
  lcd.print(ssid);
  lcd.setCursor(0, 2);

  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(550);
    Serial.print(".");
    lcd.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  server.begin();

  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi connected");
  lcd.setCursor(0, 1);
  lcd.print("IP Address: ");
  lcd.setCursor(0, 2);
  lcd.println(WiFi.localIP());

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

            client.println("</head> <body> <a href=\"/Record\"><button class=\"button button1\">Record</button><a href=\"/Play\"><button class=\"button button2\">Play</button></a>");




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
            client.println("<p><a href=\"/car\"><button class=\"button button4\">CLEAR</button></a></p>");










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
              jaw = map(jaw, 0, 180, 90, 30);
              pot1Angle = jaw;
              //Rotate the servo
              myservo1.write(jaw);
              delay(250);
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
              //  wrist = map(wrist, 0, 180, 180, 0); // uncomment if need opposite control
              pot2Angle = wrist;
              //Rotate the servo
              myservo2.write(wrist);
              delay(500);

              Serial.print("Servo 2 Angle");
              Serial.println(valueString2);
            }

            // for servo 3

            //GET /?value=180& HTTP/1.1
            if (header.indexOf("GET /?value3=") >= 0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString3 = header.substring(pos1 + 1, pos2);

              int elbow = valueString3.toInt();
              elbow = map(elbow, 0, 180, 180, 90); // uncomment if need opposite control

              pot3Angle = elbow;
              //Rotate the servo
              myservo3.write(elbow);
              delay(500);
              Serial.print("Servo 3 Angle");
              Serial.println(valueString3);
            }
            // for servo 4

            //GET /?value=180& HTTP/1.1
            if (header.indexOf("GET /?value4=") >= 0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString4 = header.substring(pos1 + 1, pos2);

              int base = valueString4.toInt();
              base = map(base, 0, 180, 180, 0);
              pot4Angle = base;
              //Rotate the servo
              myservo4.write(base);
              delay(500);
              Serial.print("Servo 4 Angle");
              Serial.println(valueString4);


            }

            if (header.indexOf("GET /Reset") >= 0) {
              Serial.println("Reset.....");

              myservo1.write(90);
              delay(200);
              myservo2.write(90);
              delay(200);
              myservo3.write(90);
              delay(200);
              myservo4.write(90);
              delay(200);

              pot1Angle = 90;
              pot2Angle = 90;
              pot3Angle = 90;
              pot4Angle = 90;
              button1Pressed = 0;
              valueString1 = String(0);
              valueString2 = String(90);
              valueString3 = String(90);
              valueString4 = String(90);
            }


            if (header.indexOf("GET /Record") >= 0) {
              Serial.println("Record.....");
              delay(500);
              button1Pressed++;
              Serial.println("Record");
              switch (button1Pressed) {
                case 1:
                  servo1PosSave[0] = pot1Angle;
                  servo2PosSave[0] = pot2Angle;
                  servo3PosSave[0] = pot3Angle;
                  servo4PosSave[0] = pot4Angle;
                  Serial.println("Position #1 Saved");
                  Serial.println(servo1PosSave[0]);
                  Serial.println(servo2PosSave[0]);
                  Serial.println(servo3PosSave[0]);
                  Serial.println(servo4PosSave[0]);
                  break;
                case 2:
                  servo1PosSave[1] = pot1Angle;
                  servo2PosSave[1] = pot2Angle;
                  servo3PosSave[1] = pot3Angle;
                  servo4PosSave[1] = pot4Angle;
                  Serial.println("Position #2 Saved");
                  Serial.println(servo1PosSave[1]);
                  Serial.println(servo2PosSave[1]);
                  Serial.println(servo3PosSave[1]);
                  Serial.println(servo4PosSave[1]);
                  break;
                case 3:
                  servo1PosSave[2] = pot1Angle;
                  servo2PosSave[2] = pot2Angle;
                  servo3PosSave[2] = pot3Angle;
                  servo4PosSave[2] = pot4Angle;
                  Serial.println("Position #3 Saved");
                  Serial.println(servo1PosSave[2]);
                  Serial.println(servo2PosSave[2]);
                  Serial.println(servo3PosSave[2]);
                  Serial.println(servo4PosSave[2]);
                  break;
                case 4:
                  servo1PosSave[3] = pot1Angle;
                  servo2PosSave[3] = pot2Angle;
                  servo3PosSave[3] = pot3Angle;
                  servo4PosSave[3] = pot4Angle;
                  Serial.println("Position #4 Saved");
                  Serial.println(servo1PosSave[3]);
                  Serial.println(servo2PosSave[3]);
                  Serial.println(servo3PosSave[3]);
                  Serial.println(servo4PosSave[3]);
                  break;
                case 5:
                  servo1PosSave[4] = pot1Angle;
                  servo2PosSave[4] = pot2Angle;
                  servo3PosSave[4] = pot3Angle;
                  servo4PosSave[4] = pot4Angle;
                  Serial.println("Position #5 Saved");
                  Serial.println(servo1PosSave[4]);
                  Serial.println(servo2PosSave[4]);
                  Serial.println(servo3PosSave[4]);
                  Serial.println(servo4PosSave[4]);
                  break;
              }

            }

            Serial.println("======================================");
            Serial.println("SERVO POSITION SAVED CHART");
            Serial.println("======================================");
            Serial.print("Position #1 Saved :");
            Serial.print(servo1PosSave[0]);
            Serial.print(" , ");
            Serial.print(servo2PosSave[0]);
            Serial.print(" , ");
            Serial.print(servo3PosSave[0]);
            Serial.print(" , ");
            Serial.println(servo4PosSave[0]);

            Serial.print("Position #2 Saved :");
            Serial.print(servo1PosSave[1]);
            Serial.print(" , ");
            Serial.print(servo2PosSave[1]);
            Serial.print(" , ");
            Serial.print(servo3PosSave[1]);
            Serial.print(" , ");
            Serial.println(servo4PosSave[1]);

            Serial.print("Position #3 Saved :");
            Serial.print(servo1PosSave[2]);
            Serial.print(" , ");
            Serial.print(servo2PosSave[2]);
            Serial.print(" , ");
            Serial.print(servo3PosSave[2]);
            Serial.print(" , ");
            Serial.println(servo4PosSave[2]);


            Serial.print("Position #4 Saved :");
            Serial.print(servo1PosSave[3]);
            Serial.print(" , ");
            Serial.print(servo2PosSave[3]);
            Serial.print(" , ");
            Serial.print(servo3PosSave[3]);
            Serial.print(" , ");
            Serial.println(servo4PosSave[3]);

            Serial.print("Position #5 Saved :");
            Serial.print(servo1PosSave[4]);
            Serial.print(" , ");
            Serial.print(servo2PosSave[4]);
            Serial.print(" , ");
            Serial.print(servo3PosSave[4]);
            Serial.print(" , ");
            Serial.println(servo4PosSave[4]);
            Serial.println("======================================");

            /*
                            servo1PosCurrent[1] = 90;
                            servo2PosCurrent[1] = 90;
                            servo3PosCurrent[1] = 90;
                            servo4PosCurrent[1] = 90;
            */
            servo1PosCurrent[1] = pot1Angle;
            servo2PosCurrent[1] = pot2Angle;
            servo3PosCurrent[1] = pot3Angle;
            servo4PosCurrent[1] = pot4Angle;

            Serial.println("======================================");
            Serial.println("Servo motor angles position");
            Serial.print(servo1PosCurrent[1]);
            Serial.print(" , ");
            Serial.print(servo2PosCurrent[1]);
            Serial.print(" , ");
            Serial.print(servo3PosCurrent[1]);
            Serial.print(" , ");
            Serial.println(servo4PosCurrent[1]);
            Serial.println("======================================");
            Serial.println("Servo current position (Sliders)");
            Serial.print(valueString1);
            Serial.print(" , ");
            Serial.print(valueString2);
            Serial.print(" , ");
            Serial.print(valueString3);
            Serial.print(" , ");
            Serial.println(valueString4);
            Serial.println("======================================");

            if (header.indexOf("GET /car") >= 0) {
              button1Pressed = 0;
              for (int i = 0; i < 5; i++)
              { servo1PosSave[i] = 90 ;
                servo2PosSave[i] = 90;
                servo3PosSave[i] = 90;
                servo4PosSave[i] = 90;
                Serial.println("Clear all Records");
                Serial.println(servo1PosSave[i]);
                Serial.println(servo2PosSave[i]);
                Serial.println(servo3PosSave[i]);
                Serial.println(servo4PosSave[i]);
              }
            }





            if (header.indexOf("GET /Play") >= 0) {
              Serial.println("Play.....");
              button2Pressed = true;





              if (button2Pressed == true) {
                Serial.println("Playing");


                for (int i = 1; i <= 5; i++)
                { if (servo1PosCurrent[i] > servo1PosSave[i - 1]) //if the last position which is saved as current position is greater than First save position
                  { for (servo1PosCurrent[i]; servo1PosCurrent[i] >= servo1PosSave[i - 1]; servo1PosCurrent[i]--)
                    {
                      myservo1.write(servo1PosCurrent[i]);
                      delay(del);
                    }
                  }
                  else
                  {
                    for (servo1PosCurrent[i]; servo1PosCurrent[i] <= servo1PosSave[i - 1]; servo1PosCurrent[i]++)
                    { myservo1.write(servo1PosCurrent[i]);
                      delay(del);
                    }
                  }

                  servo1PosCurrent[i + 1] = servo1PosSave[i - 1];
                  if (servo2PosCurrent[i] > servo2PosSave[i - 1])
                  {
                    for (servo2PosCurrent[i]; servo2PosCurrent[i] >= servo2PosSave[i - 1]; servo2PosCurrent[i]--)
                    { myservo2.write(servo2PosCurrent[i]);
                      delay(del);
                    }

                  }
                  else
                  {
                    for (servo2PosCurrent[i]; servo2PosCurrent[i] <= servo2PosSave[i - 1]; servo2PosCurrent[i]++)
                    { myservo2.write(servo2PosCurrent[i]);
                      delay(del);
                    }

                  }
                  servo2PosCurrent[i + 1] = servo2PosSave[i - 1];

                  if (servo3PosCurrent[i] > servo3PosSave[i - 1])
                  {
                    for (servo3PosCurrent[i]; servo3PosCurrent[i] >= servo3PosSave[i - 1]; servo3PosCurrent[i]--)
                    { myservo3.write(servo3PosCurrent[i]);
                      delay(del);
                    }

                  }
                  else
                  {
                    for (servo3PosCurrent[i]; servo3PosCurrent[i] <= servo3PosSave[i - 1]; servo3PosCurrent[i]++)
                    { myservo3.write(servo3PosCurrent[i]);
                      delay(del);
                    }

                  }
                  servo3PosCurrent[i + 1] = servo3PosSave[i - 1];

                  if (servo4PosCurrent[i] > servo4PosSave[i - 1])
                  {
                    for (servo4PosCurrent[i]; servo4PosCurrent[i] >= servo4PosSave[i - 1]; servo4PosCurrent[i]--)
                    { myservo4.write(servo4PosCurrent[i]);
                      delay(del);
                    }

                  }
                  else
                  {
                    for (servo4PosCurrent[i]; servo4PosCurrent[i] <= servo4PosSave[i - 1]; servo4PosCurrent[i]++)
                    { myservo4.write(servo4PosCurrent[i]);
                      delay(del);
                    }
                  }
                  servo4PosCurrent[i + 1] = servo4PosSave[i - 1];
                }
              }
              delay(1000);
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
