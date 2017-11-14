#!/bin/bash

#define parameters which are passed in.
ANS=$1
IP=$2
P=$3
I=$4
D=$5
FUNCTION1=$6
FUNCTION2=$7
ANS2=$8
COUNTER=0
MAX=2

case $ANS2 in
    y|Y) 
        if [ -c /dev/ttyACM0 ] && [ "$COUNTER" -lt "$MAX" ];
        then
            some_var=$(udevadm info --query=property -n /dev/ttyACM0 | grep ID_SERIAL_SHORT=)
            if [ ${some_var} == "ID_SERIAL_SHORT=754353530353515061D1" ];
            then
                ACTUATORS=$"/dev/ttyACM0"
                COUNTER=$((COUNTER+1))
            else
                if [ ${some_var} == "ID_SERIAL_SHORT=556393031353515190A0" ];
                then
                    SENSORS=$"/dev/ttyACM0"
                    COUNTER=$((COUNTER+1))
                fi      
            fi
        else
            echo "/dev/ttyACM0 NOT FOUND" >&2
        fi

        if [ -c /dev/ttyACM1 ] && [ "$COUNTER" -lt "$MAX" ];
        then
            some_var=$(udevadm info --query=property -n /dev/ttyACM1 | grep ID_SERIAL_SHORT=)
            if [ ${some_var} == "ID_SERIAL_SHORT=754353530353515061D1" ];
            then
                ACTUATORS=$"/dev/ttyACM1"
                COUNTER=$((COUNTER+1))
            else
                if [ ${some_var} == "ID_SERIAL_SHORT=556393031353515190A0" ];
                then
                    SENSORS=$"/dev/ttyACM1"
                    COUNTER=$((COUNTER+1))
                fi      
            fi
        else
            echo "/dev/ttyACM1 NOT FOUND" >&2
        fi

        if [ -c /dev/ttyACM2 ] && [ "$COUNTER" -lt "$MAX" ];
        then
            some_var=$(udevadm info --query=property -n /dev/ttyACM2 | grep ID_SERIAL_SHORT=)
            if [ ${some_var} == "ID_SERIAL_SHORT=754353530353515061D1" ];
            then
                ACTUATORS=$"/dev/ttyACM2"
                COUNTER=$((COUNTER+1))
            else
                if [ ${some_var} == "ID_SERIAL_SHORT=556393031353515190A0" ];
                then
                    SENSORS=$"/dev/ttyACM2"
                    COUNTER=$((COUNTER+1))
                fi      
            fi
        else
            echo "/dev/ttyACM2 NOT FOUND" >&2
        fi

        if [ -c /dev/ttyACM3 ] && [ "$COUNTER" -lt "$MAX" ];
        then
            some_var=$(udevadm info --query=property -n /dev/ttyACM3 | grep ID_SERIAL_SHORT=)
            if [ ${some_var} == "ID_SERIAL_SHORT=754353530353515061D1" ];
            then
                ACTUATORS=$"/dev/ttyACM3"
                COUNTER=$((COUNTER+1))
            else
                if [ ${some_var} == "ID_SERIAL_SHORT=556393031353515190A0" ];
            then
                SENSORS=$"/dev/ttyACM3"
                COUNTER=$((COUNTER+1))
            fi      
        fi
        else
            echo "/dev/ttyACM3 NOT FOUND" >&2
        fi

        if [ -c /dev/ttyACM4 ] && [ "$COUNTER" -lt "$MAX" ];
        then
            some_var=$(udevadm info --query=property -n /dev/ttyACM4 | grep ID_SERIAL_SHORT=)
            if [ ${some_var} == "ID_SERIAL_SHORT=754353530353515061D1" ];
            then
                ACTUATORS=$"/dev/ttyACM4"
                COUNTER=$((COUNTER+1))
            else
                if [ ${some_var} == "ID_SERIAL_SHORT=556393031353515190A0" ];
                then
                SENSORS=$"/dev/ttyACM4"
                COUNTER=$((COUNTER+1))
                fi      
            fi
        else
            echo "/dev/ttyACM4 NOT FOUND" >&2
        fi
        ;;
    *) 
        ;;
esac

case $ANS in
    y|Y) 
        if [ -f oldpid.txt ];
        then
            echo "Replacing old PID values..." >&2
            rm oldpid.txt
        else
            echo "File oldpid.txt does not exist! Creating a one..." >&2
        fi
        echo lanefollowerp=$P >> oldpid.txt
        echo lanefollowerd=$D >> oldpid.txt
        echo lanefolloweri=$I >> oldpid.txt
        ;;
    *) 
        echo "Using old PID values!" >&2
        source oldpid.txt
        P=$lanefollowerp
        I=$lanefolloweri
        D=$lanefollowerd;;
esac

echo "Creating new file configuration..." >&2
#define the template.
cat  << EOF
# This is the "one-and-only" configuration for OpenDaVINCI.
# Its format is like:
#
# section.key=value
#
# If you have several modules of the same type, the following configuration
# scheme applies:
#
# global.key=value # <-- This configuration applies for all modules.
#
# section.key=value # <-- This configuration applies for all modules of type "section".
#
# section:ID.key=value # <-- This configuration applies for the module "ID" of type "section".


###############################################################################
###############################################################################
#
# GLOBAL CONFIGURATION
#
global.car = file:///opt/configuration/Car1.objx
global.scenario = file:///opt/configuration/Parking-boxes-1.scnx
global.showGrid = 0

# Location of the origin of the reference frame (example: 57.70485804 N, 11.93831921 E)
global.reference.WGS84.latitude = 57.70485804
global.reference.WGS84.longitude = -11.93831921

# The following attributes define the buffer sizes for recording and
# replaying. You need to adjust these parameters depending on the
# camera resolution for example (640x480x3 --> 1000000 for memorySegment,
# 1280x720x3 --> 2800000).
global.buffer.memorySegmentSize = 2800000 # Size of a memory segment in bytes.
global.buffer.numberOfMemorySegments = 40 # Number of memory segments.                        // tentantive value, original = 20 !!

# The following key describes the list of modules expected to participate in this --cid session.
global.session.expectedModules = HelloWorldExample,HelloWorldExample:2


###############################################################################
###############################################################################
#
# NEXT, THE CONFIGURATION FOR OpenDaVINCI TOOLS FOLLOWS. 
#
###############################################################################
###############################################################################
#
# CONFIGURATION FOR ODSUPERCOMPONENT
#

# If the managed level is pulse_shift, all connected modules will be informed
# about the supercomponent's real time by this increment per module. Thus, the
# execution times per modules are better aligned with supercomponent and the
# data exchange is somewhat more predictable.
odsupercomponent.pulseshift.shift = 10000 # (in microseconds)

# If the managed level is pulse_time_ack, this is the timeout for waiting for
# an ACK message from a connected client.
odsupercomponent.pulsetimeack.timeout = 5000 # (in milliseconds)

# If the managed level is pulse_time_ack, the modules are triggered sequentially
# by sending pulses and waiting for acknowledgment messages. To allow the modules
# to deliver their respective containers, this yielding time is used to sleep
# before supercomponent sends the pulse messages the next module in this execution
# cycle. This value needs to be adjusted for networked simulations to ensure
# deterministic execution. 
odsupercomponent.pulsetimeack.yield = 5000 # (in microseconds)

# List of modules (without blanks) that will not get a pulse message from odsupercomponent.
odsupercomponent.pulsetimeack.exclude = odcockpit


###############################################################################
###############################################################################
#
# CONFIGURATION FOR ODCOCKPIT
#
odcockpit.plugins = #configurationviewer,livefeed,logmessage,player,sharedimageviewer,streetmapviewer,birdseyemap,iruscharts,irusmap # List of modules to show in odcockpit; leave blank to show all.
odcockpit.directoriesForSharedLibaries = /opt,/lib # List of directories to search for libodvd*.so files.


###############################################################################
###############################################################################
#
# CONFIGURATION FOR ODPLAYER
#
odplayer.input = file://recorder.rec
odplayer.autoRewind = 0 # 0 = no rewind in the case of EOF, 1 = rewind.
odplayer.remoteControl = 0 # 0 = no remote control, 1 = allowing remote control (i.e. play, pause, rewind, step_forward)
odplayer.timeScale = 1.0 # A time scale factor of 1.0 means real time, a factor of 0 means as fast as possible. The smaller the time scale factor is the faster runs the replay.


###############################################################################
###############################################################################
#
# CONFIGURATION FOR ODPLAYER
#
odplayer.input = file://recorder.rec
odplayer.autoRewind = 0 # 0 = no rewind in the case of EOF, 1 = rewind.
odplayer.remoteControl = 0 # 0 = no remote control, 1 = allowing remote control (i.e. play, pause, rewind, step_forward)
odplayer.timeScale = 1.0 # A time scale factor of 1.0 means real time, a factor of 0 means as fast as possible. The smaller the time scale factor is the faster runs the replay.

odplayerh264.input = file://recorder.rec
odplayerh264.autoRewind = 0 # 0 = no rewind in the case of EOF, 1 = rewind.
odplayerh264.remoteControl = 0 # 0 = no remote control, 1 = allowing remote control (i.e. play, pause, rewind, step_forward)
odplayerh264.timeScale = 1.0 # A time scale factor of 1.0 means real time, a factor of 0 means as fast as possible. The smaller the time scale factor is the faster runs the replay.
odplayerh264.portbaseforchildprocesses = 28000 # Every spawned child processes is connecting to the parent process via TCP using the base port plus its increasing ID.

###############################################################################
###############################################################################
#
# CONFIGURATION FOR ODRECORDER
#
odrecorder.output = file://recorder.rec
odrecorder.remoteControl = 0 # 0 = no remote control, 1 = allowing remote control (i.e. start and stop recording)
odrecorder.dumpSharedData = 1 # 0 = do not dump shared images and shared images, 1 = otherwise

odrecorderh264.output = file://recorder.rec
odrecorderh264.remoteControl = 0 # 0 = no remote control, 1 = allowing remote control (i.e. start and stop recording)
odrecorderh264.dumpSharedData = 1 # 0 = do not dump shared images and shared images, 1 = otherwise
odrecorderh264.lossless = 1 # Set to 1 to enable h264 lossless encoding.
odrecorderh264.portbaseforchildprocesses = 29000 # Every spawned child processes is connecting to the parent process via TCP using the base port plus its increasing ID.

###############################################################################
#
# NEXT, THE CONFIGURATION FOR OpenDaVINCI SIMULATION TOOLS FOLLOWS. 
#
###############################################################################
###############################################################################
#
# CONFIGURATION FOR ODSIMIRUS (infrared and ultrasonic simulation)
#
odsimirus.numberOfSensors = 6                   # Number of configured sensors.
odsimirus.showPolygons = 1                      # Show explicitly all polygons.

odsimirus.sensor0.id = 0                        # This ID is used in SensorBoardData structure.
odsimirus.sensor0.name = Infrared_FrontRight    # Name of the sensor
odsimirus.sensor0.rotZ = -90                    # Rotation of the sensor around the Z-axis in degrees, positive = counterclockwise, negative = clockwise, 0 = 12am, -90 = 3pm, ...
odsimirus.sensor0.translation = (1.0;-1.0;0.0)  # Translation (X;Y;Z) w.r.t. vehicle's center
odsimirus.sensor0.angleFOV = 5                  # In degrees.
odsimirus.sensor0.distanceFOV = 3               # In meters.
odsimirus.sensor0.clampDistance = 2.9           # Any distances greater than this distance will be ignored and -1 will be returned.
odsimirus.sensor0.showFOV = 1                   # Show FOV in monitor.

odsimirus.sensor1.id = 1                        # This ID is used in SensorBoardData structure.
odsimirus.sensor1.name = Infrared_Rear          # Name of the sensor
odsimirus.sensor1.rotZ = -180                   # Rotation of the sensor around the Z-axis in degrees, positive = counterclockwise, negative = clockwise, 0 = 12am, -90 = 3pm, ...
odsimirus.sensor1.translation = (-1.0;0.0;0.0)  # Translation (X;Y;Z) w.r.t. vehicle's center
odsimirus.sensor1.angleFOV = 5                  # In degrees.
odsimirus.sensor1.distanceFOV = 3               # In meters.
odsimirus.sensor1.clampDistance = 2.9           # Any distances greater than this distance will be ignored and -1 will be returned.
odsimirus.sensor1.showFOV = 1                   # Show FOV in monitor.

odsimirus.sensor2.id = 2                        # This ID is used in SensorBoardData structure.
odsimirus.sensor2.name = Infrared_RearRight     # Name of the sensor
odsimirus.sensor2.rotZ = -90                    # Rotation of the sensor around the Z-axis in degrees, positive = counterclockwise, negative = clockwise, 0 = 12am, -90 = 3pm, ...
odsimirus.sensor2.translation = (-1.0;-1.0;0.0) # Translation (X;Y;Z) w.r.t. vehicle's center
odsimirus.sensor2.angleFOV = 5                  # In degrees.
odsimirus.sensor2.distanceFOV = 3               # In meters.
odsimirus.sensor2.clampDistance = 2.9           # Any distances greater than this distance will be ignored and -1 will be returned.
odsimirus.sensor2.showFOV = 1                   # Show FOV in monitor.

odsimirus.sensor3.id = 3                        # This ID is used in SensorBoardData structure.
odsimirus.sensor3.name = UltraSonic_FrontCenter # Name of the sensor
odsimirus.sensor3.rotZ = 0                      # Rotation of the sensor around the Z-axis in degrees, positive = counterclockwise, negative = clockwise, 0 = 12am, -90 = 3pm, ...
odsimirus.sensor3.translation = (1.0;0.0;0.0)   # Translation (X;Y;Z) w.r.t. vehicle's center
odsimirus.sensor3.angleFOV = 20                 # In degrees.
odsimirus.sensor3.distanceFOV = 40              # In meters.
odsimirus.sensor3.clampDistance = 39            # Any distances greater than this distance will be ignored and -1 will be returned.
odsimirus.sensor3.showFOV = 1                   # Show FOV in monitor.

odsimirus.sensor4.id = 4                        # This ID is used in SensorBoardData structure.
odsimirus.sensor4.name = UltraSonic_FrontRight  # Name of the sensor
odsimirus.sensor4.rotZ = -45                    # Rotation of the sensor around the Z-axis in degrees, positive = counterclockwise, negative = clockwise, 0 = 12am, -90 = 3pm, ...
odsimirus.sensor4.translation = (1.0;-1.0;0.0)  # Translation (X;Y;Z) w.r.t. vehicle's center
odsimirus.sensor4.angleFOV = 20                 # In degrees.
odsimirus.sensor4.distanceFOV = 40              # In meters.
odsimirus.sensor4.clampDistance = 39            # Any distances greater than this distance will be ignored and -1 will be returned.
odsimirus.sensor4.showFOV = 1                   # Show FOV in monitor.

odsimirus.sensor5.id = 5                        # This ID is used in SensorBoardData structure.
odsimirus.sensor5.name = UltraSonic_RearRight   # Name of the sensor
odsimirus.sensor5.rotZ = -135                   # Rotation of the sensor around the Z-axis in degrees, positive = counterclockwise, negative = clockwise, 0 = 12am, -90 = 3pm, ...
odsimirus.sensor5.translation = (-1.0;-1.0;0.0) # Translation (X;Y;Z) w.r.t. vehicle's center
odsimirus.sensor5.angleFOV = 20                 # In degrees.
odsimirus.sensor5.distanceFOV = 40              # In meters.
odsimirus.sensor5.clampDistance = 39            # Any distances greater than this distance will be ignored and -1 will be returned.
odsimirus.sensor5.showFOV = 1                   # Show FOV in monitor.


###############################################################################
###############################################################################
#
# CONFIGURATION FOR ODSIMVEHICLE
#
odsimvehicle.posX = 0                     # Initial position X in cartesian coordinates.
odsimvehicle.posY = 0                     # Initial position Y in cartesian coordinates.
odsimvehicle.headingDEG = 90
odsimvehicle.model=LinearBicycleModelNew  # Which motion model to be used: LinearBicycleModelNew or LinearBicycleModel (for CaroloCup 2013!).
odsimvehicle.LinearBicycleModelNew.withSpeedController=1          # iff 1: use the VehicleControl.m_speed field; otherwise, ignore m_speed and use VehicleControl.m_acceleration field
odsimvehicle.LinearBicycleModelNew.minimumTurningRadiusLeft=4.85  # Minimum turning radius to the left (for calculating maximum steering angle to the left); Attention! we used data from the miniature vehicle Meili and thus, all values are scaled by factor 10 to be compatible with the simulation!
odsimvehicle.LinearBicycleModelNew.minimumTurningRadiusRight=5.32 # Minimum turning radius to the right (for calculating maximum steering angle to the right); Attention! we used data from the miniature vehicle Meili and thus, all values are scaled by factor 10 to be compatible with the simulation!
odsimvehicle.LinearBicycleModelNew.wheelbase=2.65                 # Wheelbase; Attention! we used data from the miniature vehicle Meili and thus, all values are scaled by factor 10 to be compatible with the simulation!
odsimvehicle.LinearBicycleModelNew.invertedSteering=0             # iff 0: interpret neg. steering wheel angles as steering to the left; iff 1: otherwise
odsimvehicle.LinearBicycleModelNew.maxSpeed=2.0                   # maxium speed in m/ss
odsimvehicle.LinearBicycleModel.minimumTurningRadius=4.24    # Minimum turning radius in m.
odsimvehicle.LinearBicycleModel.vehicleMass=1700.0           # Mass in kg.
odsimvehicle.LinearBicycleModel.adherenceCoefficient=100.0   # N per MPS (squared).
odsimvehicle.LinearBicycleModel.idleForce=200.0              # Force if vehicle is in idle (no gear) in N.
odsimvehicle.LinearBicycleModel.Ksteering=2.0
odsimvehicle.LinearBicycleModel.maximumSteeringRate=50.0
odsimvehicle.LinearBicycleModel.Kthrottle=40.0
odsimvehicle.LinearBicycleModel.tauBrake=1.0
odsimvehicle.LinearBicycleModel.KstaticBrake=100.0
odsimvehicle.LinearBicycleModel.KdynamicBrake=60.0


###############################################################################
###############################################################################
#
# NEXT, THE CONFIGURATION FOR USER APPLICATIONS FOLLOWS. 
#
###############################################################################
###############################################################################
#
# CONFIGURATION FOR LANEFOLLOWER
#
lanefollower.p = $P     #OG val = 1.3
lanefollower.d = $D    #OG val = 0.10
lanefollower.i= $I     #OG val = 0.01

###############################################################################
###############################################################################
#
# CONFIGURATION FOR VCR
#
VCR.camera_id = 2 # select here the proper camera
VCR.debug = 1      # set to 0 to disable any windows and further output


###############################################################################
###############################################################################
#
# CONFIGURATION FOR EGOCONTROLLER
#
egocontroller.device=keyboard              # Which device for control to be used (currently, joysticks are working only under Linux, e.g. keyboard or /dev/input/js0).
egocontroller.behavior=linearbicyclenew    # [simple,force,forcesimplifiedbicycle,forcebicycle,linearbicyclenew] Set to force or forcebicycle to get force-based control.            
egocontroller.minimumTurningRadius=4.24    # Minimum turning radius in m.
egocontroller.vehicleMass=1700.0           # Mass in kg.
egocontroller.adherenceCoefficient=100.0   # N per MPS (squared).
egocontroller.idleForce=200.0              # Force if vehicle is in idle (no gear) in N.
egocontroller.Ksteering=2.0
egocontroller.maximumSteeringRate=50.0
egocontroller.Kthrottle=40.0
egocontroller.tauBrake=1.0
egocontroller.KstaticBrake=100.0
egocontroller.KdynamicBrake=60.0
egocontroller.distanceCenterOfMassToFrontAxle = 1.344 # Parameter for bicycle model: Distance from center of mass to front axle.
egocontroller.distanceCenterOfMassToRearAxle = 1.456  # Parameter for bicycle model: Distance from center of mass to rear axle.
egocontroller.momentOfInertia = 2800                  # Parameter for bicycle model: Moment of inertia.
egocontroller.skewStiffnessFront = 75000              # Parameter for bicycle model: Skew stiffness front wheels.
egocontroller.skewStiffnessRear = 150000              # Parameter for bicycle model: Skew stiffness rear wheels.
egocontroller.wheelbase = 3.4                         # Parameter for simplified bicycle model: Wheel base.
egocontroller.start = (0;0;0)
egocontroller.rotZ = 90
egocontroller.LinearBicycleModelNew.minimumTurningRadiusLeft=4.85  # Minimum turning radius to the left (for calculating maximum steering angle to the left); Attention! we used data from the miniature vehicle Meili and thus, all values are scaled by factor 10 to be compatible with the simulation!
egocontroller.LinearBicycleModelNew.minimumTurningRadiusRight=5.32 # Minimum turning radius to the right (for calculating maximum steering angle to the right); Attention! we used data from the miniature vehicle Meili and thus, all values are scaled by factor 10 to be compatible with the simulation!
egocontroller.LinearBicycleModelNew.wheelbase=2.65                 # Wheelbase; Attention! we used data from the miniature vehicle Meili and thus, all values are scaled by factor 10 to be compatible with the simulation!
egocontroller.LinearBicycleModelNew.maxSpeed=2.0                   # maxium speed in m/ss


###############################################################################
###############################################################################
#
# CONFIGURATION FOR PROXY
#
proxy.debug = 0
proxy.useRecorder = 0 # 1 = record all captured data directly, 0 otherwise. 
proxy.recorder.output = file://recs/
proxy.camera.name = WebCam
proxy.camera.type = OpenCV # OpenCV or UEYE
proxy.camera.id = 1 # Select here the proper ID for OpenCV
proxy.camera.width = 640 #752-UEYE, 640-OpenCV 
proxy.camera.height = 480
proxy.camera.bpp = 3 #3- openCV, 1-UEYE
proxy.camera.flipped = 1 #1 = flipped image, 0 =notflipped image.
#proxy.camera.dumpSharedData = 1

Proxy.Actuator.UseRealSpeed=0
proxy.Actuator.SerialPort=/dev/ttyACM0

proxy.Sensor.SerialPort=/dev/ttyUSB0
proxy.Sensor.SerialSpeed=115200

proxy.numberOfSensors=6

proxy.sensor0.id=0 #first infra
proxy.sensor1.id=1 #second infra
proxy.sensor2.id=2 #third infra
proxy.sensor3.id=3 #fourth infra
proxy.sensor4.id=4 #first ultrasonic
proxy.sensor5.id=5 #second ultrasonic

###############################################################################
###############################################################################
#
# CONFIGURATION FOR THE DRIVER
#
driver.realSpeed = 11
driver.startInBox = 1 # 1 - true, 0 - false
driver.startboxLength = 2200 # in mm

###############################################################################
###############################################################################
#
# CONFIGURATION FOR PARKING
#

driver.MinParkingDist = 500
driver.MaxParkingDist = 700
driver.SafeDistance = 30
driver.MinSafeDistance = 2
driver.DesiredDistance1 = 250 
driver.DesiredDistance2 = 700
driver.DesiredDistance3 = 480
driver.DesiredDistance4 = 90
driver.DesiredDistance5 = 30
driver.SpeedF1 = 3
driver.SpeedF2 = 6
driver.SpeedB1 = -4
driver.SpeedB2 = -6
driver.Stop_Speed = 0
driver.IRMaxDist = 25
driver.IRMinDist = 0
driver.isSmallGapSize = 0
driver.DesiredMoreStates = 5
driver.MoreStates = 2

###############################################################################
###############################################################################
#
# CONFIGURATION FOR LANEDETECTOR
#
lanedetector.camera_id = 2  # select here the proper camera
lanedetector.debug = 1      # set to 0 to disable any windows and further output
lanedetector.inspect_rec = file://recording.rec # Config for lanedetector-inspection
# Choose what to inspect, where
# getContours = 1
# getRectangles = 2
# classification = 3
# filterAndMerge = 4
# finalFilter = 5
# finalResult = 6
# createTrajectory = 6
lanedetector.what_to_inspect = 7
# Choose which windows to display. 0: no, 1: yes
lanedetector.showRes_getContours = 1
lanedetector.showRes_getRectangles = 1
lanedetector.showRes_classification = 1
lanedetector.showRes_filterAndMerge = 1
lanedetector.showRes_finalFilter = 1
lanedetector.showRes_finalResult = 1
lanedetector.showRes_createTrajectory = 1
lanedetector.threshBaseParameter=48

###############################################################################
###############################################################################
#
# CONFIGURATION FOR CommunicationLink
#
communicationlink.functionlane = $FUNCTION1
communicationlink.function2 = $FUNCTION2

###############################################################################
###############################################################################
#
# GLOBAL CONFIGURATION
#
global.debug = 1      # set to 0 to disable any windows and further output
global.sim = 0    # Set simulation true or false

###############################################################################
###############################################################################
#
# CLIENT IP
#
udpconnectionstreamer.ip = $IP

###############################################################################
###############################################################################
#
# SERIALS
#
serialreceivehandler.sensors = $SENSORS
serialsendhandler.actuators = $ACTUATORS
EOF

