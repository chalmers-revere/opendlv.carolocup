#!/bin/bash

cwd="$( cd "$(dirname "$0")" ; pwd -P )"
BUILD=$1

trap "docker-compose down" SIGINT

if [ -n "${!BUILD}" ]; then
    echo "Building First!!!" >&2
    cd $HOME/CaroloCup/opendlv.carolocup/docker
    make updateDockerBaseImage 
    make buildIncremental
    make createDockerImage
    make removeNoneImagesFromDocker  
else
    echo "Not Building, Only Running!!!" >&2
fi

#./findport.sh

cd $cwd && xhost + && docker-compose down && docker-compose up --build >> log_data/"$(date +"%Y_%m_%d_%I_%M_%p").txt"

