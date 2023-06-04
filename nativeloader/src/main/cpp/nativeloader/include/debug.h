//
// Created by chic on 2023/5/23.
//

#ifndef RXPOSED_DEBUG_H
#define RXPOSED_DEBUG_H


#include "android/log.h"

#define LOG_TAG "RxposedInject"
#if RXDEBUG
// 调用 debug 版本方法
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define DEBUG(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,"[file %s],[line %d],[function:%s]",__FILE__, __LINE__,__func__);
#define LOGD(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)



#else
#define LOGE(...)
#define LOGD(...)
#define DEBUG(...)
#endif



#endif //RXPOSED_DEBUG_H
