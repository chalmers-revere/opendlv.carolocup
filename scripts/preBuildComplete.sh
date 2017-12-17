#!/bin/bash
dir=$(pwd)
trap "$dir/buildComplete.sh" EXIT

cd $HOME/CaroloCup/opendlv.carolocup/docker && make cleanAll &&

docker stop	$(docker ps -a -q) &&

docker rmi -f $(docker images -a -q)
