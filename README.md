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
# To be filled -> I redefine args every hour.
# Let's wait until it's stable.
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
