
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

subdirs += $(addprefix $(LOCAL_PATH)/
	))

include $(subdirs)



#APP_STL := gnustl_static stlport_static

#APP_CFLAGS := -DANDROID -D_ANDROID -DFX_SYSTEM_32BIT -DCOCOS2D_DEBUG=1 -D_COCOS_2D -D_GL_ES -DUSE_UI_TEXT -D_GLIBCXX_USE_WCHAR_T
#APP_CFLAGS += -fexceptions -frtti

#APP_TOOLCHAIN_VERSION := 4.6.3

APP_MODULES := freeimage_static freetype_static lua_static