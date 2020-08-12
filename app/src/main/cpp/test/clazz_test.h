//
// Created by lap13663 on 8/8/20.
//

#ifndef AUDIOEXAMPLE_CLAZZ_TEST_H
#define AUDIOEXAMPLE_CLAZZ_TEST_H

#include <iostream>
#include <jni.h>
#include <fstream>
#include <string>
#include <vector>
#include "../zdebug/debug.h"
#include "../zaudio/MainAudioFile.h"

using namespace std;

template<class T>
class ClazzTest {
public:
    void readAudio(const char *filePath) {
       AudioFile<double> audioFile;
       audioFile.load(filePath);
       audioFile.printSummary();
       vector<vector<T>> buffer = audioFile.samples;

//        for (int i = 0; i < buffer.size(); ++i) {
//            for (int j = 0; j < buffer[i].size(); ++j) {
//                LOG_D("buffer[%d][%d]: %f", i, j, buffer[i][j]);
//            }
//        }
    };

    void convertByteArrayToUnsignedChar(JNIEnv *env, jbyteArray byteArray) {
       int len = env->GetArrayLength(byteArray);

       unsigned char *buf = new unsigned char[len];

       env->GetByteArrayRegion(byteArray, 0, len, reinterpret_cast<jbyte *>(buf));
       for (int i = 0; i < len; ++i) {
          LOG_D("%d hex: %02x- int: %d - char: %c",i,buf[i], buf[i], buf[i]);
       }
    }
};


#endif //AUDIOEXAMPLE_CLAZZ_TEST_H
