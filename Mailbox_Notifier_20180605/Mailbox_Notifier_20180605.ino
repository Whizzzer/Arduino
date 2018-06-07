// Enable and select radio type attached
#define MY_RADIO_NRF24

// Enable debug prints to serial monitor
#define MY_DEBUG 

#include <MySensors.h>
#include <SPI.h>
#include <readVcc.h>


#define NODE_ID 21                      // ID of node
#define PRIMARY_CHILD_ID 3                  
#define SECONDARY_CHILD_ID 4

#define MAILBOX_TOP_PIN 2             // Arduino Digital I/O pin for button/reed switch
#define MAILBOX_DOOR_PIN 3              // Arduino Digital I/O pin for button/reed switch

#if (MAILBOX_TOP_PIN < 2 || MAILBOX_TOP_PIN > 3)
#error MAILBOX_TOP_PIN must be either 2 or 3 for interrupts to work
#endif
#if (MAILBOX_DOOR_PIN < 2 || MAILBOX_DOOR_PIN > 3)
#error MAILBOX_DOOR_PIN must be either 2 or 3 for interrupts to work
#endif
#if (MAILBOX_TOP_PIN == MAILBOX_DOOR_PIN)
#error MAILBOX_TOP_PIN and BUTTON_PIN2 cannot be the same
#endif

#define MIN_V 2750 // empty voltage (0%)
#define MAX_V 4200 // full voltage (100%)

// Initialize motion message
MyMessage msg(PRIMARY_CHILD_ID, V_TRIPPED);
MyMessage msg2(SECONDARY_CHILD_ID, V_TRIPPED);

boolean post = false;
boolean lastpost = false;
int oldBatteryPcnt;
int repeat = 20;

void setup()
{
  // Setup the buttons
  pinMode(MAILBOX_TOP_PIN, INPUT);
  pinMode(MAILBOX_DOOR_PIN, INPUT);

  // Activate internal pull-ups
  //digitalWrite(MAILBOX_TOP_PIN, HIGH);
  //digitalWrite(MAILBOX_DOOR_PIN, HIGH);

  Serial.println("---------- set Mailbox empty");
  resend(msg.set(0), repeat);

}

void presentation()
{
  // Register binary input sensor to gw (they will be created as child devices)
  // You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage. 
  // If S_LIGHT is used, remember to update variable type you send in. See "msg" above.
  present(PRIMARY_CHILD_ID, S_DOOR);
  present(SECONDARY_CHILD_ID, S_DOOR);
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Mailbox Alert", "1.0");
}

// Loop will iterate on changes on the BUTTON_PINs
void loop()
{
  uint8_t value;
  static uint8_t sentValue = 2;
  static uint8_t sentValue2 = 2;

  // Short delay to allow buttons to properly settle
  sleep(5);

  value = digitalRead(MAILBOX_TOP_PIN);

  if (value != sentValue)
  {
    post = true;
    //Serial.println("---------- New Mail");
    resend(msg.set(value==HIGH ? 1 : 0),repeat);
    sentValue = value;
  }

  value = digitalRead(MAILBOX_DOOR_PIN);
  
  if (value != sentValue2)
  {
    post = false;
    //Serial.println("---------- Mailbox emptied");
    resend(msg2.set(value==HIGH ? 1 : 0),repeat);
    sentValue2 = value;
  }
  
  if (post != lastpost)
  {
    Serial.print("---------- Send Mailboxstate ");
    Serial.println(post ? "full" : "empty");
    resend((msg.set(post ? "1" : "0")),repeat);
    lastpost = post;
    sendBattery();
  }
  
  // Sleep until something happens with the sensor
  sleep(MAILBOX_TOP_PIN - 2, CHANGE, MAILBOX_DOOR_PIN - 2, CHANGE, 0);
}

void resend(MyMessage &msg, int repeats)
{
  int repeat = 1;
  int repeatdelay = 0;
  boolean sendOK = false;

  while ((sendOK == false) and (repeat < repeats)) {
    if (send(msg)) {
      sendOK = true;
    } else {
      sendOK = false;
      Serial.print("Send ERROR ");
      Serial.println(repeat);
      repeatdelay += 250;
    } repeat++; delay(repeatdelay);
  }
}

void sendBattery() // Measure battery
{
  int batteryPcnt = min(map(readVcc(), MIN_V, MAX_V, 0, 100), 100);
  if (batteryPcnt != oldBatteryPcnt) {
    sendBatteryLevel(batteryPcnt); // Send battery percentage
    oldBatteryPcnt = batteryPcnt;
  }
  Serial.print("---------- Battery: ");
  Serial.println(batteryPcnt);
}

