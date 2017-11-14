#!/bin/bash

cwd=$(pwd)

cd $HOME/CaroloCup/opendlv.carolocup/docker && make updateDockerBaseImage && make buildIncremental && make createDockerImage && make removeNoneImagesFromDocker &&

cd $cwd && docker-compose down && docker-compose up --build >> log.txt && trap "docker-compose down" SIGINT

