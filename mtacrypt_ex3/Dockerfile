FROM ubuntu:latest

RUN apt update && apt install -y gcc make

WORKDIR /app

COPY common/ ./common/
COPY encrypter/ ./encrypter/

RUN mkdir -p /mnt/mta
COPY encrypter/config.txt /mnt/mta/config.txt

RUN gcc -o encrypter ./encrypter/encrypter.c ./common/util.c -I./common

ENTRYPOINT ["/app/encrypter/entrypoint.sh"]

