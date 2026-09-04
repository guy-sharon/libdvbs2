#ifndef DVBS2_DVBS2_H
#define DVBS2_DVBS2_H

/**
 * @file dvbs2.h
 * @brief Public API for libdvbs2.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define DVBS2_VERSION_MAJOR 0
#define DVBS2_VERSION_MINOR 1
#define DVBS2_VERSION_PATCH 0

/** Returns the library version as a static string. */
const char *dvbs2_version(void);

#ifdef __cplusplus
}
#endif

#endif /* DVBS2_DVBS2_H */
