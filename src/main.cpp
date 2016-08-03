/**
 * Farmy
 */
#include "Arduino.h"
#include "Farmy.h"

#define FLASH_DELAY 1000
#define LONG_FLASH_DELAY 15000

extern "C" {
  #include "spi.h"
  #include "spi_register.h"
}

/* setup Wifi */
const char* ssid     = "Fenney";
const char* password = "1357924680";

const char* device_id = "bx7eWzca";
String api_key = "uDLTp8WtTc5wQnFhyKFvFV";

const int pins_size = 8;
int input_pins[pins_size] = { 17 };
int output_pins[pins_size] = {  };

// Todo: try to support other device, such as wire cable network.
WiFiClient client;

void executeActions(char* json)
{
  StaticJsonBuffer<JSON_BUFFER> jsonBuffer;
  JsonArray& array = jsonBuffer.parseArray(json);
  for(JsonArray::iterator it=array.begin(); it!=array.end(); ++it)
  {
    JsonObject& object = *it;
    String pin_s = object["pin"];
    String action_type = object["action_type"];
    Serial.println(pin_s);
    Serial.println(action_type);

    int pin = atof(pin_s.c_str());
    pinMode(pin, OUTPUT);

    if(action_type == "turn_on") {
        Serial.println("Start to turn on");
        digitalWrite(pin, LOW);
    }
    else if(action_type == "turn_off") {
        Serial.println("Start to turn off");
        digitalWrite(pin, HIGH);
    }
    else if(action_type == "flash") {
        Serial.println("Start to flash");
        digitalWrite(pin, LOW);
        delay(FLASH_DELAY);
        digitalWrite(pin, HIGH);
    }
    else if(action_type == "flash_long") {
        Serial.println("Start to long flash");
        digitalWrite(pin, LOW);
        delay(LONG_FLASH_DELAY);
        digitalWrite(pin, HIGH);
    }
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

  if(!client.connect(host, 80)) {
    Serial.println("connection failed");
    return;
  }

  Farmy farmy;
  char* json = farmy.get(device_id, api_key, client);
  executeActions(json);
  farmy.send(device_id, input_pins, api_key, client);
}
