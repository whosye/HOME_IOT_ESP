#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <cstring>
#include <Device.h>
#include <map>
#include <Plant.h>
#define BUTTON 12


const char* ssid = "BackDoorAccess";
const char* password = "GiveItToMe!";
const char* mqttServer = "165.232.94.140"; 
const int mqttPort = 1883;
const char* mqttUser = "user1";
const char* mqttPassword = "270797mM";

WiFiClient espClient;
PubSubClient client(espClient);

Device waterPump(23, false);
Device pushButton(BUTTON, true);

Device moistureSens_1(32, true);
Device led_1(27, false); 
Device pump_1(26,false);
Plant plant1(1, moistureSens_1, pump_1, led_1, client);

Device moistureSens_2(35, true);
Device led_2(25, false); 
Device pump_2(33,false);
Plant plant2(2, moistureSens_2, pump_2, led_2, client);

Device moistureSens_3(39, true);
Device led_3(16, false); 
Device pump_3(17,false);
Plant plant3(3, moistureSens_3, pump_3, led_3, client);

Device moistureSens_4(34, true);
Device led_4(18, false); 
Device pump_4(19,false);
Plant plant4(4, moistureSens_4, pump_4, led_4, client);
std::map<int, Plant> created_plants; 


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);

  // Step 1: Convert byte* to char*
  char jsonStr[length + 1];  // Create a char array to hold the string
  memcpy(jsonStr, payload, length);  // Copy the byte array into the char array
  jsonStr[length] = '\0';  // Null-terminate the string

  // Step 2: Parse the JSON string
  StaticJsonDocument<200> doc;
  
  DeserializationError error = deserializeJson(doc, jsonStr);
  
  if (error) {
    Serial.print("JSON deserialization failed: ");
    Serial.println(error.f_str());
    return;
  }

  //////////////
  if (doc.containsKey("command")) {
    JsonObject command = doc["command"];
    


    if (command.containsKey("ID") && command.containsKey("limit")) {
      int ID = command["ID"];
      int limit = command["limit"];
      
      // Process the extracted values
      Serial.print("ID: ");
      Serial.println(ID);
      Serial.print("Limit: ");
      Serial.println(limit);


      Plant object = created_plants[ID]; 

      object.water(ID);
      //created_plants[1].water(ID, limit);
      int meas = moistureSens_1.analog_Read(10);
      Serial.println(meas);
      StaticJsonDocument<128> doc;

      // Create a nested object for "status"
      JsonObject status = doc.createNestedObject("status");
      status["message"] = "HelloFromLuzicka";
      status["moisture"] = meas;

      // Serialize the JSON into a char array
      char output[128];
      serializeJson(doc, output);

      // Publish the JSON message
      client.publish("test/response", output);
      
    }
  }

  
}
  
void setup() {
  Serial.begin(115200);
  pushButton.ButtonSetPullUp();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("Failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }

  created_plants[1] = plant1;
  created_plants[2] = plant2;
  created_plants[3] = plant3;
  created_plants[4] = plant4;


  const char* message = "Hello from ESP32!";
  StaticJsonDocument<128> doc;
  doc["message"] = "Hello from ESP32!";
  char output[128];
  serializeJson(doc,  output);
  client.publish("test/response",output);
  client.subscribe("test/commands");

}


void loop() {
  int state = pushButton.digital_Read(); 
  int ON = 1;
  int OFF = 0; 
  if (state == false){
    Serial.println("state is false"); 
    waterPump.turnDevice(ON);
    led_3.turnDevice(ON);
    pump_3.turnDevice(ON);
  }else{
    waterPump.turnDevice(OFF);
    led_3.turnDevice(OFF);
    pump_3.turnDevice(OFF);
  }

  delay(1000);
  client.loop();
}