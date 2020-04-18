#include <Arduino.h>

// Load Wi-Fi library
#include <ESP8266WiFi.h>
// Fancy .local domain
#include <ESP8266mDNS.h>

// Replace with your network credentials
const char* ssid     = "SSID";
const char* password = "xxxxxxxxxxxxxxxx";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
int output5State = 0;
int output4State = 0;
int output0State = 0;

// Assign output variables to GPIO pins
const int output5 = 5;
const int output4 = 4;
const int output0 = 0;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output5, OUTPUT);
  pinMode(output4, OUTPUT);
  pinMode(output0, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output5, LOW);
  digitalWrite(output4, LOW);
  digitalWrite(output0, LOW);

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
  if (MDNS.begin("meetingsign")) {
    Serial.println("MDNS responder started");
  }
  server.begin();
}

void loop(){
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
            if (header.indexOf("GET /5/toggle") >= 0) {
              Serial.println("GPIO 5 on");
              digitalWrite(output5, !digitalRead(output5));
              output5State = digitalRead(output5);
            }
            else if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              digitalWrite(output5, HIGH);
              output5State = digitalRead(output5);
            }
            else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("GPIO 5 off");
              digitalWrite(output5, LOW);
              output5State = digitalRead(output5);
            }
            else if (header.indexOf("GET /4/toggle") >= 0) {
              Serial.println("GPIO 4 on");
              digitalWrite(output4, !digitalRead(output4));
              output4State = digitalRead(output4);
            }
            else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              digitalWrite(output4, HIGH);
              output4State = digitalRead(output4);
            }
            else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              digitalWrite(output4, LOW);
              output4State = digitalRead(output4);
            }
            else if (header.indexOf("GET /0/toggle") >= 0) {
              Serial.println("GPIO 0 on");
              digitalWrite(output0, !digitalRead(output0));
              output0State = digitalRead(output0);
            }
            else if (header.indexOf("GET /0/on") >= 0) {
              Serial.println("GPIO 0 on");
              digitalWrite(output0, HIGH);
              output0State = digitalRead(output0);
            }
            else if (header.indexOf("GET /0/off") >= 0) {
              Serial.println("GPIO 0 off");
              digitalWrite(output0, LOW);
              output0State = digitalRead(output4);
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>Meeting Sign</h1>");
            // If the output5State is off, it displays the ON button
            if (output5State == 0) {
            client.println("<p>Blue/Meeting: GPIO 5 - State Off</p>");
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p>Blue/Meeting: GPIO 5 - State On</p>");
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // If the output4State is off, it displays the ON button
            if (output4State == 0) {
              client.println("<p>Red/Video: GPIO 4 - State Off</p>");
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p>Red/Video: GPIO 4 - State On</p>");
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            if (output0State == 0) {
            client.println("<p>Yellow/Streaming: GPIO 0 - State Off</p>");
              client.println("<p><a href=\"/0/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p>Yellow/Streaming: GPIO 0 - State On</p>");
              client.println("<p><a href=\"/0/off\"><button class=\"button button2\">OFF</button></a></p>");
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
  MDNS.update();
}
