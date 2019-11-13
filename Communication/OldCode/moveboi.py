import dronekit as dk
import dronekit_sitl as dks
import time
import math
import pymavlink as mav


sitl = dks.start_default()
connection_string = sitl.connection_string()
# connection_string = "127.0.0.1"

vehicle = dk.connect(connection_string,wait_ready=True)


def get_location_metres(original_location, dNorth, dEast):
    """
    Returns a LocationGlobal object containing the latitude/longitude `dNorth` and `dEast` metres from the 
    specified `original_location`. The returned Location has the same `alt` value
    as `original_location`.

    The function is useful when you want to move the vehicle around specifying locations relative to 
    the current vehicle position.
    The algorithm is relatively accurate over small distances (10m within 1km) except close to the poles.
    For more information see:
    http://gis.stackexchange.com/questions/2951/algorithm-for-offsetting-a-latitude-longitude-by-some-amount-of-meters
    """
    earth_radius=6378137.0 #Radius of "spherical" earth
    #Coordinate offsets in radians
    dLat = dNorth/earth_radius
    dLon = dEast/(earth_radius*math.cos(math.pi*original_location.lat/180))

    #New position in decimal degrees
    newlat = original_location.lat + (dLat * 180/math.pi)
    newlon = original_location.lon + (dLon * 180/math.pi)

    return dk.LocationGlobal(newlat, newlon,original_location.alt)


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
    print(dist) 
    if dist > 5:
        return False
    return True


def stay_in_range(loc):

    while not within_range(vehicle.location.global_frame,loc):
                     
        time.sleep(1)


def arm_takeoff(tar):

    while not vehicle.is_armable:
        time.sleep(1)

    vehicle.mode = dk.VehicleMode("GUIDED")
    vehicle.armed = True

    while not vehicle.armed:
        time.sleep(1)

    print "Taking off"

    vehicle.simple_takeoff(tar)

    while True:
        if vehicle.location.global_relative_frame.alt >= tar*0.95:
            break
        time.sleep(1)


def send_ned_velocity(velocity_x, velocity_y, velocity_z, duration):
    """
    Move vehicle in direction based on specified velocity vectors.
    """
    msg = vehicle.message_factory.set_position_target_local_ned_encode(
        0,       # time_boot_ms (not used)
        0, 0,    # target system, target component
        mav.mavutil.mavlink.MAV_FRAME_LOCAL_NED, # frame
        0b0000111111000111, # type_mask (only speeds enabled)
        0, 0, 0, # x, y, z positions (not used)
        velocity_x, velocity_y, velocity_z, # x, y, z velocity in m/s
        0, 0, 0, # x, y, z acceleration (not supported yet, ignored in GCS_Mavlink)
        0, 0)    # yaw, yaw_rate (not supported yet, ignored in GCS_Mavlink)


    # send command to vehicle on 1 Hz cycle
    for x in range(0,duration):
        vehicle.send_mavlink(msg)
        time.sleep(1)


cmds = vehicle.commands

cmds.download()

cmds.wait_ready()

# cmd1 = dk.Command(0, 0, 0, mav.mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mav.mavutil.mavlink.MAV_CMD_NAV_TAKEOFF, 0, 0, 0, 0, 0, 0, 0, 0, 10)
# cmd2 = dk.Command(0, 0, 0, mav.mavutil.mavlink.MAV_FRAME_GLOBAL_RELATIVE_ALT, mav.mavutil.mavlink.MAV_CMD_NAV_WAYPOINT, 0, 0, 0, 0, 0, 0, 10, 10, 10)

# cmds.add(cmd1)
# cmds.add(cmd2)
# cmds.upload()

# vehicle.mode = dk.VehicleMode("AUTO")
vehicle.mode = dk.VehicleMode("GUIDED")


# loc = dk.LocationGlobalRelative(-34.364114, 149.166022, 30)
# loc = dk.LocationGlobalRelative(-34.3632610, 149.1652279, 30)


arm_takeoff(30)

time.sleep(2)
origloc = vehicle.location.global_frame

loc = get_location_metres(vehicle.location.global_frame,50,50)

print(loc)
# send_ned_velocity(-2,0,-0.5,10)

# time.sleep(3)

vehicle.airspeed = 5

vehicle.groundspeed = 10

vehicle.simple_goto(loc,groundspeed=10)

stay_in_range(loc)

print get_distance_metres(origloc, vehicle.location.global_frame)

loc2 = get_location_metres(vehicle.location.global_frame,-50,50)
# loc2 = dk.LocationGlobal(-35.362404, 149.164620, 30)

vehicle.simple_goto(loc2,groundspeed=10)

stay_in_range(loc2)

print get_distance_metres(origloc, vehicle.location.global_frame)

vehicle.mode = dk.VehicleMode("LAND")

time.sleep(10)

vehicle.close()

sitl.stop()
