#include "stft.h"
#include "fft.h"
#include <stdio.h> 
#include <iostream> 
#include <vector> 
#include <cmath>
#include <complex>


#include <random> 
#include <chrono> 


void STFT::compute_hamming_window(uint32_t n_fft) {
    for (int i = 0; i < n_fft; i++) {
        this->window[i] = 0.54 - (0.46 * std::cos(2 * M_PI * (i / ((n_fft - 1) * 1.0))));
    }
}


void STFT::compute_hann_window(uint32_t n_fft) {
    for (int i = 0; i < n_fft; i++) {
        this->window[i] = 0.5 * (1 - std::cos((2 * M_PI * i) / (n_fft - 1)));
    }
}


void STFT::compute_window(uint32_t n_fft, WindowType window_type) {
    switch (window_type) {
    case HAMMING:
        this->compute_hamming_window(n_fft);
        break; 
    case HANN: 
        this->compute_hann_window(n_fft);
        break;
    default: 
        std::cerr << "Window type not supported!" << std::endl;
    }
}


void STFT::stft(std::vector<int16_t>& X, uint32_t n_fft, uint32_t hop_length, WindowType window_type) {
    uint32_t N = X.size();

    // Init
    this->x.resize(n_fft);
    this->window.resize(n_fft); 
    this->compute_window(n_fft, HAMMING);

    int start_chunk = 0; 
    bool past_end = false; 
    int start_idx;
    while (start_chunk < N && !past_end) {

        for (int i = 0; i < n_fft; i++) {
            start_idx = start_chunk + i;
            if (start_idx < N) this->x[i] = std::complex<float> (X[start_idx] * window[i], 0.0); 
            else {
                this->x[i] = std::complex<float> (0.0, 0.0);
                past_end = true;
            }
        }

        this->fft.fft(this->x, n_fft);

        // Read relevant part of the fft output
        std::vector<std::complex<float> > x_chunk;
        for (int i = 0; i < (n_fft >> 1) + 1; i++) {
            x_chunk.push_back(this->x[i]);
        }
        this->X_out.push_back(x_chunk);
        start_chunk += hop_length;
    }
}


void STFT::istft(uint32_t hop_length) {
    // TODO: Implement this
}



int main() {

    STFT stft; 

    std::random_device rd; 
    std::mt19937 gen(rd());
    std::uniform_real_distribution<int16_t> dis(-5.0, 5.0);
    
    std::vector<int16_t> X;
    int N = 111477;
    for (int i = 0; i < N; i++) {
        X.push_back(dis(gen));
    }

    // auto start = std::chrono::high_resolution_clock::now();
    stft.stft(X, 1024, 512, HAMMING);
    // auto end = std::chrono::high_resolution_clock::now();
    // float duration = float(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count());
    // std::cout << "Duration: " <<  duration << " microseconds" << std::endl;

    std::cout << stft.X_out.size() << " x " << stft.X_out[0].size() << std::endl;

    
    



    return 0;
}