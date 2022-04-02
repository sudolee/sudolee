#!/bin/bash

sudo apt-get install \
    fcitx fcitx-googlepinyin im-config \
    linux-headers-amd64 \
    vim cscope universal-ctags indent \
    bash-completion tree gawk sed \
    git tig minicom elfutils patch patchutils diffutils \
    libtool autoconf automake autogen gettext texinfo build-essential \
    exfat-utils ntfs-3g e2fsprogs dosfstools \
    libzip4 zlib1g zip unzip \
    gcc gcc-multilib g++ g++-multilib cpp gdb flex binutils \
    net-tools ethtool inetutils-tools iw tcpdump tcpreplay htop rsync \
    make cmake gcc g++ \
    python3-pip python-dev python3-dev \
    ssh curl ca-certificates gnupg lsb-release \
    wireshark

# onie
#qemu-system-x86
#sphinx-doc sphinx-common


systemctl enable ssh.socket
sudo setcap 'CAP_NET_RAW+eip CAP_NET_ADMIN+eip' /usr/bin/dumpcap

pip3 install j2cli

curl -fsSL https://download.docker.com/linux/debian/gpg | sudo gpg --dearmor -o /usr/share/keyrings/docker-archive-keyring.gpg
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] https://download.docker.com/linux/debian \
  $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
sudo apt update
sudo apt install docker-ce docker-ce-cli containerd.io
sudo usermod -a -G docker $user_name
newgrp docker
# set for wireshark
# lingot  for musical tuner
# bumblebee primus firmware-iwlwifi \
# libstdc++
# use pcmanfm instead of nautilus in gnome
