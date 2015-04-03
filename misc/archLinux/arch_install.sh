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

	[ "$GPU_INTEL" ]  && $archinstallcmd xf86-video-intel lib32-intel-dri
	[ "$GPU_NIVIDA" ] && $archinstallcmd xf86-video-nouveau
	[ "$GPU_ATI" ]    && $archinstallcmd xf86-video-ati lib32-ati-dri

	$archinstallcmd xorg-server xorg-utils mesa mesa-libgl lib32-mesa lib32-mesa-libgl
	[ "$TTOUCHPAD" ] && mkdir -p /etc/X11/xorg.conf.d && cp -f ./config/{20-thinkpad.conf,synaptics.conf} /etc/X11/xorg.conf.d/
fi

#### fonts ####
$archinstallcmd ttf-dejavu ttf-liberation wqy-zenhei ttf-dejavu ttf-liberation wqy-zenhei

#### base package ####
$archinstallcmd base-devel xf86-input-synaptics xf86-input-keyboard xf86-input-mouse

#### desktop env ####
if [ "$DESKTOPNAME" = "gnome" ];then
	$archinstallcmd gnome gnome-extra \
		fcitx fcitx-gtk2 fcitx-gtk3 fcitx-googlepinyin fcitx-configtool
	systemctl enable gdm.service
elif [ "$DESKTOPNAME" = "kde" ];then
	$archinstallcmd kde-meta kde-l10n-zh_cn kdemultimedia kdeplasma-applets-plasma-nm \
		kdemultimedia-kmix archlinux-themes-kdm appmenu-qt \
		fcitx-im fcitx-googlepinyin kcm-fcitx
	systemctl enable kdm.service
fi

#### networkmanager & ssh ####
$archinstallcmd networkmanager openssh
systemctl enable NetworkManager
systemctl enable sshd.socket

#### multimedia & virtualbox ####
if [ -n "$DESKTOPNAME" ]; then
	$archinstallcmd \
		a52dec faac faad2 flac jasper lame libdca libdv libmad libmpeg2 \
		libtheora libvorbis libxv wavpack x264 xvidcore \
		alsa-utils alsa-plugins dbus libsamplerate pulseaudio pulseaudio-alsa \
		gst-plugins-good gstreamer0.10-good-plugins gst-libav gst-plugins-ugly \
		skype flashplugin \
		chromium thunderbird thunderbird-i18n-zh-cn \
		poppler-data
	## calibre     <- for ebook
	## sox netpbm  <- for fax

	[ "$BLUETOOTH" ] && $archinstallcmd bluedevil

	$archinstallcmd virtualbox virtualbox-host-modules
	gpasswd -a $NewUserName vboxusers
#	[ -f /etc/modules-load.d/virtualbox.conf ] || echo vboxdrv > /etc/modules-load.d/virtualbox.conf
fi

#### development tools ####
$archinstallcmd \
	linux-headers linux-manpages \
	gcc binutils gcc-libs bison jdk8-openjdk clang \
	make cmake libtool autogen autoconf automake patchutils elfutils gdb diffutils \
	gnupg gperf expect dejagnu guile gperftools \
	mtd-utils util-linux ntfs-3g exfat-utils e2fsprogs dosfstools \
	tar zip unzip bzip2 p7zip libzip zlib cpio \
	flex gettext ncurses readline asciidoc rsync rrdtool texinfo \
	git subversion mercurial quilt tig gitg \
	gawk sed lua tcl tk perl markdown \
	python python2 python-markdown python2-pyopenssl python-pyopenssl scapy \
	hping libnet net-tools axel wget curl tcpdump tcpreplay acl iw ethtool \
	m4 bc gmp mpfr mpc ppl lib32-ncurses lib32-readline lib32-zlib libx11 libestr \
	vim ghex ctags cscope indent tree \
	minicom ntp \
	pm-utils acpid \
	bash-completion screenfetch cpupower

pushd /usr/bin/
[ -f python2 ] && { rm -fv python; ln -sv python2 python; }
[ -f vim ]     && { rm -fv vi;     ln -sv vim vi; }
popd

#### wireshark & misc ####
if [ -n "$DESKTOPNAME" ]; then
	$archinstallcmd wireshark-cli libreoffice-fresh libreoffice-fresh-zh-CN
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
$archinstallcmd fortune-mod

ntpd -g
hwclock -w

echo ':: Installations complete, :-)'
