#!/bin/bash

docker run -d --name clickhouse-server -p 8123:8123 \
    --ulimit nofile=262144:262144 --restart always \
    yandex/clickhouse-server

until curl -X POST -H "transfer-encoding:chunked" localhost:8123 -d \
    "CREATE TABLE IF NOT EXISTS kek (a Int32, b String, host String) ENGINE = MergeTree() PRIMARY KEY a;" -v
do
    echo "sleep 1 sec"
    sleep 1
done
