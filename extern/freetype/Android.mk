# this is now the default FreeType build for Android
#
ifndef FREETYPE_V
FREETYPE_V := 2.4.6
endif

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libfreetype

LOCAL_MODULE_FILENAME := libfreetype

# compile in ARM mode, since the glyph loader/renderer is a hotspot
# when loading complex pages in the browser
#

LOCAL_SRC_FILES := \
      freetype-$(FREETYPE_V)/src/base/ftsystem.c \
      freetype-$(FREETYPE_V)/src/base/ftinit.c \
      freetype-$(FREETYPE_V)/src/base/ftdebug.c \
      freetype-$(FREETYPE_V)/src/base/ftbase.c \
      freetype-$(FREETYPE_V)/src/base/ftbbox.c \
      freetype-$(FREETYPE_V)/src/base/ftglyph.c \
      freetype-$(FREETYPE_V)/src/base/ftbdf.c \
      freetype-$(FREETYPE_V)/src/base/ftbitmap.c \
      freetype-$(FREETYPE_V)/src/base/ftcid.c \
      freetype-$(FREETYPE_V)/src/base/ftfstype.c \
      freetype-$(FREETYPE_V)/src/base/ftgasp.c \
      freetype-$(FREETYPE_V)/src/base/ftgxval.c \
      freetype-$(FREETYPE_V)/src/base/ftlcdfil.c \
      freetype-$(FREETYPE_V)/src/base/ftmm.c \
      freetype-$(FREETYPE_V)/src/base/ftotval.c \
      freetype-$(FREETYPE_V)/src/base/ftpatent.c \
      freetype-$(FREETYPE_V)/src/base/ftpfr.c \
      freetype-$(FREETYPE_V)/src/base/ftstroke.c \
      freetype-$(FREETYPE_V)/src/base/ftsynth.c \
      freetype-$(FREETYPE_V)/src/base/fttype1.c \
      freetype-$(FREETYPE_V)/src/base/ftwinfnt.c \
      freetype-$(FREETYPE_V)/src/base/ftxf86.c \

LOCAL_SRC_FILES += \
      freetype-$(FREETYPE_V)/src/bdf/bdf.c \
      freetype-$(FREETYPE_V)/src/cff/cff.c \
      freetype-$(FREETYPE_V)/src/cid/type1cid.c \
      freetype-$(FREETYPE_V)/src/pcf/pcf.c \
      freetype-$(FREETYPE_V)/src/pfr/pfr.c \
      freetype-$(FREETYPE_V)/src/sfnt/sfnt.c \
      freetype-$(FREETYPE_V)/src/truetype/truetype.c \
      freetype-$(FREETYPE_V)/src/type1/type1.c \
      freetype-$(FREETYPE_V)/src/type42/type42.c \
      freetype-$(FREETYPE_V)/src/winfonts/winfnt.c \

#    -- rasterizers (optional; at least one is needed for vector formats)
LOCAL_SRC_FILES += \
      freetype-$(FREETYPE_V)/src/raster/raster.c \
      freetype-$(FREETYPE_V)/src/smooth/smooth.c \

#    -- auxiliary modules (optional)
LOCAL_SRC_FILES += \
      freetype-$(FREETYPE_V)/src/autofit/autofit.c \
      freetype-$(FREETYPE_V)/src/cache/ftcache.c \
      freetype-$(FREETYPE_V)/src/gzip/ftgzip.c \
      freetype-$(FREETYPE_V)/src/lzw/ftlzw.c \
      freetype-$(FREETYPE_V)/src/gxvalid/gxvalid.c \
      freetype-$(FREETYPE_V)/src/otvalid/otvalid.c \
      freetype-$(FREETYPE_V)/src/psaux/psaux.c \
      freetype-$(FREETYPE_V)/src/pshinter/pshinter.c \
      freetype-$(FREETYPE_V)/src/psnames/psnames.c

LOCAL_EXPORT_C_INCLUDES += \
      freetype-$(FREETYPE_V)/builds \
      freetype-$(FREETYPE_V)/include

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/freetype-$(FREETYPE_V)/builds \
	$(LOCAL_PATH)/freetype-$(FREETYPE_V)/include

LOCAL_CFLAGS += -W -Wall
LOCAL_CFLAGS += -fPIC -DPIC -D_ALL
LOCAL_CFLAGS += "-DDARWIN_NO_CARBON"
LOCAL_CFLAGS += "-DFT2_BUILD_LIBRARY"

# enable the FreeType internal memory debugger in the simulator
# you need to define the FT2_DEBUG_MEMORY environment variable
# when running the program to activate it. It will dump memory
# statistics when FT_Done_FreeType is called
#
ifeq ($(TARGET_SIMULATOR),true)
LOCAL_CFLAGS += "-DFT_DEBUG_MEMORY"
endif

# the following is for testing only, and should not be used in final builds
# of the product
#LOCAL_CFLAGS += "-DTT_CONFIG_OPTION_BYTECODE_INTERPRETER"

LOCAL_CFLAGS += -O2

include $(BUILD_STATIC_LIBRARY)
