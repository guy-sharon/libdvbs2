#define _POSIX_C_SOURCE 200809L

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "crc.h"
#include "bbframe.h"
#include "frame.h"

#define ITERATIONS 100
#define KB (1024)

static double now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1e6;
}

static void bench_crc8(void) {
    uint8_t *data = malloc(64 * KB);
    for (size_t i = 0; i < 64 * KB; i++)
        data[i] = (uint8_t)(i & 0xFF);

    for (size_t sz = KB; sz <= 64 * KB; sz *= 4) {
        volatile uint8_t crc = crc8(data, sz);
        double t0 = now_ms();
        for (int it = 0; it < ITERATIONS; it++)
            crc = crc8(data, sz);
        double dt = (now_ms() - t0) / ITERATIONS;
        double mbps = (sz / 1024.0) / dt;
        printf("  crc8 %5zu KB:  %8.3f ms/iter  (%.1f MB/s)  crc=%02x\n",
               sz / KB, dt, mbps, crc);
    }
    free(data);
}

static void bench_scramble(void) {
    uint8_t *data = malloc(64 * KB);
    uint8_t *ref = malloc(64 * KB);
    for (size_t i = 0; i < 64 * KB; i++) {
        data[i] = (uint8_t)(i & 0xFF);
        ref[i] = data[i];
    }

    for (size_t sz = KB; sz <= 64 * KB; sz *= 4) {
        double t0 = now_ms();
        for (int it = 0; it < ITERATIONS; it++) {
            memcpy(data, ref, sz);
            bbframe_scramble(data, sz);
        }
        double dt = (now_ms() - t0) / ITERATIONS;
        double mbps = (sz / 1024.0) / dt;
        printf("  scramble %5zu KB:  %8.3f ms/iter  (%.1f MB/s)\n",
               sz / KB, dt, mbps);
    }
    free(data);
    free(ref);
}

static void bench_frame(void) {
    frame_t frame = {0};
    modcod_t mc = SHORT_1_4;
    frame_init(&frame, mc);

    uint8_t *ref = malloc(mc.kbch);
    for (size_t i = 0; i < mc.kbch; i++) {
        frame.buffer[i] = (uint8_t)(i & 0xFF);
        ref[i] = frame.buffer[i];
    }

    double t0 = now_ms();
    for (int it = 0; it < ITERATIONS; it++) {
        memcpy(frame.buffer, ref, mc.kbch);
        frame_scramble(&frame);
    }
    double dt = (now_ms() - t0) / ITERATIONS;
    double mbps = (mc.kbch / 1024.0) / dt;
    printf("  frame_scramble (SHORT_1_4, kbch=%u):  %8.3f ms/iter  (%.1f MB/s)\n",
           mc.kbch, dt, mbps);
    free(ref);
}

int main(void) {
    printf("=== DVB-S2 Performance Benchmarks ===\n\n");
    printf("CRC-8:\n");
    bench_crc8();
    printf("\nBBFrame Scramble:\n");
    bench_scramble();
    printf("\nFrame-level:\n");
    bench_frame();
    printf("\nDone.\n");
    return 0;
}
