#!/bin/bash

cd $HOME/CaroloCup/opendlv.carolocup/docker && make updateDockerBaseImage && make buildIncremental && make createDockerImage && make removeNoneImagesFromDocker
