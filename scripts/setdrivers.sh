#!/bin/bash

BASE=$(docker images -a -q seresearch/scaledcars-on-opendlv-on-opendlv-core-on-opendavinci-on-base:v0.1.4)
DEV=$(docker images -a -q seresearch/scaledcars-on-opendlv-on-opendlv-core-on-opendavinci-on-base-dev:v0.1.4)

./carolocupbaseimage.sh 

#./carolocupbaseimage-dev.sh 

#docker ps

