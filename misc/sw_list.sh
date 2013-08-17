#!/bin/bash

sudo apt-get install \
	firmware-iwlwifi \
	vim cscope ctags indent \
	vlc \
	fcitx fcitx-googlepinyin im-config \
	bash python perl lua5.2 tcl bash-completion tree gawk sed python-markdown m4 \
	rsync axel curl \
	wireshark \
	manpages-posix glibc-doc \
	linux-headers-amd64 \
	gcc gcc-multilib g++ g++-multilib cpp gdb bison flex binutils \
	make libtool autoconf automake autogen gettext texinfo build-essential \
	gnupg gperf expect dejagnu guile-2.0 byacc fixincludes \
	patch patchutils diffutils meld \
	libc6-dev libzip2 libncurses5 libncurses5-dev libreadline6 libreadline6-dev zlib1g zlib1g-dev\
	libxml2-utils man2html xmlto xsltproc tofrodos \
	elfutils htop \
	bzip2 gzip zip unzip rar unrar \
	exfat-utils ntfs-3g e2fsprogs dosfstools cramfsprogs \
	net-tools ethtool iw tcpdump tcpreplay \
	subversion git-core gitk quilt \
	ssh minicom putty screen \

# set for wireshark
sudo setcap 'CAP_NET_RAW+eip CAP_NET_ADMIN+eip' /usr/bin/dumpcap
# used by adb/fastboot
# lib32ncurses5
# lingot  for musical tuner
# bumblebee primus firmware-iwlwifi \
# libstdc++

# use pcmanfm instead of nautilus in gnome
