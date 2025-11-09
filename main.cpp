#include <iostream>
#include <vector>
#include <string>
#include <vector>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <exception>
#include <stdexcept>
#include <memory>

template <typename SampleType>
class AudioBuffer
{
public:
    AudioBuffer(size_t num_samples, size_t num_channels);

    ~AudioBuffer();                                       // Destructor
    AudioBuffer(const AudioBuffer &other);                // Copy constructor
    AudioBuffer &operator=(const AudioBuffer &other);     // Copy assignment
    AudioBuffer(AudioBuffer &&other) noexcept;            // Move constructor
    AudioBuffer &operator=(AudioBuffer &&other) noexcept; // Move assignment

    SampleType &operator()(size_t sample_index, size_t channel);
    const SampleType &operator()(size_t sample_index, size_t channel) const;

    SampleType *data() { return buffer_.get(); }
    const SampleType *data() const { return buffer_.get(); }

    size_t num_samples() const { return num_samples_; }
    size_t num_channels() const { return num_channels_; }
    size_t size_in_byte() const { return num_samples_ * num_channels_ * sizeof(SampleType); }

    void clear();

private:
    std::unique_ptr<SampleType[]> buffer_; // actual data
    size_t num_samples_;                   // samples per channel
    size_t num_channels_;                  // mono / stereo
};

template <typename SampleType>
AudioBuffer<SampleType>::AudioBuffer(size_t num_samples, size_t num_channels)
    : buffer_(std::make_unique<SampleType[]>(num_samples * num_channels)), num_samples_(num_samples), num_channels_(num_channels)
{
    clear(); // Initialize to silence
}

template <typename SampleType>
AudioBuffer<SampleType> &AudioBuffer<SampleType>::operator=(AudioBuffer &&other) noexcept
{
    if (this != &other)
    {
        buffer_ = std::move(other.buffer_);
        num_samples_ = other.num_samples_;
        num_channels_ = other.num_channels_;
        other.num_samples_ = 0;
        other.num_channels_ = 0;
    }
    return *this;
}

template <typename SampleType>
AudioBuffer<SampleType>::~AudioBuffer() = default;

void assert(bool value);

template <typename SampleType>
const SampleType &AudioBuffer<SampleType>::operator()(size_t sample, size_t channel) const
{
    assert(sample < num_samples_ && channel < num_channels_);
    return buffer_[sample * num_channels_ + channel];
}

template <typename SampleType>
SampleType &AudioBuffer<SampleType>::operator()(size_t sample, size_t channel)
{
    assert(sample < num_samples_ && channel < num_channels_);
    return buffer_[sample * num_channels_ + channel];
}

void assert(bool value)
{
    if (!value)
    {
        throw std::runtime_error("Assertion failed!");
    }
    return;
}

template <typename SampleType>
void AudioBuffer<SampleType>::clear()
{
    std::fill_n(buffer_.get(), num_samples_ * num_channels_, SampleType(0));
}

template <typename SampleType>
AudioBuffer<SampleType>::AudioBuffer(AudioBuffer &&other) noexcept
    : buffer_(std::move(other.buffer_)), num_samples_(other.num_samples_), num_channels_(other.num_channels_)
{
    other.num_samples_ = 0;
    other.num_channels_ = 0;
}

class WavReader
{
public:
    explicit WavReader(const std::string &filename);

    template <typename SampleType>
    AudioBuffer<SampleType> read();

    uint32_t sample_rates() const { return sample_rate_; }
    uint16_t num_channels() const { return num_channels_; }
    uint16_t bits_per_sample() const { return bits_per_sample_; }
    uint32_t num_samples() const { return num_samples_; }

private:
    void read_header(); // Parse RIFF/fmt/data chunks

    std::unique_ptr<std::FILE, decltype(&std::fclose)> file_;
    uint32_t sample_rate_;
    uint16_t num_channels_;
    uint16_t bits_per_sample_;
    uint32_t num_samples_;
    size_t data_start_pos_; // Where the audio data begins in file
};

WavReader::WavReader(const std::string &filename)
    : file_{std::fopen(filename.c_str(), "rb"), &std::fclose}
{
    if (!file_)
    {
        throw std::runtime_error("Cannot open file : " + filename);
    }
    read_header();
}

void WavReader::read_header()
{
    size_t WORD_BLOCK = 4; // to consider next 4 values as char
    auto read_chunk_id = [this, WORD_BLOCK](const char *expected)
    {
        char id[4];
        if (std::fread(id, sizeof(char), WORD_BLOCK, file_.get()) != 4)
            throw std::runtime_error("Failed to read chunk ID");
        if (std::memcmp(id, expected, 4) != 0)
            throw std::runtime_error(std::string("Expected '") + expected + "' chunk");
    };

    size_t INT_BLOCK = 1; // to consider next 1 value as int
    auto read_u16 = [this, INT_BLOCK]() -> uint16_t
    {
        uint16_t value;
        std::fread(&value, sizeof(value), INT_BLOCK, file_.get());
        return value;
    };

    auto read_u32 = [this, INT_BLOCK]() -> uint32_t
    {
        uint32_t value;
        std::fread(&value, sizeof(value), INT_BLOCK, file_.get());
        return value;
    };

    read_chunk_id("RIFF");
    uint32_t file_size = read_u32();
    read_chunk_id("WAVE");

    read_chunk_id("fmt ");
    uint32_t fmt_size = read_u32();
    uint16_t audio_format = read_u16();

    if (audio_format != 1)
    { // Only support PCM
        throw std::runtime_error("Only PCM format supported");
    }

    num_channels_ = read_u16();
    sample_rate_ = read_u32();
    uint32_t byte_rate = read_u32();
    uint32_t block_align = read_u16();
    bits_per_sample_ = read_u16();

    if (fmt_size > 16)
    {
        std::fseek(file_.get(), static_cast<long>(fmt_size - 16), SEEK_CUR);
    }

    while (true)
    {
        char chunk_id[4];
        if (std::fread(chunk_id, sizeof(char), WORD_BLOCK, file_.get()) != 4)
        {
            throw std::runtime_error("Data chunk not found");
        }

        uint32_t chunk_size = read_u32();

        if (std::memcmp(chunk_id, "data", 4) == 0)
        {
            // Found it
            num_samples_ = chunk_size / (num_channels_ * bits_per_sample_ / uint32_t(8));
            data_start_pos_ = static_cast<size_t>(std::ftell(file_.get()));
            break;
        }
        else
        {
            std::fseek(file_.get(), static_cast<long>(chunk_size), SEEK_CUR);
        }
    }
}

namespace wav_reader
{
    template <typename T>
    T convert_sample(int16_t sample)
    {
        return sample;
    }

    template <>
    float convert_sample<float>(int16_t sample)
    {
        return static_cast<float>(sample) / 32768.0f;
    }

    template <>
    int8_t convert_sample<int8_t>(int16_t sample)
    {
        return static_cast<int8_t>(sample / 256);
    }

    template <typename T>
    T convert_sample(int8_t sample)
    {
        return sample;
    }

    template <>
    float convert_sample<float>(int8_t sample)
    {
        return sample / 256.0f;
    }
};

template <typename SampleType>
AudioBuffer<SampleType> WavReader::read()
{
    std::fseek(file_.get(), data_start_pos_, SEEK_SET);

    AudioBuffer<SampleType> buffer(num_samples_, num_channels_);

    if (bits_per_sample_ == 16)
    {
        std::vector<int16_t> temp(num_samples_ * num_channels_);
        std::fread(temp.data(), sizeof(int16_t), temp.size(), file_.get());

        // Convert to target type
        for (size_t i = 0; i < temp.size(); ++i)
        {
            buffer.data()[i] = wav_reader::convert_sample<SampleType>(temp[i]);
        }
    }

    if (bits_per_sample_ == 8)
    {
        std::vector<int8_t> temp(num_samples_ * num_channels_);
        std::fread(temp.data(), sizeof(int8_t), temp.size(), file_.get());

        for (size_t i = 0; i < temp.size(); ++i)
        {
            buffer.data()[i] = wav_reader::convert_sample<SampleType>(temp[i]);
        }
    }

    return buffer; // Move semantics
}
template AudioBuffer<float> WavReader::read<float>();

class WavWriter
{
public:
    WavWriter(const std::string &filename, uint32_t sample_rate, uint16_t num_channels, uint16_t bits_per_sample);

    template <typename SampleType>
    void write(const AudioBuffer<SampleType> &buffer);

private:
    void write_header(uint32_t data_size);

    std::unique_ptr<std::FILE, decltype(&std::fclose)> file_;
    uint32_t sample_rate_;
    uint16_t num_channels_;
    uint16_t bits_per_sample_;
};

namespace wav_writer
{
    template <typename T>
    T convert_sample(int16_t sample)
    { // no effect on upcasting
        return sample;
    }

    template <>
    float convert_sample<float>(int16_t sample)
    {
        return sample / 32768.0f;
    }

    template <>
    int8_t convert_sample<int8_t>(int16_t sample)
    {
        return static_cast<int8_t>(sample / 256);
    }
    template <typename T>
    T convert_sample(float sample)
    {
        return sample;
    }

    template <>
    int16_t convert_sample<int16_t>(float sample)
    {
        return static_cast<int16_t>(sample * 32768);
    }

    template <>
    int8_t convert_sample<int8_t>(float sample)
    {
        return static_cast<int8_t>(sample * 256);
    }
};

WavWriter::WavWriter(const std::string &filename, uint32_t sample_rate, uint16_t num_channels, uint16_t bits_per_sample)
    : file_(std::fopen(filename.c_str(), "wb"), &std::fclose), sample_rate_(sample_rate), num_channels_(num_channels), bits_per_sample_(bits_per_sample)
{
    if (!file_)
    {
        throw std::runtime_error("Cannot create file : " + filename);
    }
}

void WavWriter::write_header(uint32_t data_size)
{
    size_t INT_BLOCK = 1;
    auto write_u16 = [this, INT_BLOCK](uint16_t value)
    {
        std::fwrite(&value, sizeof(uint16_t), INT_BLOCK, file_.get());
    };

    auto write_u32 = [this, INT_BLOCK](uint32_t value)
    {
        std::fwrite(&value, sizeof(uint32_t), INT_BLOCK, file_.get());
    };

    size_t WORD_BLOCK = 4;
    auto write_id = [this, WORD_BLOCK](const char *id)
    {
        std::fwrite(id, sizeof(char), WORD_BLOCK, file_.get());
    };

    // RIFF Header
    write_id("RIFF");
    write_u32(36 + data_size);
    write_id("WAVE");

    // fmt chunk
    write_id("fmt ");
    const uint32_t pcm_chunk_size = 16;
    write_u32(pcm_chunk_size);
    const uint16_t linear_quantization = 1;
    write_u16(linear_quantization);
    write_u16(num_channels_);
    write_u32(sample_rate_);
    uint32_t byte_rate = sample_rate_ * num_channels_ * bits_per_sample_ / 8;
    write_u32(byte_rate);
    uint16_t block_align = static_cast<uint16_t>(num_channels_ * bits_per_sample_ / 8);
    write_u16(block_align);
    write_u16(bits_per_sample_);

    // data chunk
    write_id("data");
    write_u32(data_size);
}

template <typename SampleType>
void WavWriter::write(const AudioBuffer<SampleType> &buffer)
{
    uint32_t data_size = buffer.num_samples() * buffer.num_channels() * bits_per_sample_ / 8;
    write_header(data_size);

    // Convert and write samples
    if (bits_per_sample_ == 16)
    {
        std::vector<int16_t> temp(buffer.num_channels() * buffer.num_samples());

        for (size_t i = 0; i < temp.size(); ++i)
        {
            temp[i] = wav_writer::convert_sample<int16_t>(buffer.data()[i]);
        }

        std::fwrite(temp.data(), sizeof(int16_t), temp.size(), file_.get());
    }
}



int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input.wav> <output.wav>\n";
        return -1;
    }

    try
    {
        WavReader reader(argv[1]);

        std::cout << "Input file info:\n"
                  << "  Sample rate: " << reader.sample_rates() << " Hz\n"
                  << "  Channels: " << reader.num_channels() << "\n"
                  << "  Bit depth: " << reader.bits_per_sample() << " bits\n"
                  << "  Duration: " << ((float)reader.num_samples() / (float)reader.sample_rates()) << " seconds\n";

        AudioBuffer<float> buffer = reader.read<float>();
        
        WavWriter writer(argv[2], reader.sample_rates(), reader.num_channels(), reader.bits_per_sample());

        writer.write(buffer);

        std::cout << "Successfully wrote " << argv[2] << "\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

#if 0
int main (int argc, char* argv[]) {
    if(argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input.wav> <output.wav>\n";
        return -1;
    }

    try {
        // Read Input file
        WavReader reader(argv[1]);

        std::cout << "Input file info:\n"
                  << "  Sample rate: " << reader.sample_rates() << " Hz\n"
                  << "  Channels: " << reader.num_channels() << "\n"
                  << "  Bit depth: " << reader.bits_per_sample() << " bits\n"
                  << "  Duration: " << static_cast<float>((float)reader.num_samples() / (float)reader.sample_rates()) << " seconds\n";

        // Read audio data using float for internal processing
        auto buffer = reader.read<float>();

        // Write output file
        WavWriter writer(argv[2], reader.sample_rates(), reader.num_channels(), reader.bits_per_sample());

        writer.write(buffer);

        std::cout << "Successfully wrote " << argv[2] << "\n";
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
#endif
