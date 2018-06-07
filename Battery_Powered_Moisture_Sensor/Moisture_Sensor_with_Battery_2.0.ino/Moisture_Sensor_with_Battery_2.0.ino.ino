#define MY_NODE_ID 1
// Enable debug prints to serial monitor
#define MY_DEBUG 

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

#include <SPI.h>
#include <MySensors.h>
#include <Vcc.h>

#define round(x) ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define N_ELEMENTS(array) (sizeof(array)/sizeof((array)[0]))

#define CHILD_ID_MOISTURE 0
#define SLEEP_TIME 900000 // Sleep time between reads (in milliseconds)
#define STABILIZATION_TIME 1000 // Let the sensor stabilize before reading
#define VCC_MIN 2.8
#define VCC_MAX 3.3

int SENSOR_ANALOG_PIN = A0;
int soilPower = 3; //Variable for Soil moisture Power

MyMessage msg(CHILD_ID_MOISTURE, V_HUM);
byte direction = 0;
const float VccExpected   = 3.0;
const float VccCorrection = 2.860/2.92;  // Measured Vcc by multimeter divided by reported Vcc
Vcc vcc(VccCorrection);

static int oldBatteryPcnt = 0;
int oldMoistureLevel = -1;

void sendBatteryReport() {
          float p = vcc.Read_Perc(VCC_MIN, VCC_MAX, true);
          float voltage = vcc.Read_Volts() ;
          int batteryPcnt = static_cast<int>(p);
          sendBatteryLevel(batteryPcnt);
          Serial.print("Battery Percentage = ");    
          Serial.println(batteryPcnt);
}

void setup()
{
  sendSketchInfo("Moisture Sensor, bat-pwrd", "2.0");

  present(CHILD_ID_MOISTURE, S_HUM);
  delay(250);
  pinMode(soilPower, OUTPUT);
  digitalWrite(soilPower, LOW);
}

void loop()
{
  digitalWrite(soilPower, HIGH);
  delay(10);
  analogRead(SENSOR_ANALOG_PIN);// Read once to let the ADC capacitor start charging
  sleep(STABILIZATION_TIME);
  int moistureLevel = (1023 - analogRead(SENSOR_ANALOG_PIN));

  // Turn off the sensor to conserve battery and minimize corrosion
  digitalWrite(soilPower, LOW);

  direction = (direction + 1) % 2; // Make direction alternate between 0 and 1 to reverse polarity which reduces corrosion
  // Always send moisture information so the controller sees that the node is alive

  // Send rolling average of 2 samples to get rid of the "ripple" produced by different resistance in the internal pull-up resistors
  // See http://forum.mysensors.org/topic/2147/office-plant-monitoring/55 for more information
  if (oldMoistureLevel == -1) { // First reading, save value
    oldMoistureLevel = moistureLevel;
  }
  Serial.print("Soil Moisture = ");    
  Serial.println(moistureLevel);
  
  send(msg.set((moistureLevel + oldMoistureLevel +  0.5) / 2 / 4.885, 1));
  sendBatteryReport();
  oldMoistureLevel = moistureLevel;

  sleep(SLEEP_TIME);
}
