#include <stdio.h>
#include <blinkie.h>

char *LEDValue[19] = { "0","1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","+","-","#" };


int main(int argc,char ** argv) {
   int l,x,y;
   int red,green,blue;

   printf("Start\n");
   for (l=(NUM_LAYER-1);l>=0;l--) {
      printf("#level %d\n",l);
      for (x=0;x<4;x++) {
         for (y=0;y<4;y++) {
             getLed(l,x,y, &red, &green, &blue);
             printf(" (%2s,%2s,%2s)",LEDValue[red],LEDValue[green],LEDValue[blue]);
         }
         printf("\n");
      }
   }
   printf("Done\n");
   return 0;
}
