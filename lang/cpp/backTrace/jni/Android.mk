LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := backTrace

LOCAL_SRC_FILES += ../backTraceInSig.cc

LOCAL_LDLIBS    := -llog -ldl
include $(BUILD_EXECUTABLE)
