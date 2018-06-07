#define MY_NODE_ID 1
// Enable debug prints to serial monitor
#define MY_DEBUG 

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

// Set LOW transmit power level as default
//#define MY_RF24_PA_LEVEL RF24_PA_LOW

//set how long to wait for transport ready in milliseconds
#define MY_TRANSPORT_WAIT_READY_MS 3000

//#include <SPI.h>
#include <MySensors.h>
#include <Bounce2.h>
#include <Time.h>
#include <TimeAlarms.h>
#include <Vcc.h>

#define CHILD_ID 3
#define BUTTON_PIN  3  // Arduino Digital I/O pin for button/reed switch
#define VCC_MIN 2.8
#define VCC_MAX 3.3


Bounce debouncer = Bounce(); 
int oldValue=-1;

// Change to V_LIGHT if you use S_LIGHT in presentation below
MyMessage msg(CHILD_ID,V_TRIPPED);
//MyMessage voltage_msg(CHILD_ID_BATTERY, V_VOLTAGE);

//battery voltage
const float VccExpected   = 3.0;
const float VccCorrection = 2.860/2.92;  // Measured Vcc by multimeter divided by reported Vcc
Vcc vcc(VccCorrection);

static int oldBatteryPcnt = 0;

void sendBatteryReport() {
          float p = vcc.Read_Perc(VCC_MIN, VCC_MAX, true);
          float voltage = vcc.Read_Volts() ;
          int batteryPcnt = static_cast<int>(p);
          sendBatteryLevel(batteryPcnt);
          Serial.print("Battery Percentage = ");    
          Serial.println(batteryPcnt);
}

//  Check if digital input has changed and send in new value
void loop() 
{
  debouncer.update();
  // Get the update value
  int value = debouncer.read();
  
  if (value != oldValue) {
     // Send in the new value
     send(msg.set(value==HIGH ? 1 : 0), true);
     sendBatteryReport();
     oldValue = value;
     delay(150);
  }
  //sleep(BUTTON_PIN-2, CHANGE, 0);
}

void updateState(){
    Serial.println("Start state info");
    Serial.print("sending update for switch: ");
    Serial.println(BUTTON_PIN);
    present(CHILD_ID, S_DOOR);
    Alarm.delay(500);
    send(msg.set(oldValue), true); // Send last state from eprom to GW
    Alarm.delay(500);
  }

void setup()  
{  
  // Setup the button
  pinMode(BUTTON_PIN,INPUT);
  // Activate internal pull-up
  digitalWrite(BUTTON_PIN,HIGH);

  // After setting up the button, setup debouncer
  debouncer.attach(BUTTON_PIN);
  debouncer.interval(5);

  Alarm.timerRepeat(720, updateState); // update relay status every 2 hrs
  Alarm.delay(250);
}

void presentation() {
  // Register binary input sensor to gw (they will be created as child devices)
  // You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage. 
  // If S_LIGHT is used, remember to update variable type you send in. See "msg" above.
  present(CHILD_ID, S_DOOR);
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Oprit Poort", "1.2");  
}

// This is called when a new time value was received
void receiveTime(unsigned long time) {
    setTime(time);
}
