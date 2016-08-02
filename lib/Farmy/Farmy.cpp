#include "Farmy.h"

void Farmy::send(const char* host, const char* device_id, int input_pins[], String api_key, WiFiClient client)
{
  String data = collectData(input_pins);
  sendData(host, device_id, api_key, client, data);
}

String Farmy::collectData(int input_pins[])
{
  //char json[] = "[{\"pin\":\"5\",\"value\":233}, {\"pin\":\"4\",\"value\":348}]";
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

  char data[256];
  array.printTo(data, sizeof(data));
  Serial.println("json data:  ----------------------");
  Serial.println(data);

  String str = data;

  return data;
}

void Farmy::sendData(const char* host, const char* device_id, String api_key, WiFiClient client, String data)
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

void Farmy::getActionList(const char* host, const char* device_id, String api_key, WiFiClient client) {
  String url = String("/api/v0/user_devices/") + device_id + "/triggered_actions/";

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

void Farmy::execute()
{
}
