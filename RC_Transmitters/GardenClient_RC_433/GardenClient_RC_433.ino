#include <OneWire.h>
#include <DallasTemperature.h>
#include "RCSwitch.h"
#include <Narcoleptic.h>

//
// Data transmission setup
//
#define GARDEN_TEMP_ID 4
#define SOIL_TEMP_ID   5
#define TX_PIN 5
RCSwitch transmitter = RCSwitch();

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress insideThermometer, outsideThermometer;

void setup(void)
{
  // start serial port
  Serial.begin(9600);

  // Start up the library
  sensors.begin();

  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
  if (!sensors.getAddress(outsideThermometer, 1)) Serial.println("Unable to find address for Device 1"); 

  Serial.print("Device 0 Address: ");
  printAddress(insideThermometer);
  Serial.println();

  Serial.print("Device 1 Address: ");
  printAddress(outsideThermometer);
  Serial.println();

  // set the resolution to 9 bit
  sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(outsideThermometer, TEMPERATURE_PRECISION);

  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC); 
  Serial.println();

  Serial.print("Device 1 Resolution: ");
  Serial.print(sensors.getResolution(outsideThermometer), DEC); 
  Serial.println();

  transmitter.enableTransmit(TX_PIN); 
  transmitter.setRepeatTransmit(25);

}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}


// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
  Serial.print("Resolution: ");
  Serial.print(sensors.getResolution(deviceAddress));
  Serial.println();    
}


unsigned long seqNum=0;
void loop(void)
{ 
  sensors.requestTemperatures();

  float gardenTemp = sensors.getTempC(outsideThermometer);
  float soilTemp = sensors.getTempC(insideThermometer);
  Serial.print("Outside temp:");
  Serial.println(gardenTemp);
  Serial.print("Soil temp:");
  Serial.println(soilTemp);

  unsigned int encodedFloat = EncodeFloatToTwoBytes(gardenTemp);
  Serial.println(encodedFloat);
  unsigned long dataToSend = Code32BitsToSend(GARDEN_TEMP_ID,seqNum,encodedFloat);
  TransmitWithRepeat(dataToSend);

  encodedFloat = EncodeFloatToTwoBytes(soilTemp);
  Serial.println(encodedFloat);
  dataToSend = Code32BitsToSend(SOIL_TEMP_ID,seqNum,encodedFloat);
  TransmitWithRepeat(dataToSend);

  for (int i=0; i< 100; i++)
  {
    // Max narcoleptic delay is 8s
    Narcoleptic.delay(8000);
  }
  
  seqNum++;
  if (seqNum > 15)
  {
    seqNum = 0;
  } 
}

unsigned long Code32BitsToSend(int measurementTypeID, unsigned long seq, unsigned long data)
{
    unsigned long checkSum = measurementTypeID + seq + data;
    unsigned long byte3 = ((0x0F & measurementTypeID) << 4) + (0x0F & seq);
    unsigned long byte2_and_byte_1 = 0xFFFF & data;
    unsigned long byte0 = 0xFF & checkSum;
    unsigned long dataToSend = (byte3 << 24) + (byte2_and_byte_1 << 8) + byte0;

    return dataToSend;
}

// Encode a float as two bytes by multiplying with 100
// and reserving the highest bit as a sign flag
// Values that can be encoded correctly are between -327,67 and +327,67
unsigned int EncodeFloatToTwoBytes(float floatValue)
{
  bool sign = false;
  
  if (floatValue < 0) 
    sign=true;  
      
  int integer = (100*fabs(floatValue));
  unsigned int word = integer & 0XFFFF;
  
  if (sign)
    word |= 1 << 15;

  return word;
}

void TransmitWithRepeat(unsigned long dataToSend)
{
    transmitter.send(dataToSend, 32);
    Narcoleptic.delay(2000);
    transmitter.send(dataToSend, 32);
    Narcoleptic.delay(2000);
}


