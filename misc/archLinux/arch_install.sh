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

Confirm 'Make sure [multilib] in /etc/pacman.conf opened,' || { echo '[Warning] - multilib must enabled.'; exit 0; }

Confirm 'Make sure network accessible,' || { echo '[Warning] - network must be configured, :('; exit 0; }

pacman -Syy

read -p "--> Enter your new user name : " NewUserName
Confirm "Will create new user \"$NewUserName\"," || { echo "Cannot continue without new UserName. :("; exit; }

useradd -m -g users -s /bin/bash $NewUserName
echo "--> Enter passwd for new user \"$NewUserName\"." && passwd $NewUserName
for i in network video audio disk floppy storage uucp
do
	echo "Add $NewUserName into group $i"
	gpasswd -a $NewUserName $i
done

Confirm "Have intel  GPU ?"         && GPU_INTEL=true
Confirm "Have nivida GPU ?"         && GPU_NIVIDA=true
Confirm "Have ati    GPU ?"         && GPU_ATI=true
Confirm "Have bluetooth hardware ?" && BLUETOOTH=true
Confirm "Have thinkpad touchpad ?"  && TTOUCHPAD=true

[ "$GPU_INTEL" ]  && $archinstallcmd xf86-video-intel lib32-intel-dri
[ "$GPU_NIVIDA" ] && { $archinstallcmd xf86-video-nouveau lib32-nouveau-dri;
	$archinstallcmd bumblebee bbswitch && gpasswd -a $NewUserName bumblebee; }
[ "$GPU_ATI" ]    && $archinstallcmd xf86-video-ati lib32-ati-dri

$archinstallcmd base-devel xorg-server mesa xf86-input-synaptics xf86-input-keyboard xf86-input-mouse
$archinstallcmd kde-meta kde-l10n-zh_cn kdemultimedia phonon-gstreamer ttf-dejavu \
	ttf-liberation wqy-zenhei archlinux-themes-kdm kdeplasma-applets-plasma-nm \
	appmenu-qt
$archinstallcmd networkmanager openssh

# thinkpad touchpad setting
[ "$TTOUCHPAD" ] && mkdir -p /etc/X11/xorg.conf.d && cp -f ./config/{20-thinkpad.conf,synaptics.conf} /etc/X11/xorg.conf.d/

systemctl enable NetworkManager
systemctl enable kdm.service
systemctl enable sshd.socket

$archinstallcmd \
	a52dec faac faad2 flac jasper lame libdca libdv libmad libmpeg2 \
	libtheora libvorbis libxv wavpack x264 xvidcore \
	alsa-utils alsa-plugins dbus libsamplerate pulseaudio pulseaudio-alsa kdemultimedia-kmix \
	gst-plugins-good gstreamer0.10-good-plugins \
	vlc skype \
	fcitx-im fcitx-googlepinyin kcm-fcitx \
	bash-completion screenfetch cpupower flashplugin

. /usr/share/bash-completion/bash_completion

cat > /home/$NewUserName/.xprofile <<- EOF
export GTK_IM_MODULE=fcitx
export QT_IM_MODULE=fcitx
export XMODIFIERS="@im=fcitx"
EOF

[ "$BLUETOOTH" ] && $archinstallcmd bluedevil

$archinstallcmd chromium thunderbird thunderbird-i18n-zh-cn \
	libreoffice-en-US libreoffice-kde4 libreoffice-writer \
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
	git subversion mercurial quilt \
	gawk sed lua tcl tk python perl python-markdown \
	ntfs-3g exfat-utils e2fsprogs util-linux dosfstools \
	net-tools axel wget curl tcpdump tcpreplay acl iw ethtool wireshark-cli wireshark-gtk \
	m4 gmp mpfr mpc ppl cloog lib32-ncurses lib32-readline lib32-zlib libx11 libestr \
	vim meld indent kdiff3 \
	bash-completion linux-manpages minicom ntp \
	sox netpbm  # for fax

gpasswd -a $NewUserName wireshark
setcap 'CAP_NET_RAW+eip CAP_NET_ADMIN+eip' /usr/bin/dumpcap

pushd /usr/bin/
[ -f python2 ] && { rm -fv python; ln -sv python2 python; }
[ -f vim ]     && { rm -fv vi;     ln -sv vim vi; }
popd

ntpd -gq
hwclock -w

echo '--> Installations complete, :)'
