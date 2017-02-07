LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libfreeimage

LOCAL_MODULE_FILENAME := libfreeimage
#FreeImage/Source/FreeImage/ConversionUINT16.cpp 
LOCAL_SRC_FILES		:= \
FreeImage/Source/FreeImage/BitmapAccess.cpp \
FreeImage/Source/FreeImage/ColorLookup.cpp \
FreeImage/Source/FreeImage/FreeImage.cpp \
FreeImage/Source/FreeImage/FreeImageC.c \
FreeImage/Source/FreeImage/FreeImageIO.cpp \
FreeImage/Source/FreeImage/GetType.cpp \
FreeImage/Source/FreeImage/MemoryIO.cpp \
FreeImage/Source/FreeImage/PixelAccess.cpp \
FreeImage/Source/FreeImage/J2KHelper.cpp \
FreeImage/Source/FreeImage/PluginBMP.cpp \
FreeImage/Source/FreeImage/PluginJ2K.cpp \
FreeImage/Source/FreeImage/PluginJP2.cpp \
FreeImage/Source/FreeImage/PluginJPEG.cpp \
FreeImage/Source/FreeImage/Plugin.cpp \
FreeImage/Source/FreeImage/PluginPNG.cpp \
FreeImage/Source/FreeImage/PSDParser.cpp \
FreeImage/Source/FreeImage/Conversion.cpp \
FreeImage/Source/FreeImage/Conversion16_555.cpp \
FreeImage/Source/FreeImage/Conversion16_565.cpp \
FreeImage/Source/FreeImage/Conversion24.cpp \
FreeImage/Source/FreeImage/Conversion32.cpp \
FreeImage/Source/FreeImage/Conversion4.cpp \
FreeImage/Source/FreeImage/Conversion8.cpp \
FreeImage/Source/FreeImage/ConversionFloat.cpp \
FreeImage/Source/FreeImage/ConversionRGBF.cpp \
FreeImage/Source/FreeImage/ConversionType.cpp \
FreeImage/Source/FreeImage/Halftoning.cpp \
FreeImage/Source/FreeImage/tmoColorConvert.cpp \
FreeImage/Source/FreeImage/tmoDrago03.cpp \
FreeImage/Source/FreeImage/tmoFattal02.cpp \
FreeImage/Source/FreeImage/tmoReinhard05.cpp \
FreeImage/Source/FreeImage/ToneMapping.cpp \
FreeImage/Source/FreeImage/NNQuantizer.cpp \
FreeImage/Source/FreeImage/WuQuantizer.cpp \
FreeImage/Source/DeprecationManager/Deprecated.cpp \
FreeImage/Source/DeprecationManager/DeprecationMgr.cpp \
FreeImage/Source/FreeImage/CacheFile.cpp \
FreeImage/Source/FreeImage/MultiPage.cpp \
FreeImage/Source/FreeImage/ZLibInterface.cpp \
FreeImage/Source/Metadata/Exif.cpp \
FreeImage/Source/Metadata/FIRational.cpp \
FreeImage/Source/Metadata/FreeImageTag.cpp \
FreeImage/Source/Metadata/IPTC.cpp \
FreeImage/Source/Metadata/TagConversion.cpp \
FreeImage/Source/Metadata/TagLib.cpp \
FreeImage/Source/FreeImageToolkit/Background.cpp \
FreeImage/Source/FreeImageToolkit/BSplineRotate.cpp \
FreeImage/Source/FreeImageToolkit/Channels.cpp \
FreeImage/Source/FreeImageToolkit/ClassicRotate.cpp \
FreeImage/Source/FreeImageToolkit/Colors.cpp \
FreeImage/Source/FreeImageToolkit/CopyPaste.cpp \
FreeImage/Source/FreeImageToolkit/Display.cpp \
FreeImage/Source/FreeImageToolkit/Flip.cpp \
FreeImage/Source/FreeImageToolkit/JPEGTransform.cpp \
FreeImage/Source/FreeImageToolkit/MultigridPoissonSolver.cpp \
FreeImage/Source/FreeImageToolkit/Rescale.cpp \
FreeImage/Source/FreeImageToolkit/Resize.cpp \
FreeImage/Source/LibJPEG/jaricom.c \
FreeImage/Source/LibJPEG/jcapimin.c \
FreeImage/Source/LibJPEG/jcapistd.c \
FreeImage/Source/LibJPEG/jcarith.c \
FreeImage/Source/LibJPEG/jccoefct.c \
FreeImage/Source/LibJPEG/jccolor.c \
FreeImage/Source/LibJPEG/jcdctmgr.c \
FreeImage/Source/LibJPEG/jchuff.c \
FreeImage/Source/LibJPEG/jcinit.c \
FreeImage/Source/LibJPEG/jcmainct.c \
FreeImage/Source/LibJPEG/jcmarker.c \
FreeImage/Source/LibJPEG/jcmaster.c \
FreeImage/Source/LibJPEG/jcomapi.c \
FreeImage/Source/LibJPEG/jcparam.c \
FreeImage/Source/LibJPEG/jcprepct.c \
FreeImage/Source/LibJPEG/jcsample.c \
FreeImage/Source/LibJPEG/jctrans.c \
FreeImage/Source/LibJPEG/jdapimin.c \
FreeImage/Source/LibJPEG/jdapistd.c \
FreeImage/Source/LibJPEG/jdarith.c \
FreeImage/Source/LibJPEG/jdatadst.c \
FreeImage/Source/LibJPEG/jdatasrc.c \
FreeImage/Source/LibJPEG/jdcoefct.c \
FreeImage/Source/LibJPEG/jdcolor.c \
FreeImage/Source/LibJPEG/jddctmgr.c \
FreeImage/Source/LibJPEG/jdhuff.c \
FreeImage/Source/LibJPEG/jdinput.c \
FreeImage/Source/LibJPEG/jdmainct.c \
FreeImage/Source/LibJPEG/jdmarker.c \
FreeImage/Source/LibJPEG/jdmaster.c \
FreeImage/Source/LibJPEG/jdmerge.c \
FreeImage/Source/LibJPEG/jdpostct.c \
FreeImage/Source/LibJPEG/jdsample.c \
FreeImage/Source/LibJPEG/jdtrans.c \
FreeImage/Source/LibJPEG/jerror.c \
FreeImage/Source/LibJPEG/jfdctflt.c \
FreeImage/Source/LibJPEG/jfdctfst.c \
FreeImage/Source/LibJPEG/jfdctint.c \
FreeImage/Source/LibJPEG/jidctflt.c \
FreeImage/Source/LibJPEG/jidctfst.c \
FreeImage/Source/LibJPEG/jidctint.c \
FreeImage/Source/LibJPEG/jmemmgr.c \
FreeImage/Source/LibJPEG/jmemnobs.c \
FreeImage/Source/LibJPEG/jquant1.c \
FreeImage/Source/LibJPEG/jquant2.c \
FreeImage/Source/LibJPEG/jutils.c \
FreeImage/Source/LibJPEG/transupp.c \
FreeImage/Source/LibPNG/png.c \
FreeImage/Source/LibPNG/pngerror.c \
FreeImage/Source/LibPNG/pngget.c \
FreeImage/Source/LibPNG/pngmem.c \
FreeImage/Source/LibPNG/pngpread.c \
FreeImage/Source/LibPNG/pngread.c \
FreeImage/Source/LibPNG/pngrio.c \
FreeImage/Source/LibPNG/pngrtran.c \
FreeImage/Source/LibPNG/pngrutil.c \
FreeImage/Source/LibPNG/pngset.c \
FreeImage/Source/LibPNG/pngtrans.c \
FreeImage/Source/LibPNG/pngwio.c \
FreeImage/Source/LibPNG/pngwrite.c \
FreeImage/Source/LibPNG/pngwtran.c \
FreeImage/Source/LibPNG/pngwutil.c \
FreeImage/Source/ZLib/adler32.c \
FreeImage/Source/ZLib/compress.c \
FreeImage/Source/ZLib/crc32.c \
FreeImage/Source/ZLib/deflate.c \
FreeImage/Source/ZLib/infback.c \
FreeImage/Source/ZLib/inffast.c \
FreeImage/Source/ZLib/inflate.c \
FreeImage/Source/ZLib/inftrees.c \
FreeImage/Source/ZLib/trees.c \
FreeImage/Source/ZLib/uncompr.c \
FreeImage/Source/ZLib/zutil.c \
FreeImage/Source/LibOpenJPEG/bio.c \
FreeImage/Source/LibOpenJPEG/cio.c \
FreeImage/Source/LibOpenJPEG/dwt.c \
FreeImage/Source/LibOpenJPEG/event.c \
FreeImage/Source/LibOpenJPEG/image.c \
FreeImage/Source/LibOpenJPEG/j2k.c \
FreeImage/Source/LibOpenJPEG/j2k_lib.c \
FreeImage/Source/LibOpenJPEG/jp2.c \
FreeImage/Source/LibOpenJPEG/jpt.c \
FreeImage/Source/LibOpenJPEG/mct.c \
FreeImage/Source/LibOpenJPEG/mqc.c \
FreeImage/Source/LibOpenJPEG/openjpeg.c \
FreeImage/Source/LibOpenJPEG/pi.c \
FreeImage/Source/LibOpenJPEG/raw.c \
FreeImage/Source/LibOpenJPEG/t1.c \
FreeImage/Source/LibOpenJPEG/t2.c \
FreeImage/Source/LibOpenJPEG/tcd.c \
FreeImage/Source/LibOpenJPEG/tgt.c \
FreeImage/Source/OpenEXR/Half/toFloat.cpp \

#LOCAL_LDFLAGS := libs/libstdc++.a

LOCAL_EXPORT_C_INCLUDES	:= $(LOCAL_PATH)/include

LOCAL_C_INCLUDES	:= $(LOCAL_PATH)/FreeImage/Source \
    $(LOCAL_PATH)/FreeImage/Source/Metadata \
    $(LOCAL_PATH)/FreeImage/Source/DeprecationManager \

LOCAL_CFLAGS		+= -DFREEIMAGE_LIB=1 -DPNG_STATIC=1
LOCAL_CPPFLAGS		+= -D_STLP_DEFINE_BAD_ALLOC -fexceptions -lstdc++

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)
