//
// This is an example on MQTT publish from an ESP8266 board
// to an MQTT broker (I have used a local Mosquitto running on a Raspberry Pi)
// This example uses the PubSub client library (https://github.com/knolleary/pubsubclient)
// Install it in the Arduino IDE before compiling the sketch
// Sensor values are fetched from an indoor DHT22 sensor and outdoor DHT22 sensor


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "WIFI_and_broker_parameters.h"

//
// WIFI and MQTT setup
//
#define CLIENT_NAME "GarageClient"
WiFiClient wifiClient;
PubSubClient mqttClient(BROKER_IP,BROKER_PORT,wifiClient);

//
// Sensor setup
//
#include <DHT.h>
#define DHTPIN_OUTDOOR 4//12
#define DHTTYPE_OUTDOOR DHT22
DHT dht_outdoor(DHTPIN_OUTDOOR, DHTTYPE_OUTDOOR);
#define DHTPIN_INDOOR 5//13
#define DHTTYPE_INDOOR DHT22
DHT dht_indoor(DHTPIN_INDOOR, DHTTYPE_INDOOR);



void setup() 
{
  Serial.begin(9600);
  
  WiFi.begin(WLAN_SSID, WLAN_PASS);

  connectToWiFiAndBroker();

  dht_outdoor.begin();
  dht_indoor.begin();
}

#define SECONDS_BETWEEN_MEASUREMENTS 30
unsigned long lastTime = 0;

void loop() 
{
  if (!mqttClient.connected()) 
  {
    connectToWiFiAndBroker();
  }

  mqttClient.loop();
  
  if(millis() - lastTime > SECONDS_BETWEEN_MEASUREMENTS*1000) 
  {
    lastTime = millis();

    float h_outdoor = dht_outdoor.readHumidity();
    float t_outdoor = dht_outdoor.readTemperature();
    float h_indoor = dht_indoor.readHumidity();
    float t_indoor = dht_indoor.readTemperature();

    publishFloatValue(h_outdoor,"Home/Outdoor/Humidity");
    publishFloatValue(t_outdoor,"Home/Outdoor/Temperature");
    publishFloatValue(h_indoor,"Home/Garage/Humidity");
    publishFloatValue(t_indoor,"Home/Garage/Temperature");
  }
}

void connectToWiFiAndBroker() 
{
  Serial.print("Connecting to WIFI");
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected to WIFI!");

  Serial.println("Connecting to broker");
  while (!mqttClient.connect(CLIENT_NAME)) 
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected to broker!");
}

char msg[50];
void publishFloatValue(float value, char* topic)
{
    if (isnan(value)) 
    {
      Serial.println("Invalid value!");
      return;
    }

    Serial.println("Publishing a new value");
    ftoa(msg,value);
    Serial.println(msg);
    mqttClient.publish(topic, msg);
}

char *ftoa(char *buffer, float f)
{ 
  char *returnString = buffer;
  long integerPart = (long)f;
  itoa(integerPart, buffer, 10);
  while (*buffer != '\0') buffer++;
  *buffer++ = '.';
  long decimalPart = abs((long)((f - integerPart) * 100));
  itoa(decimalPart, buffer, 10);
  return returnString;
}

