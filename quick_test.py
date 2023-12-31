#!/usr/bin/env python3
"""Test script to check the compressor and decompressor functionality."""
import ctypes
import os
import numpy as np
from skimage import io
from concurrent.futures import ProcessPoolExecutor

input_dir = "test_data"
output_glitched_dir = "test_output_glitched"
os.makedirs(output_glitched_dir, exist_ok=True)

glitch_lib_path = "bin/krlnk_image_fft_glitch.so"
image_fft_glitch = ctypes.CDLL(glitch_lib_path)
image_fft_glitch.image_fft_glitch.argtypes = [
    ctypes.POINTER(ctypes.c_double),  # image
    ctypes.c_int,                     # image_arr_size
    ctypes.c_int,                     # chunk_size
    ctypes.c_int,
    ctypes.POINTER(ctypes.c_double),  # real_hist
    ctypes.c_int,                     # real_hist_len
    ctypes.POINTER(ctypes.c_double),  # imag_hist
    ctypes.c_int                      # imag_hist_len
]
real_hist_test = [1.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0]
real_hist_len_test = len(real_hist_test)
imag_hist_test = [0.8, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0, -1.0, 0.0, 0.0, 0.0, 0.0, 0.0]
imag_hist_len_test = len(real_hist_test)
quantization_levels = 8


def process_channel(img, channel_num, chunk_size):
    channel = img[:, :, channel_num].flatten()
    # Create ctypes-compatible arrays
    channel_c_array = (ctypes.c_double * len(channel))(*channel)
    real_hist_c_array = (ctypes.c_double * len(real_hist_test))(*real_hist_test)
    imag_hist_c_array = (ctypes.c_double * len(imag_hist_test))(*imag_hist_test)

    # Call your image_fft_glitch function for each channel
    image_fft_glitch.image_fft_glitch(
        channel_c_array,
        len(channel),
        chunk_size,
        quantization_levels,
        real_hist_c_array,
        real_hist_len_test,
        imag_hist_c_array,
        imag_hist_len_test
    )

    # Convert ctypes array back to NumPy arrays
    channel_numpy = np.ctypeslib.as_array(channel_c_array)
    # Reshape to original 2D shape
    reshaped_arr = channel_numpy.reshape(img[:, :, channel_num].shape)
    return reshaped_arr


def process_image(img_name):
    img_path = os.path.join(input_dir, img_name)
    basename = img_name.split(".")[0]
    img = io.imread(img_path)
    img_shape = img.shape
    print(f"Processing image {img_name} of shape: {img_shape}")
    chunk_size = img.shape[1] * 4

    # Process each channel with the helper function
    processed_red = process_channel(img, 0, chunk_size)
    processed_green = process_channel(img, 1, chunk_size)
    processed_blue = process_channel(img, 2, chunk_size)

    # Recombine channels into one 3D NumPy array
    glitched_img = np.stack([processed_red, processed_green, processed_blue], axis=2).astype(np.uint8)

    # Save glitched image
    output_glitched_path = os.path.join(output_glitched_dir, f"{basename}_glitched.jpg")
    io.imsave(output_glitched_path, glitched_img)


def run_test():
    # Loop through images in the 'test_input' folder
    test_image_files = [x for x in os.listdir(input_dir) if x.endswith(".jpg")]

    with ProcessPoolExecutor(max_workers=4) as executor:
        executor.map(process_image, test_image_files)


if __name__ == "__main__":
    run_test()
