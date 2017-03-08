//
// An Arduino sketch for an IoT node that sends sensor values vith 433 MHz radio
// The RCSwitch library is used for the transmitions
// The Narcopleptic library is used for power save during delay
// Sensor values are fetched from an BPM180/085 sensor via i2C
// 

#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Sensor433.h>
#include <Narcoleptic.h>

//
// Sensor setup
// The BMP085 module measures air pressure and temperature and operates via i2C
//
Adafruit_BMP085 bmp; // pin 4, SDA (data), pin 5, SDC (clock)

//
// Data transmission setup
//
#define CLIENT_NAME "TopFloorClient"
#define TX_PIN 12                     // PWM output pin to use for transmission
#define TOPFLOOR_TEMP_ID    1
#define BMP_PRESSURE_ID     2
Sensor433::Transmitter transmitter = Sensor433::Transmitter(TX_PIN);

void setup() 
{
  Serial.begin(9600);
  
  bmp.begin();
}

void loop() 
{
  float temp = bmp.readTemperature();
  
  Serial.print("Temperature = ");
  Serial.print(temp);
  Serial.println(" *C");
  transmitter.sendFloat(TOPFLOOR_TEMP_ID, temp);

  float pressure = bmp.readPressure();
  Serial.print("Pressure = ");
  Serial.print(pressure);
  Serial.println(" Pa");

  unsigned int pressureInt = pressure/100;
  Serial.print("Pressure Int = ");
  Serial.println(pressureInt);
  transmitter.sendWord(BMP_PRESSURE_ID, pressureInt);

  for (int i=0; i< 100; i++)
  {
    // Max narcoleptic delay is 8s
    Narcoleptic.delay(8000);
  }
}

