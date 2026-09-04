# libdvbs2

`libdvbs2` is a pure C library for DVB-S2 building blocks.

## Layout

- `include/dvbs2/` — installed public headers
- `src/` — library implementation (private headers stay here)
- `tests/` — unit and integration tests
- `examples/` — small, standalone usage programs
- `docs/` — design notes and API documentation
- `cmake/` — reusable CMake modules

## Build

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build
```
