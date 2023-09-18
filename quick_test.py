#!/usr/bin/env python3
"""Test script to check the compressor and decompressor functionality."""
import ctypes
import os
import numpy as np
from skimage import io

input_dir = "test_data"
output_glitched_dir = "test_output_glitched"
os.makedirs(output_glitched_dir, exist_ok=True)

glitch_lib_path = "bin/krlnk_image_fft_glitch.so"
image_fft_glitch = ctypes.CDLL(glitch_lib_path)
image_fft_glitch.image_fft_glitch.argtypes = [ctypes.POINTER(ctypes.c_double), ctypes.c_int, ctypes.c_int]


def run_test():
    # Loop through images in the 'test_input' folder
    test_image_files = [x for x in os.listdir(input_dir) if x.endswith(".jpg")]
    for img_name in test_image_files:
        img_path = os.path.join(input_dir, img_name)
        basename = img_name.split(".")[0]
        img = io.imread(img_path)
        img_shape = img.shape
        print(f"Image shape: {img_shape}")
        chunk_size = img.shape[1]

        # Separate channels and flatten each
        red_channel = img[:, :, 0].flatten()
        green_channel = img[:, :, 1].flatten()
        blue_channel = img[:, :, 2].flatten()

        # Create ctypes-compatible arrays
        red_c_array = (ctypes.c_double * len(red_channel))(*red_channel)
        green_c_array = (ctypes.c_double * len(green_channel))(*green_channel)
        blue_c_array = (ctypes.c_double * len(blue_channel))(*blue_channel)

        # Call your pseudo_compress_decompress_wrapper function for each channel
        image_fft_glitch.image_fft_glitch(red_c_array, len(red_channel), chunk_size)
        image_fft_glitch.image_fft_glitch(green_c_array, len(green_channel), chunk_size)
        image_fft_glitch.image_fft_glitch(blue_c_array, len(blue_channel), chunk_size)

        # Convert ctypes array back to NumPy arrays
        red_numpy = np.ctypeslib.as_array(red_c_array)
        green_numpy = np.ctypeslib.as_array(green_c_array)
        blue_numpy = np.ctypeslib.as_array(blue_c_array)

        # Reshape to original 2D shape
        reshaped_red = red_numpy.reshape(img[:, :, 0].shape)
        reshaped_green = green_numpy.reshape(img[:, :, 1].shape)
        reshaped_blue = blue_numpy.reshape(img[:, :, 2].shape)

        # Recombine channels into one 3D NumPy array
        glitched_img = np.stack([reshaped_red, reshaped_green, reshaped_blue], axis=2).astype(np.uint8)

        # Save glitched image
        output_glitched_path = os.path.join(output_glitched_dir, f"{basename}_glitched.jpg")
        io.imsave(output_glitched_path, glitched_img)


if __name__ == "__main__":
    run_test()
