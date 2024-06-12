#ifndef _STFT_H
#define _STFT_H

#include "fft.h"
#include <iostream> 
#include <stdio.h> 
#include <vector> 
#include <complex> 

enum WindowType {
    HAMMING,
    /* add more windows */ 
    HANN
};


class STFT {
    public: 
        std::vector<std::vector<std::complex<float> > > X_out; 
        std::vector<std::complex<float> > x;
        std::vector<float> window;
        FFT fft;

        void stft(std::vector<int16_t>& X, uint32_t n_fft, uint32_t hop_length, WindowType window_type);
        void istft(uint32_t hop_length); 

        void compute_window(uint32_t n_fft, WindowType type);
        void compute_hamming_window(uint32_t n_fft);
        void compute_hann_window(uint32_t n_fft);
};

#endif