import dronekit as dk
import time
import dronekit_sitl as dks
from mismeth import *
import pymavlink as mav
sitl = dks.start_default(52.9241747,-4.0542528)
connection_string = sitl.connection_string()

vehicle = dk.connect(connection_string,wait_ready=True)

cmds = vehicle.commands
cmds.download()
cmds.wait_ready()
alt = 4

loc1 = dk.LocationGlobalRelative(52.9243403, -4.0539125, alt)
#loc1 = get_location_metres(vehicle.location.global_frame,50,50)
loc2 = dk.LocationGlobalRelative(52.924303,-4.054105, alt)
loc3 = dk.LocationGlobalRelative(52.924393, -4.0539249, alt)
loc4 = dk.LocationGlobalRelative(52.9244998, -4.0537784, alt)

vehicle.groundspeed = 15

arm_takeoff(vehicle, alt)

vehicle.simple_goto(loc2)
stay_in_range(vehicle, loc2)

####### TASK 2 AND TASK 3 ########
vehicle.simple_goto(loc3)
stay_in_range(vehicle, loc3)

vehicle.simple_goto(loc4)
stay_in_range(vehicle, loc4)

vehicle.simple_goto(loc1)
stay_in_range(vehicle, loc1)

vehicle.simple_goto(loc2)
stay_in_range(vehicle, loc2)

vehicle.mode = dk.VehicleMode("LAND")

vehicle.close()
