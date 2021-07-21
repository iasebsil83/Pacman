#executable
run: pacman.o libisentlib.a
	gcc -Wall pacman.o -o run libisentlib.a -lm -lglut -lGL -lX11
	rm -f *.o *.a



#game
pacman.o: src/pacman.c lib/GfxLib.h lib/ESLib.h
	gcc -Wall -c src/pacman.c -O2



#GfxLib libraries
libisentlib.a: ESLib.o GfxLib.o
	ar r libisentlib.a ESLib.o GfxLib.o
	ranlib libisentlib.a

ESLib.o: lib/ESLib.c lib/ESLib.h lib/ErreurLib.h
	gcc -Wall -c lib/ESLib.c -O2

GfxLib.o: lib/GfxLib.c lib/GfxLib.h lib/ESLib.h
	gcc -Wall -c lib/GfxLib.c -I/usr/include/GL -O2
