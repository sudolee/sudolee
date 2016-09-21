#APP_ABI      := armeabi-v7a arm64-v8a x86 x86_64 mips mips64
APP_ABI      := armeabi-v7a arm64-v8a
APP_PLATFORM := android-14
APP_STL      := gnustl_static
APP_CPPFLAGS += -std=c++11 -Wall -Werror -Wno-unused-function -Wno-unused-const-variable
#NDK_TOOLCHAIN_VERSION := clang
#NDK_TOOLCHAIN_VERSION := 4.9
