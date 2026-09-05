#include <stdio.h>
#include <stdlib.h>

#include "plot.h"

enum {
    PLOT_WIDTH = 800,
    PLOT_HEIGHT = 600,
    PLOT_MARGIN = 60,
    PLOT_GRID_DIVISIONS = 10
};

#define PLOT_PADDING_FRACTION 0.05f

int plot_write_scatter_svg(const char *filename, const float *x, const float *y,
                           size_t count, const char *title)
{
    FILE *stream;
    float x_min;
    float x_max;
    float y_min;
    float y_max;
    float x_scale;
    float y_scale;
    float x_padding;
    float y_padding;
    size_t i;

    if (filename == NULL || x == NULL || y == NULL || count == 0) {
        return -1;
    }

    stream = fopen(filename, "w");
    if (stream == NULL) {
        return -1;
    }

    x_min = x[0];
    x_max = x[0];
    y_min = y[0];
    y_max = y[0];

    for (i = 1; i < count; ++i) {
        if (x[i] < x_min) {
            x_min = x[i];
        }
        if (x[i] > x_max) {
            x_max = x[i];
        }
        if (y[i] < y_min) {
            y_min = y[i];
        }
        if (y[i] > y_max) {
            y_max = y[i];
        }
    }

    if (x_min == x_max) {
        x_min -= 1.0f;
        x_max += 1.0f;
    }
    if (y_min == y_max) {
        y_min -= 1.0f;
        y_max += 1.0f;
    }

    x_padding = (x_max - x_min) * PLOT_PADDING_FRACTION;
    y_padding = (y_max - y_min) * PLOT_PADDING_FRACTION;
    x_min -= x_padding;
    x_max += x_padding;
    y_min -= y_padding;
    y_max += y_padding;

    x_scale = (float)(PLOT_WIDTH - 2 * PLOT_MARGIN) / (x_max - x_min);
    y_scale = (float)(PLOT_HEIGHT - 2 * PLOT_MARGIN) / (y_max - y_min);

    if (fprintf(stream,
                "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%d\" height=\"%d\" viewBox=\"0 0 %d %d\">\n"
                "  <rect width=\"100%%\" height=\"100%%\" fill=\"white\"/>\n"
                "  <text x=\"%d\" y=\"30\" font-family=\"sans-serif\" font-size=\"20\">%s</text>\n"
                "  <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" fill=\"none\" stroke=\"black\"/>\n",
                PLOT_WIDTH, PLOT_HEIGHT, PLOT_WIDTH, PLOT_HEIGHT,
                PLOT_MARGIN, title != NULL ? title : "Scatter plot",
                PLOT_MARGIN, PLOT_MARGIN,
                PLOT_WIDTH - 2 * PLOT_MARGIN, PLOT_HEIGHT - 2 * PLOT_MARGIN) < 0) {
        goto error;
    }

    if (fprintf(stream,
                "  <text x=\"%d\" y=\"%d\" text-anchor=\"middle\" font-family=\"sans-serif\" font-size=\"14\">X</text>\n"
                "  <text x=\"20\" y=\"%d\" text-anchor=\"middle\" font-family=\"sans-serif\" font-size=\"14\" transform=\"rotate(-90 20 %d)\">Y</text>\n",
                PLOT_WIDTH / 2, PLOT_HEIGHT - 18,
                PLOT_HEIGHT / 2, PLOT_HEIGHT / 2) < 0) {
        goto error;
    }

    for (i = 0; i <= PLOT_GRID_DIVISIONS; ++i) {
        float grid_x = (float)PLOT_MARGIN +
                       (float)i * (PLOT_WIDTH - 2 * PLOT_MARGIN) / PLOT_GRID_DIVISIONS;
        float grid_y = (float)PLOT_MARGIN +
                       (float)i * (PLOT_HEIGHT - 2 * PLOT_MARGIN) / PLOT_GRID_DIVISIONS;
        float tick_x = x_min + (x_max - x_min) * (float)i / PLOT_GRID_DIVISIONS;
        float tick_y = y_max - (y_max - y_min) * (float)i / PLOT_GRID_DIVISIONS;

        if (fprintf(stream,
                    "  <line x1=\"%.3f\" y1=\"%d\" x2=\"%.3f\" y2=\"%d\" stroke=\"#e0e0e0\"/>\n"
                    "  <line x1=\"%d\" y1=\"%.3f\" x2=\"%d\" y2=\"%.3f\" stroke=\"#e0e0e0\"/>\n",
                    grid_x, PLOT_MARGIN, grid_x, PLOT_HEIGHT - PLOT_MARGIN,
                    PLOT_MARGIN, grid_y, PLOT_WIDTH - PLOT_MARGIN, grid_y) < 0) {
            goto error;
        }

        if (fprintf(stream,
                    "  <line x1=\"%.3f\" y1=\"%d\" x2=\"%.3f\" y2=\"%d\" stroke=\"black\"/>\n"
                    "  <line x1=\"%d\" y1=\"%.3f\" x2=\"%d\" y2=\"%.3f\" stroke=\"black\"/>\n"
                    "  <text x=\"%.3f\" y=\"%d\" text-anchor=\"middle\" font-family=\"sans-serif\" font-size=\"12\">%.3g</text>\n"
                    "  <text x=\"%d\" y=\"%.3f\" text-anchor=\"end\" font-family=\"sans-serif\" font-size=\"12\">%.3g</text>\n",
                    grid_x, PLOT_HEIGHT - PLOT_MARGIN, grid_x, PLOT_HEIGHT - PLOT_MARGIN + 5,
                    PLOT_MARGIN - 5, grid_y, PLOT_MARGIN, grid_y,
                    grid_x, PLOT_HEIGHT - PLOT_MARGIN + 20, tick_x,
                    PLOT_MARGIN - 8, grid_y + 4.0f, tick_y) < 0) {
            goto error;
        }
    }

    for (i = 0; i < count; ++i) {
        float svg_x = (float)PLOT_MARGIN + (x[i] - x_min) * x_scale;
        float svg_y = (float)(PLOT_HEIGHT - PLOT_MARGIN) - (y[i] - y_min) * y_scale;

        if (fprintf(stream,
                    "  <circle cx=\"%.3f\" cy=\"%.3f\" r=\"3\" fill=\"#1565c0\"/>\n",
                    svg_x, svg_y) < 0) {
            goto error;
        }
    }

    if (fputs("</svg>\n", stream) == EOF || ferror(stream)) {
        goto error;
    }

    return fclose(stream) == 0 ? 0 : -1;

error:
    fclose(stream);
    return -1;
}
