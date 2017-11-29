# Graphics

Parses and renders a 3-dimensional world.

## Dependencies

This project requires a C++17-compatible compiler (e.g. clang 5). In addition,
it also requires the GLM library (0.9.8) to be available on your system.

The Makefile currently uses LLVM clang as the compiler, and using libc++ for
the standard library. Edit the Makefile according to your needs.

## Compilation

To compile, just run

    $ make

to generate an executable `graphics_d`. To generate a release target, run

    $ make DEBUG=0

to generate an executable `graphics`.

## Running

Some example level files can be found in the `levels` subdirectory. To use
them, run

    $ ./graphics_d levels/1.json
    $ # or, if building in release,
    $ ./graphics levels/1.json
