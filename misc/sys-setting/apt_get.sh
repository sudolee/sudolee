#!/bin/bash
#
# Just install useful softwares...
#

sudo apt-get update
sudo apt-get install gcc g++ make gdb bison libc6 libc6-dev manpages manpages-posix manpages-dev manpages-posix-dev glibc-doc \
	subversion git-core gitk libncurses5 libncurses5-dev im-switch tree indent patch tar rar unzip unrar vim \
	putty tftp vsftpd wget smbfs cramfsprogs cscope ctags meld sharutils ssh perl bash python \
	soundconverter gawk sed filezilla -y


# filezilla vim-gnome iptux kchmviewer gdebi minicom
# bluefish sqlite3 sqlite3-doc libsqlite3-0 libsqlite3-dev lftp atftp atftpd nfs-kernel-server openssh-client openssh-server gimp thunderbird
# gparted transmission-cli shotwell
# lingot ## for musical tuner.

sudo apt-get autoremove
sudo apt-get autoclean
