#include "Farmy.h"

void Farmy::collectAndSendData(char* host, char* device_id, int input_pins[], String api_key, WiFiClient client)
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

void Farmy::sendData(char* host, char* device_id, String api_key, WiFiClient client, String data)
{
  // Todo: use retry to connect internet.
  if (client.connect(host, 80))
  {
    Serial.println("Connected to ThingSpeak.");
    Serial.println("Posted:" + data);

    // Create HTTP POST Data
    client.print(String("POST /api/v0/user_devices/") + device_id +  "/sensor_datas/ HTTP/1.1\n");
    client.print(String("Host: ") + host + "\n");
    client.print("Content-Type: application/json\n");
    client.print(String("X-Farmy-Api-Key: ") + api_key + "\n");
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
