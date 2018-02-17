#include <jni.h>
#include <string.h>
#include "nRF24L01.h"

static nRF24L01 *device = nullptr;

extern "C"
JNIEXPORT jboolean

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01Create(
        JNIEnv *env,
        jobject /* this */,
        jstring CE,
        jstring INT,
        jstring CS,
        jstring SPI) {

    if (device != nullptr) {
        return JNI_FALSE;
    }

    device = new nRF24L01(
            env->GetStringUTFChars(CE, JNI_FALSE),
            env->GetStringUTFChars(INT, JNI_FALSE),
            env->GetStringUTFChars(CS, JNI_FALSE),
            env->GetStringUTFChars(SPI, JNI_FALSE));

    return (device != nullptr) ? JNI_TRUE : JNI_FALSE;
}

extern "C"
JNIEXPORT void

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01Destroy(
        JNIEnv *env,
        jobject /* this */) {

    if (device != nullptr) {
        delete(device);
        device = nullptr;
    }
}

extern "C"
JNIEXPORT void

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01Init(
        JNIEnv *env,
        jobject /* this */) {

    if (device != nullptr) {
        device->init();
    }
}

extern "C"
JNIEXPORT void

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01Transmit(
        JNIEnv *env,
        jobject /* this */,
        jbyteArray array) {

    if (device != nullptr) {
        device->transmit((uint8_t*) env->GetByteArrayElements(array, JNI_FALSE));
    }
}

extern "C"
JNIEXPORT jbyteArray

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01Receive(
        JNIEnv *env,
        jobject /* this */) {

    jbyte byte_array[PAYLOAD_SIZE];
    jbyteArray java_array = env->NewByteArray(PAYLOAD_SIZE);
    if (java_array == NULL) return NULL;

    memset(byte_array, 0, sizeof(byte_array));

    if (device != nullptr) {
        device->readPayload((uint8_t*) byte_array, PAYLOAD_SIZE);
    }

    env->SetByteArrayRegion(java_array, 0, PAYLOAD_SIZE, byte_array);
    return java_array;
}

extern "C"
JNIEXPORT void

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01PollForRXPacket(
        JNIEnv *env,
        jobject /* this */) {

    if (device != nullptr) {
        device->pollForRXPacket();
    }
}

extern "C"
JNIEXPORT void

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01PollForTXPacket(
        JNIEnv *env,
        jobject /* this */) {

    if (device != nullptr) {
        device->pollForTXPacket();
    }
}

extern "C"
JNIEXPORT void

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01PollForRXPacketWithTimeout(
        JNIEnv *env,
        jobject /* this */,
        jint timeout_ms) {

    if (device != nullptr) {
        if (timeout_ms >= 0)
            device->pollForRXPacketWithTimeout(timeout_ms);
        else
            device->pollForRXPacket();
    }
}

extern "C"
JNIEXPORT void

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01PollForTXPacketWithTimeout(
        JNIEnv *env,
        jobject /* this */,
        jint timeout_ms) {

    if (device != nullptr) {
        if (timeout_ms >= 0)
            device->pollForTXPacketWithTimeout(timeout_ms);
        else
            device->pollForTXPacket();
    }
}

extern "C"
JNIEXPORT void

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01SetMode(
        JNIEnv *env,
        jobject /* this */,
        jint mode) {

    if (device != nullptr) {
        device->setMode((mode == 0) ? TRANSMITTER : RECEIVER);
    }
}

extern "C"
JNIEXPORT void

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01SetOutputPower(
        JNIEnv *env,
        jobject /* this */,
        jint power) {

    if (device != nullptr) {
        device->setOutputPower((nRF24L01_PA) power);
    }
}

extern "C"
JNIEXPORT void

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01SetDataRate(
        JNIEnv *env,
        jobject /* this */,
        jint data_rate) {

    if (device != nullptr) {
        device->setDataRate((nRF24L01_DR) data_rate);
    }
}

extern "C"
JNIEXPORT void

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01SetCRC(
        JNIEnv *env,
        jobject /* this */,
        jbyte crc) {

    if (device != nullptr) {
        device->setCRC((nRF24L01_CRC) crc);
    }
}

extern "C"
JNIEXPORT void

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01SetChannel(
        JNIEnv *env,
        jobject /* this */,
        jint channel) {

    if (device != nullptr) {
        device->setChannel((uint8_t) channel);
    }
}

extern "C"
JNIEXPORT void

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01SetAddress(
        JNIEnv *env,
        jobject /* this */,
        jbyteArray address) {

    if (device != nullptr) {
        uint8_t* addr = (uint8_t*)env->GetByteArrayElements(address, JNI_FALSE);

        std::array<uint8_t , ADDR_LENGTH> array
                { {addr[0], addr[1], addr[2], addr[3], addr[4]} };

        device->setAddress(array);
    }
}

extern "C"
JNIEXPORT jint

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01GetMode(
        JNIEnv *env,
        jobject /* this */) {

    if (device != nullptr) {
        return device->getMode();
    }
    return INVALID_MODE;
}

extern "C"
JNIEXPORT jint

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01GetOutputPower(
        JNIEnv *env,
        jobject /* this */) {

    if (device != nullptr) {
        return device->getOutputPower();
    }
    return INVALID_PA;
}

extern "C"
JNIEXPORT jint

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01GetDataRate(
        JNIEnv *env,
        jobject /* this */) {

    if (device != nullptr) {
        return device->getDataRate();
    }
    return INVALID_DR;
}

extern "C"
JNIEXPORT jint

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01GetCRC(
        JNIEnv *env,
        jobject /* this */) {

    if (device != nullptr) {
        return device->getCRC();
    }
    return INVALID_CRC;
}

extern "C"
JNIEXPORT jbyteArray

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01GetAddress(
        JNIEnv *env,
        jobject /* this */) {

    if (device != nullptr) {
        std::array<uint8_t, ADDR_LENGTH> address = device->getAddress();
        jbyteArray jaddress= env->NewByteArray(ADDR_LENGTH);
        env->SetByteArrayRegion(jaddress, 0, ADDR_LENGTH, (jbyte*)address.data());
        return jaddress;
    }
    return nullptr;
}

extern "C"
JNIEXPORT jbyte

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01GetChannel(
        JNIEnv *env,
        jobject /* this */) {

    if (device != nullptr) {
        return device->getChannel();
    }
    return 0x00;
}

extern "C"
JNIEXPORT void

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01ClearIRQ(
        JNIEnv *env,
        jobject /* this */,
        jbyte IRQ) {

    if (device != nullptr) {
        device->clearIRQ((uint8_t)IRQ);
    }
}

extern "C"
JNIEXPORT void

JNICALL
Java_com_noosrequired_javier_1varez_nodeserver_nRF24L01_nRF24L01ApplyIRQMask(
        JNIEnv *env,
        jobject /* this */,
        jbyte mask) {

    if (device != nullptr) {
        device->applyIRQMask((uint8_t)mask);
    }
}