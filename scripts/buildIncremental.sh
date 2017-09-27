#!/bin/bash
#if you dont have xterm on your linux distro, run "sudo apt install xterm"

#xterm -hold -e "cd $HOME/CaroloCup/opendlv.carolocup/docker && make updateDockerBaseImage && make buildIncremental && make createDockerImage && make removeNoneImagesFromDocker" &

cd $HOME/CaroloCup/opendlv.carolocup/docker && make updateDockerBaseImage && make buildIncremental && make createDockerImage && make removeNoneImagesFromDocker
