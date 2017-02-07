APP_ABI := armeabi armeabi-v7a x86
APP_STL := gnustl_static
APP_CFLAGS += -fexceptions -frtti -fpermissive
APP_CFLAGS += -DPERFORMANCE_DEBUG
APP_MODULES :=libapp libfm_render_es3
#APP_TOOLCHAIN_VERSION := 4.6.3
#APP_CINCLUDES:= ../../../../extern/freetype/include/