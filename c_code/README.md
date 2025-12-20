knowing that currently the code is using

uncoverspace_lock252564.c

for compiling dll or other lib


command gcc -shared -o uncoverspace_lock252564.dll uncoverspace_lock252564.c -Wl,--out-implib,libexample.a
req mingw32