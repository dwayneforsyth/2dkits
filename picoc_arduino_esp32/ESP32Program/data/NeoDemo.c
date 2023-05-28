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
   NEO_init(7,16);
   NEO_setup(25);
   NEO_rainbow(2);
   NEO_theaterChase(0xf100,50);
   NEO_theaterChaseRainbow(50);
   NEO_colorWipe(0xfff,100);
   NEO_colorWipe(0,100);
}

void loop()
{

}