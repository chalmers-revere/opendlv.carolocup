#!/bin/bash

#script for building complete
cd $HOME/CaroloCup/opendlv.carolocup/docker && make updateDockerBaseImage &&

./carolocupbaseimage-dev.sh && ./carolocupbaseimage.sh &&

make buildComplete && make createDockerImage && make removeNoneImagesFromDocker  && make removeExitedContainers


