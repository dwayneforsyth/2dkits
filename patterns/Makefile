CC=gcc
CFLAGS=-I.
SERVER="forsyth@www.dforsyth.net"
BASE_PATH="/home/httpd/html_2dkits/kits/kit25"



%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

all: rain boom diamon

boom: pattern_boom.o core.o
	$(CC) -o boom pattern_boom.o core.o
	./boom
	scp boom.pat $(SERVER):$(BASE_PATH)/patterns

diamon: pattern_diamon.o core.o
	$(CC) -o diamon pattern_diamon.o core.o
	./diamon
	scp diamon1.pat $(SERVER):$(BASE_PATH)/patterns
	scp diamon2.pat $(SERVER):$(BASE_PATH)/patterns

rain: pattern_rain.o core.o
	$(CC) -o rain pattern_rain.o core.o
	./rain
	scp rain.pat $(SERVER):$(BASE_PATH)/patterns


clean:
	rm *.o
