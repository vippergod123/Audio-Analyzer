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

JNIEXPORT jdoubleArray JNICALL Java_com_example_audioexample_MainActivity_getPcmAudioValue(
        JNIEnv *env,
        jobject,
        jstring filePath,
        jint splitBy) {
    ClazzTest<double> test;
    const char* path = env->GetStringUTFChars(filePath,NULL);

    vector<double> arrayPcmValue = test.getPcmValueFromFile(path, splitBy);
    jdoubleArray resultArray = env->NewDoubleArray(arrayPcmValue.size());
    if (NULL == resultArray) return NULL;

    env->SetDoubleArrayRegion(resultArray, 0, arrayPcmValue.size(), &arrayPcmValue[0]);
    return resultArray;
}

}
