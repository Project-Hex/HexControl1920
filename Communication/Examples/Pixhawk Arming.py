# -*- coding: utf-8 -*-
"""
Created on Tue Dec 10 21:40:49 2019

@author: Ahmad Abbas

My idea:
    
while not vehicle.armed:
    #  Arm the vehicle using the transmitter
    time.sleep(1)
    
"""

from dronekit import connect, VehicleMode
import time

connection_string = 'com11'  # replace with your port

vehicle = connect('com11', wait_ready=True,baud=9600)

vehicle.mode = VehicleMode("GUIDED")  # Change mode first
time.sleep(5)  # wait 5 seconds
print(vehicle.mode)
print("Arming motors")
vehicle.armed = True  # arm the vehicle, might be better to put this in a loop
time.sleep(3)
print(vehicle.armed)

vehicle.close()