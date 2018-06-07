// Enable debug prints to serial monitor
#define MY_DEBUG 

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

#include <MySensors.h>
#include <SPI.h>

#define SKETCH_NAME "Bat-Pwrd Door Sensor"
#define SKETCH_MAJOR_VER "1"
#define SKETCH_MINOR_VER "1"

#define CHILD_ID 3
#define BUTTON_PIN 3       // Arduino Digital I/O pin for button/reed switch

unsigned long SLEEP_TIME = 1800000; // Sleep time between reports (in milliseconds)

// Change to V_LIGHT if you use S_LIGHT in presentation below
MyMessage msg(CHILD_ID, V_TRIPPED);

void setup()  
{  
  // Setup the buttons
  pinMode(BUTTON_PIN, INPUT);
  
  // Activate internal pull-ups
  digitalWrite(BUTTON_PIN, HIGH);
    
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo(SKETCH_NAME, SKETCH_MAJOR_VER"."SKETCH_MINOR_VER);

  // Register binary input sensor to gw (they will be created as child devices)
  // You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage. 
  // If S_LIGHT is used, remember to update variable type you send in. See "msg" above.
  present(CHILD_ID, S_DOOR);  
}

// Loop will iterate on changes on the BUTTON_PINs
void loop() 
{
  uint8_t value;
  static uint8_t sentValue=2;

  // Short delay to allow buttons to properly settle
  sleep(5);
  
  value = digitalRead(BUTTON_PIN);
  
  if (value != sentValue) {
     // Value has changed from last transmission, send the updated value
     send(msg.set(value==HIGH ? 1 : 0));
     sentValue = value;
  }

  // Sleep until something happens with the sensor
  sleep(BUTTON_PIN-2, CHANGE, SLEEP_TIME);
} 
