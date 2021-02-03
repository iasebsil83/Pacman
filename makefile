#executable
run: pacman.o libisentlib.a
	gcc -Wall pacman.o -o run libisentlib.a -lm -lglut -lGL -lX11
	rm -f *.o *.a



#game
pacman.o: pacman.c src/GfxLib.h src/ESLib.h
	gcc -Wall -c pacman.c -O2



#GfxLib libraries
libisentlib.a: ESLib.o GfxLib.o
	ar r libisentlib.a ESLib.o GfxLib.o
	ranlib libisentlib.a

ESLib.o: src/ESLib.c src/ESLib.h src/ErreurLib.h
	gcc -Wall -c src/ESLib.c -O2

GfxLib.o: src/GfxLib.c src/GfxLib.h src/ESLib.h
	gcc -Wall -c src/GfxLib.c -I/usr/include/GL -O2
