//
// Created by Bogdan Kirilenko on 18.09.23.
//

#ifndef KRLNK_COMPRESSOR_IMAGE_FFT_GLITCH_H
#define KRLNK_COMPRESSOR_IMAGE_FFT_GLITCH_H

#endif //KRLNK_COMPRESSOR_IMAGE_FFT_GLITCH_H
#include <stdbool.h>

void image_fft_glitch(double *image,
                      int image_arr_size,
                      int chunk_size,
                      double *real_hist,
                      int real_hist_len,
                      double *imag_hist,
                      int imag_hist_len);

void image_fft_glitch_with_named_hist(
        double *image,
        int image_arr_size,
        int chunk_size,
        const char *named_histogram
);
