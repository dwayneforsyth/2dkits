CC=gcc
CFLAGS=-I.
SERVER="forsyth@www.dforsyth.net"
BASE_PATH="/home/httpd/html_2dkits/kits/kit13"



%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

all: rain boom diamon cubes colcol maze axis space anoy twink

cubes: pattern_cubes.o core_utils.o core_helper.o
	$(CC) -o cubes pattern_cubes.o core_utils.o core_helper.o
	./cubes
	scp cubes4.pat $(SERVER):$(BASE_PATH)/patterns

boom: pattern_boom.o core_utils.o core_helper.o
	$(CC) -o boom pattern_boom.o core_utils.o core_helper.o
	./boom
	scp boom4.pat $(SERVER):$(BASE_PATH)/patterns

diamon: pattern_diamon.o core_utils.o core_helper.o
	$(CC) -o diamon pattern_diamon.o core_utils.o core_helper.o
	./diamon
	scp diamon1.pat $(SERVER):$(BASE_PATH)/patterns
	scp diamon2.pat $(SERVER):$(BASE_PATH)/patterns

rain: pattern_rain.o core_utils.o core_helper.o
	$(CC) -o rain pattern_rain.o core_utils.o core_helper.o
	./rain
	scp rain.pat $(SERVER):$(BASE_PATH)/patterns

axis: pattern_axis.o core_utils.o core_helper.o
	$(CC) -o axis pattern_axis.o core_utils.o core_helper.o
	./axis
	scp axis.pat $(SERVER):$(BASE_PATH)/patterns

maze: pattern_maze.o core_utils.o core_helper.o
	$(CC) -o maze pattern_maze.o core_utils.o core_helper.o
	./maze
	scp maze4.pat $(SERVER):$(BASE_PATH)/patterns

colcol: pattern_colcol.o core_utils.o core_helper.o
	$(CC) -o colcol pattern_colcol.o core_utils.o core_helper.o
	./colcol
	scp colcol1.pat $(SERVER):$(BASE_PATH)/patterns
	scp colcol2.pat $(SERVER):$(BASE_PATH)/patterns

space: pattern_space.o core_utils.o core_helper.o
	$(CC) -o space pattern_space.o core_utils.o core_helper.o
	./space
	scp space.pat $(SERVER):$(BASE_PATH)/patterns

anoy: pattern_anoy.o core_utils.o core_helper.o
	$(CC) -o anoy pattern_anoy.o core_utils.o core_helper.o
	./anoy
	scp anoy.pat $(SERVER):$(BASE_PATH)/patterns

twink: pattern_twink.o core_utils.o core_helper.o
	$(CC) -o twink pattern_twink.o core_utils.o core_helper.o
	./twink
	scp twink.pat $(SERVER):$(BASE_PATH)/patterns

clean:
	rm *.o
