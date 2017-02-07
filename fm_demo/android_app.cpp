//package com.android.gl2jni;
//Java_com_android_gl2jni_GL2JNILib_init
//package com.example.HelloWorld;

#include <jni.h>
#include <android/log.h>

#include <EGL/egl.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "../public/var_list.h"
#include "../fm_demo/message.h"
#include "app.h"
#include "jnihelper.h"
#include "jnifile.h"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "../public/module.h"

#define  LOG_TAG    "fm"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

// 新增支持nvidia的平板
#define __PerfHUD_ES__
#ifdef __PerfHUD_ES__

// declare the variables
#ifndef EGL_NV_system_time
#define EGL_NV_system_time 1
typedef khronos_uint64_t EGLuint64NV;

#ifdef EGL_EGLEXT_PROTOTYPES
EGLAPI EGLuint64NV EGLAPIENTRY eglGetSystemTimeFrequencyNV(void);
EGLAPI EGLuint64NV EGLAPIENTRY eglGetSystemTimeNV(void);
#endif

typedef EGLuint64NV (EGLAPIENTRYP PFN_EGLGSTFNV_PROC)(void);
typedef EGLuint64NV (EGLAPIENTRYP PFN_EGLGSTNV_PROC)(void);
#endif

static EGLuint64NV eglSystemTimeFrequency = 0;
static EGLuint64NV eglSystemTimeStart = 0;
static EGLuint64NV eglSystemTimeEnd = 0;
static PFN_EGLGSTFNV_PROC eglGetSystemTimeFrequencyNVProc = NULL;
static PFN_EGLGSTNV_PROC eglGetSystemTimeNVProc = NULL;

static bool g_bIsNV = true;

#endif



void showKeyBoard()
{
	static const char* package = "com/android/fmdemo/FMDEMOView";
	JniMethodInfo t;
	if (JniHelper::getStaticMethodInfo(t, package, "openIMEKeyboard", "()V")) 
	{
		t.env->CallStaticVoidMethod(t.classID, t.methodID);
		t.env->DeleteLocalRef(t.classID);
	}
	//App::Inst().OnShowKeyboard();
	LOGI("showKeyBoard");
}

void hideKeyboard()
{
	static const char* package = "com/android/fmdemo/FMDEMOView";
	JniMethodInfo t;
	if (JniHelper::getStaticMethodInfo(t, package, "closeIMEKeyboard", "()V")) 
	{
		t.env->CallStaticVoidMethod(t.classID, t.methodID);
		t.env->DeleteLocalRef(t.classID);
	}
	//App::Inst().OnHideKeyboard();
	LOGI("hideKeyboard");
}

extern "C" {
	
	JNIEXPORT void JNICALL Java_com_android_fmdemo_FMDEMOLib_setasset(JNIEnv * env, jobject obj, jobject assetManager);
    JNIEXPORT void JNICALL Java_com_android_fmdemo_FMDEMOLib_init(JNIEnv * env, jobject obj, jint width, jint height);
    JNIEXPORT void JNICALL Java_com_android_fmdemo_FMDEMOLib_step(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_android_fmdemo_FMDEMOLib_touchbegin(JNIEnv * env, jobject obj, jint id, jfloat x, jfloat y);
	JNIEXPORT void JNICALL Java_com_android_fmdemo_FMDEMOLib_touchend(JNIEnv * env, jobject obj, jint id, jfloat x, jfloat y);
	JNIEXPORT void JNICALL Java_com_android_fmdemo_FMDEMOLib_touchmove(JNIEnv * env, jobject obj, jint id, jfloat x, jfloat y);
	JNIEXPORT void JNICALL Java_com_android_fmdemo_FMDEMOLib_inserttext(JNIEnv * env, jobject obj, jstring text);
	JNIEXPORT void JNICALL Java_com_android_fmdemo_FMDEMOLib_backspace(JNIEnv * env, jobject obj);
	JNIEXPORT jstring JNICALL Java_com_android_fmdemo_FMDEMOLib_getcontexttext(JNIEnv * env, jobject obj);
};

AAssetManager* g_asset_manager = 0;
JNIEXPORT void JNICALL Java_com_android_fmdemo_FMDEMOLib_setasset(JNIEnv * env, jobject obj, jobject assetManager)
{
	 g_asset_manager = AAssetManager_fromJava(env, assetManager);
	 LOGI("asset_manager:%d", (long)g_asset_manager);
}

JNIEXPORT void JNICALL Java_com_android_fmdemo_FMDEMOLib_init(JNIEnv * env, jobject obj,  jint width, jint height)
{
	LOGI("Java_com_android_fmdemo_FMDEMOLib_init");
	CVarList args;
#ifdef _APK_FILE
	App::Inst().Init("", args, width, height, 1.0f, "jniFile");
#else
    

	App::Inst().Init("/sdcard/fmdemo_client/", args, width, height, 1.0f);
#endif

#ifdef __PerfHUD_ES__
	// get the functions
	eglGetSystemTimeFrequencyNVProc = (PFN_EGLGSTFNV_PROC)
		eglGetProcAddress("eglGetSystemTimeFrequencyNV");
	if(NULL == eglGetSystemTimeFrequencyNVProc)
		g_bIsNV = false;

	eglGetSystemTimeNVProc = (PFN_EGLGSTNV_PROC)
		eglGetProcAddress("eglGetSystemTimeNV");
	if(NULL == eglGetSystemTimeNVProc)
		g_bIsNV = false;

	eglSystemTimeFrequency = eglGetSystemTimeFrequencyNVProc();
	if(NULL == eglSystemTimeFrequency)
		g_bIsNV = false;

	eglSystemTimeStart = eglGetSystemTimeNVProc();
	if(NULL == eglSystemTimeStart)
		g_bIsNV = false;

	App::Inst().TraceLog("PerfHUD_ES Ver!");
	if(g_bIsNV)
	{
		App::Inst().TraceLog("Get NV API true!");
	}
	else
	{
		App::Inst().TraceLog("Get NV API false!");
	}
#else
	App::Inst().TraceLog("Normal Ver!");
#endif
}

JNIEXPORT void JNICALL Java_com_android_fmdemo_FMDEMOLib_step(JNIEnv * env, jobject obj)
{

#ifdef __PerfHUD_ES__
	// to pause the game if eglGetSystemTimeNVProc() returns the same value
	bool bPause = false;
	if(g_bIsNV)
	{
		eglSystemTimeEnd = eglGetSystemTimeNVProc();
		if(eglSystemTimeFrequency > 0 && eglSystemTimeEnd == eglSystemTimeStart)
		{
			bPause = true;
		}
		eglSystemTimeStart = eglSystemTimeEnd;
	}
	App::Inst().OnFrameForPerfHUD_ES(bPause);

#else
	App::Inst().OnFrame();
#endif

	App::Inst().OnRender();
}

JNIEXPORT void JNICALL Java_com_android_fmdemo_FMDEMOLib_touchbegin(JNIEnv * env, jobject obj, jint id, jfloat x, jfloat y)
{
	int result;
	if (id == 0)
	{
		App::Inst().OnMsg(WM_LBUTTONDOWN, 0, MAKELONG((int)x, (int)y), result);
	}
	App::Inst().OnMsg(FM_TOUCH_BEGIN, int(id), MAKELONG((int)x, (int)y), result);
}

JNIEXPORT void JNICALL Java_com_android_fmdemo_FMDEMOLib_touchend(JNIEnv * env, jobject obj, jint id, jfloat x, jfloat y)
{
	int result;
	if (id == 0)
	{
		App::Inst().OnMsg(WM_LBUTTONUP, 0, MAKELONG((int)x, (int)y), result);
	}

	App::Inst().OnMsg(FM_TOUCH_END, int(id), MAKELONG((int)x, (int)y), result);
}

JNIEXPORT void JNICALL Java_com_android_fmdemo_FMDEMOLib_touchmove(JNIEnv * env, jobject obj, jint id, jfloat x, jfloat y)
{
	int result;
	if (id == 0)
	{
		App::Inst().OnMsg(WM_MOUSEMOVE, 0, MAKELONG((int)x, (int)y), result);
	}
	App::Inst().OnMsg(FM_TOUCH_MOVE, int(id), MAKELONG((int)x, (int)y), result);
}

JNIEXPORT void JNICALL Java_com_android_fmdemo_FMDEMOLib_inserttext(JNIEnv * env, jobject obj, jstring text)
{
	const char* pszText = env->GetStringUTFChars(text, NULL);
	App::Inst().OnInput(pszText);
	env->ReleaseStringUTFChars(text, pszText);
}

JNIEXPORT void JNICALL Java_com_android_fmdemo_FMDEMOLib_backspace(JNIEnv * env, jobject obj)
{    
	int result;
	App::Inst().OnMsg(WM_KEYDOWN, VK_BACK, 0, result);
	LOGI("Java_com_android_fmdemo_FMDEMOLib_backspace");
}

JNIEXPORT jstring JNICALL Java_com_android_fmdemo_FMDEMOLib_getcontexttext(JNIEnv * env, jobject obj)
{
	if (JniHelper::getJavaVM()->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK || ! env) {
          return 0;
	}
	static char buf[512];
	App::Inst().GetFocusText(buf, sizeof(buf));
	return env->NewStringUTF(buf);
}

extern "C" {

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	JniHelper::setJavaVM(vm);
	LOGI("JNI_OnLoad");
	return JNI_VERSION_1_4;
}
}