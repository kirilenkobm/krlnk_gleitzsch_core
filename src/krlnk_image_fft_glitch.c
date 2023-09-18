//
// Created by Bogdan Kirilenko on 18.09.23.
//
#include <fftw3.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef void (*EffectFunc)(fftw_complex *, int);

// Effect functions
// this one is quite good
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


void process_chunk(double *image,
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
void image_fft_glitch(double *image, int size, int chunk_size, const char *effect, bool apply_interlacing)
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
    if (strcmp(effect, "zero_mid_freq") == 0)
    {
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
        process_chunk(image, offset, chunk_size, p, p_inv, in, current_effect, apply_dim);
        chunk_counter++;
    }

    // Handle the last chunk if it's smaller than chunk_size
    int remaining = size - offset;
    if (remaining > 0) {
        fftw_plan p_last = fftw_plan_dft_1d(remaining, in, in, FFTW_FORWARD, FFTW_MEASURE);
        fftw_plan p_last_inv = fftw_plan_dft_1d(remaining, in, in, FFTW_BACKWARD, FFTW_MEASURE);
        process_chunk(image, offset, remaining, p_last, p_last_inv, in, current_effect, false);
        fftw_destroy_plan(p_last);
    }

    // Clean up FFTW
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_destroy_plan(p_inv);
}
