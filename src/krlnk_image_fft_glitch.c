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
    for (int i = 20; i < chunk_size - 10; ++i) {
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
                   fftw_complex *out,
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
    effect(out, chunk_size);
//    for (int i = 20; i < chunk_size - 5; ++i) {
//        out[i][0] = 0;
//        out[i][1] = 0;
//    }

    // Execute Inverse FFT
    fftw_plan p_inv = fftw_plan_dft_1d(chunk_size, out, in, FFTW_BACKWARD, FFTW_ESTIMATE);
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
    fftw_complex *in, *out;
    fftw_plan p;

    // Initialize FFTW
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * chunk_size);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * chunk_size);
    p = fftw_plan_dft_1d(chunk_size, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    int offset;
    EffectFunc current_effect = zero_mid_freq;  // Choose your effect here
    for (offset = 0; offset + chunk_size <= size; offset += chunk_size) {
        process_chunk(image, offset, chunk_size, p, in, out, current_effect);
    }

    // Handle the last chunk if it's smaller than chunk_size
    int remaining = size - offset;
    if (remaining > 0) {
        fftw_plan p_last = fftw_plan_dft_1d(remaining, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
        process_chunk(image, offset, remaining, p_last, in, out, current_effect);
        fftw_destroy_plan(p_last);
    }

    // Clean up FFTW
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);
}
