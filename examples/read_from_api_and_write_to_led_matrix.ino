///////////////////////////////////////
// read text from an API             //
// print the text to the led matrix  //
///////////////////////////////////////

#include "WiFiS3.h"
#include "arduino_secrets.h"
#include "ArduinoJson.h"
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;
const char server[] = "www.simonecelia.it";
const bool PRINT_TO_SERIAL = false;  // false = better performance ?
int status = WL_IDLE_STATUS;

WiFiClient client;
ArduinoLEDMatrix matrix;
String text = "";

void setup() {
  setupMatrix();

  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  if (WiFi.status() == WL_NO_MODULE) {
    if (PRINT_TO_SERIAL) Serial.println("Communication with WiFi module failed!");
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    if (PRINT_TO_SERIAL) Serial.println("Please upgrade the firmware");
  }

  while (status != WL_CONNECTED) {
    if (PRINT_TO_SERIAL) Serial.print("Attempting to connect to SSID: ");
    if (PRINT_TO_SERIAL) Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  printWifiStatus();

  clientGetData();
}

void clientGetData() {
  String rsp = getResponse();
  text = getText(rsp);
  client.stop();
}

void setupMatrix() {
  matrix.begin();
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  const char text[] = "UNO r4";  // will only show "UNO" (not enough space on the display)
  matrix.textFont(Font_4x6);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText();
  matrix.endDraw();
}


String read_response() {
  uint32_t received_data_num = 0;
  String response = "";
  while (client.available()) {
    char c = client.read();
    response += c;
    received_data_num++;
  }
  return response;
}


void loop() {
  scrollText(text);
  clientGetData();
}

String getResponse() {
  connectToServerAndMakeRequest();
  String response = "";
  while (client.connected()) {
    response = read_response();
  }
  if (PRINT_TO_SERIAL) Serial.println("response:");
  if (PRINT_TO_SERIAL) Serial.println(response);
  if (PRINT_TO_SERIAL) Serial.println();
  return response;
}

String getText(String response) {
  int jsonStartIndex = response.indexOf('{');
  String jsonContent = response.substring(jsonStartIndex);
  String text = extractTextValue(jsonContent);
  return text;
}

void scrollText(String text) {
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(75);
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
}


void printWifiStatus() {
  if (PRINT_TO_SERIAL) Serial.print("SSID: ");
  if (PRINT_TO_SERIAL) Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  if (PRINT_TO_SERIAL) Serial.print("IP Address: ");
  if (PRINT_TO_SERIAL) Serial.println(ip);
  long rssi = WiFi.RSSI();
  if (PRINT_TO_SERIAL) Serial.print("signal strength (RSSI):");
  if (PRINT_TO_SERIAL) Serial.print(rssi);
  if (PRINT_TO_SERIAL) Serial.println(" dBm");
}


String extractTextValue(String jsonString) {
  DynamicJsonDocument jsonDocument(512);  // Adjust the size according to your JSON
  DeserializationError error = deserializeJson(jsonDocument, jsonString);
  if (error) {
    if (PRINT_TO_SERIAL) Serial.print("deserializeJson() failed: ");
    if (PRINT_TO_SERIAL) Serial.println(error.c_str());
    return "";
  }
  String textValue = jsonDocument["text"].as<String>();
  return textValue;
}

void connectToServerAndMakeRequest() {
  if (PRINT_TO_SERIAL) Serial.println("\nStarting connection to server...");
  if (client.connect(server, 80)) {
    if (PRINT_TO_SERIAL) Serial.print("connected to server\n");
    client.println("GET /text-api/v1/text.php HTTP/1.1");
    client.println("Host: www.simonecelia.it");
    client.println("Connection: close");
    client.println();
  }
}
