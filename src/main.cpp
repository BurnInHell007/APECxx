#include "project.h"
#include "WavIO/WavReader.hpp"
#include "WavIO/WavWriter.hpp"
#include "Effects/FilterEffects.hpp"
#include "Effects/Equalizer.hpp"
#include "Effects/BasicEffects.hpp"
#include "DSP/BiQuadFilter.hpp"
#include "DSP/FilterDesign.hpp"

using namespace audio;

void print_usage(const char *program_name)
{
    std::cout << "Usage: " << program_name << " <input.wav> <output.wav> [options]\n\n"
              << "Filter Options:\n"
              << "  --lowpass <freq> [q]       Low-pass filter (default q=0.707)\n"
              << "  --highpass <freq> [q]      High-pass filter (default q=0.707)\n"
              << "  --bandpass <freq> <bw>     Band-pass filter\n"
              << "  --notch <freq> <bw>        Notch filter\n"
              << "  --eq <freq> <gain> [bw]    Parametric EQ band (default bw=1.0)\n"
              << "  --bass <gain>              Adjust bass (3-band EQ)\n"
              << "  --mid <gain>               Adjust mid (3-band EQ)\n"
              << "  --treble <gain>            Adjust treble (3-band EQ)\n\n"
              << "Examples:\n"
              << "  " << program_name << " in.wav out.wav --lowpass 1000\n"
              << "  " << program_name << " in.wav out.wav --highpass 80 --bass +3\n"
              << "  " << program_name << " in.wav out.wav --eq 1000 -6 0.5\n";
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        print_usage(argv[0]);
        return 1;
    }

    std::string input_file = argv[1];
    std::string output_file = argv[2];

    try
    {
        // Read input file
        std::cout << "Reading: " << input_file << "\n";
        WavReader reader(input_file);

        std::cout << "  Sample rate: " << reader.sample_rate() << " Hz\n"
                  << "  Channels: " << reader.num_channels() << "\n"
                  << "  Bit depth: " << reader.bits_per_sample() << " bits\n"
                  << "  Duration: " << reader.num_samples() / (float)reader.sample_rate() << " seconds\n";

        auto buffer = reader.read<float>();

        // Parse command-line options and apply filters
        bool use_three_band_eq = false;
        double bass_gain = 0.0, mid_gain = 0.0, treble_gain = 0.0;

        std::vector<std::unique_ptr<effects::AudioEffect<float>>> filters;

        for (int i = 3; i < argc; ++i)
        {
            std::string arg = argv[i];

            if (arg == "--lowpass" && i + 1 < argc)
            {
                double freq = std::stod(argv[++i]);
                double q = 0.707;
                if (i + 1 < argc && argv[i + 1][0] != '-')
                {
                    q = std::stod(argv[++i]);
                }
                std::cout << "Applying low-pass filter: " << freq << " Hz, Q=" << q << "\n";
                filters.push_back(std::make_unique<effects::LowpassEffect<float>>(
                    reader.sample_rate(), freq, q));
            }
            else if (arg == "--highpass" && i + 1 < argc)
            {
                double freq = std::stod(argv[++i]);
                double q = 0.707;
                if (i + 1 < argc && argv[i + 1][0] != '-')
                {
                    q = std::stod(argv[++i]);
                }
                std::cout << "Applying high-pass filter: " << freq << " Hz, Q=" << q << "\n";
                filters.push_back(std::make_unique<effects::HighpassEffect<float>>(
                    reader.sample_rate(), freq, q));
            }
            else if (arg == "--bandpass" && i + 2 < argc)
            {
                double freq = std::stod(argv[++i]);
                double bw = std::stod(argv[++i]);
                std::cout << "Applying band-pass filter: " << freq << " Hz, BW=" << bw << "\n";
                filters.push_back(std::make_unique<effects::BandpassEffect<float>>(
                    reader.sample_rate(), freq, bw));
            }
            else if (arg == "--eq" && i + 2 < argc)
            {
                double freq = std::stod(argv[++i]);
                double gain = std::stod(argv[++i]);
                double bw = 1.0;
                if (i + 1 < argc && argv[i + 1][0] != '-')
                {
                    bw = std::stod(argv[++i]);
                }
                std::cout << "Applying EQ: " << freq << " Hz, "
                          << (gain >= 0 ? "+" : "") << gain << " dB, BW=" << bw << "\n";
                filters.push_back(std::make_unique<effects::ParametricEQBand<float>>(
                    reader.sample_rate(), freq, gain, bw));
            }
            else if (arg == "--bass" && i + 1 < argc)
            {
                use_three_band_eq = true;
                bass_gain = std::stod(argv[++i]);
                std::cout << "Bass: " << (bass_gain >= 0 ? "+" : "") << bass_gain << " dB\n";
            }
            else if (arg == "--mid" && i + 1 < argc)
            {
                use_three_band_eq = true;
                mid_gain = std::stod(argv[++i]);
                std::cout << "Mid: " << (mid_gain >= 0 ? "+" : "") << mid_gain << " dB\n";
            }
            else if (arg == "--treble" && i + 1 < argc)
            {
                use_three_band_eq = true;
                treble_gain = std::stod(argv[++i]);
                std::cout << "Treble: " << (treble_gain >= 0 ? "+" : "") << treble_gain << " dB\n";
            }
        }

        // Apply three-band EQ if requested
        if (use_three_band_eq)
        {
            auto eq = std::make_unique<effects::ThreeBandEQ<float>>(reader.sample_rate());
            eq->set_bass(bass_gain);
            eq->set_mid(mid_gain);
            eq->set_treble(treble_gain);
            filters.push_back(std::move(eq));
        }

        // Process audio through all filters
        std::cout << "\nProcessing audio...\n";
        for (auto &filter : filters)
        {
            filter->process(buffer);
        }

        // Write output file
        std::cout << "Writing: " << output_file << "\n";
        WavWriter writer(output_file, reader.sample_rate(),
                         reader.num_channels(), reader.bits_per_sample());
        writer.write(buffer);

        std::cout << "Done!\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
