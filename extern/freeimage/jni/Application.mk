APP_ABI := armeabi 
#armeabi-v7a x86
APP_STL := gnustl_static
#APP_STL := stlport_static
#APP_CFLAGS += -fexceptions -frtti -fpermissive
APP_CFLAGS += -DPERFORMANCE_DEBUG -DKTX_OPENGL_ES2
#APP_TOOLCHAIN_VERSION := 4.6.3
APP_MODULES := libfreeimage
