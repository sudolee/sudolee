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

Confirm ':: Make sure [multilib] in /etc/pacman.conf enabled,' || { echo '[Warning] - multilib must enabled on x86_64.'; exit; }
Confirm ':: Make sure network accessible,' || { echo '[Warning] - network must be configured, :('; exit; }
pacman -Syy

#### new user create ####
read -p ":: Enter your new user name : " NewUserName
[[ -z "$NewUserName" ]] && { echo ":: [Warning] - Cannot coutinue without new user name, :("; exit; }
Confirm ":: Will create new user \"$NewUserName\"," || { echo ":: [Warning] - Cannot continue without new UserName. :("; exit; }

useradd -m -g users -s /bin/bash $NewUserName
echo ":: Enter passwd for new user \"$NewUserName\"." && passwd $NewUserName
for i in network video audio disk floppy storage uucp
do
	echo "Add $NewUserName into group $i"
	gpasswd -a $NewUserName $i
done

#### desktop setting ####
read -p ":: witch desktop do you want ?
1) gnome, 2) kde

Enter a selection (default=none): " DESKTOPNAME
case $DESKTOPNAME in
	[gG][nN][oO][mM][eE]|1)
		DESKTOPNAME="gnome"
		echo ":: desktop gnome chose"
		;;
	[kK][dD][eE]|2)
		DESKTOPNAME="kde"
		echo ":: desktop kde chose"
		;;
	*)
        DESKTOPNAME=
		echo ":: Defaultly, set desktop as none"
		;;
esac

#### VGA driver & xorg & ttf ####
if [ -n "$DESKTOPNAME" ]; then
	GPU_INTEL=
	GPU_NIVIDA=
	GPU_ATI=
	BLUETOOTH=
	TTOUCHPAD=

	Confirm ":: Have intel  GPU ?"         && GPU_INTEL=true
	Confirm ":: Have nivida GPU ?"         && GPU_NIVIDA=true
	Confirm ":: Have ati    GPU ?"         && GPU_ATI=true
	Confirm ":: Have bluetooth hardware ?" && BLUETOOTH=true
	Confirm ":: Have thinkpad touchpad ?"  && TTOUCHPAD=true

    # don't intall lib32-nvidia-libgl
	[ "$GPU_NIVIDA" ] && { \
        $archinstallcmd nvidia nvidia-utils bumblebee; \
		$archinstallcmd bumblebee bbswitch && gpasswd -a $NewUserName bumblebee; }

	[ "$GPU_ATI" ]    && $archinstallcmd xf86-video-ati
    # install intel after nvidia
	[ "$GPU_INTEL" ]  && $archinstallcmd xf86-video-intel

	$archinstallcmd xorg-server mesa
	[ "$TTOUCHPAD" ] && mkdir -p /etc/X11/xorg.conf.d && cp -f ./config/{20-thinkpad.conf,synaptics.conf} /etc/X11/xorg.conf.d/
fi

#### fonts ####
$archinstallcmd ttf-dejavu ttf-liberation wqy-zenhei

#### base package ####
$archinstallcmd base-devel xf86-input-synaptics

#### desktop env ####
if [ "$DESKTOPNAME" = "gnome" ];then
	$archinstallcmd gnome gnome-extra \
		fcitx-im fcitx-configtool fcitx-cloudpinyin
	systemctl enable gdm.service
elif [ "$DESKTOPNAME" = "kde" ];then
	$archinstallcmd plasma sddm kde-applications kde-l10n-zh_cn \
		fcitx-im fcitx-cloudpinyin
	systemctl enable sddm.service
fi

#### networkmanager & ssh ####
$archinstallcmd networkmanager openssh
systemctl enable NetworkManager
systemctl enable sshd.service

#### development tools ####
$archinstallcmd \
	linux-headers man-pages \
	gcc binutils gcc-libs bison \
	make cmake libtool autogen autoconf automake patchutils elfutils gdb diffutils \
	m4 bc gmp mpfr mpc ppl \
	gnupg gperf expect dejagnu guile gperftools \
	mtd-utils util-linux ntfs-3g exfat-utils e2fsprogs dosfstools \
	tar zip unzip bzip2 p7zip libzip zlib cpio \
	flex readline asciidoc rsync \
	git tig gitg \
	gawk sed markdown \
	python3 scapy \
	hping libnet net-tools axel wget curl tcpdump tcpreplay acl iw ethtool \
	vim ghex ctags cscope tree \
	minicom ntp tftp-hpa acpid \
	zsh zsh-completions zsh-doc zsh-lovers zsh-syntax-highlighting \
	bash-completion screenfetch cpupower \
    valgrind tcl pkgconf docker
# zsh configure see: oh-my-zsh, theme: bira

gpasswd -a $NewUserName docker
newgrp docker # flush groups

#### wireshark & misc ####
if [ -n "$DESKTOPNAME" ]; then
	$archinstallcmd wireshark wireshark-cli
	if [ "$DESKTOPNAME" = "gnome" ];then
		$archinstallcmd wireshark-gtk meld
	elif [ "$DESKTOPNAME" = "kde" ];then
		$archinstallcmd wireshark-qt kdiff3
	fi
	gpasswd -a $NewUserName wireshark
	setcap 'CAP_NET_RAW+eip CAP_NET_ADMIN+eip' /usr/bin/dumpcap
fi

[ -f config/bashrc ] && cp config/bashrc /home/$NewUserName/.bashrc
if [ -n "$DESKTOPNAME" ]; then
	[ -f config/xprofile ] && cp config/xprofile /home/$NewUserName/.xprofile
fi

#### life style ####
$archinstallcmd fortune-mod cmatrix figlet

ntpd -g
hwclock -w

echo ':: Installations complete, :-)'
