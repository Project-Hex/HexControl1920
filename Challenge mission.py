
#This program runs a simulation of a plane taking off, going to some presets points, releasing a cargo and returning to launch to land.

from __future__ import print_function
import time
from dronekit import connect, VehicleMode, LocationGlobalRelative
import math
import pymavlink as mav

# Set up option parsing to get connection string
import argparse
parser = argparse.ArgumentParser(description='Commands vehicle using vehicle.simple_goto.')
parser.add_argument('--connect',
                    help="Vehicle connection target string. If not specified, SITL automatically started and used.")
args = parser.parse_args()

connection_string = args.connect
sitl = None



# Start SITL if no connection string specified
if not connection_string:
    import dronekit_sitl
    sitl = dronekit_sitl.start_default(52.780559, -0.707985)
    connection_string = sitl.connection_string()


# Connect to the Vehicle
print('Connecting to vehicle on: %s' % connection_string)
vehicle = connect(connection_string, wait_ready=True)


def arm_and_takeoff(aTargetAltitude):
    """
    Arms vehicle and fly to aTargetAltitude.
    """

    print("Basic pre-arm checks")
    # Don't try to arm until autopilot is ready
    while not vehicle.is_armable:
        print(" Waiting for vehicle to initialise...")
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

def within_range(loc1,loc2):

    dist = get_distance_metres(loc1,loc2)
    if dist > 1:
        return False
    return True

def release_package():

    msg = vehicle.message_factory.command_long_encode(0, 0,  # target_system, target_component
             mav.mavutil.mavlink.MAV_CMD_DO_SET_SERVO,  # command
             0,  # confirmation
             9,  # servo number
             3000, # servo pos between 1000 and 2000
             0, 0, 0, 0, 0)   # Not used (params 3 -7)

    vehicle.send_mavlink(msg)

    time.sleep(5)

    msg = vehicle.message_factory.command_long_encode(0, 0,  # target_system, target_component
             mav.mavutil.mavlink.MAV_CMD_DO_SET_SERVO,  # command
             0,  # confirmation
             9,  # servo number
             700, # servo pos between 1000 and 2000
             0, 0, 0, 0, 0)   # Not used (params 3 -7)

    vehicle.send_mavlink(msg)

def open_servo():

    msg = vehicle.message_factory.command_long_encode(0, 0,  # target_system, target_component
             mav.mavutil.mavlink.MAV_CMD_DO_SET_SERVO,  # command
             0,  # confirmation
             9,  # servo number
             3000, # servo pos between 1000 and 2000
             0, 0, 0, 0, 0)   # Not used (params 3 -7)

    vehicle.send_mavlink(msg)

def close_servo():

    msg = vehicle.message_factory.command_long_encode(0, 0,  # target_system, target_component
             mav.mavutil.mavlink.MAV_CMD_DO_SET_SERVO,  # command
             0,  # confirmation
             9,  # servo number
             700, # servo pos between 1000 and 2000
             0, 0, 0, 0, 0)   # Not used (params 3 -7)

    vehicle.send_mavlink(msg)


    
#Here the mission starts

arm_and_takeoff(20)


print("Set airspeed to 5")
vehicle.airspeed = 5


print("Going towards waypoint 1.")
point1 = LocationGlobalRelative(52.779838, -0.711133, 20)
vehicle.simple_goto(point1, groundspeed=10)

while not (within_range(vehicle.location.global_frame,point1)):
    time.sleep(1)

print("Going towards waypoint 2")
point2 = LocationGlobalRelative(52.780658, -0.713134, 20)
vehicle.simple_goto(point2, groundspeed=10)

while not (within_range(vehicle.location.global_frame,point2)):
    time.sleep(1)

print("Going towards waypoint 3")
point3 = LocationGlobalRelative(52.782598, -0.710752, 20)
vehicle.simple_goto(point3, groundspeed=10)

print("Going towards waypoint 4")
point3 = LocationGlobalRelative(52.784551, -0.709937, 20)
vehicle.simple_goto(point3, groundspeed=10)

print("Going towards waypoint 5")
point3 = LocationGlobalRelative(52.785167, -0.712351, 20)
vehicle.simple_goto(point3, groundspeed=10)

print("Going towards waypoint 6")
point3 = LocationGlobalRelative(52.783188, -0.714486, 20)
vehicle.simple_goto(point3, groundspeed=10)

print("Going towards waypoint 7")
point3 = LocationGlobalRelative(52.782552, -0.712984, 20)
vehicle.simple_goto(point3, groundspeed=10)

print("Going towards waypoint 8")
point3 = LocationGlobalRelative(52.782448, -0.707620, 20)
vehicle.simple_goto(point3, groundspeed=10)

print("Going towards waypoint 9")
point3 = LocationGlobalRelative(52.781565, -0.705893, 20)
vehicle.simple_goto(point3, groundspeed=10)

while not (within_range(vehicle.location.global_frame,point3)):
        time.sleep(1)

print("Returning to Launch")
vehicle.mode = VehicleMode("RTL")

Launch = LocationGlobalRelative(52.780559, -0.707985, 20)
while not (within_range(vehicle.location.global_frame,Launch)):
    time.sleep(1)

print("Landing")
while not(vehicle.location.global_relative_frame.alt < 0):
    time.sleep(1);    


# Close vehicle object before exiting script
print("Close vehicle object")
vehicle.close()

# Shut down simulator if it was started.
if sitl:
    sitl.stop()
