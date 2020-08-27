lcc = C:\Users\Robin\Downloads\gbdk-2.95-3-win32\gbdk\bin\lcc.exe
bgb = C:\Users\Robin\Downloads\bgb\bgb.exe


compile:
	mkdir build #
	$(lcc) -o build/flappy.gb src/main.c src/tiles.c src/welcome_bkg.c src/numbers.c src/sound.c

run: compile
	$(bgb) build/flappy.gb