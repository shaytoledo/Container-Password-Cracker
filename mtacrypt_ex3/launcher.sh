#!/bin/bash

MTA_DIR="/tmp/mtacrypt"
LOGS_DIR="/tmp/mtacrypt_logs"
mkdir -p $MTA_DIR
mkdir -p $LOGS_DIR

cp ./mtacrypt.conf $MTA_DIR/config.txt

docker run -v $MTA_DIR:/mnt/mta -v $LOGS_DIR:/var/log --name encrypter --rm -d mtacrypt_encrypter

for i in $(seq 1 $1); do
    docker run -v $MTA_DIR:/mnt/mta -v $LOGS_DIR:/var/log --name decrypter_$i --rm -d mtacrypt_decrypter
done
