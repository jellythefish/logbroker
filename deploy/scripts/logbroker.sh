#!/bin/bash

# ./logbroker.sh 84.35.32.12 8123

sudo docker run --name logbroker-container -p 80:80 -d --restart always \
    -e LOGBROKER_CH_HOST=$1 -e LOGBROKER_CH_PORT=$2 -e SERVER_HOSTNAME=$(hostname --fqdn) \
    jellythefish/logbroker
