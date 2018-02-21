#!/bin/bash

#101 serial == UDOO_UDOO_X86__K71942200
#uno serial == Arduino_Srl_Arduino_Uno_7563331323335120A121 (will chnage if change to another uno)

FILE=docker-compose.yml

if [ -f $FILE ];
then
   echo "File $FILE exists..." >&2
   sed -i '/##start/,/##end/d' $FILE
   echo "##start
    # carolocup-control-proxy-serialinhandler
    carolocup-control-proxy-serialinhandler:
        image: carolocup/carolocup-on-scaledcars-on-opendlv-on-opendlv-core-on-opendavinci-on-base-dev
        network_mode: \"host\"
        ipc : host
        depends_on:
            - odsupercomponent
        devices:
            - \"*SENSORS:*SENSORS\"
        restart: on-failure
        privileged: true
        command: \"/opt/opendlv.carolocup/bin/carolocup-control-proxy-serialhandler --cid=\${CID} --freq=10 *SENSORS\"

#    # carolocup-control-proxy-serialouthandler
#    carolocup-control-proxy-serialouthandler:
#        image: carolocup/carolocup-on-scaledcars-on-opendlv-on-opendlv-core-on-opendavinci-on-base-dev
#        network_mode: \"host\"
#        ipc : host
#        depends_on:
#            - odsupercomponent
#        devices:
#            - \"*ACTUATORS:*ACTUATORS\"
#        restart: on-failure
#        privileged: true
#        command: \"/opt/opendlv.carolocup/bin/carolocup-control-proxy-serialhandler --cid=\${CID} --freq=10 *ACTUATORS\"

#    #Micro-service for lidarhandler
#    proxy-lidarhandler:
#        image: carolocup/carolocup-on-scaledcars-on-opendlv-on-opendlv-core-on-opendavinci-on-base-dev
#        network_mode: \"host\"
#        ipc : host
#        depends_on:
#           - odsupercomponent
#        devices:
#           - \"*LIDAR:*LIDAR\"
#        restart: on-failure
#        privileged: true
#        command: \"/opt/opendlv.carolocup/bin/carolocup-control-proxy-lidarhandler --cid=\${CID} --freq=10 *LIDAR\"
##end" >> $FILE
else
   echo "Please create $FILE first and do not forget to place ##start and ##end as delimeters for the USB services!!!" >&2
   exit 1
fi

echo "echo File $FILE modified." >&2

for sysdevpath in $(find /sys/bus/usb/devices/usb*/ -name dev); do
    (
        syspath="${sysdevpath%/dev}"
        devname="$(udevadm info -q name -p $syspath)"
        [[ "$devname" == "bus/"* ]] && continue
        eval "$(udevadm info -q property --export -p $syspath)"
        if [ -n "$ID_SERIAL" ]; then
             if  [ "$ID_VENDOR_ID" == "2a03" -a "$ID_MODEL_ID" == "0043" ]; then
                    echo "SENSORS @ $DEVNAME" >&2
                    sed -i "s,*SENSORS,$DEVNAME,g" docker-compose.yml
             fi
        fi
    )
done

