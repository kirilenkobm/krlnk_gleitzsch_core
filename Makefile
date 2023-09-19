CC=gcc
PYTHON_INCLUDES = $(shell python3-config --includes) -I$(shell python3 -c "import numpy; print(numpy.get_include())")
CFLAGS=-I./include -Wall -fPIC $(PYTHON_INCLUDES) $(shell pkg-config --cflags fftw3)
LIBS=$(shell pkg-config --libs fftw3)
OUTPUT_DIR=bin
OBJ_FILES=src/krlnk_image_fft_glitch.o src/fft_effects.o src/named_histograms.o
PYTHON_OBJ_FILES=src/py_image_fft_glitch.o $(OBJ_FILES)
PYTHON_LDFLAGS = $(shell python3-config --ldflags)

# Detect M1 Mac and add -arch arm64 flag
ARCH := $(shell arch)
ifeq ($(ARCH), arm64)
    CFLAGS += -arch arm64
endif

all: $(OUTPUT_DIR)/krlnk_image_fft_glitch.so

$(OUTPUT_DIR)/krlnk_image_fft_glitch.so: $(OBJ_FILES)
	mkdir -p $(OUTPUT_DIR)
	$(CC) $(CFLAGS) -shared -o $(OUTPUT_DIR)/krlnk_image_fft_glitch.so $(OBJ_FILES) $(LIBS)
	rm -f $(OBJ_FILES)

#python: $(PYTHON_OBJ_FILES)
#	mkdir -p $(OUTPUT_DIR)
#	$(CC) $(CFLAGS) -shared -o $(OUTPUT_DIR)/py_krlnk_image_fft_glitch.so $(PYTHON_OBJ_FILES) $(LIBS) $(PYTHON_LDFLAGS)
#	rm -f $(PYTHON_OBJ_FILES)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OUTPUT_DIR)/krlnk_image_fft_glitch.so $(OBJ_FILES) $(PYTHON_OBJ_FILES) $(OUTPUT_DIR)/py_krlnk_image_fft_glitch.so
