#include <stdio.h>
#include <blinkie.h>

char *LEDValue[19] = { "0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","+","-","#" };

void allLedsColor( int red, int green, int blue) {
   int l,x,y;
   for (l=0;l<4;l++) {
      for (x=0;x<4;x++) {
         for (y=0;y<4;y++) {
             setLed(l,x,y, red, green, blue);
}  }  }  }


int main(int argc,char ** argv) {
   int r,g,b,fad,color;
   int cycles = 10;

   while(cycles != 0) {
      cycles--;
      for (color=0;color < 7; color++) {
          for (fad=1; fad < 16; fad++) {
             r = (color & 0x01)? 0 : fad;
             g = (color & 0x02)? 0 : fad;
             b = (color & 0x04)? 0 : fad;
             allLedsColor(r,g,b);
	     if (endFrame(150)) { return 0; }
          }
          for (fad= 14; fad != 0; fad--) {
             r = (color & 0x01)? 0 : fad;
             g = (color & 0x02)? 0 : fad;
             b = (color & 0x04)? 0 : fad;
             allLedsColor(r,g,b);
	     if (endFrame(150)) { return 0; }
          }
      }
   }
   return 0;
}
