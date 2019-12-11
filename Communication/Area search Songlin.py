### Area search ###
##Songlin Ma 11/12/2019 ##

####Setting parameters ####

# takeoff point latitude and longitude
wp0lat= 52.780485 
wp0long= -0.708329

# waypoint1 latitude and longitude
wp1lat= 52.779731
wp1long= -0.711432

wp2lat= 52.780666
wp2long= -0.712913

wp3lat= 52.782561
wp3long= -0.707838

wp4lat= 52.781360
wp4long= -0.705993

#### simulation set ####

# Importing appropriate libraries 
from dronekit import connect,  VehicleMode, LocationGlobalRelative
import time
import math

# Set up option parsing to get connection string
import argparse
parser = argparse.ArgumentParser(description='commands')
parser.add_argument('--connect')
args = parser.parse_args()
connection_string = args.connect


# Start SITL if no connection string specified
if not connection_string:
    import dronekit_sitl
    sitl = dronekit_sitl.start_default(wp0lat, wp0long)
    connection_string = sitl.connection_string()
    
# Connect to the Vehicle    
print("connection to the vehicle on %s" %connection_string)
vehicle = connect(connection_string, wait_ready=True)

# Allow time for the mission planner to download parameters
time.sleep(10)

#### Define Functions ####

# Define the function for arm and takeoff
def arm_and_takeoff(aTargetAltitude):
    """
    Arms vehicle and fly to aTargetAltitude.
    """

    print("Basic pre-arm checks")
    # Don't try to arm until autopilot is ready
    while not vehicle.is_armable:
        print("Waiting for vehicle to initialise...")
        time.sleep(1)

    print("Arming motors")
    # Copter should arm in GUIDED mode
    vehicle.mode = VehicleMode("GUIDED")
    vehicle.armed = True

    # Confirm vehicle armed before attempting to take off
    while not vehicle.armed:
        print(" Waiting for arming...")
        time.sleep(1)

    print("Taking off!")
    vehicle.simple_takeoff(aTargetAltitude)  # Take off to target altitude

    # Wait until the vehicle reaches a safe height before processing the goto
    #  (otherwise the command after Vehicle.simple_takeoff will execute
    #   immediately).
    while True:
        print(" Altitude: ", vehicle.location.global_relative_frame.alt)
    
        # Break and return from function just below target altitude.
        if vehicle.location.global_relative_frame.alt >= aTargetAltitude * 0.95:
            print("Reached target altitude")
            break
        time.sleep(1)

# Define the function for translating from co-ordinate to distance        
def get_distance_metres(aLocation1, aLocation2):
    """
    Returns the ground distance in metres between two LocationGlobal objects.
    This method is an approximation, and will not be accurate over large distances and close to the 
    earth's poles. It comes from the ArduPilot test code: 
    https://github.com/diydrones/ardupilot/blob/master/Tools/autotest/common.py
    """
    dlat = aLocation2.lat - aLocation1.lat
    dlong = aLocation2.lon - aLocation1.lon
    return math.sqrt((dlat*dlat) + (dlong*dlong)) * 1.113195e5

# Define the function for reaching the waypoint
def within_range(loc1,loc2):

    dist = get_distance_metres(loc1,loc2)
    if dist > 1:
        return False
    return True



#### MAIN PROGRAM ####

# arm and takeoff to a altitude of 30 m        
arm_and_takeoff(30)

# set the default speed 15
vehicle.airspeed = 15

# Go to wp1
print("go to wp1")
wp1 = LocationGlobalRelative(wp1lat, wp1long, 30)
vehicle.simple_goto(wp1)

# Task1
while not (within_range(vehicle.location.global_frame,wp1)):
    time.sleep(1)

# Go to wp2
print("go to wp2")
wp2 = LocationGlobalRelative(wp2lat, wp2long, 30)
vehicle.simple_goto(wp2)

# Task2          
while not (within_range(vehicle.location.global_frame,wp2)):
    time.sleep(1)

# Go to wp3
print("go to wp3")
wp3 = LocationGlobalRelative(wp3lat, wp3long, 30)
vehicle.simple_goto(wp3)

# Task3          
while not (within_range(vehicle.location.global_frame,wp3)):
    time.sleep(1)

# Go to wp4
print("go to wp4")
wp4 = LocationGlobalRelative(wp4lat, wp4long, 30)
vehicle.simple_goto(wp4)

# Task4          
while not (within_range(vehicle.location.global_frame,wp4)):
    time.sleep(1)

# Coming back
print("Coming back")
vehicle.mode = VehicleMode("RTL")
wp0= LocationGlobalRelative(wp0lat,wp0long, 30)
while not (within_range(vehicle.location.global_frame,wp0)):
    time.sleep(1)
print("Landing")    
land= LocationGlobalRelative(wp0lat,wp0long, 0)
while not (within_range(vehicle.location.global_frame,land)):
    time.sleep(1)
          
# Close connection
print("Close vehicle object")
vehicle.close()

# Shut down simulator if it was started.
if sitl:
    sitl.stop()          
          

          
          



        
