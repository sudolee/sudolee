#!/bin/bash
#
# Just install useful softwares...
#

sudo apt-get install gcc g++ gcc-multilib g++-multilib make gdb bison libc6 libc6-dev manpages manpages-posix manpages-dev manpages-posix-dev glibc-doc \
	subversion git-core gitk libncurses5 libncurses5-dev im-switch tree indent patch tar zip rar unzip unrar vim \
	putty axel wget cramfsprogs cscope ctags diffutils meld sharutils ssh perl bash python \
	soundconverter gawk sed build-essential -y


# filezilla vim-gnome iptux kchmviewer gdebi minicom tftp vsftpd
# bluefish sqlite3
# gparted transmission-cli shotwell
# lingot ## for musical tuner.

sudo apt-get autoremove
sudo apt-get autoclean
