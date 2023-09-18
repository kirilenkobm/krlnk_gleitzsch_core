# Krlnk Gleitzsch Core

![Apple M1 Supported](https://img.shields.io/badge/Apple%20M1-Supported-black.svg)
![Build Status](https://github.com/kirilenkobm/krlnk_gleitzsch_core/actions/workflows/c-cpp.yml/badge.svg)

A simple algorithm that applies FFT to introduce glitch to image arrays.
Imitates sound compression and decompression.

## Build

The project depends on `fftw3` which can be installed using `brew` on macOS 
or `sudo apt-get install -y libfftw3-dev` on Ubuntu.
Afterwards, just call `make`.

To run tests:

```bash
pip3 install -r requirements.txt
./quick_test.py
```

## Usage

### image_fft_glitch function

The following arguments: to be filled

#### Available effects are:
```txt
fill this part
```

### As a shared library in a Python-based project

The snippet below...

```python
# locate the shared library
glitch_lib_path = "bin/krlnk_image_fft_glitch.so"
image_fft_glitch = ctypes.CDLL(glitch_lib_path)
image_fft_glitch.image_fft_glitch.argtypes = [ctypes.POINTER(ctypes.c_double),  # image 2D array
                                              ctypes.c_int,  # number of pixels in 2D array
                                              ctypes.c_int,  # chunk size -> recommended image width
                                              ctypes.c_char_p,  # effect name
                                              ctypes.c_bool]  # apply interlacing
# apply the function to a 2D array
channel = img[:, :, channel_num].flatten()
# Create ctypes-compatible arrays
channel_c_array = (ctypes.c_double * len(channel))(*channel)
# Call your pseudo_compress_decompress_wrapper function for each channel
image_fft_glitch.image_fft_glitch(channel_c_array, len(channel), chunk_size, test_effect, True)
# Convert ctypes array back to NumPy arrays
channel_numpy = np.ctypeslib.as_array(channel_c_array)
# Reshape to original 2D shape
reshaped_arr = channel_numpy.reshape(img[:, :, channel_num].shape)
```

### In a swiftUI app

...

### In a Kotlin app

...

## Linked projects

Those projects use `lame` to achieve the desired effect.
This project aims to replace `lame` for glitchy images.

* [Kotlin-based Gleitzsch-Kt](https://github.com/kirilenkobm/Gleitzsch_Kt)
* [Python-based early version](https://github.com/kirilenkobm/gleitzsch_v4)

## Examples

See examples in the aforementioned projects.
... to be filled here
