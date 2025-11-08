#include <array>
#include <cstdint>
#include <memory>
#include <string>
// required for unique_ptr functionalities

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

bool readWAVHeader(const std::string& file_path, wav_header_ptr &header_ptr);

bool writeBackWAV(const std::string& init_file_path, const std::string& final_file_path);
