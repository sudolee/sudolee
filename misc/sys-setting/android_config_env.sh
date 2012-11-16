#!/bin/sh

# http://source.android.com/source/initializing.html

# 32-bits?
BIT=$(getconf LONG_BIT)
echo "**** This is a $BIT-bits computer. ****"

if [ 32 = "$BIT" ];then
	ARCH=i386
	sudo apt-get install git-core gnupg flex bison gperf build-essential \
	  zip curl libc6-dev libncurses5-dev x11proto-core-dev \
	  libx11-dev libreadline6-dev libgl1-mesa-dev \
	  g++-multilib mingw32 tofrodos python-markdown \
	  libxml2-utils xsltproc zlib1g-dev
elif [ 64 = "$BIT" ];then
	ARCH=iamd64
	sudo apt-get install git-core gnupg flex bison gperf build-essential zip curl \
		zlib1g-dev libc6-dev lib32ncurses5-dev ia32-libs x11proto-core-dev libx11-dev \
		lib32readline5-dev lib32z-dev libgl1-mesa-dev g++-multilib mingw32 tofrodos \
		python-markdown libxml2-utils xsltproc
else
	echo "Sorry, neither 32-bits nor 64-bits, no software matched."
fi

./apt_get.sh

curl https://dl-ssl.google.com/dl/googlesource/git-repo/repo > ./repo
chmod +x repo
sudo mv ./repo /usr/local/bin/repo

exit
