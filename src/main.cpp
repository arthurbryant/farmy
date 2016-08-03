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

  if(!client.connect(host, 80)) {
    Serial.println("connection failed");
    return;
  }

  Farmy farmy;
  farmy.execute(device_id, api_key, client);
  delay(2000);
  farmy.send(device_id, input_pins, api_key, client);
}
