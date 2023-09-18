# Krlnk Gleitzsch Core

![Apple M1 Supported](https://img.shields.io/badge/Apple%20M1-Supported-black.svg)

A simple algorithm that applies FFT to introduce glitch to image arrays.
Imitates sound compression and decompression.
The project depends on `fftw3` which can be installed using `brew`.
Just do `git clone` and `make`.

To run tests:

```bash
pip3 install -r requirements.txt
./quick_test.py
```

Drafty usage in a Python-based project:
```txt
# initiate the library
image_fft_glitch = ctypes.CDLL(glitch_lib_path)
image_fft_glitch.image_fft_glitch.argtypes = [ctypes.POINTER(ctypes.c_double), ctypes.c_int, ctypes.c_int]

# get some image's channel
img = io.imread(img_path)
red_channel = img[:, :, 0].flatten()
# and process it
red_c_array = (ctypes.c_double * len(red_channel))(*red_channel)
image_fft_glitch.image_fft_glitch(red_c_array, len(red_channel), chunk_size)
red_numpy = np.ctypeslib.as_array(red_c_array)
reshaped_red = red_numpy.reshape(img[:, :, 0].shape)
# then recover the whole image
glitched_img = np.stack([reshaped_red, reshaped_green, reshaped_blue], axis=2).astype(np.uint8)
```
## Linked projects

Those projects use `lame` to achieve the desired effect.
This project aims to replace `lame` for glitchy images.

* [Kotlin-based Gleitzsch-Kt](https://github.com/kirilenkobm/Gleitzsch_Kt)
* [Python-based early version](https://github.com/kirilenkobm/gleitzsch_v4)

## Examples

See examples in the aforementioned projects.
... to be filled here
