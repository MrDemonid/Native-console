/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class mr_demonid_Main */

#ifndef _Included_mr_demonid_Main
#define _Included_mr_demonid_Main
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     mr_demonid_Main
 * Method:    logOpenConsole
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_mr_demonid_Main_logOpenConsole(JNIEnv *, jobject);

/*
 * Class:     mr_demonid_Main
 * Method:    logOpenFile
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_mr_demonid_Main_logOpenFile(JNIEnv *, jobject, jstring);

/*
 * Class:     mr_demonid_Main
 * Method:    logClose
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_mr_demonid_Main_logClose(JNIEnv *, jobject);

/*
 * Class:     mr_demonid_Main
 * Method:    logPrintf
 * Signature: (Ljava/lang/String;[Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_mr_demonid_Main_logPrintf(JNIEnv *env, jobject obj, jstring format, jobjectArray args);

#ifdef __cplusplus
}
#endif
#endif
