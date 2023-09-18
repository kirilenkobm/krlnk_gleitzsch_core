//
// Created by Bogdan Kirilenko on 18.09.23.
//
#include <fftw3.h>
#include <math.h>
#include <stdlib.h>


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
                   fftw_complex *in,
                   EffectFunc effect)
                   {
    // Copy chunk of input image data to FFTW input
    for (int i = 0; i < chunk_size; ++i) {
        in[i][0] = image[offset + i];
        in[i][1] = 0;
    }

    // Perform FFT
    fftw_execute(p);

    // HERE is where the magic could happen
    effect(in, chunk_size);

    // Execute Inverse FFT
    fftw_plan p_inv = fftw_plan_dft_1d(chunk_size, in, in, FFTW_BACKWARD, FFTW_MEASURE);
    fftw_execute(p_inv);

    // Copy back to original image array
    for (int i = 0; i < chunk_size; ++i) {
        image[offset + i] = in[i][0] / chunk_size;
    }

    // Destroy the inverse FFT plan
    fftw_destroy_plan(p_inv);
}


// function to be imported
void image_fft_glitch(double *image, int size, int chunk_size)
{
    fftw_complex *in;
    fftw_plan p;

    // Initialize FFTW
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * chunk_size);
    p = fftw_plan_dft_1d(chunk_size, in, in, FFTW_FORWARD, FFTW_MEASURE);

    int offset;
    EffectFunc current_effect = zero_mid_freq;  // Choose your effect here

    // not sure whether it helped me a lot =)
    #pragma omp parallel for
    for (offset = 0; offset + chunk_size <= size; offset += chunk_size) {
        process_chunk(image, offset, chunk_size, p, in, current_effect);
    }

    // Handle the last chunk if it's smaller than chunk_size
    int remaining = size - offset;
    if (remaining > 0) {
        fftw_plan p_last = fftw_plan_dft_1d(remaining, in, in, FFTW_FORWARD, FFTW_MEASURE);
        process_chunk(image, offset, remaining, p_last, in, current_effect);
        fftw_destroy_plan(p_last);
    }

    // Clean up FFTW
    fftw_destroy_plan(p);
    fftw_free(in);
}
