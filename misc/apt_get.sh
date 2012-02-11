#!/bin/bash
#
# Just install useful softwares...
#

sudo apt-get install gcc g++ make gdb libc6 libc6-dev manpages manpages-posix manpages-dev manpages-posix-dev glibc-doc \
	python subversion git git-core libncurses-dev im-switch tree indent patch kchmviewer tar rar unzip unrar gparted vim vim-gnome \
	minicom putty tftp vsftpd filezilla iptux wget smbfs linux-source-2.6 cramfsprogs cscope meld gdebi sharutils thunderbird \
	transmission-cli brasero ibus shotwell startupmanager soundconverter gstreamer0.10-plugins-ugly-multiverse

#bluefish sqlite3 sqlite3-doc libsqlite3-0 libsqlite3-dev lftp atftp atftpd nfs-kernel-server openssh-client openssh-server gimp
sudo apt-get autoremove
sudo apt-get autoclean
