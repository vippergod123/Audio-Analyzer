#include <jni.h>
#include <string>
//#include "zaudio/AudioFile.h"
#include "zdebug/debug.h"
#include "test/clazz_test.h"

using namespace std;

extern "C" {
JNIEXPORT jstring JNICALL Java_com_example_audioexample_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT jstring JNICALL Java_com_example_audioexample_MainActivity_audioFileTest(
        JNIEnv *env,
        jobject,
        jstring filePath) {
    ClazzTest<double> test;
    const char* path = env->GetStringUTFChars(filePath,NULL);

    test.readAudio(path);

    return env->NewStringUTF(path);
}

JNIEXPORT jstring JNICALL Java_com_example_audioexample_MainActivity_convertByteArrayToUnsignChar(
        JNIEnv *env,
        jobject,
        jbyteArray byteArray) {
   ClazzTest<double> test;

   test.convertByteArrayToUnsignedChar(env,byteArray);
   return env->NewStringUTF("path");
}

}
