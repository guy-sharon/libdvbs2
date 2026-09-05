#ifndef TESTS_UTILS_PLOT_PLOT_H
#define TESTS_UTILS_PLOT_PLOT_H

#include <stddef.h>

#include "complex.h"

/**
 * Write an SVG scatter plot to a file.
 *
 * @param filename Destination SVG file path.
 * @param x      X-coordinate array.
 * @param y      Y-coordinate array.
 * @param count  Number of points in both arrays; must be greater than zero.
 * @param title  Plain-text plot title, or NULL for the default title.
 * @return 0 on success, or -1 for invalid input or a write error.
 */
int plot_write_scatter_svg(const char *filename, const float *x, const float *y,
                           size_t count, const char *title);

/**
 * Write an SVG scatter plot of complex numbers to a file.
 * @param filename Destination SVG file path.
 * @param data   Array of complex numbers.
 * @param count  Number of complex numbers in the array; must be greater than zero.
 * @param title  Plain-text plot title, or NULL for the default title.
 * @return 0 on success, or -1 for invalid input or a write error.
 */
int plot_write_complex(const char *filename, const complexf_t *data,
                       size_t count, const char *title);

#endif /* TESTS_UTILS_PLOT_PLOT_H */
