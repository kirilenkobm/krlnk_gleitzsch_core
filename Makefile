CC=gcc
CFLAGS=-I./include -Wall -fPIC $(shell pkg-config --cflags fftw3)
LIBS=$(shell pkg-config --libs fftw3)
OUTPUT_DIR=bin
OBJ_FILES=src/krlnk_image_fft_glitch.o src/fft_effects.o

# Detect M1 Mac and add -arch arm64 flag
ARCH := $(shell arch)
ifeq ($(ARCH), arm64)
    CFLAGS += -arch arm64
endif

all: $(OUTPUT_DIR)/krlnk_image_fft_glitch.so

$(OUTPUT_DIR)/krlnk_image_fft_glitch.so: $(OBJ_FILES)
	mkdir -p $(OUTPUT_DIR)
	$(CC) $(CFLAGS) -shared -o $(OUTPUT_DIR)/krlnk_image_fft_glitch.so $(OBJ_FILES) $(LIBS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OUTPUT_DIR)/krlnk_image_fft_glitch.so $(OBJ_FILES)
