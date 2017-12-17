#!/bin/bash

#101 serial == UDOO_UDOO_X86__K71942200
#uno serial == Arduino_Srl_Arduino_Uno_7563331323335120A121 (will chnage if chnage to another uno)

FILE=docker-compose.yml

if [ -f $FILE ]; 
then
   echo "File $FILE exists..." >&2
   sed -i '/##start/,/##end/d' $FILE
   echo "##start
    # carolocup-control-proxy-serialinhandler
    carolocup-control-proxy-serialinhandler:
        build: .
        network_mode: \"host\"
        ipc : host
        depends_on:
            - odsupercomponent
        devices:
            - \"/dev/ttyUSB0:/dev/ttyUSB0\"
        restart: on-failure
        privileged: true
        command: \"/opt/opendlv.carolocup/bin/carolocup-control-proxy-serialhandler --cid=\${CID} --freq=10 *SENSORS\"

    # carolocup-control-proxy-serialouthandler
    carolocup-control-proxy-serialouthandler:
        build: .
        network_mode: \"host\"
        ipc : host
        depends_on:
            - odsupercomponent
        devices:
            - \"/dev/ttyUSB0:/dev/ttyUSB0\"
        restart: on-failure
        privileged: true
        command: \"/opt/opendlv.carolocup/bin/carolocup-control-proxy-serialhandler --cid=\${CID} --freq=10 *ACTUATORS\"
##end" >> $FILE
else
   echo "Please create $FILE first and do not forget to place ##start and ##end as delimeters for the serial services!!!" >&2
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
                    echo "$DEVNAME" >&2
                    sed -i "s,*SENSORS,$DEVNAME,g" docker-compose.yml                             
             fi
             if  [ "$ID_VENDOR_ID" == "8087" -a "$ID_MODEL_ID" == "0ab6" ]; then
                    echo "$DEVNAME" >&2
                    sed -i "s,*ACTUATORS,$DEVNAME,g" docker-compose.yml  
             fi

        fi
    )
done

