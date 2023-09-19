//
// Created by Bogdan Kirilenko on 18.09.23.
//
// Collection of effects on FFT
#include "fft_effects.h"

// Let them stay here just for the reference.


// good filter, I keep it
void preserve_low_cut_high(fftw_complex *out, int chunk_size) {
    for (int i = 30; i < chunk_size - 10; ++i) {
        out[i][0] /= 7;
        out[i][1] /= 14;
    }
}

// has a great potential
void linear_fade(fftw_complex *out, int chunk_size) {
    int low_end = 10;
    for (int i = low_end; i < chunk_size - 10; ++i) {
        double factor = 1.0 - (6 * (double)(i - low_end) / chunk_size);
        if (factor < -1.0) {
            factor = -1.0;
        }
        out[i][0] *= factor;
        out[i][1] *= factor;
    }
    for (int i = chunk_size - 10; i < chunk_size; ++i) {
        out[i][0] = 0;
        out[i][1] = 0;
    }
}


// now it's interesting
void boost_mids(fftw_complex *out, int chunk_size) {
    int mid_start = chunk_size / 10;
    int mid_end = 10 * chunk_size / 11;
    for (int i = mid_start; i < mid_end; ++i) {
        out[i][0] *= 4;
        out[i][1] *= 4;
    }
}

// not so cool
void inverse_fade(fftw_complex *out, int chunk_size) {
    int low_end = 10;
    for (int i = low_end; i < chunk_size - low_end; ++i) {
        double factor = (double)(i - low_end) / chunk_size;
        out[i][0] *= (1 + factor);
        out[i][1] *= (1 + factor);
    }
}


// quite good
void zero_mid_freq(fftw_complex *out, int chunk_size) {
    int null_from = (int)(chunk_size / 10);
    for (int i = null_from; i < chunk_size - 1; ++i) {
        out[i][0] = 0;
        out[i][1] /= 5;
    }
}


// not so cool
void amplify_high_freq(fftw_complex *out, int chunk_size) {
    for (int i = chunk_size - 30; i < chunk_size; ++i) {
        out[i][0] *= 1.5;
        out[i][1] *= 2;
    }
}