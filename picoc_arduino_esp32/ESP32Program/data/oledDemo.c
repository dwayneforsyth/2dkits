FILE MANAGER SETTINGS CODE TEMPLATE HELP   
 
/oledDemo.c
   Program Arguments 
 Reset ESP32 
void setup();
void loop();
int main(int argc,char ** argv)
{
  setup();
  //for (;;) loop();
  // Uncomment to run loop forever.
  //  When running loop forever,
  //  you'll get no response to web events
  return 0;
}

void setup()
{
  int i;char buf[33]; 
  oclear();
  osetFontSize(1);
  odrawString(0,0,"Display is 6x21");
  for (i=1;i<6;i++) {
    sprintf((char *)&buf,"Display line %d is drawn here",i);
    odrawString(0,10*i,(char *)&buf);
  }
  odisplay();
  delay(5000);
  oclear();
  odrawString(40,25,"Hello World");
  odisplay();delay(3000);oconsoleInit();
  for (i=0;i<21;i++)
  {
    sprintf((char *)&buf,"Line %d is put on the display here",i);
    oconsolePrintln((char *)&buf);delay(25);
  }
  delay(5000);
  oclear();
  for (i=0;i<60;i+=8)
  { odrawRect(i,i/2,128-i*2,64-i); odisplay();}
  delay(5000);
  oclear();
  osetFontSize(1);
  odrawRect(0,0,128,64);
  odrawString(2,5,"Hello World");
  osetFontSize(2);
  odrawString(2,18,"Hello World");
  osetFontSize(3);
  odrawString(2,33,"Hello World");
  osetCursor(0,0);
  odisplay(); 
  delay(5000);
  oclear();
  osetFontSize(3);
  odrawString(2,20,"Hello World");
  odisplay();
  delay(1000);
  osetColor(2);
  ofillRect(0,0,128,33);
  odrawLine(0,0,128,64);
  odrawLine(0,64,128,0);
  osetFontSize(10);
  odisplay();
  for (i=1;i<7;i++)
  { oinvertDisplay(i%2); odisplay(); delay(500); }
}

void loop()
{
  doLoop();
}
