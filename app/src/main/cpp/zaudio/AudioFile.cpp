//
// Created by cris on 8/7/20.
//

#include <iostream>
#include "AudioFile.h"
#include <fstream>

using namespace std;

//-----------------------------------------------------------------------------
template<class T>
AudioFile<T>::AudioFile() {
   bitDepth = 16;
   sampleRate = 44100;
   samples.resize(1);
   samples[0].resize(0);
   audioFileFormat = AudioFileFormat::NOT_LOADED;
}

//PRIVATE FUNCTION---------------------------------------------------------------------
template<class T>
AudioFileFormat AudioFile<T>::getAudioFileFormatFromFileData(vector <uint8_t> fileData) {
   string header(fileData.begin(), fileData.begin() + 4);
   if (header == "RIFF")
      return AudioFileFormat::WAVE;
   else if (header == "FORM")
      return AudioFileFormat::AIFF;
   else
      return AudioFileFormat::ERROR;
}

//TODO check this function fourBytesToInt
template<class T>
int32_t AudioFile<T>::fourBytesToInt(vector <int8_t> &source, int startIndex,
                                     AudioFile<T>::Endianness endianness) {
   int32_t result;

   if (endianness == Endianness::LittleEndian)
      result = (source[startIndex + 3] << 24) | (source[startIndex + 2] << 16) |
               (source[startIndex + 1] << 8) | source[startIndex];
   else
      result = (source[startIndex] << 24) | (source[startIndex + 1] << 16) |
               (source[startIndex + 2] << 8) | source[startIndex + 3];
   return result;
}

template<class T>
int16_t AudioFile<T>::twoBytesToInt(vector <int8_t> &source, int startIndex,
                                    AudioFile::Endianness endianness) {
   int16_t result;

   if (endianness == Endianness::LittleEndian)
      result = (source[startIndex + 1] << 8) | source[startIndex];
   else
      result = (source[startIndex] << 8) | source[startIndex + 1];

   return result;
}


//=============================================================
template<class T>
int AudioFile<T>::getIndexOfString(std::vector<uint8_t> &source, std::string stringToSearchFor) {
   int index = -1;
   int stringLength = (int) stringToSearchFor.length();

   for (size_t i = 0; i < source.size() - stringLength; i++) {
      std::string section(source.begin() + i, source.begin() + i + stringLength);

      if (section == stringToSearchFor) {
         index = static_cast<int> (i);
         break;
      }
   }

   return index;
}

//=============================================================
template<class T>
int AudioFile<T>::getIndexOfChunk(std::vector<uint8_t> &source, const std::string &chunkHeaderID,
                                  int startIndex, AudioFile::Endianness endianness) {
   constexpr int dataLen = 4;
   if (chunkHeaderID.size() != dataLen) {
      assert (false && "Invalid chunk header ID string");
      return -1;
   }

   int i = startIndex;
   while (i < source.size() - dataLen) {
      if (memcmp(&source[i], chunkHeaderID.data(), dataLen) == 0) {
         return i;
      }

      i += dataLen;
      auto chunkSize = fourBytesToInt(source, i, endianness);
      i += (dataLen + chunkSize);
   }

   return -1;
}

//=============================================================
template<class T>
T AudioFile<T>::sixteenBitIntToSample(int16_t sample) {
   return static_cast<T> (sample) / static_cast<T> (32768.);
}

//=============================================================
template<class T>
int16_t AudioFile<T>::sampleToSixteenBitInt(T sample) {
   sample = clamp(sample, -1., 1.);
   return static_cast<int16_t> (sample * 32767.);
}

//=============================================================
template<class T>
uint8_t AudioFile<T>::sampleToSingleByte(T sample) {
   sample = clamp(sample, -1., 1.);
   sample = (sample + 1.) / 2.;
   return static_cast<uint8_t> (sample * 255.);
}

//=============================================================
template<class T>
T AudioFile<T>::singleByteToSample(uint8_t sample) {
   return static_cast<T> (sample - 128) / static_cast<T> (128.);
}

//=============================================================
template<class T>
T AudioFile<T>::clamp(T value, T minValue, T maxValue) {
   value = std::min(value, maxValue);
   value = std::max(value, minValue);
   return value;
}



//PUBLIC FUNCTION-----------------------------------------------------------------------------

template<class T>
uint32_t AudioFile<T>::getSampleRate() const {
   return sampleRate;
}


template<class T>
int AudioFile<T>::getNumChannels() const {
   return (int) samples.size();
}

template<class T>
bool AudioFile<T>::isMono() const {
   return getNumChannels() == 1;
}

template<class T>
bool AudioFile<T>::isStereo() const {
   return getNumChannels() == 2;
}

template<class T>
int AudioFile<T>::getBitDepth() const {
   return bitDepth;
}

template<class T>
int AudioFile<T>::getNumSamplesPerChannel() const {
   if (samples.size() > 0) {
      return (int) samples[0].size();
   } else {
      return 0;
   }
}

template<class T>
double AudioFile<T>::getLengthInSeconds() const {
   return (double) getNumSamplesPerChannel() / (double) sampleRate;
}

template<class T>
void AudioFile<T>::printSummary() const {
   std::cout << "|======================================|" << std::endl;
   std::cout << "Num Channels: " << getNumChannels() << std::endl;
   std::cout << "Num Samples Per Channel: " << getNumSamplesPerChannel() << std::endl;
   std::cout << "Sample Rate: " << sampleRate << std::endl;
   std::cout << "Bit Depth: " << bitDepth << std::endl;
   std::cout << "Length in Seconds: " << getLengthInSeconds() << std::endl;
   std::cout << "|======================================|" << std::endl;
}

template<class T>
bool AudioFile<T>::setAudioBuffer(AudioBuffer &newBuffer) {
   int numChannles = (int) newBuffer.size();
   if (numChannles <= 0) {
      LOG_W("The buffer your are trying to use has no channels");
      return false;
   }

   size_t numSamples = newBuffer[0].size();
   samples.resize(newBuffer.size());

   for (int i = 0; i < getNumChannels(); ++i) {
      samples[i].resize(numSamples);
      for (size_t j = 0; j < numSamples; ++j) {
         samples[i][j] = newBuffer[i][j];
      }
   }
   return true;
}

template<class T>
void AudioFile<T>::setAudioBufferSize(int numChannel, int numSamples) {
   samples.resize(numChannel);
   setNumSamplesPerChannel(numSamples);
}

template<class T>
void AudioFile<T>::setNumSamplesPerChannel(int numSamples) {
   int originalSize = getNumSamplesPerChannel();
   for (int i = 0; i < getNumChannels(); ++i) {
      samples[i].resize(numSamples);

      if (numSamples > originalSize) {
         fill(samples[i].begin() + originalSize, samples[i].end(), (T) 0.);
      }
   }
}


template<class T>
void AudioFile<T>::setNumChannels(int numChannels) {
   int originalNumChannels = getNumChannels();
   int originalNumSamplesPerChannel = getNumSamplesPerChannel();

   samples.resize(numChannels);

   if (numChannels > originalNumChannels) {
      for (int i = originalNumChannels; i < numChannels; ++i) {
         samples[i].resize(originalNumSamplesPerChannel);
         fill(samples[i].begin(), samples[i].end(), (T) 0.);
      }
   }
}

template<class T>
void AudioFile<T>::setBitDepth(int numBitsPerSample) {
   bitDepth = numBitsPerSample;
}

template<class T>
void AudioFile<T>::setSampleRate(uint32_t newSampleRate) {
   sampleRate = newSampleRate;
}

template<class T>
void AudioFile<T>::shouldLogErrorsToConsole(bool logErrors) {
   logErrorsToConsole = logErrors;
}

template<class T>
bool AudioFile<T>::load(string filePath) {
   fstream fileStream(filePath, ios::binary);

   if (!fileStream.good()) {
      return false;
   }

   fileStream.unsetf(ios::skipws);
   istream_iterator <uint8_t> begin(fileStream), end;
   vector <uint8_t> fileData(begin, end);

   audioFileFormat = getAudioFileFormatFromFileData(fileData);

   if (audioFileFormat == AudioFileFormat::WAVE) {
      return decodeWaveFile(fileData);
   } else if (audioFileFormat == AudioFileFormat::AIFF) {
      return decodeAiffFile(fileData);
   } else {
      LOG_E("Audio File Type: Error");
      return false;
   }
}

template<class T>
bool AudioFile<T>::save(string filePath, AudioFileFormat format) {
   if (format == AudioFileFormat::WAVE) {
      return saveToWaveFile(filePath);
   } else if (format == AudioFileFormat::AIFF) {
      return saveToAiffFile(filePath);
   }

   return false;
}

template<class T>
bool AudioFile<T>::decodeWaveFile(std::vector<uint8_t> &fileData) {
   string headerChunkID(fileData.begin(), fileData.begin() + 4);
   string format(fileData.begin() + 8, fileData.begin() + 12);

   // try and find the start points of key chunks
   int indexOfDataChunk = getIndexOfChunk(fileData, "data", 12);
   int indexOfFormatChunk = getIndexOfChunk(fileData, "fmt ", 12);
   int indexOfXMLChunk = getIndexOfChunk(fileData, "iXML", 12);

   if (indexOfDataChunk == -1 || indexOfFormatChunk == -1 || headerChunkID != "RIFF" ||
       format != "WAVE") {
      LOG_E("ERROR: this doesn't seem to be a valid .WAV file");
      return false;
   }

   // FORMAT CHUNK
   int f = indexOfFormatChunk;
   string formatChunkID(fileData.begin() + f, fileData.begin() + f + 4);

   int16_t audioFormat = twoBytesToInt(fileData, f + 8);
   int16_t numChannels = twoBytesToInt(fileData, f + 10);
   sampleRate = (uint32_t) fourBytesToInt(fileData, f + 12);
   int32_t numBytesPerSecond = fourBytesToInt(fileData, f + 16);
   int16_t numBytesPerBlock = twoBytesToInt(fileData, f + 20);
   bitDepth = (int) twoBytesToInt(fileData, f + 22);

   int numBytesPerSample = bitDepth / 8;

   // check that the audio format is PCM or Float
   if (audioFormat != WavAudioFormat::PCM && audioFormat != WavAudioFormat::IEEEFloat) {
      LOG_E("ERROR: this .WAV file is encoded in a format that this library does not support at present");
      return false;
   }

   // check the number of channels is mono or stereo
   if (numChannels < 1 || numChannels > 128) {
      LOG_E("ERROR: this WAV file seems to be an invalid number of channels (or corrupted?)");
      return false;
   }

   // check header data is consistent
   if ((numBytesPerSecond != (numChannels * sampleRate * bitDepth) / 8) ||
       (numBytesPerBlock != (numChannels * numBytesPerSample))) {
      LOG_E("ERROR: the header data in this WAV file seems to be inconsistent");
      return false;
   }

   // check bit depth is either 8, 16, 24 or 32 bit
   if (bitDepth != 8 && bitDepth != 16 && bitDepth != 24 && bitDepth != 32) {
      LOG_E("ERROR: this file has a bit depth that is not 8, 16, 24 or 32 bits");
      return false;
   }

   // DATA CHUNK
   int d = indexOfDataChunk;
   string dataChunkID(fileData.begin() + d, fileData.begin() + d + 4);
   int32_t dataChunkSize = fourBytesToInt(fileData, d + 4);

   int numSamples = dataChunkSize / (numChannels * bitDepth / 8);
   int samplesStartIndex = indexOfDataChunk + 8;

   clearAudioBuffer();
   samples.resize(numChannels);

   for (int i = 0; i < numSamples; ++i) {
      for (int channel = 0; channel < numChannels; ++channel) {
         int sampleIndex =
                 samplesStartIndex + (numBytesPerBlock * i) + channel * numBytesPerSample;

         if (bitDepth == 8) {
            T sample = singleByteToSample(fileData[sampleIndex]);
            samples[channel].push_back(sample);
         } else if (bitDepth == 16) {
            int16_t sampleAsInt = twoBytesToInt(fileData, sampleIndex);
            T sample = sixteenBitIntToSample(sampleAsInt);
            samples[channel].push_back(sample);
         } else if (bitDepth == 24) {
            int32_t sampleAsInt = 0;
            sampleAsInt = (fileData[sampleIndex + 2] << 16) | (fileData[sampleIndex + 1] << 8) |
                          fileData[sampleIndex];

            if (sampleAsInt &
                0x800000) //  if the 24th bit is set, this is a negative number in 24-bit world
               sampleAsInt = sampleAsInt |
                             ~0xFFFFFF; // so make sure sign is extended to the 32 bit float

            T sample = (T) sampleAsInt / (T) 8388608.;
            samples[channel].push_back(sample);
         } else if (bitDepth == 32) {
            int32_t sampleAsInt = fourBytesToInt(fileData, sampleIndex);
            T sample;

            if (audioFormat == WavAudioFormat::IEEEFloat)
               sample = (T) reinterpret_cast<float &> (sampleAsInt);
            else // assume PCM
               sample = (T) sampleAsInt /
                        static_cast<float> (std::numeric_limits<std::int32_t>::max());

            samples[channel].push_back(sample);
         } else {
            assert(false);
         }

      }
   }
   // iXML CHUNK
   if (indexOfXMLChunk != -1) {
      int32_t chunkSize = fourBytesToInt(fileData, indexOfXMLChunk + 4);
      iXMLChunk = std::string((const char *) &fileData[indexOfXMLChunk + 8], chunkSize);
   }

   return true;
}


template<class T>
bool AudioFile<T>::decodeAiffFile(std::vector<uint8_t> &fileData) {
   // -----------------------------------------------------------
   // HEADER CHUNK
   std::string headerChunkID(fileData.begin(), fileData.begin() + 4);
   //int32_t fileSizeInBytes = fourBytesToInt (fileData, 4, Endianness::BigEndian) + 8;
   std::string format(fileData.begin() + 8, fileData.begin() + 12);

   int audioFormat = format == "AIFF" ? AIFFAudioFormat::Uncompressed : format == "AIFC"
                                                                        ? AIFFAudioFormat::Compressed
                                                                        : AIFFAudioFormat::Error;

   // -----------------------------------------------------------
   // try and find the start points of key chunks
   int indexOfCommChunk = getIndexOfChunk(fileData, "COMM", 12, Endianness::BigEndian);
   int indexOfSoundDataChunk = getIndexOfChunk(fileData, "SSND", 12, Endianness::BigEndian);
   int indexOfXMLChunk = getIndexOfChunk(fileData, "iXML", 12, Endianness::BigEndian);

   // if we can't find the data or format chunks, or the IDs/formats don't seem to be as expected
   // then it is unlikely we'll able to read this file, so abort
   if (indexOfSoundDataChunk == -1 || indexOfCommChunk == -1 || headerChunkID != "FORM" ||
       audioFormat == AIFFAudioFormat::Error) {
      LOG_E("ERROR: this doesn't seem to be a valid AIFF file");
      return false;
   }

   // -----------------------------------------------------------
   // COMM CHUNK
   int p = indexOfCommChunk;
   std::string commChunkID(fileData.begin() + p, fileData.begin() + p + 4);
   //int32_t commChunkSize = fourBytesToInt (fileData, p + 4, Endianness::BigEndian);
   int16_t numChannels = twoBytesToInt(fileData, p + 8, Endianness::BigEndian);
   int32_t numSamplesPerChannel = fourBytesToInt(fileData, p + 10, Endianness::BigEndian);
   bitDepth = (int) twoBytesToInt(fileData, p + 14, Endianness::BigEndian);
   sampleRate = getAiffSampleRate(fileData, p + 16);

   // check the sample rate was properly decoded
   if (sampleRate == 0) {
      LOG_E("ERROR: this AIFF file has an unsupported sample rate");
      return false;
   }

   // check the number of channels is mono or stereo
   if (numChannels < 1 || numChannels > 2) {
      LOG_E(
              "ERROR: this AIFF file seems to be neither mono nor stereo (perhaps multi-track, or corrupted?)");
      return false;
   }

   // check bit depth is either 8, 16, 24 or 32-bit
   if (bitDepth != 8 && bitDepth != 16 && bitDepth != 24 && bitDepth != 32) {
      LOG_E("ERROR: this file has a bit depth that is not 8, 16, 24 or 32 bits");
      return false;
   }

   // -----------------------------------------------------------
   // SSND CHUNK
   int s = indexOfSoundDataChunk;
   std::string soundDataChunkID(fileData.begin() + s, fileData.begin() + s + 4);
   int32_t soundDataChunkSize = fourBytesToInt(fileData, s + 4, Endianness::BigEndian);
   int32_t offset = fourBytesToInt(fileData, s + 8, Endianness::BigEndian);
   //int32_t blockSize = fourBytesToInt (fileData, s + 12, Endianness::BigEndian);

   int numBytesPerSample = bitDepth / 8;
   int numBytesPerFrame = numBytesPerSample * numChannels;
   int totalNumAudioSampleBytes = numSamplesPerChannel * numBytesPerFrame;
   int samplesStartIndex = s + 16 + (int) offset;

   // sanity check the data
   if ((soundDataChunkSize - 8) != totalNumAudioSampleBytes ||
       totalNumAudioSampleBytes > static_cast<long>(fileData.size() - samplesStartIndex)) {
      LOG_E("ERROR: the metadatafor this file doesn't seem right");
      return false;
   }

   clearAudioBuffer();
   samples.resize(numChannels);

   for (int i = 0; i < numSamplesPerChannel; i++) {
      for (int channel = 0; channel < numChannels; channel++) {
         int sampleIndex = samplesStartIndex + (numBytesPerFrame * i) + channel * numBytesPerSample;

         if (bitDepth == 8) {
            int8_t sampleAsSigned8Bit = (int8_t) fileData[sampleIndex];
            T sample = (T) sampleAsSigned8Bit / (T) 128.;
            samples[channel].push_back(sample);
         } else if (bitDepth == 16) {
            int16_t sampleAsInt = twoBytesToInt(fileData, sampleIndex, Endianness::BigEndian);
            T sample = sixteenBitIntToSample(sampleAsInt);
            samples[channel].push_back(sample);
         } else if (bitDepth == 24) {
            int32_t sampleAsInt = 0;
            sampleAsInt = (fileData[sampleIndex] << 16) | (fileData[sampleIndex + 1] << 8) |
                          fileData[sampleIndex + 2];

            if (sampleAsInt &
                0x800000) //  if the 24th bit is set, this is a negative number in 24-bit world
               sampleAsInt =
                       sampleAsInt | ~0xFFFFFF; // so make sure sign is extended to the 32 bit float

            T sample = (T) sampleAsInt / (T) 8388608.;
            samples[channel].push_back(sample);
         } else if (bitDepth == 32) {
            int32_t sampleAsInt = fourBytesToInt(fileData, sampleIndex, Endianness::BigEndian);
            T sample;

            if (audioFormat == AIFFAudioFormat::Compressed)
               sample = (T) reinterpret_cast<float &> (sampleAsInt);
            else // assume uncompressed
               sample = (T) sampleAsInt /
                        static_cast<float> (std::numeric_limits<std::int32_t>::max());

            samples[channel].push_back(sample);
         } else {
            assert (false);
         }
      }
   }

   // -----------------------------------------------------------
   // iXML CHUNK
   if (indexOfXMLChunk != -1) {
      int32_t chunkSize = fourBytesToInt(fileData, indexOfXMLChunk + 4);
      iXMLChunk = std::string((const char *) &fileData[indexOfXMLChunk + 8], chunkSize);
   }

   return true;
}

//=============================================================
template<class T>
uint32_t AudioFile<T>::getAiffSampleRate(std::vector<uint8_t> &fileData, int sampleRateStartIndex) {
   for (auto it : aiffSampleRateTable) {
      if (tenByteMatch(fileData, sampleRateStartIndex, it.second, 0))
         return it.first;
   }

   return 0;
}

template<class T>
bool AudioFile<T>::tenByteMatch(std::vector<uint8_t> &v1, int startIndex1, std::vector<uint8_t> &v2,
                                int startIndex2) {
   for (int i = 0; i < 10; i++) {
      if (v1[startIndex1 + i] != v2[startIndex2 + i])
         return false;
   }

   return true;
}

//=============================================================
template<class T>
void AudioFile<T>::addSampleRateToAiffData(std::vector<uint8_t> &fileData, uint32_t sampleRate) {
   if (aiffSampleRateTable.count(sampleRate) > 0) {
      for (int i = 0; i < 10; i++)
         fileData.push_back(aiffSampleRateTable[sampleRate][i]);
   }
}

template<class T>
bool AudioFile<T>::saveToWaveFile(std::string filePath) {
   std::vector<uint8_t> fileData;

   int32_t dataChunkSize = getNumSamplesPerChannel() * (getNumChannels() * bitDepth / 8);
   int16_t audioFormat = bitDepth == 32 ? WavAudioFormat::IEEEFloat : WavAudioFormat::PCM;
   int32_t formatChunkSize = audioFormat == WavAudioFormat::PCM ? 16 : 18;
   int32_t iXMLChunkSize = static_cast<int32_t> (iXMLChunk.size());

   // -----------------------------------------------------------
   // HEADER CHUNK
   addStringToFileData(fileData, "RIFF");

   // The file size in bytes is the header chunk size (4, not counting RIFF and WAVE) + the format
   // chunk size (24) + the metadata part of the data chunk plus the actual data chunk size
   int32_t fileSizeInBytes = 4 + formatChunkSize + 8 + 8 + dataChunkSize;
   if (iXMLChunkSize > 0) {
      fileSizeInBytes += (8 + iXMLChunkSize);
   }

   addInt32ToFileData(fileData, fileSizeInBytes);

   addStringToFileData(fileData, "WAVE");

   // -----------------------------------------------------------
   // FORMAT CHUNK
   addStringToFileData(fileData, "fmt ");
   addInt32ToFileData(fileData, formatChunkSize); // format chunk size (16 for PCM)
   addInt16ToFileData(fileData, audioFormat); // audio format
   addInt16ToFileData(fileData, (int16_t) getNumChannels()); // num channels
   addInt32ToFileData(fileData, (int32_t) sampleRate); // sample rate

   int32_t numBytesPerSecond = (int32_t) ((getNumChannels() * sampleRate * bitDepth) / 8);
   addInt32ToFileData(fileData, numBytesPerSecond);

   int16_t numBytesPerBlock = getNumChannels() * (bitDepth / 8);
   addInt16ToFileData(fileData, numBytesPerBlock);

   addInt16ToFileData(fileData, (int16_t) bitDepth);

   if (audioFormat == WavAudioFormat::IEEEFloat)
      addInt16ToFileData(fileData, 0); // extension size

   // -----------------------------------------------------------
   // DATA CHUNK
   addStringToFileData(fileData, "data");
   addInt32ToFileData(fileData, dataChunkSize);

   for (int i = 0; i < getNumSamplesPerChannel(); i++) {
      for (int channel = 0; channel < getNumChannels(); channel++) {
         if (bitDepth == 8) {
            uint8_t byte = sampleToSingleByte(samples[channel][i]);
            fileData.push_back(byte);
         } else if (bitDepth == 16) {
            int16_t sampleAsInt = sampleToSixteenBitInt(samples[channel][i]);
            addInt16ToFileData(fileData, sampleAsInt);
         } else if (bitDepth == 24) {
            int32_t sampleAsIntAgain = (int32_t) (samples[channel][i] * (T) 8388608.);

            uint8_t bytes[3];
            bytes[2] = (uint8_t) (sampleAsIntAgain >> 16) & 0xFF;
            bytes[1] = (uint8_t) (sampleAsIntAgain >> 8) & 0xFF;
            bytes[0] = (uint8_t) sampleAsIntAgain & 0xFF;

            fileData.push_back(bytes[0]);
            fileData.push_back(bytes[1]);
            fileData.push_back(bytes[2]);
         } else if (bitDepth == 32) {
            int32_t sampleAsInt;

            if (audioFormat == WavAudioFormat::IEEEFloat)
               sampleAsInt = (int32_t)
                       reinterpret_cast<int32_t &> (samples[channel][i]);
            else // assume PCM
               sampleAsInt = (int32_t) (samples[channel][i] * std::numeric_limits<int32_t>::max());

            addInt32ToFileData(fileData, sampleAsInt, Endianness::LittleEndian);
         } else {
            assert (false && "Trying to write a file with unsupported bit depth");
            return false;
         }
      }
   }

   // -----------------------------------------------------------
   // iXML CHUNK
   if (iXMLChunkSize > 0) {
      addStringToFileData(fileData, "iXML");
      addInt32ToFileData(fileData, iXMLChunkSize);
      addStringToFileData(fileData, iXMLChunk);
   }

   // check that the various sizes we put in the metadata are correct
   if (fileSizeInBytes != static_cast<int32_t> (fileData.size() - 8) ||
       dataChunkSize != (getNumSamplesPerChannel() * getNumChannels() * (bitDepth / 8))) {
      LOG_E("ERROR: couldn't save file to %s", filePath);
      return false;
   }

   // try to write the file
   return writeDataToFile(fileData, filePath);
}

//=============================================================
template<class T>
bool AudioFile<T>::saveToAiffFile(std::string filePath) {
   std::vector<uint8_t> fileData;

   int32_t numBytesPerSample = bitDepth / 8;
   int32_t numBytesPerFrame = numBytesPerSample * getNumChannels();
   int32_t totalNumAudioSampleBytes = getNumSamplesPerChannel() * numBytesPerFrame;
   int32_t soundDataChunkSize = totalNumAudioSampleBytes + 8;
   int32_t iXMLChunkSize = static_cast<int32_t> (iXMLChunk.size());

   // -----------------------------------------------------------
   // HEADER CHUNK
   addStringToFileData(fileData, "FORM");

   // The file size in bytes is the header chunk size (4, not counting FORM and AIFF) + the COMM
   // chunk size (26) + the metadata part of the SSND chunk plus the actual data chunk size
   int32_t fileSizeInBytes = 4 + 26 + 16 + totalNumAudioSampleBytes;
   if (iXMLChunkSize > 0) {
      fileSizeInBytes += (8 + iXMLChunkSize);
   }

   addInt32ToFileData(fileData, fileSizeInBytes, Endianness::BigEndian);

   addStringToFileData(fileData, "AIFF");

   // -----------------------------------------------------------
   // COMM CHUNK
   addStringToFileData(fileData, "COMM");
   addInt32ToFileData(fileData, 18, Endianness::BigEndian); // commChunkSize
   addInt16ToFileData(fileData, getNumChannels(), Endianness::BigEndian); // num channels
   addInt32ToFileData(fileData, getNumSamplesPerChannel(),
                      Endianness::BigEndian); // num samples per channel
   addInt16ToFileData(fileData, bitDepth, Endianness::BigEndian); // bit depth
   addSampleRateToAiffData(fileData, sampleRate);

   // -----------------------------------------------------------
   // SSND CHUNK
   addStringToFileData(fileData, "SSND");
   addInt32ToFileData(fileData, soundDataChunkSize, Endianness::BigEndian);
   addInt32ToFileData(fileData, 0, Endianness::BigEndian); // offset
   addInt32ToFileData(fileData, 0, Endianness::BigEndian); // block size

   for (int i = 0; i < getNumSamplesPerChannel(); i++) {
      for (int channel = 0; channel < getNumChannels(); channel++) {
         if (bitDepth == 8) {
            uint8_t byte = sampleToSingleByte(samples[channel][i]);
            fileData.push_back(byte);
         } else if (bitDepth == 16) {
            int16_t sampleAsInt = sampleToSixteenBitInt(samples[channel][i]);
            addInt16ToFileData(fileData, sampleAsInt, Endianness::BigEndian);
         } else if (bitDepth == 24) {
            int32_t sampleAsIntAgain = (int32_t) (samples[channel][i] * (T) 8388608.);

            uint8_t bytes[3];
            bytes[0] = (uint8_t) (sampleAsIntAgain >> 16) & 0xFF;
            bytes[1] = (uint8_t) (sampleAsIntAgain >> 8) & 0xFF;
            bytes[2] = (uint8_t) sampleAsIntAgain & 0xFF;

            fileData.push_back(bytes[0]);
            fileData.push_back(bytes[1]);
            fileData.push_back(bytes[2]);
         } else if (bitDepth == 32) {
            // write samples as signed integers (no implementation yet for floating point, but looking at WAV implementation should help)
            int32_t sampleAsInt = (int32_t) (
                    samples[channel][i] * std::numeric_limits<int32_t>::max());
            addInt32ToFileData(fileData, sampleAsInt, Endianness::BigEndian);
         } else {
            assert (false && "Trying to write a file with unsupported bit depth");
            return false;
         }
      }
   }

   // -----------------------------------------------------------
   // iXML CHUNK
   if (iXMLChunkSize > 0) {
      addStringToFileData(fileData, "iXML");
      addInt32ToFileData(fileData, iXMLChunkSize, Endianness::BigEndian);
      addStringToFileData(fileData, iXMLChunk);
   }

   // check that the various sizes we put in the metadata are correct
   if (fileSizeInBytes != static_cast<int32_t> (fileData.size() - 8) ||
       soundDataChunkSize != getNumSamplesPerChannel() * numBytesPerFrame + 8) {
      LOG_E("ERROR: couldn't save file to %s", filePath);
      return false;
   }

   // try to write the file
   return writeDataToFile(fileData, filePath);
}

//=============================================================
template<class T>
bool AudioFile<T>::writeDataToFile(std::vector<uint8_t> &fileData, std::string filePath) {
   std::ofstream outputFile(filePath, std::ios::binary);

   if (outputFile.is_open()) {
      for (size_t i = 0; i < fileData.size(); i++) {
         char value = (char) fileData[i];
         outputFile.write(&value, sizeof(char));
      }

      outputFile.close();

      return true;
   }

   return false;
}

//=============================================================
template<class T>
void AudioFile<T>::addStringToFileData(std::vector<uint8_t> &fileData, std::string s) {
   for (size_t i = 0; i < s.length(); i++)
      fileData.push_back((uint8_t) s[i]);
}

//=============================================================
template<class T>
void
AudioFile<T>::addInt32ToFileData(std::vector<uint8_t> &fileData, int32_t i, Endianness endianness) {
   uint8_t bytes[4];

   if (endianness == Endianness::LittleEndian) {
      bytes[3] = (i >> 24) & 0xFF;
      bytes[2] = (i >> 16) & 0xFF;
      bytes[1] = (i >> 8) & 0xFF;
      bytes[0] = i & 0xFF;
   } else {
      bytes[0] = (i >> 24) & 0xFF;
      bytes[1] = (i >> 16) & 0xFF;
      bytes[2] = (i >> 8) & 0xFF;
      bytes[3] = i & 0xFF;
   }

   for (int i = 0; i < 4; i++)
      fileData.push_back(bytes[i]);
}

//=============================================================
template<class T>
void
AudioFile<T>::addInt16ToFileData(std::vector<uint8_t> &fileData, int16_t i, Endianness endianness) {
   uint8_t bytes[2];

   if (endianness == Endianness::LittleEndian) {
      bytes[1] = (i >> 8) & 0xFF;
      bytes[0] = i & 0xFF;
   } else {
      bytes[0] = (i >> 8) & 0xFF;
      bytes[1] = i & 0xFF;
   }

   fileData.push_back(bytes[0]);
   fileData.push_back(bytes[1]);
}

//=============================================================
template<class T>
void AudioFile<T>::clearAudioBuffer() {
   for (size_t i = 0; i < samples.size(); i++) {
      samples[i].clear();
   }

   samples.clear();
}
