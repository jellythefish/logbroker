#build
docker build . -t logbroker

# clickhouse run
docker run -d --name clickhouse-server -p 8123:8123 --ulimit nofile=262144:262144 ^
--volume="C:\Users\Slava\Desktop\hw2\logbroker\clickhouse_database":/var/lib/clickhouse yandex/clickhouse-server

# clickhouse client
docker run -it --rm --link clickhouse-server:clickhouse-server yandex/clickhouse-client --host clickhouse-server

# dev
docker run -it --entrypoint /bin/bash --name logbroker-container ^
    --rm -p 80:80 ^
    -v "C:\Users\Slava\Desktop\hw2\logbroker":/usr/app/logbroker ^
    --link clickhouse-server:clickhouse-server ^
    logbroker

# single process detached (prod)
docker run --name logbroker-container -p 80:80 -it --restart always logbroker

# requests to CH server from host OS (hostname is clickhouse-server from docker)
curl -X POST -H "transfer-encoding:chunked" localhost:8123 -d "SELECT 0;"
# py
requests.post("http://localhost:8123/?query=SELECT%200%3B").text
