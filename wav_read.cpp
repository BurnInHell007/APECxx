/**
 * WAVE file read functionalities
 * Author: Vijay Kumar B
 */
#include <iostream>
#include <memory>
#include <fstream>
#include <array>
#include <cstdint>
#include <string>

using wav_header = struct wav_header_file;
using wav_header_ptr = std::unique_ptr<wav_header>;

/**
 * @brief RIFF format header file for WAV audio files
 * for detailed explanation of WAV header format visit: http://soundfile.sapp.org/doc/WaveFormat/
 */
struct wav_header_file {
    // The "RIFF" Chunk
    std::array<char, 4> chunkId;
    int32_t chunkSize;
    std::array<char, 4> format;

    // The "fmt " sub-chunk
    std::array<char, 4> subChunk1ID;
    int32_t subChunk1Size;
    int16_t audioFormat;   // 1 for PCM | Linear quantization
    int16_t numChannels;   // 1 for Mono, 2 for Stereo
    int32_t sampleRate;    // DataSize/sampleRate = no. of sec audio
    int32_t byteRate;      // SampleRate * NumChannels * BitsPerSample/8
    int16_t blockAlign;    // NumChannels * BitsPerSample/8
    int16_t bitsPerSample; // 8 bits = 8, 16 bits = 16

    // The "data" subchunk
    std::array<char, 4> subChunk2ID;
    int32_t subChunk2Size;
    
    // Rest of the bytes belongs to data
};

/**
 * @brief using Fstream read the wav file and the metadata or "Header"
 * @param file_path : can take in cmd or normally
 * @param headerPtr : wav header instance | std::make_unique<wav_header> (similar to new keyword)
 */
bool readWAVHeader(const std::string& file_path, wav_header_ptr &headerPtr) {
    // open file in binary mode
    std::ifstream file(file_path, std::ios::binary | std::ios::in);
    
    if(!file.is_open()) {
        std::cerr << "Error: Could not open the file " << file_path << std::endl;
        return false;
    }

    // read the header data
    // read() takes 2 argument
    // a. A pointer to header location where the data should be stored cast to char* (address ->const char* so use reinerpret_cast)
    // b. Number of bytes to read or the sizeof the wav_header
    // for C-style file reading, we cannot use unique pointers, only raw pointers are allowed (using get on smartpointers)
    file.read(
        reinterpret_cast<char*>(headerPtr.get()), 
        sizeof(wav_header)
    );
    
    if(file.gcount() != sizeof(wav_header)) {
        std::cerr << "Error: Failed to read the full header." << std::endl;
        return false;
    }

    file.close();
    return true;
}
