

#include <stdint.h>
#include <stdio.h>
#include <string.h>


// header is 24 bytes
typedef struct blinkieHeader_t {
   uint8_t type;
   uint8_t speed;
   uint8_t cycles;
   uint8_t free[5];
   uint8_t name[16];
} blinkieHeader_t;

typedef struct blinkieFrame32_t {
   uint16_t led[8][4][4];
   uint8_t cycles;
   uint8_t delay;
} blinkieFrame32_t;

#define LED_PLUS 16
#define LED_MINUS 17
#define LED_NOOP 18

blinkieFrame32_t patternFrame = {
   .delay = 10,
};
blinkieHeader_t header = {
   .type = 32,
   .speed = 100,
   .cycles = 20,
   .free = {0x00},
};

void setLed(uint8_t l, uint8_t x, uint8_t y, uint8_t iR, uint8_t iG, uint8_t iB) {
    patternFrame.led[l][x][y] = (iR <<10) | (iG <<5) | iB;
}

void getLed(uint8_t l, uint8_t x, uint8_t y, uint8_t *iR, uint8_t *iG, uint8_t *iB) {
    *iB = patternFrame.led[l][x][y] & 0x1f;
    *iG = (patternFrame.led[l][x][y] >>5) & 0x1f;
    *iR = (patternFrame.led[l][x][y] >>10) & 0x1f;
}

void allLedsColor( uint8_t red, uint8_t green, uint8_t blue) {
   uint8_t l,x,y;
   for (l=0;l<8;l++) {
      for (x=0;x<4;x++) {
         for (y=0;y<4;y++) {
             setLed(l,x,y, red, green, blue);
}  }  }  }

void levelLedsColor( uint8_t l, uint8_t red, uint8_t green, uint8_t blue) {
   uint8_t x,y;
   for (x=0;x<4;x++) {
      for (y=0;y<4;y++) {
          setLed(l,x,y, red, green, blue);
}  }  }  

void allLedsOff(void) {
   allLedsColor( 0, 0, 0);
}

const char *LEDValue[19] = { "0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","+","-","#" };

void printHeader(void) {
   printf("#######################################\n");
   printf("name=%s\n",header.name);
   printf("type=%d\n",header.type);
   printf("speed=%d\n",header.speed);
   printf("cycles=%d\n",header.cycles);
   printf("#######################################\n");
}
void printFrame(void) {
   int8_t l,x,y;
   uint8_t red,green,blue;
   static uint16_t frame=0;

   printf("\nframe %d\n",frame);
   for (l=7;l>=0;l--) {
      printf("#level %d\n",l);
      for (x=0;x<4;x++) {
         for (y=0;y<4;y++) {
             getLed(l,x,y, &red, &green, &blue);
             printf(" (%2s,%2s,%2s)",LEDValue[red],LEDValue[green],LEDValue[blue]);
         }
         printf("\n");
      }
   }
   printf("cycles=%d\n",patternFrame.cycles);
   frame++;
}

void writeFrame(FILE *ptr) {
        fwrite( &patternFrame, sizeof(patternFrame), 1, ptr);
}

void make_boom(void) {
   int8_t l,x,y;

        FILE *ptr = fopen("boom.pat","wb");
        strncpy(header.name,"Boom",16);
        fwrite( &header, sizeof(header), 1, ptr);
        printHeader();

	allLedsColor(LED_MINUS,LED_NOOP,LED_NOOP);
        for (l=3;l<5;l++) {
	    setLed(l,1,1, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,2, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,1, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,2, LED_PLUS, LED_NOOP, LED_NOOP);
        }
        patternFrame.cycles = 8;
	patternFrame.delay = 1;
        writeFrame(ptr);
        printFrame();

        for (l=2;l<6;l++) {
	    setLed(l,1,1, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,2, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,1, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,2, LED_PLUS, LED_NOOP, LED_NOOP);
        }
        for (l=3;l<5;l++) {
	    setLed(l,0,1, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,0,2, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,0, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,3, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,0, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,3, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,3,1, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,3,2, LED_PLUS, LED_NOOP, LED_NOOP);
        }
        patternFrame.cycles = 8;
	patternFrame.delay = 1;
        writeFrame(ptr);
        printFrame();

        for (l=1;l<7;l++) {
	    setLed(l,1,1, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,2, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,1, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,2, LED_PLUS, LED_NOOP, LED_NOOP);
        }
        for (l=2;l<6;l++) {
	    setLed(l,0,1, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,0,2, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,0, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,3, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,0, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,3, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,3,1, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,3,2, LED_PLUS, LED_NOOP, LED_NOOP);
        }
        levelLedsColor(3, LED_PLUS, LED_NOOP, LED_NOOP);
        levelLedsColor(4, LED_PLUS, LED_NOOP, LED_NOOP);
        patternFrame.cycles = 8;
	patternFrame.delay = 1;
        writeFrame(ptr);
        printFrame();

        for (l=0;l<8;l++) {
	    setLed(l,1,1, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,2, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,1, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,2, LED_PLUS, LED_NOOP, LED_NOOP);
        }
        for (l=1;l<7;l++) {
	    setLed(l,0,1, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,0,2, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,0, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,3, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,0, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,3, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,3,1, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,3,2, LED_PLUS, LED_NOOP, LED_NOOP);
        }
        levelLedsColor(2, LED_PLUS, LED_NOOP, LED_NOOP);
        levelLedsColor(3, LED_PLUS, LED_NOOP, LED_NOOP);
        levelLedsColor(4, LED_PLUS, LED_NOOP, LED_NOOP);
        levelLedsColor(5, LED_PLUS, LED_NOOP, LED_NOOP);
        patternFrame.cycles = 8;
	patternFrame.delay = 1;
        writeFrame(ptr);
        printFrame();

        for (l=0;l<8;l++) {
	    setLed(l,0,1, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,0,2, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,0, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,3, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,0, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,3, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,3,1, LED_PLUS, LED_NOOP, LED_NOOP);
	    setLed(l,3,2, LED_PLUS, LED_NOOP, LED_NOOP);
        }
        levelLedsColor(1, LED_PLUS, LED_NOOP, LED_NOOP);
        levelLedsColor(2, LED_PLUS, LED_NOOP, LED_NOOP);
        levelLedsColor(3, LED_PLUS, LED_NOOP, LED_NOOP);
        levelLedsColor(4, LED_PLUS, LED_NOOP, LED_NOOP);
        levelLedsColor(5, LED_PLUS, LED_NOOP, LED_NOOP);
        levelLedsColor(6, LED_PLUS, LED_NOOP, LED_NOOP);
        patternFrame.cycles = 8;
	patternFrame.delay = 1;
        writeFrame(ptr);
        printFrame();

	allLedsColor(LED_PLUS,LED_NOOP,LED_NOOP);
        for (l=3;l<5;l++) {
	    setLed(l,1,1, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,2, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,1, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,2, LED_MINUS, LED_NOOP, LED_NOOP);
        }
        patternFrame.cycles = 8;
	patternFrame.delay = 1;
        writeFrame(ptr);
        printFrame();

        for (l=2;l<6;l++) {
	    setLed(l,1,1, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,2, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,1, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,2, LED_MINUS, LED_NOOP, LED_NOOP);
        }
        for (l=3;l<5;l++) {
	    setLed(l,0,1, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,0,2, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,0, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,3, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,0, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,3, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,3,1, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,3,2, LED_MINUS, LED_NOOP, LED_NOOP);
        }
        patternFrame.cycles = 8;
	patternFrame.delay = 1;
        writeFrame(ptr);
        printFrame();

        for (l=1;l<7;l++) {
	    setLed(l,1,1, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,2, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,1, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,2, LED_MINUS, LED_NOOP, LED_NOOP);
        }
        for (l=2;l<6;l++) {
	    setLed(l,0,1, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,0,2, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,0, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,3, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,0, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,3, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,3,1, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,3,2, LED_MINUS, LED_NOOP, LED_NOOP);
        }
        levelLedsColor(3, LED_MINUS, LED_NOOP, LED_NOOP);
        levelLedsColor(4, LED_MINUS, LED_NOOP, LED_NOOP);
        patternFrame.cycles = 8;
	patternFrame.delay = 1;
        writeFrame(ptr);
        printFrame();

        for (l=0;l<8;l++) {
	    setLed(l,1,1, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,2, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,1, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,2, LED_MINUS, LED_NOOP, LED_NOOP);
        }
        for (l=1;l<7;l++) {
	    setLed(l,0,1, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,0,2, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,0, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,3, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,0, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,3, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,3,1, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,3,2, LED_MINUS, LED_NOOP, LED_NOOP);
        }
        levelLedsColor(2, LED_MINUS, LED_NOOP, LED_NOOP);
        levelLedsColor(3, LED_MINUS, LED_NOOP, LED_NOOP);
        levelLedsColor(4, LED_MINUS, LED_NOOP, LED_NOOP);
        levelLedsColor(5, LED_MINUS, LED_NOOP, LED_NOOP);
        patternFrame.cycles = 8;
	patternFrame.delay = 1;
        writeFrame(ptr);
        printFrame();

        for (l=0;l<8;l++) {
	    setLed(l,0,1, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,0,2, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,0, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,1,3, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,0, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,2,3, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,3,1, LED_MINUS, LED_NOOP, LED_NOOP);
	    setLed(l,3,2, LED_MINUS, LED_NOOP, LED_NOOP);
        }
        levelLedsColor(1, LED_MINUS, LED_NOOP, LED_NOOP);
        levelLedsColor(2, LED_MINUS, LED_NOOP, LED_NOOP);
        levelLedsColor(3, LED_MINUS, LED_NOOP, LED_NOOP);
        levelLedsColor(4, LED_MINUS, LED_NOOP, LED_NOOP);
        levelLedsColor(5, LED_MINUS, LED_NOOP, LED_NOOP);
        levelLedsColor(6, LED_MINUS, LED_NOOP, LED_NOOP);
        patternFrame.cycles = 8;
	patternFrame.delay = 1;
        writeFrame(ptr);
        printFrame();

	allLedsColor(LED_MINUS,LED_NOOP,LED_NOOP);
        patternFrame.cycles = 8;
	patternFrame.delay = 1;
        writeFrame(ptr);
        printFrame();

        fclose(ptr);
}
void make_test(void) {
   int8_t l,x,y;

        FILE *ptr = fopen("test99.pat","wb");
        strncpy(header.name,"test99",16);
        fwrite( &header, sizeof(header), 1, ptr);
        printHeader();

        levelLedsColor(0, 0, 0, 0);
        levelLedsColor(1, 1, 1, 1);
        levelLedsColor(2, 2, 2, 2);
        levelLedsColor(3, 3, 3, 3);
        levelLedsColor(4, 4, 4, 4);
        levelLedsColor(5, 5, 5, 5);
        levelLedsColor(6, 6, 6, 6);
        levelLedsColor(7, 7, 7, 7);

        patternFrame.cycles = 8;
        writeFrame(ptr);
        printFrame();

        levelLedsColor(0, 2, 2, 2);
        levelLedsColor(1, 3, 3, 3);
        levelLedsColor(2, 4, 4, 4);
        levelLedsColor(3, 5, 5, 5);
        levelLedsColor(4, 6, 6, 6);
        levelLedsColor(5, 7, 7, 7);
        levelLedsColor(6, 8, 8, 8);
        levelLedsColor(7, 9, 9, 9);

        patternFrame.cycles = 8;
        writeFrame(ptr);
        printFrame();

        fclose(ptr);
}

void main(void) {
    make_boom();
    make_test();
}
