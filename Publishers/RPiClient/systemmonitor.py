#!/usr/bin/env python3
import paho.mqtt.publish as publish
from subprocess import check_output
from re import findall
import psutil

def get_temp():
    temp = check_output(["vcgencmd","measure_temp"]).decode("UTF-8")
    return(findall("\d+\.\d+",temp)[0])

def get_disk_usage():
    return str(psutil.disk_usage('/').percent)

def get_memory_usage():
    return str(psutil.virtual_memory().percent)

def get_cpu_usage():
    return str(psutil.cpu_percent(interval=None))

def publish_message(topic, message):
    print("Publishing to MQTT topic: " + topic)
    print("Message: " + message)

    publish.single(topic, message, hostname="192.168.1.16")

publish_message("Home/RPI3/Temp", get_temp())
publish_message("Home/RPI3/DiskUsagePercent", get_disk_usage())
publish_message("Home/RPI3/MemoryUsagePercent", get_memory_usage())
publish_message("Home/RPI3/CpuUsagePercent", get_cpu_usage())
