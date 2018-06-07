//#define MY_NODE_ID 99
// Enable debug prints to serial monitor
#define MY_DEBUG 

// Enable and select radio type attached
#define MY_RADIO_NRF24

//set how long to wait for transport ready in milliseconds
#define MY_TRANSPORT_WAIT_READY_MS 3000

#include <MySensors.h>
#include <Time.h>
#include <TimeAlarms.h>
#include <Vcc.h>

#define CHILD_ID 3
#define BUTTON_PIN  3  // Arduino Digital I/O pin for button/reed switch
#define VCC_MIN 2.8
#define VCC_MAX 3.3

uint32_t SLEEP_TIME = 900000; //in msec, so 15 minutes

int oldValue=-1;
int value=-1;

MyMessage msg(CHILD_ID,V_TRIPPED);

//battery voltage
const float VccExpected   = 3.0;
const float VccCorrection = 2.860/2.92;  // Measured Vcc by multimeter divided by reported Vcc
Vcc vcc(VccCorrection);

static int oldBatteryPcnt = 0;

// This is called when a new time value was received
void receiveTime(unsigned long time)
{
  setTime(time);
  Serial.print("Time = ");
  Serial.println(time);
}

void sendBatteryReport()
{
  float p = vcc.Read_Perc(VCC_MIN, VCC_MAX, true);
  float voltage = vcc.Read_Volts() ;
  int batteryPcnt = static_cast<int>(p);
  sendBatteryLevel(batteryPcnt);
  Serial.print("Battery Percentage = ");    
  Serial.println(batteryPcnt);
}

void resend(MyMessage &msg, int repeats) // Resend messages if not received by GW
{
  int repeat = 1;
  int repeatDelay = 0;

  while ((!send(msg)) and (repeat < repeats)) {
      repeatDelay += 250;
      repeat++;
      delay(repeatDelay);
    }    
}

//  Check if digital input has changed and send in new value
void loop() 
{
  uint8_t value;
  static uint8_t oldValue=2;

  // Short delay to allow buttons to properly settle
  sleep(5);

  value = digitalRead(BUTTON_PIN);
  
  if (value != oldValue) {
     // Send in the new value
     resend(msg.set(value==HIGH ? 1 : 0), 5);
     sendBatteryReport();
     oldValue = value;
     delay(500);
  }
  sleep(BUTTON_PIN-2, CHANGE, SLEEP_TIME);
  resend(msg.set(value==HIGH ? 1 : 0), 5);
  sendBatteryReport();
}

void setup()  
{  
  // Setup the button
  pinMode(BUTTON_PIN,INPUT);
  // Activate internal pull-up
  digitalWrite(BUTTON_PIN,HIGH);
  // Request time
  requestTime(receiveTime);
  //Alarm.timerRepeat(10, updateState); // update relay status every 5 mins
  Alarm.delay(250);
}

void presentation()
{
  // Register binary input sensor to gw (they will be created as child devices)
  // You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage. 
  // If S_LIGHT is used, remember to update variable type you send in. See "msg" above.
  present(CHILD_ID, S_DOOR);
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Oprit Poort", "1.3");  
}
