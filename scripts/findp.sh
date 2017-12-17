#!/bin/bash

#101 serial == UDOO_UDOO_X86__K71942200
#uno serial == Arduino_Srl_Arduino_Uno_7563331323335120A121 (will chnage if change to another uno)

for sysdevpath in $(find /sys/bus/usb/devices/usb*/ -name dev); do
    (
        syspath="${sysdevpath%/dev}"
        devname="$(udevadm info -q name -p $syspath)"
        [[ "$devname" == "bus/"* ]] && continue
        eval "$(udevadm info -q property --export -p $syspath)"
        if [ -n "$ID_SERIAL" ]; then
             if  [ "$ID_VENDOR_ID" == "2a03" -a "$ID_MODEL_ID" == "0043" ]; then                 
                    echo "uno $DEVNAME" >&2
                                              
             fi
             if  [ "$ID_VENDOR_ID" == "8087" -a "$ID_MODEL_ID" == "0ab6" ]; then
                    echo "udoo $DEVNAME" >&2
                     
             fi
             if  [ "$ID_VENDOR_ID" == "0403" -a "$ID_SERIAL" == "FTDI_FT230X_Basic_UART_DM00LTCO" ]; then
                    echo "LIDAR $DEVNAME" >&2
                   
             fi
        fi
    )
done

