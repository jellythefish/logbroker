FROM drogonframework/drogon

WORKDIR /usr/app/logbroker

COPY . .

RUN apt-get update
RUN apt-get install -y curl openssl libssl-dev libcurl4-openssl-dev

RUN cd build \
    && cmake .. \ 
    && make

ENTRYPOINT "./build/logbroker"
