//
// Created by Bogdan Kirilenko on 19.09.23.
//
#include <stdlib.h>
#include "named_histograms.h"  // Include the header file


NamedHistogram named_histograms[] = {
        {"default",
         (double[]){1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
         (double[]){1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
         4},
        {"another_name",
                (double[]){1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.9},
                (double[]){1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.7},
                4},
        {"some_name",
                (double[]){1.0, 0.5, 0.0, 0.0, 0.1},
                (double[]){1.0, 0.5, 0.0, 0.0, 0.5},
                4},
        {NULL, NULL, NULL, 0}
        // to be filled
};
