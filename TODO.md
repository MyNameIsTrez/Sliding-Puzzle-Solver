# TODO: Create the alias `make` for either of these
C:/msys64/mingw64/bin/mingw32-make.exe
C:/msys64/usr/bin/make.exe

# Is version 11.2.0 and doesn't do anything in bash, but works in CMD.
C:/msys64/mingw64/bin/g++.exe main.cpp -o main.out


# Is version 8.2.0 and gives these errors:
# cc1plus: error: unrecognized command-line option '-auxbase'
# cc1plus: error: too many filenames given; type 'cc1plus --help' for usage
# cc1plus: error: CPU you selected does not support x86-64 instruction set
C:/MinGW/bin/gcc.exe main.cpp -o main.out

# Is version 11.2.0 and doesn't do anything in bash, but works in CMD.
# Doesn't recognize basic library iostream.
C:/msys64/mingw64/bin/gcc.exe main.cpp -o main.out