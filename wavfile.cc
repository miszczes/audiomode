#include <iostream>
#include <fstream>
#include <sstream>
#include "wavfile.h"

typedef std::string string_t;

class WavFile
{
public:
    int num_channels;    // initialized here, but calculated in the constructor
    int sample_rate;     // initialized here, but calculated in the constructor
    int bits_per_sample; // initialized here, but calculated in the constructor
    int byte_rate;       // initialized here, but calculated in the constructor
    int block_align;     // The number of bytes for one sample including all channels. I wonder what happens when this number isn't an integer?
    WavFile(int num_channels, int sample_rate, int bits_per_sample)
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

        this->max_amplitude = std::pow(2, this->bits_per_sample) / 2;

        this->init_header();
    }
    void basic_wave_to_file(string_t file_name, char *type, double freq = 0.0, int duration_s = 0)
    {
        // if ((freq == 0.0 || duration_s == 0) && values == nullptr)
        //     throw std::invalid_argument("ERROR:\nIf there is no passed array of values - frequency and duration should be provided and should be different from 0");

        /// Writing the desired data to a wav file
        std::ofstream wav;
        wav.open(file_name, std::ios::binary);

        // writing header
        std::string content = header.str();
        wav.write(content.c_str(), content.size());

        if (wav.is_open())
        {

            int start_audio = wav.tellp();

            // writing audio data to channels
            for (int i = 0; i < this->sample_rate * duration_s; i++)
            {
                // fadein and fadeout
                double amplitude = (double)i / this->sample_rate * this->max_amplitude;

                // signal value ; TODO: changable value generation
                double value;
                switch (str2int(type))
                {
                case str2int((char *)"sin"):
                    value = BasicWaves.sinusoid(2 * 3.14 * i * freq, this->sample_rate);
                    break;
                case str2int((char *)"square"):
                    value = BasicWaves.square(2 * 3.14 * i * freq, this->sample_rate);
                    break;

                default:
                    throw std::invalid_argument("\nERROR:\nInvalid Basic Wave shape: choose from 'sin', 'square', 'triangle', 'saw'");
                    break;
                };

                // channels
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
    // The canonical WAVE format starts with the RIFF header
    string_t chunk_id = "RIFF";
    string_t chunk_size = "----";
    string_t format = "WAVE";

    // The "fmt " subchunk describes the sound data's format
    string_t subchunk1_id = "fmt ";
    int subchunk1_size = 16; // 16 for PCM
    char audio_format = 1;   // PCM = 1 (i.e. Linear quantization); Values other than 1 indicate some form of compression.

    // The "data" subchunk contains the size of the data and the actual sound
    string_t subchunk2_id = "data";
    string_t subchunk2_size = "----";

    std::stringstream header;

    int max_amplitude;
    void write_as_bytes(std::ofstream &file, int value, int byte_size)
    {
        /// Writing given value as char bytes
        file.write(reinterpret_cast<const char *>(&value), byte_size);
    }

    void write_to_stream(std::stringstream &header, int value, int byte_size)
    {
        header.write(reinterpret_cast<const char *>(&value), byte_size);
    }

    void init_header()
    {
        // HEADER -> 44 bytes
        // header begin
        this->header << this->chunk_id;
        this->header << this->chunk_size;
        this->header << this->format;

        this->header << this->subchunk1_id;
        this->write_to_stream(this->header, this->subchunk1_size, 4);
        this->write_to_stream(this->header, this->audio_format, 2);
        this->write_to_stream(this->header, this->num_channels, 2);
        this->write_to_stream(this->header, this->sample_rate, 4);
        this->write_to_stream(this->header, this->byte_rate, 4);
        this->write_to_stream(this->header, this->block_align, 2);
        this->write_to_stream(this->header, this->bits_per_sample, 2);

        this->header << subchunk2_id;
        this->header << subchunk2_size;
        // header end
    }
};
