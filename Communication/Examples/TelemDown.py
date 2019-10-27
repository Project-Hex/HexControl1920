"""
This script will start a simulator and get telemetry data from it
The connection string will follow the following pattern:
    protocoltype:address:port
    e.g: tcp:127.0.0.1:5770
To connect, go to mission planner, select the appropriate prototype rate, and
keep the baud rate at the default 9600, and click on connect.
Once prompted, enter the ip (then press ok), and then enter the port+3
    so if your port is at 5770 enter 5773
Now your mission planner will attempt to connect to the SITL.
"""
# Importing appropriate libraries 
import dronekit_sitl
import time
from dronekit import connect, VehicleMode

print "Start simulator (SITL)"
# Start the simulator using default settings
sitl = dronekit_sitl.start_default()
# The connection information such as type and address
connection_string = sitl.connection_string()

# Connect to the Vehicle.
print("Connecting to vehicle on: %s" % (connection_string,))
vehicle = connect(connection_string, wait_ready=True)

# Delay to give the mission planner time to download the paramaters
time.sleep(10) 

# Get some vehicle attributes (state)
print "Get some vehicle attribute values:"
print " GPS: %s" % vehicle.gps_0
print " Battery: %s" % vehicle.battery
print " Last Heartbeat: %s" % vehicle.last_heartbeat
print " Is Armable?: %s" % vehicle.is_armable
print " System status: %s" % vehicle.system_status.state
print " Mode: %s" % vehicle.mode.name    # settable

# Close vehicle object before exiting script
vehicle.close()

# Shut down simulator
sitl.stop()
print("Completed")