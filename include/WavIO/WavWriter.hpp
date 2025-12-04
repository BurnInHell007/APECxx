#ifndef WAV_WRITER_HPP_
#define WAV_WRITER_HPP_
#include "AudioBuffer.hpp"
#include "WavIO/WavCommon.hpp"

namespace WavTools
{
    /**********************
     * @brief Writer class to write down a wav file
     *********************/
    class Writer
    {
    public:
        /******************************
         * @brief Wav Writer instance for creating wav copies
         * @param sample_rate
         * @param num_channels
         * @param bits_per_sample
         ******************************/
        Writer(uint32_t sample_rate, uint16_t num_channels, uint16_t bits_per_sample);

        /**********************************
         * @brief Copies metadata from buffer and save it to given path
         * @param filepath
         * @param buffer
         *********************************/
        template <typename SampleType>
        void save(const std::string &filepath, const AudioBuffer<SampleType> &buffer);

    private:
        /******************************
         * @brief write header to files
         * @note did a dirty fix for stereo to mono buffer change
         *****************************/
        void write_header(std::FILE *file_, uint32_t data_size);

        uint32_t sample_rate_;
        uint16_t num_channels_;
        uint16_t bits_per_sample_;
    };
};

#endif
