
CC=gcc -c
CFLAGS=

LD=gcc
LDFLAGS= -lglut

default: copernicus.elf bin/copernicus.o

copernicus.elf: bin/copernicus.o bin/3dsloader.o bin/astro.o bin/camera.o bin/objects.o bin/rings.o bin/util.o bin/tgaload.o
	$(LD) -o copernicus.elf bin/copernicus.o bin/3dsloader.o bin/astro.o bin/camera.o bin/objects.o bin/rings.o bin/util.o bin/tgaload.o $(LDFLAGS)

bin/copernicus.o: src/copernicus.cpp src/include/util.h src/include/astro.h src/include/tgaload.h src/include/3dsloader.h src/include/objects.h src/include/rings.h src/include/copernicus.h src/include/camera.h
	$(CC) -o bin/copernicus.o src/copernicus.cpp $(CFLAGS)

bin/astro.o: src/astro.cpp src/include/util.h src/include/astro.h src/include/tgaload.h
	$(CC) -o bin/astro.o src/astro.cpp $(CFLAGS)

bin/rings.o: src/rings.cpp src/include/util.h src/include/astro.h src/include/rings.h src/include/objects.h
	$(CC) -o bin/rings.o src/rings.cpp $(CFLAGS)

bin/objects.o: src/objects.cpp src/include/util.h src/include/camera.h src/include/objects.h
	$(CC) -o bin/objects.o src/objects.cpp $(CFLAGS)

bin/tgaload.o: src/tgaload.cpp src/include/tgaload.h
	$(CC) -o bin/tgaload.o src/tgaload.cpp $(CFLAGS)

bin/camera.o: src/camera.cpp src/include/camera.h src/include/util.h
	$(CC) -o bin/camera.o src/camera.cpp $(CFLAGS)

bin/util.o: src/util.cpp src/include/util.h
	$(CC) -o bin/util.o src/util.cpp $(CFLAGS)

bin/3dsloader.o: src/3dsloader.cpp src/include/3dsloader.h
	$(CC) -o bin/3dsloader.o src/3dsloader.cpp $(CFLAGS)

clean:
	rm -f bin/*.o
