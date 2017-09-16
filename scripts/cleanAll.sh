#!/bin/bash
#if you dont have xterm on your linux distro, run "sudo apt install xterm"

xterm -hold -e "cd $HOME/CaroloCup/opendlv.carolocup/docker && make cleanAll" &

