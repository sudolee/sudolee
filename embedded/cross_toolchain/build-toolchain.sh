#!/bin/bash -e
# build arm cross toolchain from source code by <desoxydate@gmail.com>

# make -- Specifies the number of jobs to run simultaneously.
CPUS=$(grep -c processor /proc/cpuinfo)
JOBS=$((2*$CPUS))
PKGVERSION="Matti Lee @ $(date +%Y%m%e)"

TARGET=arm-linux-gnueabi
HOST=$MACHTYPE
BUILD=$MACHTYPE

CTC_PRO_ROOT=$HOME/ctc
PREFIX=$CTC_PRO_ROOT/arm-cross-toolchain
PRO_PACKAGES=$CTC_PRO_ROOT/packages
SYSROOT=$PREFIX/$TARGET/sys-root
TMP_INSTALL=$CTC_PRO_ROOT/tmp-install

# export project root for uclibc
export CTC_PRO_ROOT
export PATH=$TMP_INSTALL/bin:$PREFIX/bin:$PREFIX/bin:$PATH

rm    -rf $PREFIX $TMP_INSTALL $SYSROOT
mkdir -pv $PREFIX $TMP_INSTALL $SYSROOT/usr

# Packages version define....
BC_VERSION=bc-1.06
KERNEL_VERSION=linux-3.17.6
M4_VERSION=m4-1.4.17
GMP_VERSION=gmp-6.0.0
MPFR_VERSION=mpfr-3.1.2
MPC_VERSION=mpc-1.0.2
ISL_VERSION=isl-0.12.2
CLOOG_VERSION=cloog-0.18.1
BINUTILS_VERSION=binutils-2.24
GCC__VERSION=gcc-4.8.3
UCLIBC_VERSION=uClibc-0.9.33.2
FILE_VERSION=file-5.20
NCURSES_VERSION=ncurses-5.9

cd $PRO_PACKAGES

############### file ###############
rm -rf $FILE_VERSION
tar zxf $FILE_VERSION.tar.gz
pushd $FILE_VERSION

./configure --prefix=$TMP_INSTALL
make -j$JOBS
make install
popd

############### bc ###############
rm -rf  $BC_VERSION
tar xzf $BC_VERSION.tar.gz
pushd $BC_VERSION

./configure --prefix=$TMP_INSTALL
make -j$JOBS
make install
popd

############### kernel headers ###############
pushd $KERNEL_VERSION

make mrproper
make ARCH=arm headers_check
make ARCH=arm INSTALL_HDR_PATH=$SYSROOT/usr headers_install
popd

############### m4 ###############
rm -rf $M4_VERSION
tar jxf $M4_VERSION.tar.bz2
pushd $M4_VERSION

./configure --prefix=$TMP_INSTALL --enable-c++
make -j$JOBS
make install
popd

############### ncurses ###############
rm -rf $NCURSES_VERSION
tar zxf $NCURSES_VERSION.tar.gz
pushd $NCURSES_VERSION

./configure --prefix=$TMP_INSTALL --without-debug --without-shared
make -C include
make -C progs tic -j$JOBS
make install
popd

############### gmp ###############
rm -rf $GMP_VERSION
tar jxf $GMP_VERSION.tar.bz2
pushd $GMP_VERSION

CPPFLAGS=-fexceptions \
    ./configure --prefix=$TMP_INSTALL \
    --enable-cxx --enable-fft --disable-shared --enable-static
make -j$JOBS
make install-strip
popd

############### mpfr ###############
rm -rf $MPFR_VERSION
tar jxf $MPFR_VERSION.tar.bz2
pushd $MPFR_VERSION

LDFLAGS="-Wl,-rpath,$TMP_INSTALL/lib" \
    ./configure --prefix=$TMP_INSTALL --with-gmp=$TMP_INSTALL --disable-shared --enable-static
make -j$JOBS
make install-strip
popd

############### mpc ###############
rm -rf $MPC_VERSION
tar zxf $MPC_VERSION.tar.gz
pushd $MPC_VERSION

LDFLAGS="-Wl,-rpath,$TMP_INSTALL/lib" \
    ./configure --prefix=$TMP_INSTALL \
    --with-mpfr=$TMP_INSTALL --with-gmp=$TMP_INSTALL --disable-shared --enable-static
make -j$JOBS
make install-strip
popd

############### isl ###############
rm -rf $ISL_VERSION
tar jxf $ISL_VERSION.tar.bz2
pushd $ISL_VERSION

LDFLAGS="-Wl,-rpath,$TMP_INSTALL/lib" \
    ./configure --prefix=$TMP_INSTALL \
    --disable-shared --enable-static --with-gmp-prefix=$TMP_INSTALL
make -j$JOBS
make install-strip
popd

############### CLooG ###############
rm -rf $CLOOG_VERSION
tar zxf $CLOOG_VERSION.tar.gz
pushd $CLOOG_VERSION

LDFLAGS="-Wl,-rpath,$TMP_INSTALL/lib" \
    ./configure --prefix=$TMP_INSTALL \
    --with-gmp-prefix=$TMP_INSTALL --with-isl-prefix=$TMP_INSTALL --disable-shared --enable-static
make -j$JOBS
make install-strip
popd

############### binutils ###############
rm -rf $BINUTILS_VERSION
tar jxf $BINUTILS_VERSION.tar.bz2

rm -rf binutils_build
mkdir  binutils_build
pushd  binutils_build

AR=ar AS=as \
	$PRO_PACKAGES/binutils_build/../$BINUTILS_VERSION/configure --prefix=$PREFIX \
	--build=$HOST --target=$TARGET --with-sysroot=$SYSROOT \
	--with-mpc=$TMP_INSTALL --with-mpfr=$TMP_INSTALL --with-gmp=$TMP_INSTALL \
	--with-cloog=$TMP_INSTALL --with-isl=$TMP_INSTALL --disable-isl-version-check \
	--disable-nls --disable-shared --enable-64-bit-bfd \
	--disable-werror
make -j$JOBS
make install
popd

############### gcc static ###############
rm -rf $GCC__VERSION
tar jxf $GCC__VERSION.tar.bz2

rm -rf gcc_build_static
mkdir gcc_build_static
pushd gcc_build_static

AR=ar LDFLAGS="-Wl,-rpath,$PREFIX/lib" \
    $PRO_PACKAGES/gcc_build_static/../$GCC__VERSION/configure --prefix=$PREFIX \
    --build=$BUILD --host=$HOST --target=$TARGET --with-sysroot=$SYSROOT  \
    --with-local-prefix=$PREFIX \
    --with-native-system-headers-dir=$SYSROOT/usr/include \
    --with-mpfr=$TMP_INSTALL --with-gmp=$TMP_INSTALL --with-mpc=$TMP_INSTALL --with-cloog=$TMP_INSTALL --with-isl=$TMP_INSTALL \
    --with-newlib --without-headers --disable-threads --enable-languages=c \
    --disable-nls --disable-shared \
    --disable-libgomp --disable-libmudflap --disable-libssp \
    --enable-lto
#--with-host-libstdcxx="-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm"
make all-gcc all-target-libgcc -j$JOBS
make install-gcc install-target-libgcc
popd

############### uclibc ###############
rm -rf $UCLIBC_VERSION
tar xf $UCLIBC_VERSION.tar.xz

pushd $UCLIBC_VERSION
patch -p1 < ../uClibc_sysinfo.h_unknown_type_name__kernel_long_t.patch
cp -vf ../uClibc-0.9.33.2_config .config

make -j$JOBS
make install
popd

############### gcc final ###############
rm -rf $GCC__VERSION
tar jxf $GCC__VERSION.tar.bz2

rm -rf gcc_build_final
mkdir gcc_build_final
pushd gcc_build_final

AR=ar LDFLAGS="-Wl,-rpath,$PREFIX/lib" \
    $PRO_PACKAGES/gcc_build_final/../$GCC__VERSION/configure --prefix=$PREFIX \
    --build=$BUILD --host=$HOST --target=$TARGET \
    --with-sysroot=$SYSROOT --with-local-prefix=$PREFIX \
    --with-native-system-headers-dir=$SYSROOT/usr/include \
    --enable-languages=c,c++ --enable-__cxa_atexit \
    --with-mpfr=$TMP_INSTALL --with-gmp=$TMP_INSTALL --with-mpc=$TMP_INSTALL --with-isl=$TMP_INSTALL --with-cloog=$TMP_INSTALL \
    --enable-c99 --enable-threads=posix --enable-lto \
	--with-pkgversion="${PKGVERSION}"
#    --with-host-libstdcxx="-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm" \
# --enable-languages=c,c++,fortran,objc,obj-c++
make -j$JOBS
make install
popd
