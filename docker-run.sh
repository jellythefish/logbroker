#build
docker build . -t logbroker

# dev
docker run -it --entrypoint /bin/bash --name logbroker-container ^
    --rm -p 80:80 ^
    -v "":/usr/app/logbroker ^
    logbroker

# single process detached (prod)
docker run --name logbroker-container -p 8083:8083 -it --restart always logbroker
