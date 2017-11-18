#!/bin/bash

cwd=$(pwd)
trap "docker-compose down" SIGINT

cd $HOME/CaroloCup/opendlv.carolocup/docker && make updateDockerBaseImage && make buildIncremental && make createDockerImage && make removeNoneImagesFromDocker &&

cd $cwd && xhost + && docker-compose down && docker-compose up --build >> $(date)log.txt

