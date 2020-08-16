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
#include <math.h>
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
       vector<vector<T>> fileStream = audioFile.samples;

       vector<T> firstChannel = fileStream[0];
       int bufferSize = audioFile.getSampleRate() * 3  ; //get buffer N second of audio
//       int startIndex = 0;
//       int count =0 ;
//       while ( startIndex < firstChannel.size()) {
//          startIndex+= bufferSize;
//          count ++;
//          LOG_D("%d", count);
//       }

       vector<vector<T>> arrayBuffer = splitBufferIntoArrayBuffer(firstChannel, bufferSize);
       for (int i = 0; i < arrayBuffer.size(); ++i) {

          LOG_D("arrayBuffer[%d] => %f", i, rmsPcmRawValue(arrayBuffer[i]));
       }

//       logBuffer(arrayBuffer.back());
//
//       for (int sampleIndex = 0; sampleIndex < lastBuffer.size(); ++sampleIndex) {
//
//       }
    };

    vector<T> getPcmValueFromFile(const char *filePath, jint splitBy) {
        AudioFile<double> audioFile;
        audioFile.load(filePath);
        audioFile.printSummary();
        vector<vector<T>> fileStream = audioFile.samples;

        vector<T> firstChannel = fileStream[0];
        int bufferSize = audioFile.getSampleRate() * splitBy  ; //split buffer by N second
        vector<T> result;
        vector<vector<T>> arrayBuffer = splitBufferIntoArrayBuffer(firstChannel, bufferSize);
        for (int i = 0; i < arrayBuffer.size(); ++i) {
            T value = rmsPcmRawValue(arrayBuffer[i]);
            result.push_back(value);
        }

        return result;
    }

    void convertByteArrayToUnsignedChar(JNIEnv *env, jbyteArray byteArray) {
       int len = env->GetArrayLength(byteArray);

       unsigned char *buf = new unsigned char[len];

       env->GetByteArrayRegion(byteArray, 0, len, reinterpret_cast<jbyte *>(buf));
       for (int i = 0; i < len; ++i) {
          LOG_D("%d hex: %02x- int: %d - char: %c", i, buf[i], buf[i], buf[i]);
       }
    }

    vector<vector<T>> splitBufferIntoArrayBuffer(vector<T> buffer, int smallBufferSize) {
       vector<vector<T>> arraySplitBuffer;
       int bufferSize = buffer.size();
       int startIndex = 0;
       while (startIndex < bufferSize) {
          vector<T> tempBuffer;

          if (bufferSize - startIndex < smallBufferSize) {
             smallBufferSize = bufferSize - startIndex;
          }

          tempBuffer.resize(smallBufferSize);
          for (int sampleIndex = 0; sampleIndex < smallBufferSize; ++sampleIndex) {
             tempBuffer.push_back(buffer[startIndex + sampleIndex]);
          }

          arraySplitBuffer.push_back(tempBuffer);

          startIndex += smallBufferSize;
       }

       return arraySplitBuffer;
    }

    double ratingSilentOfBuffer(vector<T> buffer) {
       T sum = 0;
//       for (var i = 0; i < _buffer.length; i = i + 2)
//       {
//          double sample = BitConverter.ToInt16(_buffer, i) / 32768.0;
//          sum += (sample * sample);
//       }
//       double rms = Math.Sqrt(sum / (_buffer.length / 2));
//       var decibel = 20 * Math.Log10(rms);
       int count = 0;
       for (int sampleIndex = 0; sampleIndex < buffer.size(); ++sampleIndex) {
          T sample = buffer[sampleIndex];
          sum += (sample * sample);
       }
       const float kMaxSquaredLevel = 32768 * 32768;
       T rms = sqrt(sum / (buffer.size()));
       T decibel = 20 * log10(rms);
//
       return decibel;
    }

    void logBuffer(vector<T> buffer) {
       for (int i = 0; i < buffer.size(); ++i) {
          LOG_D("buffer[%d]: %f", i, buffer[i]);
       }
    }

    T rmsPcmRawValue(vector<T> buffer) {
       T sum = 0;
       for (int sampleIndex = 0; sampleIndex < buffer.size(); ++sampleIndex) {
          T sample = buffer[sampleIndex];
          sum += (sample * sample);
       }
       T rms = sqrt(sum / (buffer.size()));
       return rms;
    }
};


#endif //AUDIOEXAMPLE_CLAZZ_TEST_H
