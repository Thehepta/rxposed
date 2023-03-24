
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := dobby
ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
    LOCAL_SRC_FILES += $(PWD)/libs/arm64-v8a/libdobby.a     #通过路径添加静态库
endif

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_SRC_FILES += $(PWD)/libs/armeabi-v7a/libdobby.a
endif

include $(PREBUILT_STATIC_LIBRARY)



include $(CLEAR_VARS)

LOCAL_MODULE    := hookbase
LOCAL_SRC_FILES := entry.cpp  jni_helper.cpp main_hook.cpp rprocess.cpp
LOCAL_LIBS := -llog

LOCAL_STATIC_LIBRARIES += dobby

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include

include $(BUILD_SHARED_LIBRARY)