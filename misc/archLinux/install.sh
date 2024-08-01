#!/bin/bash -e

archinstallcmd='pacman -S --needed --noconfirm -q'

Confirm () {
    # call with a prompt string or use a default
    read -p "--> ${1:-Are you sure ?} [Y/n] " response
    case $response in
        [nN]|[nN][oO])
            false
            ;;
        *)
            true
            ;;
    esac
}

pacman -Syy

#### new user create ####
read -p ":: Enter your new user name : " NewUserName
[[ -z "$NewUserName" ]] && { echo ":: [Warning] - Cannot coutinue without new user name, :("; exit; }
Confirm ":: Will create new user \"$NewUserName\"," || { echo ":: [Warning] - Cannot continue without new UserName. :("; exit; }

useradd -m -g users,wheel -s /bin/bash $NewUserName
echo ":: Enter passwd for new user \"$NewUserName\"." && passwd $NewUserName

#### base package ####
$archinstallcmd base-devel sudo
# Uncomment to allow members of group wheel to execute any command
sed -i 's/^# %wheel ALL=(ALL:ALL) ALL$/%wheel ALL=(ALL:ALL) ALL/g' /etc/sudoers


#### fonts & input ####
$archinstallcmd xf86-input-libinput fcitx ttf-fira-code ttf-firacode-nerd adobe-source-han-serif-cn-fonts
# wqy-microhei

#### desktop env ####

#### networkmanager & ssh ####
$archinstallcmd networkmanager openssh
systemctl enable NetworkManager
systemctl enable sshd.service

#### development tools ####
$archinstallcmd \
    linux-headers \
    man-db man-pages man-pages-zh_cn \
    gcc binutils gcc-libs bison \
    make cmake libtool autogen autoconf automake patchutils elfutils gdb diffutils \
    util-linux ntfs-3g exfat-utils e2fsprogs dosfstools \
    tar zip unzip bzip2 p7zip libzip zlib cpio \
    flex readline asciidoc rsync \
    git tig sed \
    python3 python-pip scapy \
    net-tools axel wget curl tcpdump tcpreplay iw ethtool \
    vim ctags cscope tree \
    minicom \
    zsh zsh-completions zsh-doc zsh-syntax-highlighting \
    bash-completion screenfetch cpupower \
    nodejs npm \
    docker
    # zsh configure see: oh-my-zsh, theme: bira

gpasswd -a $NewUserName docker

#### wireshark & misc ####
gpasswd -a $NewUserName wireshark
setcap 'CAP_NET_RAW+eip CAP_NET_ADMIN+eip' /usr/bin/dumpcap

#### user config
[ -f config/bashrc ] && cp config/bashrc /home/$NewUserName/.bashrc
[ -f config/inputrc ] && cp config/inputrc /home/$NewUserName/.inputrc
[ -f config/fcitx.sh ] && bash ./config/fcitx.sh

#### life style ####
$archinstallcmd fortune-mod figlet

echo ':: Installations complete, :-)'
newgrp docker # flush groups make script exit
