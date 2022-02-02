#!/bin/bash

sudo docker rm -f $(docker ps -a -q)
sudo docker pull jellythefish/logbroker
sudo docker run --name logbroker-container -p 80:80 -d --restart always \
    -v ~/logbroker:/usr/app/logbroker \
    -e LOGBROKER_CH_HOST=$1 -e LOGBROKER_CH_PORT=$2 -e SERVER_HOSTNAME=$(hostname --fqdn) \
    jellythefish/logbroker
