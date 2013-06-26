#!/bin/bash

pushd /usr/bin/
sudo rm python; sudo ln -s python2 python
sudo rm vi; sudo ln -s vim vi
popd


sudo pacman -S --needed libtool autoconf automake gcc-libs-multilib bison screen gcc-multilib binutils-multilib \
	patchutils bzip2 flex make gettext zip unzip libestr ncurses zlib gawk quilt asciidoc libzip rsync lua \
	linux-headers linux-manpages ctags acl rrdtool elfutils mercurial texinfo git tcl tk curl \
	kdiff3 meld lib32-ncurses readline man-pages man2html xml2 \
	ntfs-3g exfat-utils e2fsprogs util-linux dosfstools \
	gperf gperftools perl-switch

curl https://dl-ssl.google.com/dl/googlesource/git-repo/repo > repo
chmod a+x repo
sudo mv repo /usr/local/bin/

echo "\
SUBSYSTEM==\"usb\", ATTR{idVendor}==\"18d1\", MODE=\"0660\", OWNER=\"$USER\"" \
> 51-android.rules
sudo mv 51-android.rules /etc/udev/rules.d/
