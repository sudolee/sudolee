#!/bin/sh

# 32-bits?
BIT=$(getconf LONG_BIT)

if [ 32 = "$BIT" ];then
	ARCH=i386
elif [ 64 = "$BIT" ];then
	ARCH=iamd64
else
	echo "Sorry, neither 32-bits nor 64-bits, no software matched."
fi
wget http://skype.tom.com/download/linux/skype-ubuntu_2.2.0.25-1_${ARCH}.deb
sudo dpkg -i skype-ubuntu_2.2.0.25-1_${ARCH}.deb
