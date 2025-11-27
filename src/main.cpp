#include "project.h"
#include "WavIO/WavReader.hpp"
#include "WavIO/WavWriter.hpp"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input.wav> <output.wav>\n";

        return -1;
    }

    try
    {
        // Read Input file
        WavTools::Reader reader(argv[1]);

        std::cout << "Input file info:\n"
                  << "\tSample rate: " << reader.sample_rate() << " Hz\n"
                  << "\tChannels: " << reader.num_channels() << "\n"
                  << "\tBit depth: " << reader.bits_per_sample() << " bits\n"
                  << "\tDuration: " << static_cast<float>(reader.num_samples() / (float)reader.sample_rate()) << " seconds\n";

        // Read audio data using float for processing precision
        auto buffer = reader.read<float>();

        // Write output file
        WavTools::Writer writer(reader.sample_rate(), reader.num_channels(), reader.bits_per_sample());

        writer.save(argv[2], buffer);

        std::cout << "Successfully wrote " << argv[2] << "\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error : " << e.what() << "\n";
        return 1;
    }

    return 0;
}
