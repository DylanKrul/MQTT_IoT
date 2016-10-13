#!/usr/bin/env python3
import paho.mqtt.client as mqtt
import datetime
import time
from Adafruit_IO import MQTTClient

#
# Subscribes to messages via a local MQTT broker
# and forward them to a cloud service (Adafruit IO)
#

def on_connected(client, userdata, rc):
    print("Connected to local broker with result code "+str(rc))
    client.subscribe("Home/#")

def on_disconnected():
    print("Disconnected from local broker")

def on_message(client, userdata, msg):
    print(str(datetime.datetime.now()) + ": " + msg.topic + " " + str(msg.payload))
    # Forward the data to Adafruit IO. Replace topic with a valid feed name
    feedname=msg.topic.replace("/","_")
    print("Publish to Adafruit feedname: " + feedname)
    adafruitClient.publish(feedname,msg.payload)

# Initialize the client that should connect to the local Mosquitto broker
client = mqtt.Client()
client.on_connect = on_connected
client.on_disconnect = on_disconnected
client.on_message = on_message
connOK=False
while(connOK == False):
    try:
        client.connect("192.168.1.16", 1883, 60)
        connOK = True
    except:
        connOK = False
    time.sleep(2)


# Set to your Adafruit IO key and username
ADAFRUIT_IO_KEY      = '4c9f8157459a1597a500abfb74536032a160bf0a'
ADAFRUIT_IO_USERNAME = 'LarsBq'

# Initialize the client that should connect to io.adafruit.com
adafruitClient = MQTTClient(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

def adafruit_disconnected(client):
    client.connect()
    print("Disconnected from Adafruit IO")

def adafruit_connected(client):
    print("Connected to Adafruit IO")

adafruitClient.on_disconnect = adafruit_disconnected
adafruitClient.on_connect = adafruit_connected
adafruitClient.connect()
# Run loop in a separate thread
adafruitClient.loop_background()

# Blocking loop to the local Mosquitto broker
client.loop_forever()
