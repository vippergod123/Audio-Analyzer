#include <jni.h>
#include <string>
#include "zaudio/AudioFile.h"
#include "zdebug/debug.h"

using namespace std;

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_audioexample_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


extern "C" JNIEXPORT jstring JNICALL
Java_com_example_audioexample_MainActivity_audioFileTest(
        JNIEnv* env,
        jobject /* this */) {

    string hello = "Java_com_example_audioexample_MainActivity_audioFile";
    return env->NewStringUTF(hello.c_str());
}
