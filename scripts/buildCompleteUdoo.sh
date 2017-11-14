#!/bin/bash

cd $HOME/CaroloCup/opendlv.carolocup/docker && make cleanAll &&
docker stop	$(docker ps -a -q) &&
docker rmi -f $(docker images -a -q) &&
cd $HOME/CaroloCup/opendlv.carolocup/docker && make updateDockerBaseImage && make buildComplete && make createDockerImage && make removeNoneImagesFromDocker
