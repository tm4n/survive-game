
# Uncomment this if you're using STL in your project
# See CPLUSPLUS-SUPPORT.html in the NDK documentation for more information
NDK_TOOLCHAIN_VERSION := 4.8

APP_STL := gnustl_static 

APP_CPPFLAGS += -std=c++11 -pthread
APP_CPPFLAGS += -frtti

APP_ABI := armeabi-v7a

APP_PLATFORM := android-16
