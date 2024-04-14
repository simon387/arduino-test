///////////////////////////////////////
// read text from an API             //
// print the text to the led matrix  //
///////////////////////////////////////

#include "WiFiS3.h"
#include "arduino_secrets.h"
#include "ArduinoJson.h"
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;           // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;
char server[] = "www.simonecelia.it";  // (using DNS)

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;
ArduinoLEDMatrix matrix;
String response = "";  // Variable to store the response
String text = "";

void setup() {
  setupMatrix();


  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  printWifiStatus();

  String rsp = getResponse();
  text = getText(rsp);
  Serial.println("disconnecting from server.");
  client.stop();

  response = "";
}

void setupMatrix() {
  matrix.begin();

  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  // add some static text
  // will only show "UNO" (not enough space on the display)
  const char text[] = "UNO r4";
  matrix.textFont(Font_4x6);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText();

  matrix.endDraw();
}


/* just wrap the received data up to 80 columns in the serial print*/
void read_response() {
  uint32_t received_data_num = 0;
  while (client.available()) {
    /* actual data reception */
    char c = client.read();
    response += c;
    /* print data to serial port */
    // Serial.print(c);
    /* wrap data to 80 columns*/
    received_data_num++;
    // if (received_data_num % 512 == 0) {
    //Serial.println();
    // }
  }
}


void loop() {
  scrollText(text);
}

String getResponse() {
  connectToServer();
  while (client.connected()) {
    read_response();
  }
  Serial.println("response:");
  Serial.println(response);
  Serial.println();
  return response;
}

String getText(String response) {
  // Find the start of JSON content
  int jsonStartIndex = response.indexOf('{');
  // Extract JSON content
  String jsonContent = response.substring(jsonStartIndex);
  // Extract the "text" value from JSON content
  String text = extractTextValue(jsonContent);
  return text;
}

void scrollText(String text) {
  // Make it scroll!
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(75);
  // add the text
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


String extractTextValue(String jsonString) {
  // Parse the JSON
  DynamicJsonDocument jsonDocument(512);  // Adjust the size according to your JSON
  DeserializationError error = deserializeJson(jsonDocument, jsonString);
  // Check for parsing errors
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return "";
  }
  String textValue = jsonDocument["text"].as<String>();
  return textValue;
}

void connectToServer() {
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET /text-api/v1/text.php HTTP/1.1");
    client.println("Host: www.simonecelia.it");
    client.println("Connection: close");
    client.println();
  }
}