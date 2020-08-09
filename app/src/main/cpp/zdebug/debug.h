//
// Created by cris on 8/7/20.
//

#ifndef AUDIOEXAMPLE_DEBUG_H
#define AUDIOEXAMPLE_DEBUG_H

#include <android/log.h>


#if !defined(NDEBUG)

#if !defined(LOG_TAG)
#define LOG_TAG "NATIVE_CODE"

#endif

#define LOG_V(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOG_D(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOG_I(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOG_W(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOG_E(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_F(...) (ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)

#else

#define LOG_V(...)
#define LOG_D(...)
#define LOG_I(...)
#define LOG_W(...)
#define LOG_E(...)
#define LOG_F(...)
#endif

#endif //AUDIOEXAMPLE_DEBUG_H
