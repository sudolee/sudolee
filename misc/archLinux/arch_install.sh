#!/bin/bash

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

Confirm ":: Have intel  GPU ?"         && GPU_INTEL=true
Confirm ":: Have nivida GPU ?"         && GPU_NIVIDA=true
Confirm ":: Have ati    GPU ?"         && GPU_ATI=true
Confirm ":: Have bluetooth hardware ?" && BLUETOOTH=true
Confirm ":: Have thinkpad touchpad ?"  && TTOUCHPAD=true

# desktop setting
DESKTOPNAME="gnome"
read -p ":: witch desktop do you want ?
1) gnome, 2) kde

Enter a selection (default=gnome): " DESKTOPNAME
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
		DESKTOPNAME="gnome"
		echo ":: Defaultly, set desktop as gnome"
		;;
esac

[ "$GPU_INTEL" ]  && $archinstallcmd xf86-video-intel lib32-intel-dri
[ "$GPU_NIVIDA" ] && { $archinstallcmd xf86-video-nouveau lib32-nouveau-dri;
	$archinstallcmd bumblebee bbswitch && gpasswd -a $NewUserName bumblebee; }
[ "$GPU_ATI" ]    && $archinstallcmd xf86-video-ati lib32-ati-dri

$archinstallcmd base-devel xorg-server mesa xf86-input-synaptics xf86-input-keyboard xf86-input-mouse

if [ "$DESKTOPNAME" = "gnome" ];then
	$archinstallcmd gnome gnome-extra \
		libreoffice-gnome \
		fcitx fcitx-gtk2 fcitx-gtk3 fcitx-googlepinyin fcitx-configtool
	systemctl enable gdm.service
	# gnome-terminal: keep track of directory in new tab
	[[ -f config/bashrc ]] && echo '. /etc/profile.d/vte.sh' >> config/bashrc
elif [ "$DESKTOPNAME" = "kde" ];then
	$archinstallcmd kde-meta kde-l10n-zh_cn kdemultimedia kdeplasma-applets-plasma-nm \
		kdemultimedia-kmix archlinux-themes-kdm appmenu-qt \
		libreoffice-kde4 kdiff3 \
		fcitx fcitx-qt4 fcitx-googlepinyin kcm-fcitx
	systemctl enable kdm.service
fi

$archinstallcmd ttf-dejavu ttf-liberation wqy-zenhei

$archinstallcmd networkmanager openssh
systemctl enable NetworkManager
systemctl enable sshd.socket

# thinkpad touchpad setting
[ "$TTOUCHPAD" ] && mkdir -p /etc/X11/xorg.conf.d && cp -f ./config/{20-thinkpad.conf,synaptics.conf} /etc/X11/xorg.conf.d/

$archinstallcmd \
	a52dec faac faad2 flac jasper lame libdca libdv libmad libmpeg2 \
	libtheora libvorbis libxv wavpack x264 xvidcore \
	alsa-utils alsa-plugins dbus libsamplerate pulseaudio pulseaudio-alsa \
	gst-plugins-good gstreamer0.10-good-plugins \
	vlc skype \
	bash-completion screenfetch cpupower flashplugin

[ "$BLUETOOTH" ] && $archinstallcmd bluedevil

$archinstallcmd chromium thunderbird thunderbird-i18n-zh-cn \
	libreoffice-en-US libreoffice-writer \
	libreoffice-calc libreoffice-draw libreoffice-impress \
	poppler-data

$archinstallcmd virtualbox virtualbox-host-modules
gpasswd -a $NewUserName vboxusers
[ -f /etc/modules-load.d/virtualbox.conf ] || echo vboxdrv > /etc/modules-load.d/virtualbox.conf

$archinstallcmd linux-headers gcc binutils gcc-libs bison make \
	libtool autogen autoconf automake patchutils elfutils gdb diffutils \
	gnupg gperf expect dejagnu guile gperftools \
	tar zip unzip bzip2 p7zip libzip zlib \
	flex gettext ncurses readline asciidoc rsync ctags cscope rrdtool texinfo \
	git gitg subversion mercurial quilt \
	gawk sed lua tcl tk perl markdown \
	python python2 python-markdown python2-pyopenssl python-pyopenssl scapy \
	ntfs-3g exfat-utils e2fsprogs util-linux dosfstools \
	hping libnet net-tools axel wget curl tcpdump tcpreplay acl iw ethtool wireshark-cli wireshark-gtk \
	m4 bc gmp mpfr mpc ppl cloog lib32-ncurses lib32-readline lib32-zlib libx11 libestr \
	vim meld indent tree jdk7-openjdk \
	linux-manpages minicom ntp \
	pm-utils acpid \
	sox netpbm  # for fax

gpasswd -a $NewUserName wireshark
setcap 'CAP_NET_RAW+eip CAP_NET_ADMIN+eip' /usr/bin/dumpcap

pushd /usr/bin/
[ -f python2 ] && { rm -fv python; ln -sv python2 python; }
[ -f vim ]     && { rm -fv vi;     ln -sv vim vi; }
popd

[ -f config/bashrc ] && cp config/bashrc /home/$NewUserName/.bashrc
[ -f config/xprofile ] && cp config/xprofile /home/$NewUserName/.xprofile

. /usr/share/bash-completion/bash_completion

ntpd -gq
hwclock -w

echo ':: Installations complete, :)'
