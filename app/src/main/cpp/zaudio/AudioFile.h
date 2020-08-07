//
// Created by cris on 8/7/20.
//

#ifndef AUDIOEXAMPLE_AUDIOFILE_H
#define AUDIOEXAMPLE_AUDIOFILE_H

#include <iostream>
#include <vector>
#include <assert.h>
#include <string>
#include <fstream>
#include <unordered_map>
#include <iterator>
#include <algorithm>
#include "../zdebug/debug.h"

using namespace std;

enum class AudioFileFormat {
    ERROR,
    NOT_LOADED,
    WAVE,
    AIFF,
};

typedef vector<vector<int>> AudioBuffer;

template<class T>
class AudioFile {
private:
    enum class Endianness {
        LITTLE_ENDIAN,
        BIG, ENDIAN,
    };
    AudioFileFormat audioFileFormat;
    uint32_t sampleRate;
    int bitDepth;
    bool logErrorsToConsole{true};


public:
    AudioBuffer samples;
    string iXMLChunk;


private:
    AudioFileFormat getAudioFileFormatFromFileData(vector<uint8_t> fileData);

    bool decodeWaveFile(vector<uint8_t> &fileData);

    bool decodeAiffFile(vector<uint8_t> &fileData);

    bool saveToWaveFile(string filePath);

    bool saveToAiffFile(string filePath);

    void clearAudioBuffer();

    //-----------------------------------------------------------------------------
    int32_t fourBytesToInt(vector<int8_t> &source, int startIndex,
                           Endianness endianness = Endianness::LITTLE_ENDIAN);

    int16_t twoBytesToInt(vector<int8_t> &source, int startIndex,
                          Endianness endianness = Endianness::LITTLE_ENDIAN);

    int getIndexOfString(vector<uint8_t> &source, string s);

    int
    getIndexOfChunk(std::vector<uint8_t> &source, const std::string &chunkHeaderID, int startIndex,
                    Endianness endianness = Endianness::LittleEndian);


    //-----------------------------------------------------------------------------
    T sixteenBitIntToSample(int16_t sample);

    int16_t sampleToSixteenBitInt(T sample);

    //-----------------------------------------------------------------------------
    uint8_t sampleToSingleByte(T sample);

    T singleByteToSample(uint8_t sample);

    uint32_t getAiffSampleRate(std::vector<uint8_t> &fileData, int sampleRateStartIndex);

    bool tenByteMatch(std::vector<uint8_t> &v1, int startIndex1, std::vector<uint8_t> &v2,
                      int startIndex2);

    void addSampleRateToAiffData(std::vector<uint8_t> &fileData, uint32_t sampleRate);

    T clamp(T v1, T minValue, T maxValue);

    //-----------------------------------------------------------------------------
    void addStringToFileData(std::vector<uint8_t> &fileData, std::string s);

    void addInt32ToFileData(std::vector<uint8_t> &fileData, int32_t i,
                            Endianness endianness = Endianness::LittleEndian);

    void addInt16ToFileData(std::vector<uint8_t> &fileData, int16_t i,
                            Endianness endianness = Endianness::LittleEndian);

    //-----------------------------------------------------------------------------
    bool writeDataToFile(std::vector<uint8_t> &fileData, std::string filePath);
    //-----------------------------------------------------------------------------

public:

    AudioFile();

    bool load(string filePath);

    bool save(string filePath, AudioFileFormat format = AudioFileFormat::WAVE);

    uint32_t getSampleRate() const;

    int getNumChannels() const;

    bool isMono() const;

    bool isStereo() const;

    int getBitDepth() const;

    int getNumSamplesPerChannel() const;

    double getLengthInSeconds() const;

    void printSummary() const;


    bool setAudioBuffer(AudioBuffer &newBuffer);

    void setAudioBufferSize(int numChannel, int numSamples);

    void setNumSamplesPerChannel(int numSamples);

    void setNumChannels(int numChannels);

    void setBitDepth(int numBitsPerSample);

    void setSampleRate(uint32_t nSampleRate);

    void shouldLogErrorsToConsole(bool logErrors);
};


//-----------------------------------------------------------------------------
// Pre-defined 10-byte representations of common sample rates
static unordered_map<uint32_t, std::vector<uint8_t>> aiffSampleRateTable = {
        {8000,    {64, 11, 250, 0,   0, 0, 0, 0, 0, 0}},
        {11025,   {64, 12, 172, 68,  0, 0, 0, 0, 0, 0}},
        {16000,   {64, 12, 250, 0,   0, 0, 0, 0, 0, 0}},
        {22050,   {64, 13, 172, 68,  0, 0, 0, 0, 0, 0}},
        {32000,   {64, 13, 250, 0,   0, 0, 0, 0, 0, 0}},
        {37800,   {64, 14, 147, 168, 0, 0, 0, 0, 0, 0}},
        {44056,   {64, 14, 172, 24,  0, 0, 0, 0, 0, 0}},
        {44100,   {64, 14, 172, 68,  0, 0, 0, 0, 0, 0}},
        {47250,   {64, 14, 184, 146, 0, 0, 0, 0, 0, 0}},
        {48000,   {64, 14, 187, 128, 0, 0, 0, 0, 0, 0}},
        {50000,   {64, 14, 195, 80,  0, 0, 0, 0, 0, 0}},
        {50400,   {64, 14, 196, 224, 0, 0, 0, 0, 0, 0}},
        {88200,   {64, 15, 172, 68,  0, 0, 0, 0, 0, 0}},
        {96000,   {64, 15, 187, 128, 0, 0, 0, 0, 0, 0}},
        {176400,  {64, 16, 172, 68,  0, 0, 0, 0, 0, 0}},
        {192000,  {64, 16, 187, 128, 0, 0, 0, 0, 0, 0}},
        {352800,  {64, 17, 172, 68,  0, 0, 0, 0, 0, 0}},
        {2822400, {64, 20, 172, 68,  0, 0, 0, 0, 0, 0}},
        {5644800, {64, 21, 172, 68,  0, 0, 0, 0, 0, 0}}
};

//-----------------------------------------------------------------------------
enum WavAudioFormat {
    PCM = 0x0001,
    IEEEFloat = 0x0003,
    ALaw = 0x0006,
    MULaw = 0x0007,
    Extensible = 0xFFFE
};

//-----------------------------------------------------------------------------
enum AIFFAudioFormat {
    Uncompressed,
    Compressed,
    Error
};

#endif //AUDIOEXAMPLE_AUDIOFILE_H
