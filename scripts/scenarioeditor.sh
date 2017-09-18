#!/bin/bash
# scenario editor script

xterm -hold -e "xhost + && docker run -t -i --rm -e DISPLAY=$DISPLAY -e QT_X11_NO_MITSHM=1 -v /tmp/.X11-unix:/tmp/.X11-unix -v /home/$USER/scenarios:/opt/data -w /opt/ScenarioEditor seresearch/scenario_editor:v2 ./ScUI" &



