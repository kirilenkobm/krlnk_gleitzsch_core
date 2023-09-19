//
// Created by Bogdan Kirilenko on 18.09.23.
//

#ifndef KRLNK_GLEITZSCH_CORE_FFT_EFFECTS_H
#define KRLNK_GLEITZSCH_CORE_FFT_EFFECTS_H

#include <fftw3.h>

void preserve_low_cut_high(fftw_complex *out, int chunk_size);
void linear_fade(fftw_complex *out, int chunk_size);
void boost_mids(fftw_complex *out, int chunk_size);
void inverse_fade(fftw_complex *out, int chunk_size);
void zero_mid_freq(fftw_complex *out, int chunk_size);
void amplify_high_freq(fftw_complex *out, int chunk_size);

#endif //KRLNK_GLEITZSCH_CORE_FFT_EFFECTS_H
