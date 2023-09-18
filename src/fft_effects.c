//
// Created by Bogdan Kirilenko on 18.09.23.
//
#include "fft_effects.h"

void preserve_low_cut_high(fftw_complex *out, int chunk_size) {
    for (int i = 10; i < chunk_size - 10; ++i) {
        out[i][0] = 0;
        out[i][1] = 0;
    }
}

// boring
void linear_fade(fftw_complex *out, int chunk_size) {
    for (int i = 10; i < chunk_size - 10; ++i) {
        double factor = 1.0 - (double)(i - 10) / (chunk_size - 20);
        out[i][0] *= factor;
        out[i][1] *= factor;
    }
    for (int i = chunk_size - 10; i < chunk_size; ++i) {
        out[i][0] = 0;
        out[i][1] = 0;
    }
}


void boost_mids(fftw_complex *out, int chunk_size) {
    int mid_start = chunk_size / 4;
    int mid_end = 3 * chunk_size / 4;
    for (int i = mid_start; i < mid_end; ++i) {
        out[i][0] *= 2;
        out[i][1] *= 2;
    }
}


void inverse_fade(fftw_complex *out, int chunk_size) {
    for (int i = 10; i < chunk_size - 10; ++i) {
        double factor = (double)(i - 10) / (chunk_size - 20);
        out[i][0] *= (1 + factor);
        out[i][1] *= (1 + factor);
    }
}


void zero_mid_freq(fftw_complex *out, int chunk_size) {
    for (int i = 30; i < chunk_size - 1; ++i) {
        out[i][0] = 0;
        out[i][1] = 0;
    }
}


// not so cool
void amplify_high_freq(fftw_complex *out, int chunk_size) {
    for (int i = chunk_size - 10; i < chunk_size; ++i) {
        out[i][0] *= 2;
        out[i][1] *= 2;
    }
}