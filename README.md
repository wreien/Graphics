# Graphics

Parses and renders a 3-dimensional world.

## Dependencies

This project requires a C++17-compatible compiler (e.g. clang 5). In addition,
it also requires the GLM library (0.9.8) to be available on your system.

The Makefile currently uses LLVM clang as the compiler, and using libc++ for
the standard library. Edit the Makefile according to your needs.

## Compilation

### With Make

To compile, just run

    $ make

to generate an executable `graphics_d`. To generate a release target, run

    $ make DEBUG=0

to generate an executable `graphics`.

### With Tup

First run

    $ tup variants

to generate the `build-release` and `build-debug` folders. Then, running

    $ tup

anywhere in the tree will generate executables inside each of those folders.
For ease of use, you can symlink those executables to the root level of the
tree with

    $ ln -s build-release/graphics g
    $ ln -s build-debug/graphics gd

(Note that we can't use `graphics` as a name, since `tup` will complain that
we are trying to generate a file that already exists.)

## Running

Some example level files can be found in the `levels` subdirectory. To use
them, run

    $ ./graphics_d levels/1.json
    $ # or, if building in release,
    $ ./graphics levels/1.json
