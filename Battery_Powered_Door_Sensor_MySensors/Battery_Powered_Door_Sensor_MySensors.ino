// Enable debug prints to serial monitor
#define MY_DEBUG 

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

//#include <SPI.h>
#include <MySensors.h>
#include <Bounce2.h>
#include <Time.h>
#include <TimeAlarms.h>

#define CHILD_ID 3
#define BUTTON_PIN  3  // Arduino Digital I/O pin for button/reed switch

int BATTERY_SENSE_PIN = A0;  // select the input pin for the battery sense point
int oldBatteryPcnt = 0;

Bounce debouncer = Bounce(); 
int oldValue=-1;

// Change to V_LIGHT if you use S_LIGHT in presentation below
MyMessage msg(CHILD_ID,V_TRIPPED);

void setup()  
{  
  // Setup the button
  pinMode(BUTTON_PIN,INPUT);
  // Activate internal pull-up
  digitalWrite(BUTTON_PIN,HIGH);

  // After setting up the button, setup debouncer
  debouncer.attach(BUTTON_PIN);
  debouncer.interval(5);

    // use the 1.1 V internal reference
  #if defined(__AVR_ATmega2560__)
    analogReference(INTERNAL1V1);
  #else
    analogReference(INTERNAL);
  #endif

  Alarm.timerRepeat(720, updateState); // update relay status every 2 hrs
  Alarm.delay(250);
}

void presentation() {
  // Register binary input sensor to gw (they will be created as child devices)
  // You can use S_DOOR, S_MOTION or S_LIGHT here depending on your usage. 
  // If S_LIGHT is used, remember to update variable type you send in. See "msg" above.
  present(CHILD_ID, S_DOOR);
  // Send the sketch version information to the gateway and Controller
  sendSketchInfo("Battery Powered Door Sensor", "1.0");  
}


//  Check if digital input has changed and send in new value
void loop() 
{
  debouncer.update();
  // Get the update value
  int value = debouncer.read();
    
    // get the battery Voltage
    int sensorValue = analogRead(BATTERY_SENSE_PIN);
    // #ifdef MY_DEBUG
    //  Serial.println(sensorValue);
    // #endif

    // 1M, 470K divider across battery and using internal ADC ref of 1.1V
    // Sense point is bypassed with 0.1 uF cap to reduce noise at that point
    // ((1e6+470e3)/470e3)*1.1 = Vmax = 3.44 Volts
    // 3.44/1023 = Volts per bit = 0.003363075

    int batteryPcnt = sensorValue / 10;

    //#ifdef MY_DEBUG
      //float batteryV  = sensorValue * 0.003363075;
      //Serial.print("Battery Voltage: ");
      //Serial.print(batteryV);
      //Serial.println(" V");

      //Serial.print("Battery percent: ");
      //Serial.print(batteryPcnt);
      //Serial.println(" %");
    //#endif

    if (oldBatteryPcnt != batteryPcnt) {
       // Power up radio after sleep
       sendBatteryLevel(batteryPcnt);
       oldBatteryPcnt = batteryPcnt;
    }
    
  if (value != oldValue) {
     // Send in the new value
     send(msg.set(value==HIGH ? 1 : 0));
     // sendBatteryLevel(batteryPcnt);
     oldValue = value;
  }
}

void updateState(){
    Serial.println("Start state info");
    Serial.print("sending update for switch: ");
    Serial.println(BUTTON_PIN);
    present(CHILD_ID, S_DOOR);
    Alarm.delay(250);
    //MyMessage msg(relayPin[pin],V_LIGHT);
    send(msg.set(HIGH), false); // Send last state from eprom to GW
    Alarm.delay(250);
  }

// This is called when a new time value was received
void receiveTime(unsigned long time) {
    setTime(time);
}
