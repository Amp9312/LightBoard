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
#define FAST_LED_STRIP_ONE_PIN 25
#define FAST_LED_STRIP_TWO_PIN 26
#define FAST_LED_STRIP_THREE_PIN 33
#define FAST_LED_STRIP_FOUR_PIN 32

CRGB leds1[NUM_LEDS_PER_STRIP];
CRGB leds2[NUM_LEDS_PER_STRIP];
CRGB leds3[NUM_LEDS_PER_STRIP];
CRGB leds4[NUM_LEDS_PER_STRIP];
decode_results results; // this is a variable to hold the received IR signal
unsigned long key_value = 0; // variable to store the key value
bool ON_SWITCH = false;
bool TRACK_ONE = false;
bool TRACK_TWO = false;
bool TRACK_THREE = false;
bool TRACK_FOUR = false;
bool CHANGED = false;
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
            Serial.println("Turned On");
            ON_SWITCH = true;
            TRACK_ONE = true;
        } else if (IrReceiver.decodedIRData.command == 0x47) {
            Serial.println("Turned Off");
            ON_SWITCH = false;
            TRACK_ONE = false;
            TRACK_TWO = false;
            TRACK_THREE = false;
            TRACK_FOUR = false;
            CHANGED = true;
        }else if(IrReceiver.decodedIRData.command == 0x9){
            SPEED = SPEED / 2;
            Serial.println("Delay has been halved, SPEED is now:");
            Serial.println(SPEED);
          }
          else if(IrReceiver.decodedIRData.command == 0x7){
            SPEED = SPEED * 2;
            Serial.println("Delay has been doubled, SPEED is now:");
            Serial.println(SPEED);
          }
          else if(IrReceiver.decodedIRData.command == 0x1c){
            Serial.println("Channel one has been turned on");
            TRACK_ONE = true;
            TRACK_TWO = false;
            TRACK_THREE = false;
            TRACK_FOUR = false;
            CHANGED = true;
          }
          else if(IrReceiver.decodedIRData.command == 0x8){
            Serial.println("Channel two has been turned on");
            TRACK_ONE = false;
            TRACK_TWO = true;
            TRACK_THREE = false;
            TRACK_FOUR = false;
            CHANGED = true;
          }
          else if(IrReceiver.decodedIRData.command == 0x5A){
            Serial.println("Channel three has been turned on");
            TRACK_ONE = false;
            TRACK_TWO = false;
            TRACK_THREE = true;
            TRACK_FOUR = false;
            CHANGED = true;
          }
          else if(IrReceiver.decodedIRData.command == 0x42){
            Serial.println("Channel four has been turned on");
            TRACK_ONE = false;
            TRACK_TWO = false;
            TRACK_THREE = false;
            TRACK_FOUR = true;
            CHANGED = true;
          }
    }
    vTaskDelay(100);
  }
}

void changeLEDs(void * pvParameters){
  Serial.print("changeLEDs running on core ");
  Serial.println(xPortGetCoreID());
  while(true){
  if(ON_SWITCH == true){
  // Code for Track One ---------------------------------------------------------------------

  if(TRACK_ONE){
  Serial.println("changeLEDs");
    Serial.println("ON 1");
      for(int i = 0; i < NUM_LEDS_PER_STRIP && !CHANGED; i++) 
      {
        // Green loop
        // set our current dot to Red
        leds1[i] = CRGB::Black;    
        FastLED.show();
        // clear our current dot before we move on
        leds1[i] = CRGB::Green;
        vTaskDelay(SPEED);
        if(CHANGED){
          CHANGED = false;
          blackout();
          goto pattern1;
        }
      }
      if(CHANGED){
          CHANGED = false;
          blackout();
          goto pattern1;
      }   
      Serial.println("ON 2");
      for(int i = NUM_LEDS_PER_STRIP-1; i >= 0 && !CHANGED; i--)
      {
        // set our current dot to White
        leds1[i] = CRGB::Black;
        FastLED.show();
        // clear our current dot before we move on
        leds1[i] = CRGB::Blue;
        vTaskDelay(SPEED);
        if(CHANGED){
          CHANGED = false;
          blackout();
          goto pattern1;
        }
      }
      pattern1:;
  }
      //end of turn off other tracks

    //end of track one---------------------------------------------------------------------

    //start of track two---------------------------------------------------------------------
    if(TRACK_TWO){
    Serial.println("changeLEDs");  
    Serial.println("ON 1");
      for(int i = 0; i < NUM_LEDS_PER_STRIP && !CHANGED; i++) 
      {
        // Green loop
        // set our current dot to Red
        leds2[i] = CRGB::Black;    
        FastLED.show();
        // clear our current dot before we move on
        leds2[i] = CRGB::Green;
        vTaskDelay(SPEED);
        if(CHANGED){
          CHANGED = false;
          blackout();
          goto pattern2;
        }
      }
      if(CHANGED){
          CHANGED = false;
          blackout();
          goto pattern2;
      }
      Serial.println("ON 2");
      for(int i = NUM_LEDS_PER_STRIP-1; i >= 0 && !CHANGED; i--)
      {
        // set our current dot to White
        leds2[i] = CRGB::Black;
        FastLED.show();
        // clear our current dot before we move on
        leds2[i] = CRGB::Blue;
        vTaskDelay(SPEED);
        if(CHANGED){
          CHANGED = false;
          blackout();
          goto pattern2;
        }
      }
      pattern2:;
    }
//End of track two---------------------------------------------------------------------

//start of track three---------------------------------------------------------------------
  if(TRACK_THREE){
    Serial.println("TRACK THREE1");
    Serial.println("ON 1");
      for(int i = 0; i < NUM_LEDS_PER_STRIP && !CHANGED; i++) 
      {
        // Green loop
        // set our current dot to Red
        leds3[i] = CRGB::Black;    
        FastLED.show();
        // clear our current dot before we move on
        leds3[i] = CRGB::Green;
        vTaskDelay(SPEED);
        if(CHANGED){
          CHANGED = false;
          blackout();
          goto pattern3;
        }
      }
      Serial.println("TRACK THREE2");
      if(CHANGED){
          CHANGED = false;
          blackout();
          goto pattern3;
      }
      Serial.println("TRACK THREE3");
      Serial.println("ON 2");
      for(int i = 0; i < NUM_LEDS_PER_STRIP && !CHANGED; i++)
      {
        // set our current dot to White
        leds3[i] = CRGB::Black;
        FastLED.show();
        // clear our current dot before we move on
        leds3[i] = CRGB::Blue;
        vTaskDelay(SPEED);
        if(CHANGED){
          CHANGED = false;
          blackout();
          goto pattern3;
        }
      }
      Serial.println("TRACK THREE4");
      pattern3:;
  }
    //end of track three---------------------------------------------------------------------
    
    //start of track four---------------------------------------------------------------------
    if(TRACK_FOUR){
      Serial.println("TRACK FOUR1");
      Serial.println("ON 1");
      for(int i = 0; i < NUM_LEDS_PER_STRIP && !CHANGED; i++) 
      {
        // Green loop
        // set our current dot to Red
        leds4[i] = CRGB::Black;    
        FastLED.show();
        // clear our current dot before we move on
        leds4[i] = CRGB::Green;
        vTaskDelay(SPEED);
        if(CHANGED){
          CHANGED = false;
          blackout();
          goto pattern4;
        }
      }
      Serial.println("TRACK FOUR2");
      if(CHANGED){
          CHANGED = false;
          blackout();
          goto pattern4;
      }
      Serial.println("TRACK FOUR3");
      Serial.println("ON 2");
      for(int i = 0; i < NUM_LEDS_PER_STRIP && !CHANGED; i++)
      {
        // set our current dot to White
        leds4[i] = CRGB::Black;
        FastLED.show();
        // clear our current dot before we move on
        leds4[i] = CRGB::Blue;
        vTaskDelay(SPEED);
        if(CHANGED){
          CHANGED = false;
          blackout();
          goto pattern4;
        }
      }
      pattern4:;
      Serial.println("TRACK FOUR4");
    }
    //end of track four---------------------------------------------------------------------
    
    //end of while loop
  }else{ //end of on switch if statement
      Serial.println("OFF");
      for(int i = NUM_LEDS_PER_STRIP-1; i >= 0; i--) {
        // set our current dot to White
        leds1[i] = CRGB::Black;
        leds2[i] = CRGB::Black;
        leds3[i] = CRGB::Black;
        leds4[i] = CRGB::Black;
        FastLED.show();
    }
  }
  }
}

void blackout(){
  Serial.println("OFF");
  for(int i = NUM_LEDS_PER_STRIP-1; i >= 0; i--) {
    // set our current dot to White
    leds1[i] = CRGB::Black;
    leds2[i] = CRGB::Black;
    leds3[i] = CRGB::Black;
    leds4[i] = CRGB::Black;
  }
  FastLED.show();
  vTaskDelay(10);
}

void disableWiFi(){
        adc_power_off();
    WiFi.disconnect(true);  // Disconnect from the network
    WiFi.mode(WIFI_OFF);    // Switch WiFi off
    btStop();
}

// For mirroring strips, all the "special" stuff happens just in setup.  We
// just addleds multiple times, once for each strip

void setup() {
  disableWiFi();
  FastLED.addLeds<NEOPIXEL, FAST_LED_STRIP_ONE_PIN>(leds1, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, FAST_LED_STRIP_TWO_PIN>(leds2, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, FAST_LED_STRIP_THREE_PIN>(leds3, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, FAST_LED_STRIP_FOUR_PIN>(leds4, NUM_LEDS_PER_STRIP);
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
