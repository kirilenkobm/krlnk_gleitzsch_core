//
// Created by Bogdan Kirilenko on 18.09.23.
//
#include <fftw3.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "fft_effects.h"
#include "named_histograms.h"


void process_chunk(double *image,
                   int offset,
                   int chunk_size,
                   int quantization_levels,
                   fftw_plan p,
                   fftw_plan p_inv,
                   fftw_complex *in,
                   const double *real_histogram,
                   const double *imag_histogram)
{
    // Copy chunk of input image data to FFTW input
    for (int i = 0; i < chunk_size; ++i) {
        in[i][0] = image[offset + i];
        in[i][1] = 0;
    }

    // Quantize to X levels
    for (int i = 0; i < chunk_size; ++i) {
        in[i][0] = round(in[i][0] * quantization_levels) / quantization_levels;
        in[i][1] = round(in[i][1] * quantization_levels) / quantization_levels;
    }

    // Perform FFT
    fftw_execute(p);
    // Apply histograms to real and imaginary parts
    for (int i = 0; i < chunk_size; ++i) {
        in[i][0] *= real_histogram[i];
        in[i][1] *= imag_histogram[i];
    }

    // Quantize to X levels (in frequency domain)
    for (int i = 0; i < chunk_size; ++i) {
        in[i][0] = round(in[i][0] * quantization_levels) / quantization_levels;
        in[i][1] = round(in[i][1] * quantization_levels) / quantization_levels;
    }
    // Execute Inverse FFT
    fftw_execute(p_inv);
    // Copy back to original image array
    for (int i = 0; i < chunk_size; ++i) {
        image[offset + i] = in[i][0] / chunk_size;
    }
}


double* interpolate_histogram(double *hist, int hist_len, int chunk_size) {
    double *new_hist = (double*) malloc(chunk_size * sizeof(double));

    // Handle the edge case where hist_len is 1
    if (hist_len == 1) {
        for (int i = 0; i < chunk_size; ++i) {
            new_hist[i] = hist[0];
        }
        return new_hist;
    }

    double step = (double) (hist_len - 1) / (chunk_size - 1);

    for (int i = 0; i < chunk_size; ++i) {
        int index = (int) (i * step);
        double remainder = i * step - index;
        if (index + 1 < hist_len) {
            new_hist[i] = hist[index] * (1 - remainder) + hist[index + 1] * remainder;
        } else {
            new_hist[i] = hist[index];
        }
    }
    return new_hist;
}


// Main entry point
void image_fft_glitch(
        double *image,
        int image_arr_size,
        int chunk_size,
        int quantization_levels,
        double *real_hist,
        int real_hist_len,
        double *imag_hist,
        int imag_hist_len
) {
    fftw_complex *in;
    fftw_plan p;
    fftw_plan p_inv;

    // Initialize FFTW
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * chunk_size);
    p = fftw_plan_dft_1d(chunk_size, in, in, FFTW_FORWARD, FFTW_MEASURE);
    p_inv = fftw_plan_dft_1d(chunk_size, in, in, FFTW_BACKWARD, FFTW_MEASURE);
    // initiate filter
    double *real_histogram = interpolate_histogram(real_hist, real_hist_len, chunk_size);
    double *imag_histogram = interpolate_histogram(imag_hist, imag_hist_len, chunk_size);

    int offset;

#pragma omp parallel for
    for (offset = 0; offset + chunk_size <= image_arr_size; offset += chunk_size) {
        process_chunk(image, offset, chunk_size, quantization_levels, p, p_inv, in, real_histogram, imag_histogram);
    }

    // Handle the last chunk if it's smaller than chunk_size
    int remaining = image_arr_size - offset;
    if (remaining > 0) {
        fftw_plan p_last = fftw_plan_dft_1d(remaining, in, in, FFTW_FORWARD, FFTW_MEASURE);
        fftw_plan p_last_inv = fftw_plan_dft_1d(remaining, in, in, FFTW_BACKWARD, FFTW_MEASURE);
        process_chunk(image, offset, remaining, quantization_levels, p_last, p_last_inv, in, real_histogram, imag_histogram);
        fftw_destroy_plan(p_last);
    }

    // Clean up FFTW
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_destroy_plan(p_inv);

    free(real_histogram);
    free(imag_histogram);
}


// standalone func to process 1 chunk
void process_1D_arr(double *arr,
                    int arr_size,
                    int quantization_levels,
                    double *real_hist,
                    int real_hist_len,
                    double *imag_hist,
                    int imag_hist_len
) {
    fftw_complex *in;
    fftw_plan p;
    fftw_plan p_inv;

    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * arr_size);
    p = fftw_plan_dft_1d(arr_size, in, in, FFTW_FORWARD, FFTW_MEASURE);
    p_inv = fftw_plan_dft_1d(arr_size, in, in, FFTW_BACKWARD, FFTW_MEASURE);

    // Copy chunk of input image data to FFTW input
    for (int i = 0; i < arr_size; ++i) {
        in[i][0] = arr[i];
        in[i][1] = 0;
    }

    // Quantize to X levels
    for (int i = 0; i < arr_size; ++i) {
        in[i][0] = round(in[i][0] * quantization_levels) / quantization_levels;
        // in[i][1] = round(in[i][1] * quantization_levels) / quantization_levels;
    }

    double *real_histogram = interpolate_histogram(real_hist, real_hist_len, arr_size);
    double *imag_histogram = interpolate_histogram(imag_hist, imag_hist_len, arr_size);

    // Perform FFT
    fftw_execute(p);
    // Apply histograms to real and imaginary parts
    for (int i = 0; i < arr_size; ++i) {
        in[i][0] *= real_histogram[i];
        in[i][1] *= imag_histogram[i];
    }

    // Quantize to X levels (in frequency domain)
    for (int i = 0; i < arr_size; ++i) {
        in[i][0] = round(in[i][0] * quantization_levels) / quantization_levels;
        in[i][1] = round(in[i][1] * quantization_levels) / quantization_levels;
    }
    // Execute Inverse FFT
    fftw_execute(p_inv);
    // Copy back to original image array
    for (int i = 0; i < arr_size; ++i) {
        arr[i] = in[i][0] / arr_size;
    }
}


// Alternative, simpler entry point
void image_fft_glitch_with_named_hist(
        double *image,
        int image_arr_size,
        int chunk_size,
        const char *named_histogram
) {
    double *real_hist = NULL;
    double *imag_hist = NULL;
    int hist_len = 0;
    int quant_levels = 8;
    for (int i = 0; named_histograms[i].name != NULL; ++i) {
        if (strcmp(named_histograms[i].name, named_histogram) == 0) {
            real_hist = named_histograms[i].hist_real;
            imag_hist = named_histograms[i].hist_imag;
            hist_len = named_histograms[i].len;
            break;
        }
    }

    if (real_hist == NULL) {
        printf("Error! Unknown named histogram: %s\n", named_histogram);
        return;
    }

    image_fft_glitch(image,
                     image_arr_size,
                     chunk_size,
                     quant_levels,
                     real_hist,
                     hist_len,
                     imag_hist,
                     hist_len);
}
