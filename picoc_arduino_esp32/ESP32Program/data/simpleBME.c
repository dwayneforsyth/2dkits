void setup();
void loop();
int main(int argc,char ** argv)
{
  setup();
  for (;;) loop();
  // Uncomment to run loop forever.
  //  When running loop forever,
  //  you'll get no response to web events without having doLoop() in the loop function
  
  // code below will never run if the for (;;) loop() line above is uncommented
  // conversly, if for (::) is commented out the code below will give one set of readings and the program will terminate
  
  float temp=BME_readTemp();
  float pressure=BME_readPressure();
  float rh=BME_readRH();
  float tc=(temp-32)*5/9;
    //DP =  243.04*( LN(RH/100)+( 17.625*T  )/ (243.04 + T ))  / (17.625 -  LN(RH / 100) -  (17.625*T ) / (243.04 + T ))
  float dp = (243.04*(log(rh/100)+((17.625*tc) / (243.04 + tc))) / (17.625 - log(rh / 100) - ((17.625*tc) / (243.04 + tc))))*9.0 / 5 + 32;
  printf("\n");
  printf("     Temp is %0.1f F\n",temp);
  printf(" Pressure is %0.2f in HG\n",pressure);
  printf("       RH is %0.1f %%\n",rh);
  printf("Dew Point is %0.0f F\n",dp);
  return 0;
}

void setup()
{
  TFT_setTextSize(2);setConsoleOn(1);
  BME_init();
}
int lCount=0;
void loop()
{
  doLoop();
  if ((lCount++%3000)==0)
  {
    float temp=BME_readTemp();
    float pressure=BME_readPressure();
    float rh=BME_readRH();
    float tc=(temp-32)*5/9;
    //DP =  243.04*( LN(RH/100)+( 17.625*T  )/ (243.04 + T ))  / (17.625 -  LN(RH / 100) -  (17.625*T ) / (243.04 + T ))
    float dp = (243.04*(log(rh/100)+((17.625*tc) / (243.04 + tc))) / (17.625 - log(rh / 100) - ((17.625*tc) / (243.04 + tc))))*9.0 / 5 + 32;
    printf("\n");
    printf("     Temp is %0.1f F\n",temp);
    printf(" Pressure is %0.2f in HG\n",pressure);
    printf("       RH is %0.1f %%\n",rh);
    printf("Dew Point is %0.0f F\n",dp);
  }
  delay(5);  // this delays 5 msec and doLoop() calls the ESP32Program loop() which adds another 5 msec of delay
             // for a total of 10 msec delay per loop cycle thus lCount%3000==0 is true every 30 seconds
}