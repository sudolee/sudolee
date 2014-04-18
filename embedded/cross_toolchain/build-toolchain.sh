#!/bin/bash
# build arm cross toolchain from source code by <desoxydate@gmail.com>

# make -- Specifies the number of jobs to run simultaneously.
CPUS=$(grep -c processor /proc/cpuinfo)
JOBS=$((2*$CPUS))
_DATE_=$(date +%Y%m%e)

TARGET=arm-linux-gnueabi
HOST=$MACHTYPE
BUILD=$MACHTYPE

PRO_ROOT=$HOME/cross
PREFIX=$PRO_ROOT/arm-cross-toolchain
PRO_PACKAGES=$PRO_ROOT/packages
SYSROOT=$PREFIX/$TARGET/sys-root
TMP_INSTALL=$PRO_ROOT/tmp-install

export PATH=$TMP_INSTALL/bin:$PREFIX/bin:$PREFIX/bin:$PATH

rm    -rf $PREFIX $TMP_INSTALL $SYSROOT
mkdir -pv $PREFIX $TMP_INSTALL $SYSROOT/usr

# Packages version define....
BC_VERSION=bc-1.06
KERNEL_VERSION=linux-3.12.12
M4_VERSION=m4-1.4.17
GMP_VERSION=gmp-5.1.3
MPFR_VERSION=mpfr-3.1.2
MPC_VERSION=mpc-1.0.1
ISL_VERSION=isl-0.12.1
CLOOG_VERSION=cloog-0.18.0
BINUTILS_VERSION=binutils-2.23.2
GCC__VERSION=gcc-4.8.1
GLIBC_VERSION=glibc-2.18
FILE_VERSION=file-5.15
NCURSES_VERSION=ncurses-5.9

cd $PRO_PACKAGES

## bc
rm -rf $BC_VERSION
tar xzf $BC_VERSION.tar.gz
pushd $BC_VERSION

./configure --prefix=$TMP_INSTALL
make -j$JOBS
make install-strip
popd

## kernel headers
pushd $KERNEL_VERSION

make mrproper
make ARCH=arm headers_check
make ARCH=arm INSTALL_HDR_PATH=$SYSROOT/usr headers_install
popd

## file
rm -rf $FILE_VERSION
tar zxf $FILE_VERSION.tar.gz
pushd $FILE_VERSION

./configure --prefix=$TMP_INSTALL
make -j$JOBS
make install-strip
popd

## m4
rm -rf $M4_VERSION
tar jxf $M4_VERSION.tar.bz2
pushd $M4_VERSION

./configure --prefix=$TMP_INSTALL --enable-c++
make -j$JOBS
make install-strip
popd

## ncurses
rm -rf $NCURSES_VERSION
tar zxf $NCURSES_VERSION.tar.gz
pushd $NCURSES_VERSION

./configure --prefix=$TMP_INSTALL --without-debug --without-shared
make -C include
make -C progs tic -j$JOBS
make install
popd

## gmp
rm -rf $GMP_VERSION
tar jxf $GMP_VERSION.tar.bz2
pushd $GMP_VERSION

CPPFLAGS=-fexceptions \
	./configure --prefix=$TMP_INSTALL \
	--enable-cxx --enable-fft --disable-shared --enable-static
make -j$JOBS
make install-strip
popd

## mpfr
rm -rf $MPFR_VERSION
tar jxf $MPFR_VERSION.tar.bz2
pushd $MPFR_VERSION

LDFLAGS="-Wl,-rpath,$TMP_INSTALL/lib" \
	./configure --prefix=$TMP_INSTALL --with-gmp=$TMP_INSTALL --disable-shared --enable-static
make -j$JOBS
make install-strip
popd

## mpc
rm -rf $MPC_VERSION
tar zxf $MPC_VERSION.tar.gz
pushd $MPC_VERSION

LDFLAGS="-Wl,-rpath,$TMP_INSTALL/lib" \
	./configure --prefix=$TMP_INSTALL \
	--with-mpfr=$TMP_INSTALL --with-gmp=$TMP_INSTALL --disable-shared --enable-static
make -j$JOBS
make install-strip
popd

## isl
rm -rf $ISL_VERSION
tar jxf $ISL_VERSION.tar.bz2
pushd $ISL_VERSION

LDFLAGS="-Wl,-rpath,$TMP_INSTALL/lib" \
	./configure --prefix=$TMP_INSTALL \
	--disable-shared --enable-static --with-gmp=$TMP_INSTALL
make -j$JOBS
make install-strip
popd

## CLooG
rm -rf $CLOOG_VERSION
tar zxf $CLOOG_VERSION.tar.gz
pushd $CLOOG_VERSION

LDFLAGS="-Wl,-rpath,$TMP_INSTALL/lib" \
	./configure --prefix=$TMP_INSTALL \
	--with-gmp=$TMP_INSTALL --with-isl-prefix=$TMP_INSTALL --disable-shared --enable-static
##	--with-gmp=$TMP_INSTALL --with-isl-prefix=$TMP_INSTALL --disable-shared --enable-static --with-host-libstdcxx='-lstdc++'
make -j$JOBS
make install-strip
popd

## binutils
rm -rf $BINUTILS_VERSION
tar jxf $BINUTILS_VERSION.tar.bz2
pushd $BINUTILS_VERSION

# bug fix, see: http://cross-lfs.org
sed -i -e 's/@colophon/@@colophon/' -e 's/doc@cygnus.com/doc@@cygnus.com/' bfd/doc/bfd.texinfo
popd

rm -rf binutils_build
mkdir  binutils_build
pushd  binutils_build

AR=ar AS=as \
	$PRO_PACKAGES/binutils_build/../$BINUTILS_VERSION/configure --prefix=$PREFIX \
	--target=$TARGET --build=$HOST --with-sysroot=$SYSROOT \
	--with-lib-path=$TMP_INSTALL/lib --disable-nls --enable-64-bit-bfd
make -j$JOBS
make configure-host
make install
popd

## gcc static
rm -rf $GCC__VERSION
tar jxf $GCC__VERSION.tar.bz2

rm -rf gcc_build_static
mkdir gcc_build_static
pushd gcc_build_static

AR=ar LDFLAGS="-Wl,-rpath,$PREFIX/lib" \
	$PRO_PACKAGES/gcc_build_static/../$GCC__VERSION/configure --prefix=$PREFIX \
	--build=$BUILD --host=$HOST --target=$TARGET --with-sysroot=$SYSROOT  \
	--with-native-system-headers-dir=$SYSROOT/usr/include \
	--with-mpfr=$TMP_INSTALL --with-gmp=$TMP_INSTALL --with-mpc=$TMP_INSTALL --with-cloog=$TMP_INSTALL --with-isl=$TMP_INSTALL \
	--enable-cloog-backend=isl --disable-isl-version-check \
	--with-gnu-ld --with-gnu-as --with-system-zlib \
	--with-newlib --without-headers --disable-threads --enable-languages=c \
	--disable-nls --disable-shared \
	--disable-libgomp --disable-libmudflap --disable-libssp \
	--enable-lto --with-host-libstdcxx="-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm"
##--with-ecj-jar= # for java support
make all-gcc all-target-libgcc -j$JOBS
make install-gcc install-target-libgcc
popd

## glibc, require make 3.[789], 4.x not okay.
rm -rf $GLIBC_VERSION
tar jxf ${GLIBC_VERSION}.tar.bz2

rm -rf glibc_build
mkdir glibc_build
pushd glibc_build

BUILD_CC="gcc" CC="${TARGET}-gcc" AR="${TARGET}-ar" RANLIB="${TARGET}-ranlib" \
	$PRO_PACKAGES/glibc_build/../${GLIBC_VERSION}/configure --prefix=/usr \
	--host=$TARGET --build=$BUILD --with-binutils=$PREFIX/bin --enable-add-ons \
	--disable-profile --with-tls --enable-kernel=2.6.32 --with-__thread --with-headers=$SYSROOT/usr/include \
	--enable-obsolete-rpc
make -j$JOBS
make install install_root=$SYSROOT
popd

## gcc final
rm -rf $GCC__VERSION
tar jxf $GCC__VERSION.tar.bz2

rm -rf gcc_build_final
mkdir gcc_build_final
pushd gcc_build_final

AR=ar LDFLAGS="-Wl,-rpath,$PREFIX/lib" \
	$PRO_PACKAGES/gcc_build_final/../$GCC__VERSION/configure --prefix=$PREFIX \
	--build=$BUILD --host=$HOST --target=$TARGET \
	--with-sysroot=$SYSROOT --with-native-system-headers-dir=$SYSROOT/usr/include \
	--disable-shared --enable-languages=c,c++ \
	--with-gnu-as --with-gnu-ld --disable-nls --disable-libssp --disable-libmudflap --disable-libgomp \
	--with-mpfr=$TMP_INSTALL --with-gmp=$TMP_INSTALL --with-mpc=$TMP_INSTALL --with-isl=$TMP_INSTALL --with-cloog=$TMP_INSTALL \
	--enable-cloog-backend=isl --disable-isl-version-check \
	--enable-c99 --enable-long-long --with-system-zlib --enable-threads=posix --enable-lto \
	--with-host-libstdcxx="-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm" \
	--with-pkgversion="By Matti Lee @ ${_DATE_}"
# --enable-languages=c,c++,fortran,objc,obj-c++
make -j$JOBS
make install-strip
popd
