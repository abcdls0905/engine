
LOCAL_PATH := $(call my-dir)

TESSERACT_PATH := $(LOCAL_PATH)/tesseract-ocr
LEPTONICA_PATH := $(LOCAL_PATH)/leptonica
LIBPNG_PATH := $(LOCAL_PATH)/libpng

#=========================================
# Libpng
#=========================================

include $(CLEAR_VARS)

LOCAL_MODULE := libpng

LIBPNG_SRC_FILES := \
    $(wildcard $(LIBPNG_PATH)/*.c)

LIBPNG_HEADER_PATH := \
    $(LIBPNG_PATH)

LOCAL_SRC_FILES := $(LIBPNG_SRC_FILES)

LOCAL_LDLIBS := -lz -lm -llog

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

#=========================================
# Leptonica
#=========================================

include $(CLEAR_VARS)

LOCAL_MODULE := liblept

#Do not include freetype
BLACKLIST_SRC_FILES := \
    %endiantest.c \
    %freetype.c \
    %xtractprotos.c

LEPTONICA_SRC_FILES := \
    $(wildcard $(LEPTONICA_PATH)/src/*.c) \
    $(wildcard $(LEPTONICA_PATH)/stdio/*.c)
    #$(subst $(LOCAL_PATH),,$(wildcard $(LEPTONICA_PATH)/src/*.c))

LEPTONICA_HEADER_PATH := \
    $(LEPTONICA_PATH)/src

LOCAL_SRC_FILES := \
    $(filter-out $(BLACKLIST_SRC_FILES),$(LEPTONICA_SRC_FILES))

#LOCAL_CFLAGS := \
#   -DHAVE_CONFIG_H

#Modify environ.h to enbale jpeg,png... support.

LOCAL_LDLIBS := -lz -lm -llog

LOCAL_C_INCLUDES := \
    $(LIBPNG_PATH)

LOCAL_PRELINK_MODULE := false
LOCAL_SHARED_LIBRARIES := libpng

include $(BUILD_SHARED_LIBRARY)

#=========================================
# TESSERACT
#=========================================

include $(CLEAR_VARS)

LOCAL_MODULE := libtess

#remote some source file
BLACKLIST_SRC_FILES := \
    %api/tesseractmain.cpp \
    %viewer/svpaint.cpp

TESSERACT_SRC_FILES := \
    $(wildcard $(TESSERACT_PATH)/api/*.cpp) \
    $(wildcard $(TESSERACT_PATH)/api/*.c) \
    $(wildcard $(TESSERACT_PATH)/ccmain/*.cpp) \
    $(wildcard $(TESSERACT_PATH)/ccstruct/*.cpp) \
    $(wildcard $(TESSERACT_PATH)/ccutil/*.cpp) \
    $(wildcard $(TESSERACT_PATH)/classify/*.cpp) \
    $(wildcard $(TESSERACT_PATH)/cube/*.cpp) \
    $(wildcard $(TESSERACT_PATH)/cutil/*.cpp) \
    $(wildcard $(TESSERACT_PATH)/dict/*.cpp) \
    $(wildcard $(TESSERACT_PATH)/image/*.cpp) \
    $(wildcard $(TESSERACT_PATH)/neural_networks/runtime/*.cpp) \
    $(wildcard $(TESSERACT_PATH)/textord/*.cpp) \
    $(wildcard $(TESSERACT_PATH)/viewer/*.cpp) \
    $(wildcard $(TESSERACT_PATH)/wordrec/*.cpp)

TESSERACT_HEADER_PATH := \
    $(TESSERACT_PATH)/api       \
    $(TESSERACT_PATH)/ccmain    \
    $(TESSERACT_PATH)/ccstruct  \
    $(TESSERACT_PATH)/ccutil    \
    $(TESSERACT_PATH)/classify  \
    $(TESSERACT_PATH)/cube  \
    $(TESSERACT_PATH)/cutil \
    $(TESSERACT_PATH)/dict  \
    $(TESSERACT_PATH)/image \
    $(TESSERACT_PATH)/neural_networks/runtime \
    $(TESSERACT_PATH)/textord   \
    $(TESSERACT_PATH)/viewer    \
    $(TESSERACT_PATH)/wordrec   \
    $(LEPTONICA_PATH)/src

    
LOCAL_SRC_FILES := \
    $(filter-out $(BLACKLIST_SRC_FILES), $(TESSERACT_SRC_FILES))

LOCAL_C_INCLUDES := \
    $(TESSERACT_HEADER_PATH)

LOCAL_CFLAGS := \
    -DHAVE_LIBLEPT \
    -DUSE_STD_NAMESPACE \
    -D'VERSION="Android"' \
    -include ctype.h \
    -include unistd.h 

LOCAL_LDLIBS += \
#    -ljnigraphics \
    -llog

LOCAL_PRELINK_MODULE := false
LOCAL_SHARED_LIBRARIES := liblept
LOCAL_DISABLE_FORMAT_STRING_CHECKS := true

include $(BUILD_SHARED_LIBRARY)


#===============
# JNI
#===============

#include $(CLEAR_VARS)

#LOCAL_MODULE := JNIOCRWorker
#LOCAL_SRC_FILES := JNIOCRWorker.cpp

#LOCAL_SHARED_LIBRARIES := \
#    liblept \
#    libtess

#LOCAL_C_INCLUDES := \
#    $(LEPTONICA_HEADER_PATH) \
#    $(TESSERACT_HEADER_PATH)

#LOCAL_LDLIBS += \
#    -ljnigraphics \
#    -llog

#include $(BUILD_SHARED_LIBRARY)

#===============
# LibSnailOCR 
#===============

include $(CLEAR_VARS)

LOCAL_MODULE := libSnailOCR
LOCAL_SRC_FILES := libSnailOCR.cpp

#LOCAL_SHARED_LIBRARIES := \
#    liblept \
#    libtess

#LOCAL_C_INCLUDES := \
 #   $(LEPTONICA_HEADER_PATH) \
  #  $(TESSERACT_HEADER_PATH)

LOCAL_LDLIBS += \
    -llog

#include $(BUILD_SHARED_LIBRARY)

#===============
# Executeable that use libSnail.OCR
#===============

include $(CLEAR_VARS)

LOCAL_MODULE := UseLibSnailOCRExec
LOCAL_SRC_FILES := UseLibSnailOCR.cpp

LOCAL_SHARED_LIBRARIES := \
    libSnailOCR 

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)

LOCAL_LDLIBS += \
    -llog

include $(BUILD_EXECUTABLE)

#===============
# Executeable
#===============

include $(CLEAR_VARS)

LOCAL_MODULE := SnailOCRExec
LOCAL_SRC_FILES := SnailOCR.cpp

LOCAL_SHARED_LIBRARIES := \
    liblept \
    libtess

LOCAL_C_INCLUDES := \
    $(LEPTONICA_HEADER_PATH) \
    $(TESSERACT_HEADER_PATH)

LOCAL_LDLIBS += \
    -llog

include $(BUILD_EXECUTABLE)
