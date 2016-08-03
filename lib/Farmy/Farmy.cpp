#include "Farmy.h"

#define JSON_BUFFER 256
#define FLASH_DELAY 1000
#define LONG_FLASH_DELAY 15000

void Farmy::send( const char* device_id, int input_pins[], String api_key, WiFiClient client)
{
  String data = collectData(input_pins);
  sendData(device_id, api_key, client, data);
}

void Farmy::execute(const char* device_id, String api_key, WiFiClient client)
{
  char* json = getActionList(device_id, api_key, client);
  executeActions(json);
}

String Farmy::collectData(int input_pins[])
{
  StaticJsonBuffer<200> jsonBuffer, buffer;
  JsonArray& array = jsonBuffer.createArray();
  JsonObject& object = buffer.createObject();

  int i = 0;
  while(input_pins[i]) {
    object["pin"] = input_pins[i];
    object["value"] = analogRead(input_pins[i]);
    array.add(object);
    ++i;
  }

  char data[JSON_BUFFER];
  array.printTo(data, sizeof(data));
  Serial.println("collected json data:  ----------------------");
  Serial.println(data);

  String str = data;

  return data;
}

void Farmy::sendData(const char* device_id, String api_key, WiFiClient client, String data)
{
  // Todo: use retry to connect internet.
  Serial.println("Connected to ThingSpeak.");
  Serial.println("Posted:" + data);

  // Create HTTP POST Data
  String url = String("/api/v0/user_devices/") + device_id + "/sensor_datas/";
  client.print(String("POST ") + url + " HTTP/1.1\n"+ "Host: " + host + "\n");
  client.print(String("Host: ") + host + "\n");
  client.print("Content-Type: application/json\n");
  client.print(String("X-Farmy-Api-Key: ") + api_key + "\n");
  client.print("Content-Length: ");
  client.print(data.length());
  client.print("\n\n");

  client.print(data);
  client.stop();
}

char* Farmy::getActionList(const char* device_id, String api_key, WiFiClient client) {
  String url = String("/api/v0/user_devices/") + device_id + "/triggered_actions/";

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n");
  client.print("Content-Type: application/json\r\n");
  client.print(String("X-Farmy-Api-Key: ") + api_key + "\r\n");
  client.print("Connection: close\r\n\r\n");
  client.print("\r\n\r\n");

  char* json = (char *)malloc(JSON_BUFFER);
  int timeout = millis() + 5000;
  while (client.available() == 0) {
    if (timeout - millis() < 0) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return json;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  bool begin = false;
  int index = 0;
  while(client.available()) {
    char c = client.read();
    Serial.print(c);
    if (c == '[') begin = true;
    if (begin) json[index++] = c;
    if (c == ']') break;
  }
  Serial.println("\n\nGot json data ---------------");
  Serial.println(json);

  Serial.println();
  Serial.println("closing connection");

  return json;
}

void Farmy::executeActions(char* json)
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
