#include <string>
#include <cstdint>
#include <fstream>
#include <cstring>
#include <memory>
#include "audio_buffer.hpp"

class WavReader {
public:
    // RAII: Open file in constructor
    explicit WavReader(const std::string& filename);

    // Read entire file into an Audio Buffer
    template<typename SampleType>
    AudioBuffer<SampleType> read();
    
    // getters for file properties
    uint32_t sample_rates() const { return sample_rate_; }
    uint32_t num_channels() const { return num_channels_; }
    uint32_t bits_per_sample() const { return bits_per_sample_; }
    uint32_t num_samples() const { return num_samples_; }

private:
    void read_header(); // Parse RIFF/fmt/data chunks

    std::unique_ptr<std::FILE, decltype(&std::fclose)> file_;
    uint32_t sample_rate_;
    uint32_t num_channels_;
    uint32_t bits_per_sample_;
    uint32_t num_samples_;
    size_t data_start_pos_; // Where the audio data begins in file
};

// Custom deleter - tells unique pointer how to clean
auto file_deleter = [](std::FILE* f){
    if(f) std::fclose(f);
};

std::unique_ptr<std::FILE, decltype(file_deleter)> file_;

// Usage in constructor
WavReader::WavReader(const std::string& filename)
    : file_(std::fopen(filename.c_str(), "rb"), &std::fclose)
{
    if(!file_) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    read_header();
}

// Reading the header
void WavReader::read_header() {
    // Helper to read and validate chunk ID
    size_t WORD_BLOCK = 4; // to consider next 4 values as char
    auto read_chunk_id = [this, WORD_BLOCK](const char* expected) {
        char id[4];
        if(std::fread(id, sizeof(char), WORD_BLOCK, file_.get()) != 4) throw std::runtime_error("Failed to read chunk ID");
        if(std::memcmp(id, expected, 4) != 0) throw std::runtime_error(std::string("Expected '") + expected + "' chunk");
    };

    // Helper to read little-endian integers
    size_t INT_BLOCK = 1; // to consider next 1 value as int
    auto read_u16 = [this, INT_BLOCK]() -> uint16_t {
        uint16_t value;
        std::fread(&value, sizeof(value), INT_BLOCK, file_.get());
        return value;
    };

    auto read_u32 = [this, INT_BLOCK]() -> uint32_t {
        uint32_t value;
        std::fread(&value, sizeof(value), INT_BLOCK, file_.get());
        return value;
    };

    // Read RIFF header
    read_chunk_id("RIFF");
    uint32_t file_size = read_u32();
    read_chunk_id("WAVE");

    // Read fmt  chunk
    read_chunk_id("fmt ");
    uint32_t fmt_size = read_u32();
    uint16_t audio_format = read_u16();

    if(audio_format != 1) { // Only support PCM
        throw std::runtime_error("Only PCM format supported");
    }

    num_channels_ = read_u16();
    sample_rate_ = read_u32();
    uint32_t byte_rate = read_u32();
    uint32_t block_align = read_u16();
    bits_per_sample_ = read_u16();

    // Skip any extra fmt bytes (some WAV have extended format)
    if (fmt_size > 16) {
        std::fseek(file_.get(), fmt_size - 16, SEEK_CUR);
    }

    // Find data chunk 
    while(true) {
        char chunk_id[4];
        if (std::fread(chunk_id, sizeof(char), WORD_BLOCK, file_.get()) != 4) {
            throw std::runtime_error("Data chunk not found");
        }

        uint32_t chunk_size = read_u32();

        if(std::memcmp(chunk_id, "data", 4) == 0) {
            // Found it
            num_samples_ = chunk_size / (num_channels_ * bits_per_sample_ / 8);
            data_start_pos_ = std::ftell(file_.get());
            break;
        }
        else {
            std::fseek(file_.get(), chunk_size, SEEK_CUR);
        }
    }
}

template<typename SampleType>
AudioBuffer<SampleType> WavReader::read() {
    // Seek to start of audio data
    std::fseek(file_.get(), data_start_pos_, SEEK_SET);
    
    AudioBuffer<SampleType> buffer(num_samples_, num_channels_);

    // Read based on bit depth
    if(bits_per_sample_ = 16) {
        std::vector<int16_t> temp(num_samples_ * num_channels_);
        std::fread(temp.data(), sizeof(int16_t), temp.size(), file_.get());

        // Convert to target type
        for(size_t i = 0; i < temp.size(); ++i) {
            buffer.data()[i] = convert_sample<SampleType>(temp[i]);
        }
    }

    // TODO: Handle other bit depths...

    return buffer; // Move semantics
}

#if 0
template<>
float convert_sample<float>(int16_t sample) {
    return sample / 32768.0f; // Normalize to [-1.0, 1.0]
}
#endif
