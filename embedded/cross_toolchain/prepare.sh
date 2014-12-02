#!/bin/bash

PACKAGES=$HOME/cross/packages

# Packages....
BC_VERSION=bc-1.06
KERNEL_VERSION=linux-3.12.12
FILE_VERSION=file-5.20
M4_VERSION=m4-1.4.17
GMP_VERSION=gmp-6.0.0
MPFR_VERSION=mpfr-3.1.2
MPC_VERSION=mpc-1.0.2
ISL_VERSION=isl-0.12.2
CLOOG_VERSION=cloog-0.18.1
BINUTILS_VERSION=binutils-2.24
GCC__VERSION=gcc-4.8.3
GLIBC_VERSION=glibc-2.19
NCURSES_VERSION=ncurses-5.9
MAKE_VERSION=make-3.82

## Request softwares...
#sudo apt-get install gcc gnat gawk binutils gzip bzip2 make tar perl \
#	zip unzip autoconf m4 automake gettext gperf dejagnu expect tcl autogen \
#	guile-2.0 flex texinfo subversion ssh diffutils patch ecj libtool byacc bison fixincludes \
#	git-core gnupg build-essential libc6 libc6-dev libncurses5-dev libreadline6-dev g++-multilib \
#	axel wget
#   pkg-config

## Prepare packages...
mkdir -p $PACKAGES
pushd $PACKAGES

# bc
if [ ! -f $BC_VERSION.tar.gz ];then
	axel -a ftp://ftp.gnu.org/gnu/bc/$BC_VERSION.tar.gz
fi

# kernel (long term kernel)
if [ ! -f ${KERNEL_VERSION}.tar.gz ];then
	case $KERNEL_VERSION in
		*-2.6.*)
			wget http://www.kernel.org/pub/linux/kernel/v2.6/${KERNEL_VERSION}.tar.gz
			;;
		*-3.*)
			wget http://www.kernel.org/pub/linux/kernel/v3.x/${KERNEL_VERSION}.tar.gz
			;;
		*)
			echo "No kernel matched. Abort..."
			exit
			;;
	esac
fi
if [ ! -e $KERNEL_VERSION ];then
	tar -zxf ${KERNEL_VERSION}.tar.gz
fi

# file
if [ ! -f $FILE_VERSION.tar.gz ];then
	axel -a ftp://ftp.astron.com/pub/file/$FILE_VERSION.tar.gz
fi

# ncurses
if [ ! -f $NCURSES_VERSION.tar.gz ];then
	axel -a ftp://ftp.gnu.org/gnu/ncurses/$NCURSES_VERSION.tar.gz
fi

# m4
if [ ! -f $M4_VERSION.tar.bz2 ];then
	axel -a ftp://ftp.gnu.org/gnu/m4/$M4_VERSION.tar.bz2
fi

# gmp
if [ ! -f $GMP_VERSION.tar.bz2 ];then
	axel -a ftp://ftp.gnu.org/gnu/gmp/${GMP_VERSION}a.tar.bz2
    mv ${GMP_VERSION}a.tar.bz2 ${GMP_VERSION}.tar.bz2
fi

# mpfr
if [ ! -f $MPFR_VERSION.tar.bz2 ];then
	axel -a ftp://ftp.gnu.org/gnu/mpfr/${MPFR_VERSION}.tar.bz2
fi

# mpc
if [ ! -f $MPC_VERSION.tar.gz ];then
	axel -a http://www.multiprecision.org/mpc/download/$MPC_VERSION.tar.gz
fi

# cloog
if [ ! -f $CLOOG_VERSION.tar.gz ];then
	axel -a ftp://gcc.gnu.org/pub/gcc/infrastructure/$CLOOG_VERSION.tar.gz
fi

if [ ! -f $ISL_VERSION.tar.bz2 ];then
	axel -a http://isl.gforge.inria.fr/$ISL_VERSION.tar.bz2
fi

# binutils
if [ ! -f ${BINUTILS_VERSION}.tar.bz2 ];then
	axel -a ftp://ftp.gnu.org/gnu/binutils/${BINUTILS_VERSION}.tar.bz2
fi

# gcc
if [ ! -f $GCC__VERSION.tar.bz2 ];then
	axel -a ftp://ftp.gnu.org/gnu/gcc/$GCC__VERSION/$GCC__VERSION.tar.bz2
fi

# glibc
if [ ! -f ${GLIBC_VERSION}.tar.bz2 ];then
	axel -a ftp://ftp.gnu.org/gnu/glibc/${GLIBC_VERSION}.tar.bz2
fi

if [ ! -f ${MAKE_VERSION}.tar.gz ];then
#    axel -a ftp://ftp.gnu.org/gnu/make/${MAKE_VERSION}.tar.gz
fi

popd
