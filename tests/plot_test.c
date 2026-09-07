#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "utils/plot/plot.h"
#include "complex.h"

static void test_write_scatter_svg(void)
{
    const float x[] = { -10.0f, 0.0f, 1.0f };
    const float y[] = { 1.0f, 0.0f, -1.0f };
    const char *filename = "test.svg";
    char output[8192];
    FILE *stream;
    size_t bytes_read;

    assert(plot_write_scatter_svg(filename, x, y, 3, "Test plot") == 0);
    stream = fopen(filename, "r");
    assert(stream != NULL);

    bytes_read = fread(output, 1, sizeof(output) - 1, stream);
    output[bytes_read] = '\0';

    assert(strstr(output, "<svg") != NULL);
    assert(strstr(output, "Test plot") != NULL);
    assert(strstr(output, "<line") != NULL);
    assert(strstr(output, "text-anchor") != NULL);
    assert(strstr(output, "<circle") != NULL);
    assert(strstr(output, "</svg>") != NULL);
    assert(fclose(stream) == 0);
    assert(remove(filename) == 0);
}

static void test_write_complex(void) {
    const complexf_t data[] = { 
        { -10.0f, 1.0f }, 
        { 0.0f, 0.0f }, 
        { 1.0f, -1.0f } 
    };
    const char *filename = "test_complex.svg";
    char output[8192];
    FILE *stream;
    size_t bytes_read;
    int result;

    result = plot_write_complex(filename, data, 3, "Test complex plot");
    assert(result == 0);

    stream = fopen(filename, "r");
    assert(stream != NULL);

    bytes_read = fread(output, 1, sizeof(output) - 1, stream);
    output[bytes_read] = '\0';

    assert(strstr(output, "<svg") != NULL);
    assert(strstr(output, "Test complex plot") != NULL);
    assert(strstr(output, "<line") != NULL);
    assert(strstr(output, "text-anchor") != NULL);
    assert(strstr(output, "<circle") != NULL);
    assert(strstr(output, "</svg>") != NULL);
    assert(fclose(stream) == 0);
    assert(remove(filename) == 0);
}

static void test_invalid_input(void) {
    const float point[] = { 0.0f };

    assert(plot_write_scatter_svg(NULL, point, point, 1, NULL) == -1);
    assert(plot_write_scatter_svg("test.svg", NULL, point, 1, NULL) == -1);
    assert(plot_write_scatter_svg("test.svg", point, NULL, 1, NULL) == -1);
    assert(plot_write_scatter_svg("test.svg", point, point, 0, NULL) == -1);
}

int main(void)
{
    test_write_scatter_svg();
    test_write_complex();
    test_invalid_input();
    return 0;
}
