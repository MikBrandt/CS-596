#!/bin/bash

# this script will loop forever. It allows the Pi to read the BLE signal from the ESP32 and
# run the script to take a picture and send that picture to the cloud.

while true
do  
    # 4c:eb:d6:44:05:1a is the MAC address of the ESP32
    # ' 6d 6f 74 69 6f 6e ' is the signal from the ESP32 to run the script
    if [ "$(sudo gatttool -b 4c:eb:d6:44:05:1a --char-read -a 0x002a | awk -F':' '{print $NF}')" = " 6d 6f 74 69 6f 6e " ]
    then
    echo "movement!" # for testing only
    python ./takePicture.py # runs the script
    else
    echo "$(sudo gatttool -b 4c:eb:d6:44:05:1a --char-read -a 0x002a | awk -F':' '{print $NF}')" # for testing only
    fi
done