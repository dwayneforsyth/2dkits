CC=gcc
CFLAGS=-I.
SERVER="forsyth@www.dforsyth.net"
BASE_PATH="/home/httpd/html_2dkits/kits/kit25"



%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

all: boom diamon colcol anoy

boom: pattern_boom.o core_utils.o core_helper.o
	$(CC) -o boom pattern_boom.o core_utils.o core_helper.o
	./boom
	scp boom.pat $(SERVER):$(BASE_PATH)/patterns

diamon: pattern_diamon.o core_utils.o core_helper.o
	$(CC) -o diamon pattern_diamon.o core_utils.o core_helper.o
	./diamon
	scp diamon1.pat $(SERVER):$(BASE_PATH)/patterns
	scp diamon2.pat $(SERVER):$(BASE_PATH)/patterns

anoy: pattern_anoy.o core_utils.o core_helper.o
	$(CC) -o anoy pattern_anoy.o core_utils.o core_helper.o
	./anoy
	scp anoy.pat $(SERVER):$(BASE_PATH)/patterns

colcol: pattern_colcol.o core_utils.o core_helper.o
	$(CC) -o colcol pattern_colcol.o core_utils.o core_helper.o
	./colcol
	scp colcol1.pat $(SERVER):$(BASE_PATH)/patterns
	scp colcol2.pat $(SERVER):$(BASE_PATH)/patterns

other: hello.c nested.c printled.c
	scp hello.c $(SERVER):$(BASE_PATH)/patterns
	scp nested.c $(SERVER):$(BASE_PATH)/patterns
	scp printled.c $(SERVER):$(BASE_PATH)/patterns
	scp testpat0.c $(SERVER):$(BASE_PATH)/patterns
	scp cubes.c $(SERVER):$(BASE_PATH)/patterns
	scp rain.c $(SERVER):$(BASE_PATH)/patterns
	scp twink.c $(SERVER):$(BASE_PATH)/patterns
	scp maze.c $(SERVER):$(BASE_PATH)/patterns
	scp random.c $(SERVER):$(BASE_PATH)/patterns
	scp random1.c $(SERVER):$(BASE_PATH)/patterns

clean:
	rm *.o
