/**
 * Farmy
 */
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include "include/ArduinoJson.h"

extern "C" {
  #include "spi.h"
  #include "spi_register.h"
}
/* setup Wifi */
const char* ssid     = "Fenney";
const char* password = "1357924680";

/* setup host */
const char* host = "farmy.net";
const char* device_id = "bx7eWzca";

/* setup api key */
String HTTP_X_FARMY_API_KEY = "uDLTp8WtTc5wQnFhyKFvFV";

const int pin_number = 1;
int pins[pin_number] = { 17 };

WiFiClient client;

String prepareData()
{
  //char json[] = "[{\"pin\":\"5\",\"value\":233}, {\"pin\":\"4\",\"value\":348}]";
  StaticJsonBuffer<200> jsonBuffer, buffer;
  JsonArray& array = jsonBuffer.createArray();
  JsonObject& object = buffer.createObject();

  for(int i = 0; i < pin_number; ++i) {
    object["pin"] = pins[i];
    object["value"] = analogRead(pins[i]);
    array.add(object);
  }

  char data[256];
  array.printTo(data, sizeof(data));
  Serial.println("json data:  ----------------------");
  Serial.println(data);

  String str = data;

  return data;
}

void postSensorDatas(String data)
{
  if (client.connect(host, 80))
  {
    Serial.println("Connected to ThingSpeak.");
    Serial.println("Posted:" + data);

    // Create HTTP POST Data
    client.print(String("POST /api/v0/user_devices/") + device_id +  "/sensor_datas/ HTTP/1.1\n");
    client.print(String("Host: ") + host + "\n");
    client.print("Content-Type: application/json\n");
    client.print(String("X-Farmy-Api-Key: ") + HTTP_X_FARMY_API_KEY + "\n");
    client.print("Content-Length: ");
    client.print(data.length());
    client.print("\n\n");

    client.print(data);
    client.stop();
  }
  else
  {
     Serial.println("Connection failed.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  delay(2000);

  Serial.print("connecting to ");
  Serial.println(host);

  // postSensorDatas(prepareData());

  String url = "/api/v0/user_devices/bx7eWzca/triggered_actions/";

  if (!client.connect(host, 80)) {
      Serial.println("connection failed");
      return;
  }
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n");
  client.print("Content-Type: application/json\r\n");
  client.print(String("X-Farmy-Api-Key: ") + HTTP_X_FARMY_API_KEY + "\r\n");
  client.print("Connection: close\r\n\r\n");
  client.print("\r\n\r\n");

  int timeout = millis() + 5000;
  while (client.available() == 0) {
    if (timeout - millis() < 0) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while(client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
    Serial.println("---------------");
  }

  Serial.println();
  Serial.println("closing connection");
}
