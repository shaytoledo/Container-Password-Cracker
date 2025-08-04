FROM ubuntu:latest

RUN apt update && apt install -y gcc make

WORKDIR /app

COPY decrypter/decrypter.c .
COPY decrypter/entrypoint.sh .
COPY common/mtacrypt.h common/
RUN chmod +x entrypoint.sh

RUN mkdir -p /var/log && chmod 777 /var/log

RUN gcc -o decrypter decrypter.c -Icommon

ENTRYPOINT ["./entrypoint.sh"]

