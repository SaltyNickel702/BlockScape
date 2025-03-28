Download glfw3

compile glad.c into library
inside of srcdir

gcc -I./include/ -L./statLib/libglfw3.a -c ./src/lib/glad.c -o gladlib.o
ar rcs libglad.a gladlib.o


move libglad.a into statLib, and delete gladlib.o