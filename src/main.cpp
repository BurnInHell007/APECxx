#include "project.h"
#include "WavIO/WavReader.hpp"
#include "WavIO/WavWriter.hpp"
#include "Effects/GainEffect.hpp"
#include "Effects/FadeEffect.hpp"
#include "Effects/MixEffect.hpp"

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
        /**
         * Can do something like
         * AudioBuffer<float> buffer;
         * { /// START BLOCK
         *      WavTools::Reader reader(argv[1]);
         *      buffer = reader.read<float>();
         * } /// END BLOCK
         * This will release the memory block which is created for reader
         * Necessary when doing performance heavy works
         */

        // Write output file
        // WavTools::Writer writer(reader.sample_rate(), reader.num_channels(), reader.bits_per_sample());

        // /// To save file by default
        // writer.save(argv[2], buffer);

        // std::cout << "Successfully wrote " << argv[2] << "\n";

        // /// Gain Effect test
        // GainEffect<float> gainEffect(1.2f);
        // gainEffect.process(buffer);

        // writer.save("../wav-files/gainEffect.wav", buffer);

        // /// Fade Effect test
        // {
        //     float FadeInStart = 0.1, FadeInEnd = 1.0;
        //     size_t samples = buffer.num_samples() / 4;
        //     FadeEffect<float> fadeIn(FadeInStart, FadeInEnd, 0, samples);
        //     fadeIn.process(buffer);
        //     writer.save("../wav-files/fadeInEffect.wav", buffer);
            
        //     float FadeOutStart = 1.0, FadeOutEnd = 0.5;
        //     FadeEffect<float> fadeOut(FadeOutStart, FadeOutEnd, 3 * samples, samples);
        //     fadeOut.process(buffer);
        //     writer.save("../wav-files/fadeOutEffect.wav", buffer);
        // }

        // {
        //     float panValue = 0.2f;
        //     MixEffect<float> panMixEffect(true, panValue);
        //     panMixEffect.process(buffer);
        //     writer.save("../wav-files/panEffect.wav", buffer);

        //     // MixEffect<float> mixEffect;
        //     // mixEffect.process(buffer);
        //     // writer.save("../wav-files/mixEffect.wav", buffer);
        // }

        // std::cout << "Sucessfully wrote Effect filters\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error : " << e.what() << "\n";
        return 1;
    }

    return 0;
}
