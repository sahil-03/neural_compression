#include "encode.h" 
#include "data_handler.h"
#include "stft.h"
#include <iostream> 
#include <stdio.h> 
#include <vector> 


#include <fstream>

Encoder::Encoder(std::string file_path): output_file_path(file_path) {
    // Init other class variables
}

void Encoder::encode(std::vector<int16_t> &data, uint32_t sample_rate) {
    
    /*
        Steps: 
            1. stft = STFT(data, n_fft, hop_length)
            2. Perform compression of stft (SVD, etc...)
            3. istft = ISTFT(compressed_stft, hop_length)
            4. compute statistics on the compressed data
    */

   // Perform stft on 'data'
   STFT stft;
   stft.stft(data, data.size(), 1024, 512);

   std::ofstream outfile; 
    outfile.open("../debug_dir/stft_out.csv"); 
    
    for (int i = 0; i < this->num_samples; i++) {
        outfile << this->wav_data[i] << std::endl;
    }

}

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cerr << "Please provide an input file (from /data/) and an output file path." << std::endl;
        return 1;
    }

    std::string input_file_path = argv[1]; 
    std::string output_file_path = argv[2];

    WAVHandler wav_handler; 
    wav_handler.open_wav_file(input_file_path.c_str());

    // std::cout << wav_handler.wav_data.size() << std::endl;
    // std::cout << wav_handler.wav->sampleRate << std::endl;
    
    Encoder encoder(output_file_path); 
    encoder.encode(wav_handler.wav_data, wav_handler.wav->sampleRate);

    return 0;

}