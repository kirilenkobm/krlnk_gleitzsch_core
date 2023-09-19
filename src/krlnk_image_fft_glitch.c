//
// Created by Bogdan Kirilenko on 18.09.23.
//
#include <fftw3.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "fft_effects.h"  // Add this line


typedef void (*EffectFunc)(fftw_complex *, int);

// Effect functions >>>>>
// this one is quite good

// >>>> Effect functions


void process_chunk(double *image,
                   int offset,
                   int chunk_size,
                   fftw_plan p,
                   fftw_plan p_inv,
                   fftw_complex *in,
                   double *real_histogram,
                   double *imag_histogram)
{
    // Copy chunk of input image data to FFTW input
    for (int i = 0; i < chunk_size; ++i) {
        in[i][0] = image[offset + i];
        in[i][1] = 0;
    }

    // Perform FFT
    fftw_execute(p);

    // Apply histograms to real and imaginary parts
    for (int i = 0; i < chunk_size; ++i) {
        in[i][0] *= real_histogram[i];
        in[i][1] *= imag_histogram[i];
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


// Function to be imported
void image_fft_glitch(
        double *image,
        int image_arr_size,
        int chunk_size,
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
        process_chunk(image, offset, chunk_size, p, p_inv, in, real_histogram, imag_histogram);
    }

    // Handle the last chunk if it's smaller than chunk_size
    int remaining = image_arr_size - offset;
    if (remaining > 0) {
        fftw_plan p_last = fftw_plan_dft_1d(remaining, in, in, FFTW_FORWARD, FFTW_MEASURE);
        fftw_plan p_last_inv = fftw_plan_dft_1d(remaining, in, in, FFTW_BACKWARD, FFTW_MEASURE);
        process_chunk(image, offset, remaining, p_last, p_last_inv, in, real_histogram, imag_histogram);
        fftw_destroy_plan(p_last);
    }

    // Clean up FFTW
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_destroy_plan(p_inv);

    free(real_histogram);
    free(imag_histogram);
}


// Old implementation that is based on named effect filters
void process_chunk_effect(double *image,
                   int offset,
                   int chunk_size,
                   fftw_plan p,
                   fftw_plan p_inv,
                   fftw_complex *in,
                   EffectFunc effect,
                   bool apply_dim)
                   {
    // Copy chunk of input image data to FFTW input
    for (int i = 0; i < chunk_size; ++i) {
        double i_value = image[offset + i];
        if (apply_dim) {
            i_value *= 0.78;
        }
        in[i][0] = i_value;
        in[i][1] = 0;
    }

    // Perform FFT
    fftw_execute(p);

    // HERE is where the magic could happen
    effect(in, chunk_size);

    // Execute Inverse FFT
    fftw_execute(p_inv);

    // Copy back to original image array
    for (int i = 0; i < chunk_size; ++i) {
        image[offset + i] = in[i][0] / chunk_size;
    }
}


// function to be imported
void image_fft_glitch_effect(double *image, int size, int chunk_size, const char *effect, bool apply_interlacing)
{
    fftw_complex *in;
    fftw_plan p;
    fftw_plan p_inv;

    // Initialize FFTW
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * chunk_size);
    p = fftw_plan_dft_1d(chunk_size, in, in, FFTW_FORWARD, FFTW_MEASURE);
    p_inv = fftw_plan_dft_1d(chunk_size, in, in, FFTW_BACKWARD, FFTW_MEASURE);


    // select the desired effect
    EffectFunc current_effect;
    if (strcmp(effect, "preserve_low_cut_high") == 0) {
        current_effect = preserve_low_cut_high;
    } else if (strcmp(effect, "linear_fade") == 0) {
        current_effect = linear_fade;
    } else if (strcmp(effect, "boost_mids") == 0) {
        current_effect = boost_mids;
    } else if (strcmp(effect, "inverse_fade") == 0) {
        current_effect = inverse_fade;
    } else if (strcmp(effect, "zero_mid_freq") == 0) {
        current_effect = zero_mid_freq;
    } else if (strcmp(effect, "amplify_high_freq") == 0) {
        current_effect = amplify_high_freq;
    } else {
        // show error message?
        printf("Error! Cannot process effect %s\n", effect);
        return;
    }

    // go chunk-by-chunk
    int offset;
    bool apply_dim = false;
    int chunk_counter = 0;

    #pragma omp parallel for  // not sure whether it helped me a lot...
    for (offset = 0; offset + chunk_size <= size; offset += chunk_size) {
        if (apply_interlacing) {  // Only toggle if interlacing is enabled
            if (chunk_counter % 4 < 2) {
                apply_dim = true;
            } else {
                apply_dim = false;
            }
        }
        process_chunk_effect(image, offset, chunk_size, p, p_inv, in, current_effect, apply_dim);
        chunk_counter++;
    }

    // Handle the last chunk if it's smaller than chunk_size
    int remaining = size - offset;
    if (remaining > 0) {
        fftw_plan p_last = fftw_plan_dft_1d(remaining, in, in, FFTW_FORWARD, FFTW_MEASURE);
        fftw_plan p_last_inv = fftw_plan_dft_1d(remaining, in, in, FFTW_BACKWARD, FFTW_MEASURE);
        process_chunk_effect(image, offset, remaining, p_last, p_last_inv, in, current_effect, false);
        fftw_destroy_plan(p_last);
    }

    // Clean up FFTW
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_destroy_plan(p_inv);
}
