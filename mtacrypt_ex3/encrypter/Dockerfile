FROM ubuntu:latest

RUN apt update && apt install -y gcc make

WORKDIR /app

COPY encrypter/encrypter.c .
COPY encrypter/entrypoint.sh .
COPY common/util.c common/util.c
COPY common/util.h common/util.h
COPY common/mtacrypt.h common/mtacrypt.h
COPY encrypter/config.txt config.txt

RUN gcc -o encrypter encrypter.c common/util.c -Icommon

RUN chmod +x /app/entrypoint.sh
ENTRYPOINT ["/app/entrypoint.sh"]


