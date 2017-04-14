#create a library
gcc -c -fPIC Library.c
gcc -shared -fPIC -o Library.so Library.o

#compile file
gcc -rdynamic -o program.exe program.c -ldl
