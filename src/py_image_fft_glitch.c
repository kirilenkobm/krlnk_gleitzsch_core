//
// Created by Bogdan Kirilenko on 19.09.23.
//
#define PY_SSIZE_T_CLEAN
#include "image_fft_glitch.h"
#include <Python.h>
#include <numpy/arrayobject.h>  // Include the NumPy header

static PyObject* py_image_fft_glitch(PyObject* self, PyObject* args) {
    PyArrayObject *image_array, *real_hist_array, *imag_hist_array;
    int image_arr_size, chunk_size, quantization_levels, real_hist_len, imag_hist_len;

    // Parse Python arguments
    if (!PyArg_ParseTuple(args, "O!iiiiO!iO!i", &PyArray_Type, &image_array, &image_arr_size, &chunk_size, &quantization_levels,
                          &PyArray_Type, &real_hist_array, &real_hist_len,
                          &PyArray_Type, &imag_hist_array, &imag_hist_len)) {
        return NULL;
    }

    // Get pointers to array data
    double *image = (double*)PyArray_DATA(image_array);
    double *real_hist = (double*)PyArray_DATA(real_hist_array);
    double *imag_hist = (double*)PyArray_DATA(imag_hist_array);

    // Call your C function
    image_fft_glitch(image, image_arr_size, chunk_size, quantization_levels, real_hist, real_hist_len, imag_hist, imag_hist_len);

    Py_RETURN_NONE;
}

static PyMethodDef methods[] = {
        {"image_fft_glitch", py_image_fft_glitch, METH_VARARGS, "Description"},
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef module = {
        PyModuleDef_HEAD_INIT,
        "your_module_name",
        "Module description",
        -1,
        methods
};

PyMODINIT_FUNC PyInit_your_module_name(void) {
    import_array();  // Initialize NumPy
    return PyModule_Create(&module);
}
