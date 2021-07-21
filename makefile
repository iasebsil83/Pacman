#executable
run: pacman.o S2DE.o
	gcc -Wall -o run pacman.o S2DE.o -lm -lglut -lGL
	rm -f *.o



#game
pacman.o: src/pacman.c lib/S2DE.h
	gcc -Wall -O2 -c src/pacman.c



#S2DE management (use S2DE.o/.h to access to 2D graphic engine)
S2DE.o: lib/S2DE.c lib/S2DE.h
	gcc -Wall -O2 -c lib/S2DE.c -I/usr/include/GL

