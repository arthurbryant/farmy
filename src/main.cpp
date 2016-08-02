/**
 * Farmy
 */
#include "Arduino.h"
#include "Farmy.h"

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
String api_key = "uDLTp8WtTc5wQnFhyKFvFV";

const int pins_size = 8;
int input_pins[pins_size] = { 17 };
int output_pins[pins_size] = {  };

// Todo: try to support other device, such as wire cable network.
WiFiClient client;

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

  collectAndSendData(host, device_id, input_pins, api_key, client);

  String url = "/api/v0/user_devices/bx7eWzca/triggered_actions/";

  if (!client.connect(host, 80)) {
      Serial.println("connection failed");
      return;
  }
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n");
  client.print("Content-Type: application/json\r\n");
  client.print(String("X-Farmy-Api-Key: ") + api_key + "\r\n");
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
