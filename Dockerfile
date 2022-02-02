FROM drogonframework/drogon

RUN apt-get update
RUN apt-get install -y git curl openssl libssl-dev libcurl4-openssl-dev

WORKDIR /usr/app

RUN git clone --recursive https://github.com/jellythefish/logbroker \
    && mkdir logbroker/build \
    && cd logbroker/build \
    && cmake .. \ 
    && make

WORKDIR /usr/app/logbroker/build

CMD "./logbroker"
