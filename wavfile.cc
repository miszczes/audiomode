#include <iostream>
#include <fstream>
#include <cmath>

typedef std::string string_t;
using byte = unsigned char;

class WavFile
{
public:
    // The canonical WAVE format starts with the RIFF header
    string_t chunk_id = "RIFF";
    string_t chunk_size = "----";
    string_t format = "WAVE";

    // The "fmt " subchunk describes the sound data's format
    string_t subchunk1_id = "fmt ";
    int subchunk1_size = 16; // 16 for PCM
    char audio_format = 1;   // PCM = 1 (i.e. Linear quantization); Values other than 1 indicate some form of compression.
    int num_channels;        // initialized here, but calculated in the constructor
    int sample_rate;         // initialized here, but calculated in the constructor
    int bits_per_sample;     // initialized here, but calculated in the constructor
    int byte_rate;           // initialized here, but calculated in the constructor
    int block_align;         // The number of bytes for one sample including all channels. I wonder what happens when this number isn't an integer?

    // The "data" subchunk contains the size of the data and the actual sound
    string_t subchunk2_id = "data";
    string_t subchunk2_size = "----";

    int duration_s; // duration in seconds
    int max_amplitude;
    double frequency;

    WavFile(double freq, int duration_s, int num_channels, int sample_rate, int bits_per_sample)
    {
        /**
         * New wav file header
         *
         * creates
         *
         * @param num_channels
         */
        this->num_channels = num_channels;
        this->sample_rate = sample_rate;
        this->bits_per_sample = bits_per_sample;

        this->byte_rate = sample_rate * num_channels * bits_per_sample / 8;
        this->block_align = num_channels * bits_per_sample / 8;

        this->duration_s = duration_s;
        this->max_amplitude = std::pow(2, this->bits_per_sample) / 2;
        this->frequency = freq;
    }

    void write_to_file(string_t file_name)
    {
        /// Writing the desired data to a wav file including  all nescessary header information
        std::ofstream wav;
        wav.open(file_name, std::ios::binary);

        if (wav.is_open())
        {
            // HEADER -> 44 bytes
            // header begin
            wav << this->chunk_id;
            wav << this->chunk_size;
            wav << this->format;

            wav << this->subchunk1_id;
            this->write_as_bytes(wav, this->subchunk1_size, 4);
            this->write_as_bytes(wav, this->audio_format, 2);
            this->write_as_bytes(wav, this->num_channels, 2);
            this->write_as_bytes(wav, this->sample_rate, 4);
            this->write_as_bytes(wav, this->byte_rate, 4);
            this->write_as_bytes(wav, this->block_align, 2);
            this->write_as_bytes(wav, this->bits_per_sample, 2);

            wav << subchunk2_id;
            wav << subchunk2_size;
            // header end

            int start_audio = wav.tellp();

            for (int i = 0; i < this->sample_rate * this->duration_s; i++)
            {
                // fadein and fadeout
                double amplitude = (double)i / this->sample_rate * this->max_amplitude;

                // signal value ; TODO: changable value generation
                double value = sin((2 * 3.14 * i * this->frequency) / this->sample_rate);

                double ch1 = amplitude * value;
                this->write_as_bytes(wav, ch1, 2);
                if (this->num_channels == 2)
                {
                    double ch2 = (this->max_amplitude - amplitude) * value;
                    this->write_as_bytes(wav, ch2, 2);
                }
            }
            int end_audio = wav.tellp();
            // setting wav stream pointer to a subchunk 2 size position and filling it with 4byte size data
            wav.seekp(start_audio - 4);
            this->write_as_bytes(wav, end_audio - start_audio, 4);

            // setting wav stream pointer to ChunkSize position from beginning of the stream
            wav.seekp(4, std::ios::beg);
            // writing the desired equation
            this->write_as_bytes(wav, end_audio - 8, 4); // or 36 + subchunk2 size
        }
        wav.close();
    }

private:
    void write_as_bytes(std::ofstream &file, int value, int byte_size)
    {
        /// Writing given value as char bytes
        file.write(reinterpret_cast<const char *>(&value), byte_size);
    }
};