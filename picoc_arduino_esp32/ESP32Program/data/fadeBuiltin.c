void setup();
void loop();
int brightStep = 1,brightness = 0,LED_BUILTIN=2;
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

}

void loop()
{
  doLoop(); // causes the interpreters loop() function to be called allowing servicing of web events
  brightness += brightStep;
  if ( brightness == 0 || brightness == 255 ) brightStep = -brightStep;
  analogWrite(LED_BUILTIN, brightness);
  delay(5);
}
