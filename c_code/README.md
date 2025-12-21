knowing that currently the code is using

uncoverspace_lock252564.c

for compiling main.dll or other lib (main.dylib)


command gcc -shared -o main.dll uncoverspace_lock252564.c -Wl,--out-implib,libexample.a
req mingw32