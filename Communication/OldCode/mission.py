import dronekit as dk
import dronekit_sitl as dks
from mismeth import *
import pymavlink as mav

# WP 0 52.815022, -4.127964
sitl = dks.start_default(52.815022, -4.127964)
home = dk.LocationGlobal(52.815022, -4.127964, 590)
connection_string = sitl.connection_string()

vehicle = dk.connect(connection_string,wait_ready=True)

cmds = vehicle.commands
cmds.download()
cmds.wait_ready()


############## Send confirmation to air pi #####################################


################################################################################

vehicle.mode = dk.VehicleMode("GUIDED")

alt = 7
#vehicle.airspeed = 10
vehicle.groundspeed = 15



#52.814406-4.129175
#loc1 = get_location_metres(vehicle.location.global_frame,50,50)
loc1 = dk.LocationGlobalRelative(52.814406, -4.129175, alt)
loc2 = dk.LocationGlobalRelative(52.811242, -4.128031, alt)
loc3 = dk.LocationGlobalRelative(52.811078, -4.125119, alt)
loc4 = dk.LocationGlobalRelative(52.81316, -4.121333, alt)
loc5 = dk.LocationGlobalRelative(52.814772, -4.120708, alt)
loc6 = dk.LocationGlobalRelative(52.815950, -4.121686, alt)
loc7 = dk.LocationGlobalRelative(52.815850, -4.125336, alt)
loc8 = dk.LocationGlobalRelative(52.815822, -4.126181, alt)
loc9 = dk.LocationGlobalRelative(52.814656, -4.126853, alt)
loc10 = dk.LocationGlobalRelative(52.813269, -4.125403, alt)
loc11 = dk.LocationGlobalRelative(52.813547, -4.128811, alt)

###### TASK 1 #######
arm_takeoff(vehicle, alt)

vehicle.simple_goto(loc1)
stay_in_range(vehicle, loc1)

####### TASK 2 AND TASK 3 ########
vehicle.simple_goto(loc2)
stay_in_range(vehicle, loc2)

vehicle.simple_goto(loc3)
stay_in_range(vehicle, loc3)

vehicle.simple_goto(loc4)
stay_in_range(vehicle, loc4)

vehicle.simple_goto(loc5)
stay_in_range(vehicle, loc5)

vehicle.simple_goto(loc6)
stay_in_range(vehicle, loc6)

###### Task 4 ######
vehicle.simple_goto(loc7)
stay_in_range(vehicle, loc7)

time.sleep(1)

vehicle.mode = dk.VehicleMode("LOITER")

############################## RELEASE PACKAGE ####################

msg = vehicle.message_factory.command_long_encode(0, 0,  # target_system, target_component
                                                  mav.mavutil.mavlink.MAV_CMD_DO_SET_SERVO,  # command
                                                  0,  # confirmation
                                                  1,  # servo number
                                                  988, # servo pos between 1000 and 2000
                                                  0, 0, 0, 0, 0)   # Not used (params 3 -7)

vehicle.send_mavlink(msg)

time.sleep(5)

###################################################################

vehicle.mode = dk.VehicleMode("GUIDED")

vehicle.simple_goto(loc8)
stay_in_range(vehicle, loc8)

#### TASK 6 ####
#vehicle.airspeed = 25
vehicle.groundspeed = 35

vehicle.simple_goto(loc9)
stay_in_range(vehicle, loc9)

vehicle.simple_goto(loc10)
stay_in_range(vehicle, loc10)

vehicle.simple_goto(loc4)
stay_in_range(vehicle, loc4)

vehicle.simple_goto(loc5)
stay_in_range(vehicle, loc5)

vehicle.simple_goto(loc6)
stay_in_range(vehicle, loc6)

vehicle.simple_goto(loc8)
stay_in_range(vehicle, loc8)

#vehicle.airspeed = 10
vehicle.groundspeed = 15

vehicle.simple_goto(home)
stay_in_range(vehicle, home)

##### TASK 5 #####
time.sleep(1)

vehicle.mode = dk.VehicleMode("LAND")

while vehicle.armed:
    time.sleep(1)

arm_takeoff(vehicle, alt)

vehicle.mode = dk.VehicleMode("LAND")


while vehicle.armed:
    time.sleep(1)

vehicle.close()

sitl.stop()
