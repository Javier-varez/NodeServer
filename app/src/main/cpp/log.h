//
// Created by javi on 19/1/18.
//

#ifndef NODESERVER_LOG_H
#define NODESERVER_LOG_H

#include <android/log.h>

#ifndef NDEBUG
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#else
#define LOGV(...)
#define LOGI(...)
#define LOGD(...)
#endif

#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define ASSERT(cond, ...) if (!(cond)) { __android_log_assert(#cond, TAG, __VA_ARGS__);}

#define IMPLEMENT() { \
                        std::string pattern ("Implement: "); \
                        std::string func (__func__); \
                        ASSERT(0, (pattern + func).c_str()); \
                    }

#endif //NODESERVER_LOG_H
