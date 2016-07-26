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
        $archinstallcmd nvidia nvidia-utils lib32-nvidia-utils bumblebee; \
		$archinstallcmd bumblebee bbswitch && gpasswd -a $NewUserName bumblebee; }

	[ "$GPU_ATI" ]    && $archinstallcmd xf86-video-ati lib32-ati-dri
    # install intel after nvidia
	[ "$GPU_INTEL" ]  && $archinstallcmd xf86-video-intel lib32-intel-dri

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
systemctl enable sshd.socket

#### multimedia & virtualbox ####
if [ -n "$DESKTOPNAME" ]; then
	$archinstallcmd \
		a52dec faac faad2 flac jasper lame libdca libdv libmad libmpeg2 \
		libtheora libvorbis libxv wavpack x264 xvidcore \
		mplayer \
		alsa-utils alsa-plugins dbus libsamplerate pulseaudio pulseaudio-alsa \
		gst-plugins-good gstreamer0.10-good-plugins gst-libav gst-plugins-ugly \
		skype flashplugin \
		chromium thunderbird thunderbird-i18n-zh-cn \
		poppler-data
	## calibre     <- for ebook
	## sox netpbm  <- for fax
    ## cmatrix

	[ "$BLUETOOTH" ] && $archinstallcmd bluedevil

    # /usr/lib/virtualbox/additions/VBoxGuestAdditions.iso
	$archinstallcmd virtualbox virtualbox-host-modules-arch virtualbox-guest-iso
	gpasswd -a $NewUserName vboxusers
	systemctl enable systemd-modules-load.service
fi

#### development tools ####
$archinstallcmd \
	linux-headers linux-manpages \
	gcc binutils gcc-libs bison jdk8-openjdk clang \
	make cmake libtool autogen autoconf automake patchutils elfutils gdb diffutils \
	m4 bc gmp mpfr mpc ppl lib32-ncurses lib32-readline lib32-zlib libx11 libestr \
	php gnupg gperf expect dejagnu guile gperftools \
	mtd-utils util-linux ntfs-3g exfat-utils e2fsprogs dosfstools \
	tar zip unzip bzip2 p7zip libzip zlib cpio \
	flex gettext ncurses readline asciidoc rsync rrdtool texinfo \
	git subversion mercurial quilt cvs tig gitg perl-term-readkey perl-term-read-password \
	gawk sed lua tcl tk perl markdown \
	python python2 python-markdown python2-pyopenssl python-pyopenssl scapy \
	hping libnet net-tools axel wget curl tcpdump tcpreplay acl iw ethtool \
	vim ghex ctags cscope indent tree \
	minicom ntp tftp-hpa \
	pm-utils acpid \
	zsh zsh-completions zsh-doc zsh-lovers zsh-syntax-highlighting \
	bash-completion screenfetch cpupower
# zsh configure see: oh-my-zsh, theme: bira

pushd /usr/bin/
[ -f vim ] && { rm -fv vi; ln -sv vim vi; }
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
$archinstallcmd fortune-mod cmatrix figlet figlet-fonts

ntpd -g
hwclock -w

echo ':: Installations complete, :-)'
