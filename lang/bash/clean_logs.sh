#!/bin/bash

#Cleanup,version 2

#run as root, of course

LOG_DIR=/var/log
cd $LOG_DIR

cat /dev/null > messages
cat /dev/null > wtmp

echo "Logs cleaned up."
exit
