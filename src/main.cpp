#include <iostream>
#include <exception>
#include "../include/wav_reader.hpp"
#include "wav_reader.cpp"
#include "../include/wav_writer.hpp"
#include "wav_writer.cpp"

int main () {
    std::cout << "Hello" << std::endl;
    return 0;
}

#if 0
int main (int argc, char** argv) {
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
