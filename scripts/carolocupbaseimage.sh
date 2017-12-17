#!/bin/bash

docker rm -f $(docker ps -a -q)

DRIVER_DIR=$HOME/CaroloCup/ueye/
ORIGINAL_BASE_IMAGE=seresearch/scaledcars-on-opendlv-on-opendlv-core-on-opendavinci-on-base:v0.1.4

cd $DRIVER_DIR

xterm -hold -e "docker run -ti -v `pwd`:/opt/ueyeDriver/ $ORIGINAL_BASE_IMAGE /bin/bash" &

IMAGE_PID=$!

sleep 3

DOCKER_CONTAINER=$(docker ps -qf ancestor=$ORIGINAL_BASE_IMAGE)

echo "Container $DOCKER_CONTAINER" >&2

docker exec -d $DOCKER_CONTAINER /opt/ueyeDriver/install_driver.sh

sleep 3

COMMAND_PROCESS=$!

#docker commit $DOCKER_CONTAINER carolocup/scaledcars-on-opendlv-on-opendlv-core-on-opendavinci-on-base:v0.2 (maybe not needed)

docker commit $DOCKER_CONTAINER carolocup/scaledcars-on-opendlv-on-opendlv-core-on-opendavinci-on-base:latest

docker exec -d $DOCKER_CONTAINER /opt/ueyeDriver/finish.sh

sleep 3

kill $IMAGE_PID
