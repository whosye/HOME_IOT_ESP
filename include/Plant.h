#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
class Plant{

    private:
        int ID; 
        Device sensor; 
        Device valve; 
        Device waterpump;
        Device led; 
        PubSubClient client;

    public:
        Plant(){};
        Plant(int Id, Device& moisture, Device& pump, Device& led_device, PubSubClient& Client)
                : ID(Id), sensor(moisture), valve(pump), led(led_device), client(Client) {}
        

    void send_handshake(int value){
        StaticJsonDocument<128> doc;

        // Create a nested object for "status"
        JsonObject status = doc.createNestedObject("status");
        status["ID"] = this->ID;
        status["state"] = value;

        // Serialize the JSON into a char array
        char output[128];
        serializeJson(doc, output);

        // Publish the JSON message
        this->client.publish("test/response", output);

        // Debug print
        Serial.println(output);

    }

    void send_moisture(){
        StaticJsonDocument<128> doc;
        // Create a nested object for "status"
        JsonObject status = doc.createNestedObject("moisture");
        int value = this->sensor.analog_Read(10);
        status["ID"] = this->ID;
        status["value"] = value;
        Serial.print("Measured: ");
        Serial.println(value);
        // Serialize the JSON into a char array
        char output[128];
        serializeJson(doc, output);

        // Publish the JSON message
        client.publish("test/response", output);

        // Debug print
        Serial.println(output);

    }

    void send_end(){
        StaticJsonDocument<128> doc;

        // Create a nested object for "status"
        JsonObject status = doc.createNestedObject("end");
        status["ID"] = this->ID;
        status["state"] = 1;

        // Serialize the JSON into a char array
        char output[128];
        serializeJson(doc, output);

        // Publish the JSON message
        client.publish("test/response", output);

        // Debug print
        Serial.println(output);

    }

    void water(int& time_limit){
        Serial.println("Enter wartering loop");
        Serial.print("Got: "); 
        Serial.print(this->ID);
        Serial.print(" "); 
        Serial.println(time_limit);

        Serial.println("Seding handshake");
        // Send Handshake  
        //send_handshake(1);

        // Wait 
        delay(1000); 
        unsigned long previousMillis = millis();
        unsigned long startMillis = millis();

        Serial.println("Turning on the pump"); 
        this->waterpump.turnDevice(1); 
        this->led.turnDevice(1);

        while (millis() - startMillis < time_limit * 1000) {
            unsigned long currentMillis = millis();


            // Kontrola na čas pro odeslání zprávy o vlhkosti
            if (currentMillis - previousMillis >= 1000) {  // Každou sekundu
                previousMillis = currentMillis;

                //send_moisture();
                Serial.println("Moisture sent");

                // Zkontrolovat stav zrušení
                //if (abort_watering) {
                 //   abort_watering = false;
                 //   send_abort(ID); // confirm aborting
                  //  Serial.println("Watering aborted");
                  //  return;
                //}
            }

            // Obsluhuj MQTT klienta
            this->client.loop();
            }

        send_end();
        this->waterpump.turnDevice(0); 
        this->led.turnDevice(0);

    }




};