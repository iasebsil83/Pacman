#executable
run: pacman.o libisentlib.a
	gcc -Wall pacman.o -o run libisentlib.a -lm -lglut -lGL -lX11
	rm -f *.o libisentlib.a



#game
pacman.o: pacman.c GfxLib.h ESLib.h
	gcc -Wall -c pacman.c -O2



#GfxLib libraries
libisentlib.a: BmpLib.o ESLib.o GfxLib.o
	ar r libisentlib.a BmpLib.o ESLib.o GfxLib.o
	ranlib libisentlib.a

BmpLib.o: BmpLib.c BmpLib.h OutilsLib.h
	gcc -Wall -O2 -c BmpLib.c

ESLib.o: ESLib.c ESLib.h ErreurLib.h
	gcc -Wall -O2 -c ESLib.c

GfxLib.o: GfxLib.c GfxLib.h ESLib.h
	gcc -Wall -O2 -c GfxLib.c -I/usr/include/GL

zip:
	tar -cvzf libisentlib.tgz *.[ch] *.bmp *.pdf makefile
