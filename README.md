# dcc
Quick and dirty compilation database generator.
Unlike [Bear]: https://github.com/rizsotto/Bear dcc does not use LD_PRELOAD
or the like but instead should be used as a replacement executable for the
compiler. For example:
    dcc $(pwd) gcc -DFOO foo.c
or
    make CC="dcc $(pwd) gcc"

# issues
* check for duplicates
