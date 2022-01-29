FROM drogonframework/drogon

WORKDIR /usr/app/logbroker

COPY . .

RUN cd build \
    && cmake .. \ 
    && make

ENTRYPOINT "./build/logbroker"
