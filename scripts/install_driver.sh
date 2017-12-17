#!/bin/bash

cd /etc/udev/ && mkdir rules.d &&

cd /opt/ueyeDriver/uEye_Linux_4.9_64_Bit/ && 

if [ -f ueyesdk-setup-4.90-usb-amd64.gz.run ]; then
     sh ./ueyesdk-setup-4.90-usb-amd64.gz.run
fi

