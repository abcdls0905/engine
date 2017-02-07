//package com.android.gl2jni;
//Java_com_android_gl2jni_GL2JNILib_init
//package com.example.HelloWorld;

#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "../public/var_list.h"
#include "app.h"
#include "jnihelper.h"


#define  LOG_TAG    "fm"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

void showKeyBoard()
{
	static const char* package = "com/android/gl2jni/GL2JNIView";
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
	static const char* package = "com/android/gl2jni/GL2JNIView";
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
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj, jint width, jint height);
    JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchbegin(JNIEnv * env, jobject obj, jint id, jfloat x, jfloat y);
	JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchend(JNIEnv * env, jobject obj, jint id, jfloat x, jfloat y);
	JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchmove(JNIEnv * env, jobject obj, jint id, jfloat x, jfloat y);
	JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_inserttext(JNIEnv * env, jobject obj, jstring text);
	JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_backspace(JNIEnv * env, jobject obj);
	JNIEXPORT jstring JNICALL Java_com_android_gl2jni_GL2JNILib_getcontexttext(JNIEnv * env, jobject obj);
};

//
JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_init(JNIEnv * env, jobject obj,  jint width, jint height)
{
	LOGI("Java_com_android_gl2jni_GL2JNILib_init");
	CVarList args;
	App::Inst().Init("/sdcard/amber_client/", args, width, height, 1.0f);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_step(JNIEnv * env, jobject obj)
{
	App::Inst().OnFrame();
	App::Inst().OnRender();
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchbegin(JNIEnv * env, jobject obj, jint id, jfloat x, jfloat y)
{
	int result;
	App::Inst().OnMsg(WM_LBUTTONDOWN, 0, MAKELONG((int)x, (int)y), result);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchend(JNIEnv * env, jobject obj, jint id, jfloat x, jfloat y)
{
	int result;
	App::Inst().OnMsg(WM_LBUTTONUP, 0, MAKELONG((int)x, (int)y), result);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_touchmove(JNIEnv * env, jobject obj, jint id, jfloat x, jfloat y)
{
	int result;
	App::Inst().OnMsg(WM_MOUSEMOVE, 0, MAKELONG((int)x, (int)y), result);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_inserttext(JNIEnv * env, jobject obj, jstring text)
{
	const char* pszText = env->GetStringUTFChars(text, NULL);
	App::Inst().OnInput(pszText);
	env->ReleaseStringUTFChars(text, pszText);
}

JNIEXPORT void JNICALL Java_com_android_gl2jni_GL2JNILib_backspace(JNIEnv * env, jobject obj)
{
	int result;
	App::Inst().OnMsg(WM_KEYDOWN, VK_BACK, 0, result);
	LOGI("Java_com_android_gl2jni_GL2JNILib_backspace");
}

JNIEXPORT jstring JNICALL Java_com_android_gl2jni_GL2JNILib_getcontexttext(JNIEnv * env, jobject obj)
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