#include <Arduino.h>

class Device{
    private:
        int pin; 
        bool sensor;
    public:
        Device() {
        // Default constructor implementation
        }

        Device(uint8_t Pin_to_device, bool sensor){
            this->pin = Pin_to_device;
            this->sensor = sensor;

            if (sensor){
                pinMode(this->pin, INPUT); 
            }else{
                pinMode(this->pin,OUTPUT);
            }
        };

        
    void ButtonSetPullUp(){
        pinMode(this->pin, INPUT_PULLUP);
    }
    void turnDevice(int STATE){
        if (this->sensor){
            if (Serial){
                Serial.println("Cant turn sensor on!");
                
            }
            return;

        }else{
            digitalWrite(this->pin, STATE);
        };
    }

    int digital_Read(){

        int state = digitalRead(this->pin);
        return state;

    };

    int analog_Read(int samples){
        if (!this->sensor){
            if (Serial){
                Serial.println("Cant read from power device!");
                
            }
            return 0;
        }else{
            float sum = 0;
            for (int i = 0 ; i < samples ; i++){
                sum += analogRead(this->pin);
                delay(50);
            }
            return int(sum / samples);
        }
    }

    int getPin(){
        if (Serial){
            Serial.print("Pin is ");
            Serial.println(this->pin);
                
        }
        return this->pin;
    }

    void setPin(int pin){
        this->pin = pin; 
        if (Serial){
            Serial.print("Pin is set to ");
            Serial.println(this->pin);
                
        }
    }
};