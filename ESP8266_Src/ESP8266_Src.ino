#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

SoftwareSerial mySerial(D4, D5); //RX, TX
// D4 RX - PA9
// D5 TX - PA10
// (Send and Receive)\

//Replace with my networl credentials
const char* ssid = "jck";
const char* password = "jck12345678";
//Set web server port number to 8888
WiFiServer server(80);

//Variable to store the HTTP request
String header;

//Auxiliar variable to store the current output store
String outputLD2 = "off";

//Assign output variable to GPIO pins
const byte outputPin = LED_BUILTIN;

//Current time
unsigned long currentTime = millis();
//Previous time 
unsigned long previousTime = 0;
//Define timeout time in milliseconds (example: 2000 ms = 2s )
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);
  // put your setup code here, to run once:
  // Connect to Wi-Fi network with SSID and password
//  pinMode(LED_BUILTIN,OUTPUT); 
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
//  mySerial.begin(9600);
//
//  pinMode(rxPin, INPUT);      
//  pinMode(txPin, OUTPUT);
  pinMode(outputPin, OUTPUT);
//
  digitalWrite(outputPin, HIGH);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {

  if(Serial.available() > 0){//Read from serial monitor and send 
      String input = Serial.readString();
      mySerial.println(input);    
    }
   
    if(mySerial.available() > 1){//Read from  STM module and send to serial monitor
      String input = mySerial.readString();
      Serial.println(input);    
    }
    delay(20);
    
    // put your main code here, to run repeatedly:
    WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
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
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              outputLD2 = "on";
              digitalWrite(outputPin, LOW);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("GPIO 5 off");
              outputLD2 = "off";
              digitalWrite(outputPin, HIGH);
            } 
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta charset=\"UTF-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><title>Final project</title>");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; text-align: center; background-color: #fff0d9;}");
            client.println(".buttonOn { margin: 2px; background-color: #8FE381; border: 2px solid #422800; border-radius: 30px;box-shadow: #422800 4px 4px 0 0;");
            client.println("color: #422800; cursor: pointer; display: inline-block; font-weight: 600; font-size: 18px; padding: 0 18px; line-height: 50px;");
            client.println("text-align: center; text-decoration: none; user-select: none; -webkit-user-select: none; touch-action: manipulation;}");
            client.println(".buttonOn:hover { background-color: #5fa95b;}");
            client.println(".buttonOn:active { box-shadow: #422800 2px 2px 0 0; transform: translate(2px, 2px);}");
            client.println("@media (min-width: 768px) {.buttonOn { min-width: 120px; padding: 0 25px; }}");
            client.println(".buttonOff { margin: 2px; background-color: #FE6862; border: 2px solid #422800; border-radius: 30px;box-shadow: #422800 4px 4px 0 0;");
            client.println("color: #422800; cursor: pointer; display: inline-block; font-weight: 600; font-size: 18px; padding: 0 18px; line-height: 50px;");
            client.println("text-align: center; text-decoration: none; user-select: none; -webkit-user-select: none; touch-action: manipulation;}");
            client.println(".buttonOff:hover { background-color: #c23b22;}");
            client.println(".buttonOff:active { box-shadow: #422800 2px 2px 0 0; transform: translate(2px, 2px);}");
            client.println("@media (min-width: 768px) {.buttonOff { min-width: 120px; padding: 0 25px; }}");
            client.println(".header { color: #422800; margin-top: 150px;} ");
            client.println(".showState{ color: #836953; font-family: sans-serif; font-weight: 18px; font-size: large; margin: 30px;");
            client.println(" border: 2px solid #422800; border-radius: 30px; display: inline-block; line-height: 50px; text-align: center; text-decoration: none;");
            client.println(" user-select: none; padding: 0 50px;}</style></head>");
            
            // Web Page Heading
            client.println("<body><div class=\"header\"><h1>ESP8266 Web Server</h1></div>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<div class=\"showState\"><p>GPIO 5 - State " + outputLD2 + "</p></div>");
            // If the outputLD2 is off, it displays the ON button       
            if (outputLD2=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"buttonOn\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"buttonOff\">OFF</button></a></p></body></html>");
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
