#!/bin/bash

cd $HOME/CaroloCup/opendlv.carolocup/docker && make buildIncremental && make createDockerImage && make removeNoneImagesFromDocker && make removeExitedContainers
