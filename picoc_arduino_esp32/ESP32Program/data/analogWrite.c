void setup();
void loop();
#define LED 5
int main(int argc,char ** argv)
{
  setup();
  for (;;) loop();
  // Uncomment to run loop forever.
  //  When running loop forever,
  //  you'll get no response to web events
  return 0;
}

void setup()
{
  pinMode(LED,"INPUT");
  printf("Starting to run loop");
  setServoReport(0);
}

  float i=2.0;
  int dir=1;
void loop()
{
  doLoop();
  if (dir==1)
  {
    i+=0.5*sqrt(i);analogWrite(LED,(int)(i));
    if (i>4095) {i=4096;dir=-1;}
  }
  else 
  {
    i-=0.5*sqrt(i);
    analogWrite(LED,(int)(i));
    if (i<2) {dir=1;i=2;}
  }
}