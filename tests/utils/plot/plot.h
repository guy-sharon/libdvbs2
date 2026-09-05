#ifndef TESTS_UTILS_PLOT_PLOT_H
#define TESTS_UTILS_PLOT_PLOT_H

#include <stddef.h>
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

#endif /* TESTS_UTILS_PLOT_PLOT_H */
