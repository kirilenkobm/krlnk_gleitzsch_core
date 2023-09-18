CC=gcc
CFLAGS=-I./include -Wall -fPIC $(shell pkg-config --cflags fftw3) -arch arm64
LIBS=$(shell pkg-config --libs fftw3)
OUTPUT_DIR=bin

all: $(OUTPUT_DIR)/krlnk_image_fft_glitch.so

$(OUTPUT_DIR)/krlnk_image_fft_glitch.so: src/krlnk_image_fft_glitch.c
	mkdir -p $(OUTPUT_DIR)
	$(CC) $(CFLAGS) -shared -o $(OUTPUT_DIR)/krlnk_image_fft_glitch.so src/krlnk_image_fft_glitch.c $(LIBS)

clean:
	rm -f $(OUTPUT_DIR)/krlnk_image_fft_glitch.so
