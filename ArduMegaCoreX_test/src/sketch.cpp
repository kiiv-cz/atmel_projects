/*
 * ArduMegaCoreX_test.cpp
 *
 * Created: 19.11.2022 8:45:50
 * Author : mikus
 */ 

#include <avr/io.h>
#include <Arduino.h>
#include <paj7620.h>
#include <Wire.h>

void setup() {
    Serial.swap(1);
    Serial.begin(115200);
    
    pinMode(8, INPUT_PULLUP);
    
    Wire.pins(10,11);

    if (auto error = paj7620Init()) {
        Serial.print("INIT ERROR,CODE:");
        Serial.println(error);
    } else {
        Serial.println("INIT OK");
    }
    
}

#define GES_REACTION_TIME		500				// You can adjust the reaction time according to the actual circumstance.
#define GES_ENTRY_TIME			800				// When you want to recognize the Forward/Backward gestures, your gestures' reaction time must less than GES_ENTRY_TIME(0.8s).
#define GES_QUIT_TIME			1000


void loop() {
    while (Serial.available()) {
        Serial.write(Serial.read());
    }

    if (digitalReadFast(8) == LOW) {
        uint8_t data = 0;
        paj7620ReadReg(0x43, 1, &data);
        
        switch (data) {
            case GES_RIGHT_FLAG:            Serial.println("Right");            break;
            case GES_LEFT_FLAG:             Serial.println("Left");             break;
            case GES_UP_FLAG:               Serial.println("Up");               break;
            case GES_DOWN_FLAG:             Serial.println("Down");             break;
            case GES_FORWARD_FLAG:          Serial.println("Forward");          break;
            case GES_BACKWARD_FLAG:         Serial.println("Backward");         break;
            case GES_CLOCKWISE_FLAG:        Serial.println("Clockwise");        break;
            case GES_COUNT_CLOCKWISE_FLAG:  Serial.println("anti-clockwise");   break;
            default:
                paj7620ReadReg(0x44, 1, &data);
                Serial.printf("neco jineho: %d\n", data);
                break;            
        }
    }

/*
    uint8_t data;
    auto error = paj7620ReadReg(0x43, 1, &data);				// Read Bank_0_Reg_0x43/0x44 for gesture result.
    if (!error) {
        switch (data) {								// When different gestures be detected, the variable 'data' will be set to different values by paj7620ReadReg(0x43, 1, &data).
            case GES_RIGHT_FLAG:
                delay(GES_ENTRY_TIME);
                paj7620ReadReg(0x43, 1, &data);
                if (data == GES_FORWARD_FLAG) {
                    Serial.println("Forward");
                    delay(GES_QUIT_TIME);
                } else if (data == GES_BACKWARD_FLAG) {
                    Serial.println("Backward");
                    delay(GES_QUIT_TIME);
                } else {
                    Serial.println("Right");
                }
                break;
            case GES_LEFT_FLAG:
                delay(GES_ENTRY_TIME);
                paj7620ReadReg(0x43, 1, &data);
                if (data == GES_FORWARD_FLAG) {
                    Serial.println("Forward");
                    delay(GES_QUIT_TIME);
                } else if (data == GES_BACKWARD_FLAG) {
                    Serial.println("Backward");
                    delay(GES_QUIT_TIME);
                } else {
                    Serial.println("Left");
                }
                break;
            case GES_UP_FLAG:
                delay(GES_ENTRY_TIME);
                paj7620ReadReg(0x43, 1, &data);
                if (data == GES_FORWARD_FLAG) {
                    Serial.println("Forward");
                    delay(GES_QUIT_TIME);
                } else if (data == GES_BACKWARD_FLAG) {
                    Serial.println("Backward");
                    delay(GES_QUIT_TIME);
                } else {
                    Serial.println("Up");
                }
                break;
            case GES_DOWN_FLAG:
                delay(GES_ENTRY_TIME);
                paj7620ReadReg(0x43, 1, &data);
                if (data == GES_FORWARD_FLAG) {
                    Serial.println("Forward");
                    delay(GES_QUIT_TIME);
                } else if (data == GES_BACKWARD_FLAG) {
                    Serial.println("Backward");
                    delay(GES_QUIT_TIME);
                } else {
                    Serial.println("Down");
                }
                break;
            case GES_FORWARD_FLAG:
                Serial.println("Forward");
                delay(GES_QUIT_TIME);
                break;
            case GES_BACKWARD_FLAG:
                Serial.println("Backward");
                delay(GES_QUIT_TIME);
                break;
            case GES_CLOCKWISE_FLAG:
                Serial.println("Clockwise");
                break;
            case GES_COUNT_CLOCKWISE_FLAG:
                Serial.println("anti-clockwise");
                break;
            default:
                paj7620ReadReg(0x44, 1, &data);
                if (data == GES_WAVE_FLAG) {
                    Serial.println("wave");
                }
                break;
        }
    }
    
    delay(100);
    
*/
}
