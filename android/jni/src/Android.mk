LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

PRJ_PATH := ../../..

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(PRJ_PATH)/shared
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(PRJ_PATH)/shared/backends
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(PRJ_PATH)/client
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(PRJ_PATH)/server
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(PRJ_PATH)/libs
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(PRJ_PATH)/libs/glm
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(PRJ_PATH)/libs/enet/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(PRJ_PATH)/libs/SDL2_ttf/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(PRJ_PATH)/libs/SDL2_mixer/include

FILE_LIST := $(wildcard $(LOCAL_PATH)/$(PRJ_PATH)/shared/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/$(PRJ_PATH)/shared/backends/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/$(PRJ_PATH)/client/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/$(PRJ_PATH)/client/backends/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/$(PRJ_PATH)/server/*.cpp)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
LOCAL_SRC_FILES += $(PRJ_PATH)/libs/enet/callbacks.c\
$(PRJ_PATH)/libs/enet/compress.c\
$(PRJ_PATH)/libs/enet/host.c\
$(PRJ_PATH)/libs/enet/list.c\
$(PRJ_PATH)/libs/enet/packet.c\
$(PRJ_PATH)/libs/enet/peer.c\
$(PRJ_PATH)/libs/enet/protocol.c\
$(PRJ_PATH)/libs/enet/unix.c

# add sdl main
LOCAL_SRC_FILES += $(SDL_PATH)/src/main/android/SDL_android_main.c

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_ttf SDL2_mixer

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
