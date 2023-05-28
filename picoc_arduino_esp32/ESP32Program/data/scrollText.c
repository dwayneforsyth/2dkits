//#include "TFT_Stuff.h"
drop("main");drop("__exit_value");drop("__argc");drop("__argv");
int main(int argc,char ** argv)
{
  int i,tsize=1; char buf[60]; 
  consoleOut("\n");
  TFT_setTextColor(b16_GREENYELLOW,b16_BLACK);
  TFT_setTextSize(1);
  tsize=1; // tsize is only used to allow calculation of available screen rows and cols
             // and you don't normally need to do this when doing Console Output
  // setScrollPixels(8); // Only use setScrollPixels after a TFT_setTextSize if you want to change the scroll pixel amount
  for (i=1;i<30/tsize;i++)
  {
    sprintf((char *)&buf,"Console line %2d is here\n",i);
    consoleOut((char *)&buf);
  }
  TFT_setTextColor(b16_WHITE,b16_BLACK);
  sprintf((char *)&buf,"textSize(1) gives %d lines x %d cols",240/8/tsize,320/tsize/6);
  consoleOut((char *)&buf); 
  screenCapture();
  consoleOut("\n");
  TFT_setTextColor(b16_GREEN,b16_BLACK);
  TFT_setTextSize(2);tsize=2;
  for (i=1;i<30/tsize;i++)
  {
    sprintf((char *)&buf,"Console line %2d\n",i);
    consoleOut((char *)&buf);
  }
  sprintf((char *)&buf,"size 2 gives %d x %d",240/8/tsize,320/tsize/6);
  consoleOut((char *)&buf);
  screenCapture();
  consoleOut("\n");
  TFT_setTextColor(b16_BLACK,b16_WHITE);
  Debug();
  TFT_setTextSize(3);tsize=3;
  for (i=1;i<30/tsize;i++)
  {
    sprintf((char *)&buf,"line %2d\n",i);
    consoleOut((char *)&buf);
  }
  TFT_setTextColor(b16_WHITE,b16_BLACK);
  sprintf((char *)&buf,"size 3 %d x %d",240/8/tsize,320/tsize/6);
  consoleOut((char *)&buf);
  stopDebug();
  screenCapture();
  consoleOut("\n");
  TFT_setTextColor(b16_BLACK,b16_WHITE);
  TFT_setTextSize(4);tsize=4;
  for (i=1;i<30/tsize;i++)
  {
    sprintf((char *)&buf,"line %2d\n",i);
    consoleOut((char *)&buf);
  }
  TFT_setTextColor(b16_WHITE,b16_BLACK);
  sprintf((char *)&buf,"4: %d x %d \n",240/8/tsize,320/tsize/6);
  consoleOut((char *)&buf);
  screenCapture();
  TFT_setTextSize(2);
  consoleOut("Only text size 1 or\n");
  consoleOut("2 are really useful\n");
  consoleOut("But you can also mix\n");
  consoleOut("sizes&colors for emphasis!\n");
  TFT_setTextSize(2);
  setScrollPixels(20); // 1.25 line spacing
  consoleOut("A few lines of\n");
  consoleOut("text to demonstrate\n");
  consoleOut("line spacing of 1&1/4");
  setScrollPixels(1);
  for (i=0;i<17;i++) scroll();
  TFT_setTextSize(4);
  consoleAlert("Done! ! ! ! !"); // consoleAlert prints black on a red background
  printf("Program Done\n");
  screenCapture();
  return 1;
 }

