#include <jni.h>

#include "nRF24L01.h"

static nRF24L01 *device = nullptr;

extern "C"
JNIEXPORT jboolean

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_MainDisplay_nRF24L01Create(
        JNIEnv *env,
        jobject /* this */,
        jstring CE,
        jstring INT,
        jstring SPI) {

    if (device != nullptr) {
        return JNI_FALSE;
    }

    device = new nRF24L01(
            env->GetStringUTFChars(CE, JNI_FALSE),
            env->GetStringUTFChars(INT, JNI_FALSE),
            env->GetStringUTFChars(SPI, JNI_FALSE));

    return (device != nullptr) ? JNI_TRUE : JNI_FALSE;
}

extern "C"
JNIEXPORT void

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_MainDisplay_nRF24L01Destroy(
        JNIEnv *env,
        jobject /* this */) {

    if (device != nullptr) {
        delete(device);
        device = nullptr;
    }
}
