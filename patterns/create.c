// header is 24 bytes
typedef struct blinkieHeader_t {
   uint8_t type;
   uint8_t speed;
   uint8_t free[22];
} blinkieHeader_t;

typedef struct blinkieFrame32_t {
   uint16_t led[8][4][4];
   uint8_t cycles;
   uint8_t pad;
}

#define LED_PLUS 16
#define LED_MINUS 17
#define LED_NOOP 18

blinkieFrame32_t frame;
blinkieHeader_t header = {
   .type = 32,
   .speed = 100,
   .free = {0x00},
}

void setLed(uint8_t l, uint8_t x, uint8_t y, uint8_t iR, uint8_t iG, uint8_t iB) {
    frame.led[l,x,y] = (iR <<10) | (iG <<5) | iB;
}

void allLedsColor( uint8_t red, uint8_t green, uint8_t blue) {
   uint8_t l,x,y;
   for (l=0;l<8;l++) {
      for (x=0;x<4;x++) {
         for (y=0;y<4;y++) {
             setLed(l,x,y, red, green, blue);
}  }  }  }

void allLedsOff(void) {
   allLedsColor( 0, 0, 0);
}

void printFrame(vod) {
}

void main(void) {
	writeHeader();

	allLedsOff();
	writeFrame();

	allLedsColor(LED_NOOP,LED_NOOP,LED_NOOP);
	setLed(3,1,1, LED_PLUS, 0, 0);
	setLed(3,1,2, LED_PLUS, 0, 0);
	setLed(3,2,1, LED_PLUS, 0, 0);
	setLed(3,2,2, LED_PLUS, 0, 0);
	setLed(4,1,1, LED_PLUS, 0, 0);
	setLed(4,1,2, LED_PLUS, 0, 0);
	setLed(4,2,1, LED_PLUS, 0, 0);
	setLed(4,2,2, LED_PLUS, 0, 0);
	writeFrame();


