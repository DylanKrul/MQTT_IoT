#!/usr/bin/env python3
import paho.mqtt.client as mqtt
import datetime
import time
from Adafruit_IO import MQTTClient
from adafruit_credentials import ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY

#
# Subscribes to messages via a local MQTT broker
# and forwards them to a cloud service (Adafruit IO)
#

def on_connected(client, userdata, rc):
    print("Connected to local MQTT broker with result code "+str(rc))
    client.subscribe("Home/#")

def on_disconnected():
    print("Disconnected from local MQTT broker")
    client = mqtt.Client()

def on_message(client, userdata, msg):
    print(str(datetime.datetime.now()) + ": " + msg.topic + " " + str(msg.payload))
    # Forward the data to Adafruit IO. Replace topic with a valid feed name
    feedname=msg.topic.replace("/","_")
    print("Publish to Adafruit feedname: " + feedname)
    adafruitClient.publish(feedname,msg.payload)

# Initialize the client that should connect to the local MQTT broker
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

# Initialize the client that should connect to io.adafruit.com
adafruitClient = MQTTClient(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

def adafruit_disconnected(client):
    print("Disconnected from Adafruit IO")
    adafruit_connect(client)

def adafruit_connected(client):
    print("Connected to Adafruit IO")

def adafruit_connect(client):
    while(client.is_connected() == False):
        print("Trying connect to Adafruit IO")
        client.connect()
        # Run loop in a separate thread
        adafruitClient.loop_background()
        time.sleep(2)

adafruitClient.on_disconnect = adafruit_disconnected
adafruitClient.on_connect = adafruit_connected
adafruit_connect(adafruitClient)

# Blocking loop to the local Mosquitto broker
client.loop_forever()
