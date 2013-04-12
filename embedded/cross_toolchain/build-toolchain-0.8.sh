#!/bin/bash
# build arm cross toolchain from source code by <desoxydate@gmail.com>

# make -- Specifies the number of jobs to run simultaneously.
CPUS=$(grep -c processor /proc/cpuinfo)
JOBS=$((2*$CPUS))
_DATE_=$(date +%Y%m%e)

#TARGET=arm-none-linux-gnueabi
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

# Packages....
KERNEL_VERSION=linux-3.0.71
M4_VERSION=m4-1.4.16
GMP_VERSION=gmp-5.0.4
MPFR_VERSION=mpfr-3.1.1
MPC_VERSION=mpc-0.9
PPL_VERSION=ppl-0.12
CLOOG_VERSION=cloog-ppl-0.15.11
BINUTILS_VERSION=binutils-2.21.1
GCC__VERSION=gcc-4.6.3
GLIBC_V=2.14.1
GDB_VERSION=gdb-7.3.1
FILE_VERSION=file-5.11
NCURSES_VERSION=ncurses-5.9
LIBELF_VERSION=libelf-0.8.13


cd $PRO_PACKAGES

#kernel headers
cd $KERNEL_VERSION
make mrproper
make ARCH=arm headers_check
make ARCH=arm INSTALL_HDR_PATH=$SYSROOT/usr headers_install
cd ..

#file
rm -rf $FILE_VERSION
tar zxf $FILE_VERSION.tar.gz
cd $FILE_VERSION
./configure --prefix=$TMP_INSTALL
make -j$JOBS
make install-strip
cd ..

#m4
rm -rf $M4_VERSION
tar jxf $M4_VERSION.tar.bz2
cd $M4_VERSION
./configure --prefix=$TMP_INSTALL --enable-c++
make -j$JOBS
make install-strip
cd ..

#ncurses
rm -rf $NCURSES_VERSION
tar zxf $NCURSES_VERSION.tar.gz
cd $NCURSES_VERSION
./configure --prefix=$TMP_INSTALL --without-debug --without-shared
make -j$JOBS
make install
cd ..

#gmp
rm -rf $GMP_VERSION
tar jxf $GMP_VERSION.tar.bz2
cd $GMP_VERSION
CPPFLAGS=-fexceptions \
	./configure --prefix=$TMP_INSTALL \
	--enable-cxx --enable-fft --enable-mpbsd --disable-shared --enable-static
make -j$JOBS
#make -j$JOBS check
make install-strip
cd ..

#mpfr
rm -rf $MPFR_VERSION
tar jxf $MPFR_VERSION.tar.bz2
cd $MPFR_VERSION
LDFLAGS="-Wl,-rpath,$TMP_INSTALL/lib" \
	./configure --prefix=$TMP_INSTALL --with-gmp=$TMP_INSTALL --disable-shared --enable-static
make -j$JOBS
make install-strip
cd ..

#mpc
rm -rf $MPC_VERSION
tar zxf $MPC_VERSION.tar.gz
cd $MPC_VERSION
LDFLAGS="-Wl,-rpath,$TMP_INSTALL/lib" \
	./configure --prefix=$TMP_INSTALL \
	--with-mpfr=$TMP_INSTALL --with-gmp=$TMP_INSTALL --disable-shared --enable-static
make -j$JOBS
make install-strip
cd ..

#ppl
rm -rf $PPL_VERSION
tar jxf $PPL_VERSION.tar.bz2
cd $PPL_VERSION
CPPFLAGS="-I$TMP_INSTALL/include" LDFLAGS="-Wl,-rpath,$TMP_INSTALL/lib" \
	./configure --prefix=$TMP_INSTALL --enable-interfaces="c,cxx" \
	--with-gmp=$TMP_INSTALL \
	--disable-shared --enable-static
make -j$JOBS
make install-strip
cd ..

#CLooG
rm -rf $CLOOG_VERSION
tar zxf $CLOOG_VERSION.tar.gz
cd $CLOOG_VERSION
LDFLAGS="-Wl,-rpath,$TMP_INSTALL/lib" \
	./configure --prefix=$TMP_INSTALL --with-bits=gmp \
	--with-gmp=$TMP_INSTALL --with-ppl=$TMP_INSTALL --disable-shared --enable-static --with-host-libstdcxx='-lstdc++'
make -j$JOBS
make install-strip
cd ..

#libelf
rm -rf $LIBELF_VERSION
tar zxf $LIBELF_VERSION.tar.gz
cd $LIBELF_VERSION
	./configure --prefix=$TMP_INSTALL --host=$HOST --build=$BUILD --target=$TARGET \
		--enable-compat --enable-elf64 --enable-extended-format \
		--enable-static --disable-shared
make -j$JOBS
make install
cd ..

#binutils
rm -rf $BINUTILS_VERSION
tar jxf ${BINUTILS_VERSION}a.tar.bz2
rm -rf binutils_build
mkdir  binutils_build
cd binutils_build

AR=ar AS=as \
	$PRO_PACKAGES/binutils_build/../$BINUTILS_VERSION/configure --prefix=$PREFIX \
	--target=$TARGET --build=$HOST --with-sysroot=$SYSROOT \
	--with-mpfr=$TMP_INSTALL --with-gmp=$TMP_INSTALL --with-mpc=$TMP_INSTALL --with-ppl=$TMP_INSTALL --with-cloog=$TMP_INSTALL \
	--disable-nls --disable-werror
make -j$JOBS
make install
cd ..

#gcc static
rm -rf $GCC__VERSION
tar jxf $GCC__VERSION.tar.bz2 
rm -rf gcc_build_static
mkdir gcc_build_static
cd gcc_build_static

AR=ar LDFLAGS="-Wl,-rpath,$PREFIX/lib" \
	$PRO_PACKAGES/gcc_build_static/../$GCC__VERSION/configure --prefix=$PREFIX \
	--build=$BUILD --host=$HOST --target=$TARGET --with-sysroot=$SYSROOT  \
	--with-mpfr=$TMP_INSTALL --with-gmp=$TMP_INSTALL --with-mpc=$TMP_INSTALL --with-ppl=$TMP_INSTALL --with-cloog=$TMP_INSTALL \
	--with-gnu-ld --with-gnu-as --with-system-zlib --with-newlib \
	--disable-nls --disable-shared --without-headers --disable-threads --enable-languages=c \
	--disable-libgomp --disable-libmudflap --disable-libssp \
	--enable-lto --with-libelf=$TMP_INSTALL \
	--with-host-libstdcxx="-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm"
# --with-libelf=no --disable-lto
# --disable-multilib
#--with-ecj-jar= # for java support
make all-gcc all-target-libgcc -j$JOBS
make install-gcc install-target-libgcc
cd ..

# Bug fixed --
# new gcc unsupport libgcc_eh.a library but searched
function bug_fixed_glibc(){
	GCC_SN=$(echo $GCC__VERSION | cut -c5-12)
	cd $PREFIX/lib/gcc/$TARGET/$GCC_SN/
	#ln -s libgcc.a libgcc_eh.a
	if [ -f libgcc.a ];then
		cp libgcc.a libgcc_eh.a
	else
		echo '********* no such file libgcc.a *********'
	fi
	cd $PRO_PACKAGES
}
bug_fixed_glibc

#glibc
rm -rf glibc-${GLIBC_V}
tar jxf glibc-${GLIBC_V}.tar.bz2 
tar -jxf glibc-ports-${GLIBC_V}.tar.bz2
mv glibc-ports-${GLIBC_V} glibc-${GLIBC_V}/ports
rm -rf glibc_build
mkdir glibc_build
cd glibc_build

BUILD_CC="gcc" CC="${TARGET}-gcc" AR="${TARGET}-ar" RANLIB="${TARGET}-ranlib" \
	$PRO_PACKAGES/glibc_build/../glibc-${GLIBC_V}/configure --prefix=/usr \
	--host=$TARGET --build=$BUILD --with-binutils=$PREFIX/bin --enable-add-ons \
	--disable-profile --with-tls --enable-kernel=2.6.25 --with-__thread --with-headers=$SYSROOT/usr/include \
	libc_cv_forced_unwind=yes libc_cv_c_cleanup=yes libc_cv_gnu89_inline=yes libc_cv_arm_tls=yes
# glibc-2.15 need this config: libc_cv_ctors_header=no
make -j$JOBS
make install install_root=$SYSROOT
#make localedata/install-locales
cd ..

#gcc final
rm -rf $GCC__VERSION
tar jxf $GCC__VERSION.tar.bz2 
rm -rf gcc_build_final
mkdir gcc_build_final
cd gcc_build_final/

AR=ar LDFLAGS="-Wl,-rpath,$PREFIX/lib" \
	$PRO_PACKAGES/gcc_build_final/../$GCC__VERSION/configure --prefix=$PREFIX --build=$BUILD --host=$HOST --target=$TARGET \
	--with-gnu-as --with-gnu-ld --disable-libssp --disable-libmudflap --disable-libgomp \
	--with-sysroot=$SYSROOT --disable-nls --disable-shared --enable-languages=c,c++ \
	--enable-lto --with-libelf=$TMP_INSTALL \
	--with-mpfr=$TMP_INSTALL --with-gmp=$TMP_INSTALL --with-mpc=$TMP_INSTALL --with-ppl=$TMP_INSTALL --with-cloog=$TMP_INSTALL \
	--enable-c99 --enable-long-long --with-system-zlib --enable-threads=posix \
	--with-host-libstdcxx="-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm" \
	--with-pkgversion="By sudolee ${_DATE_}"
# --enable-languages=c,c++,fortran,objc,obj-c++
make -j$JOBS
make install-strip

cd ..
