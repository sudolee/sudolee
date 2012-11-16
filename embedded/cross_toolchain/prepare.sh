#!/bin/bash

KERNEL_VERSION=linux-3.0.41
#5.09
FILE_VERSION=file-5.11
NCURSES_VERSION=ncurses-5.9
#5.02
GMP_VERSION=gmp-5.0.4
#3.0.1
MPFR_VERSION=mpfr-3.1.1
MPC_VERSION=mpc-0.9
#0.11.2
PPL_VERSION=ppl-0.12
CLOOG_VERSION=cloog-ppl-0.15.11
BINUTILS_VERSION=binutils-2.21.1a
GCC__VERSION=gcc-4.6.3
#2.15 this version cannt be compiled ???
GLIBC_VERSION=2.14.1
M4_VERSION=m4-1.4.16
GDB_VERSION=gdb-7.3.1
LIBELF_VERSION=libelf-0.8.13

## Request softwares...
sudo apt-get install gcc gnat gawk binutils gzip bzip2 make tar perl \
	zip unzip autoconf m4 automake gettext gperf dejagnu expect tcl autogen \
	guile-2.0 flex texinfo subversion ssh diff patch ecj libtool byacc bison fixincludes \
	git-core gnupg build-essential libc6 libc6-dev libncurses5-dev libreadline6-dev g++-multilib msgfmt \
	axel

## Prepare packages...

# kernel (long term kernel)
if [ ! -f ${KERNEL_VERSION}.tar.bz2 ];then
	case $KERNEL_VERSION in
		*-2.6.*)
			axel -a http://www.kernel.org/pub/linux/kernel/v2.6/${KERNEL_VERSION}.tar.bz2
			;;
		*-3.*)
			axel -a http://www.kernel.org/pub/linux/kernel/v3.x/${KERNEL_VERSION}.tar.bz2
			;;
		*)
			echo "No kernel matched. Abort..."
			exit
			;;
	esac
fi
if [ ! -e $KERNEL_VERSION ];then
	tar -jxf ${KERNEL_VERSION}.tar.bz2
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
	axel -a ftp://ftp.gnu.org/gnu/gmp/${GMP_VERSION}.tar.bz2
fi

# mpfr
if [ ! -f $MPFR_VERSION.tar.bz2 ];then
	axel -a ftp://ftp.gnu.org/gnu/mpfr/${MPFR_VERSION}.tar.bz2
fi

# mpc
if [ ! -f $MPC_VERSION.tar.gz ];then
	axel -a http://www.multiprecision.org/mpc/download/$MPC_VERSION.tar.gz
fi

# ppl (with pwl)
if [ ! -f ppl-0.11.2.tar.bz2 ];then
	#axel -a ftp://ftp.cs.unipr.it/pub/ppl/releases/0.11.2/ppl-0.11.2.tar.bz2
	axel -a ftp://ftp.cs.unipr.it/pub/ppl/releases/0.12/ppl-0.12.tar.bz2
fi

# cloog (base on ppl)
if [ ! -f $CLOOG_VERSION.tar.gz ];then
	axel -a ftp://gcc.gnu.org/pub/gcc/infrastructure/$CLOOG_VERSION.tar.gz
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
if [ ! -f glibc-${GLIBC_VERSION}.tar.bz2 ];then
	axel -a ftp://ftp.gnu.org/gnu/glibc/glibc-${GLIBC_VERSION}.tar.bz2
	axel -a ftp://ftp.gnu.org/gnu/glibc/glibc-ports-${GLIBC_VERSION}.tar.bz2
fi

# gdb
#if [ ! -f $GDB_VERSION.tar.bz2 ];then
#	axel -a ftp://ftp.gnu.org/gnu/gdb/$GDB_VERSION.tar.bz2
#fi

# libelf-0.8.13
if [ ! -f $LIBELF_VERSION.tar.gz ];then
	axel -a http://www.mr511.de/software/$LIBELF_VERSION.tar.gz
fi
