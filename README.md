# dcc
Quick and dirty compilation database generator.

Unlike [Bear][BEAR] dcc does not use `LD_PRELOAD` or the like but instead is used as a replacement executable for the
compiler. For example:

    dcc $(pwd) gcc -DFOO foo.c
    
or

    make CC="dcc $(pwd) gcc"
    
[BEAR]: https://github.com/rizsotto/Bear

# issues
* check for duplicates
