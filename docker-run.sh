#build
docker build . -t jellythefish/logbroker

# clickhouse server dev
docker run -d --name clickhouse-server -p 8123:8123 --ulimit nofile=262144:262144 ^
    --volume="C:\Users\Slava\Desktop\hw2\logbroker\clickhouse_database":/var/lib/clickhouse ^
    yandex/clickhouse-server

# clickhouse server prod
docker run -d --name clickhouse-server -p 8123:8123 --ulimit nofile=262144:262144 --restart always ^
    yandex/clickhouse-server

# clickhouse client
docker run -it --rm --link clickhouse-server:clickhouse-server yandex/clickhouse-client ^
    --host clickhouse-server

# logbroker dev (all containers in one network)
docker run -it --entrypoint /bin/bash --name logbroker-container ^
    --rm -p 80:80 ^
    -v "C:\Users\Slava\Desktop\hw2\logbroker":/usr/app/logbroker ^
    --link clickhouse-server:clickhouse-server ^
    -e LOGBROKER_CH_HOST="clickhouse-server" -e LOGBROKER_CH_PORT="8123" -e SERVER_HOSTNAME=$(hostname --fqdn) ^
    jellythefish/logbroker

# logbroker prod (run with args)
docker run -p 80:80 -d --restart always ^
    -e LOGBROKER_CH_HOST=$1 -e LOGBROKER_CH_PORT=$2 -e SERVER_HOSTNAME=$(hostname --fqdn) ^
    jellythefish/logbroker

# requests to CH server from host OS (hostname is clickhouse-server from docker)
curl -X POST -H "transfer-encoding:chunked" localhost:8123 -d "SELECT 0;"
# from py
requests.post("http://localhost:8123/?query=SELECT%200%3B").text
