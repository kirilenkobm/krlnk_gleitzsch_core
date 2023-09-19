//
// Created by Bogdan Kirilenko on 19.09.23.
//

#ifndef KRLNK_GLEITZSCH_CORE_NAMED_HISTOGRAMS_H
#define KRLNK_GLEITZSCH_CORE_NAMED_HISTOGRAMS_H

typedef struct {
    const char *name;
    double *hist_real;
    double *hist_imag;
    int len;
} NamedHistogram;

extern NamedHistogram named_histograms[];
#endif //KRLNK_GLEITZSCH_CORE_NAMED_HISTOGRAMS_H
