#include <jni.h>
#include <string>
#include "nRF24L01.h"

extern "C"
JNIEXPORT jstring

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_MainDisplay_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    nRF24L01 instance;
    std::string hello = instance.getString();

    return env->NewStringUTF(hello.c_str());
}
