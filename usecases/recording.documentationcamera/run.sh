#!/bin/bash

cwd=$(pwd)
d=$(echo $(date))

trap "docker-compose down" SIGINT

cd $HOME/CaroloCup/opendlv.carolocup/docker && make updateDockerBaseImage && make buildIncremental && make createDockerImage && make removeNoneImagesFromDocker &&

cd $cwd && xhost + && docker-compose down && docker-compose up --build >> "$(date +"%Y_%m_%d_%I_%M_%p").txt"

