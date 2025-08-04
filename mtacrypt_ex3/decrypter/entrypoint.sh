#!/bin/sh

ID=$(echo $HOSTNAME | sed 's/[^0-9]*//g')
/app/decrypter > /var/log/decrypter_${ID}.log 2>&1

