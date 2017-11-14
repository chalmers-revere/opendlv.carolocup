#!/bin/bash

#script for building complete
cd $HOME/CaroloCup/opendlv.carolocup/docker && make updateDockerBaseImage && make buildComplete && make createDockerImage && make removeNoneImagesFromDocker
