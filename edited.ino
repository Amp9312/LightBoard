// MirroringSample - see https://github.com/FastLED/FastLED/wiki/Multiple-Controller-Examples for more info on
// using multiple controllers.  In this example, we're going to set up four NEOPIXEL strips on four
// different pins, and show the same thing on all four of them, a simple bouncing dot/cyclon type pattern
#include <FastLED.h>
#include <IRremote.h> // used for Elegoo IR remote
#include <WiFi.h>
#include <BluetoothSerial.h>
#include <esp_wifi.h>
#include "driver/adc.h"

#define NUM_LEDS_PER_STRIP 29

CRGB leds[NUM_LEDS_PER_STRIP];
decode_results results; // this is a variable to hold the received IR signal
unsigned long key_value = 0; // variable to store the key value
bool ON_SWITCH = true;
int SPEED = 35;
TaskHandle_t changeLED;
TaskHandle_t readIRs;

void readIR(void * pvParameters) {
  String taskMessage = "ReadIR running on core ";
  taskMessage = taskMessage + xPortGetCoreID();
  Serial.println(taskMessage);
  while(true){
    //Serial.println("Checking IR");
    if (IrReceiver.decode()) {
        // Print a short summary of received data
        IrReceiver.printIRResultShort(&Serial);
        if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
            // We have an unknown protocol here, print more info
            IrReceiver.printIRResultRawFormatted(&Serial, true);
        }
        Serial.println();
        IrReceiver.resume(); // Enable receiving of the next value
        if (IrReceiver.decodedIRData.command == 0x45) {
            ON_SWITCH = true;
        } else if (IrReceiver.decodedIRData.command == 0x47) {
            ON_SWITCH = false;
        }
          else if(IrReceiver.decodedIRData.command == 0x9){
            SPEED = SPEED / 2;
            Serial.println("Delay has been halved, SPEED is now:");
            Serial.println(SPEED);
          }
          else if(IrReceiver.decodedIRData.command == 0x7){
            SPEED = SPEED * 2;
            Serial.println("Delay has been doubled, SPEED is now:");
            Serial.println(SPEED);
          }
    }
    vTaskDelay(1);
  }
}

void changeLEDs(void * pvParameters){
  Serial.print("changeLEDs running on core ");
  Serial.println(xPortGetCoreID());
  while(true){  
  Serial.println("changeLEDs");
  if(ON_SWITCH == true){
    Serial.println("ON 1");
      for(int i = 0; i < NUM_LEDS_PER_STRIP; i++) 
      {
        // Green loop
        // set our current dot to Red
        leds[i] = CRGB::Black;    
        FastLED.show();
        // clear our current dot before we move on
        leds[i] = CRGB::Green;
        vTaskDelay(SPEED);
      }
    }
    
    if(ON_SWITCH == true)
    {    
      Serial.println("ON 2");
      for(int i = NUM_LEDS_PER_STRIP-1; i >= 0; i--)
      {
        // set our current dot to White
        leds[i] = CRGB::Black;
        FastLED.show();
        // clear our current dot before we move on
        leds[i] = CRGB::Blue;
        vTaskDelay(SPEED);
      }
    }
    
    if(ON_SWITCH == false){
      Serial.println("OFF");
      for(int i = NUM_LEDS_PER_STRIP-1; i >= 0; i--) {
        // set our current dot to White
        leds[i] = CRGB::Black;
        FastLED.show();
        
        // clear our current dot before we move on
        vTaskDelay(SPEED);
      }
    }
    
  }
}

void disableWiFi(){
        adc_power_off();
    WiFi.disconnect(true);  // Disconnect from the network
    WiFi.mode(WIFI_OFF);    // Switch WiFi off
    btStop();
}

// For mirroring strips, all the "special" stuff happens just in setup.  We
// just addLeds multiple times, once for each strip

void setup() {
  disableWiFi();
  FastLED.addLeds<NEOPIXEL, 25>(leds, NUM_LEDS_PER_STRIP);
  Serial.begin(115200); // begin listening for serial communication on this frequency
  Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));
  // Start the receiver and if not 2. parameter specified
  IrReceiver.begin(27, ENABLE_LED_FEEDBACK);
  Serial.println();
  xTaskCreatePinnedToCore(
      changeLEDs, /* Function to implement the task */
      "ChangeLEDs", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      5,  /* Priority of the task */
      &changeLED,  /* Task handle. */
      1); /* Core where the task should run */ delay(500);

  xTaskCreatePinnedToCore(
      readIR, /* Function to implement the task */
      "ReadIR", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      5,  /* Priority of the task */
      &readIRs,  /* Task handle. */
      0); /* Core where the task should run */
  delay(500);
  
  
}

void loop() {
  vTaskDelete(NULL);
}

//void readIR(void * pvParameters){
//  while(true){
//    Serial.print("readIR running on core ");
//    Serial.println(xPortGetCoreID());
//    //IRdetect();
//  }
//}
