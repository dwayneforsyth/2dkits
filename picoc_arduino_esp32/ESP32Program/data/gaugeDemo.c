void setup();
void loop();

int tempGauge,rhGauge,dpGauge,baroGauge;
int hTempGauge,hrh,hDP,hbaro;

//#define BAR

void weatherSetupGauges()
{
  BME_init();
  TFT_fillScreen(0);  
  TFT_setTextSize(2);
  TFT_setTextColor(0x001F,0);
  TFT_setCursor(35,28);
  TFT_print("Temp");
  TFT_setCursor(2,105);
  TFT_print("RH");
  TFT_setCursor(218,134);
  TFT_print("DP");
  TFT_setTextSize(2);
  TFT_setCursor(272,206);
  TFT_setTextColor(0x001F,0);
  TFT_print("Baro");
  TFT_setTextSize(1);
  TFT_setCursor(282,224);
  TFT_print(" in Hg");
  TFT_Gauge_dropGauges();TFT_VbarGraph_dropGauges();
  tempGauge=TFT_Gauge(153,115,360-90,360+90,100,60,90,6,1,0xf100,"%0.0f","%0.0f F");
  rhGauge=TFT_Gauge(30,220,360-10,360+100,90,30,100,7,1,0xf100,"%0.0f","%0.0f%%");
  dpGauge=TFT_Gauge(158,220,360-20,360+100,90,40,90,5,1,0xf100,"%0.0f","%0.0f F");
  baroGauge=TFT_VbarGraph(272,22,16,180,28.0,31.0,6,0.1,0xf100,"%0.1f","%0.2f");
  float temp=BME_readTemp();
  float rh=BME_readRH(); 
  float pressure=BME_readPressure();
  float tc=(temp-32)*5/9;
  //DP =  243.04*( LN(RH/100)+( 17.625*T  )/ (243.04 + T ))  / (17.625 -  LN(RH / 100) -  (17.625*T ) / (243.04 + T ))
  float dp = (243.04*(log(rh/100)+((17.625*tc) / (243.04 + tc))) / (17.625 - log(rh / 100) - ((17.625*tc) / (243.04 + tc))))*9.0 / 5 + 32;
  TFT_Gauge_draw(tempGauge);
  TFT_Gauge_setPosition(tempGauge,temp);
  TFT_Gauge_drawDangerByValue(tempGauge,82,90,0xf100);
  TFT_Gauge_draw(rhGauge);
  TFT_Gauge_setPosition(rhGauge,rh);
  TFT_Gauge_draw(dpGauge);
  TFT_Gauge_setPosition(dpGauge,dp);
  TFT_VbarGraph_draw(baroGauge);
  TFT_VbarGraph_setPosition(baroGauge,pressure);
}
int main(int argc,char ** argv)
{
  setup();
  for (;;) loop();
  // Uncomment to run loop forever.
  //  When running loop forever,
  //  you'll get no response to web events
  return 0;
}

void weatherSetupBars()
{
  BME_init();
  TFT_fillScreen(0);  
  TFT_setTextSize(2);
  TFT_setTextColor(0x001F,0);
  TFT_setCursor(5,5);  TFT_print("Temp");
  TFT_setCursor(2,50); TFT_print("  RH");
  TFT_setCursor(2,95); TFT_print(" DPt");
  TFT_setCursor(2,135);TFT_print("Baro");
  hTempGauge=TFT_HbarGraph(54,5,200,10,60,90,6,1,0xf100,"%0.0f","%0.0f F");
  hrh=TFT_HbarGraph(54,50,200,10,30,100,7,1,0xf100,"%0.0f","%0.0f%%");
  hDP=TFT_HbarGraph(54,95,200,10,40,90,5,1,0xf100,"%0.0f","%0.0f F");
  hbaro=TFT_HbarGraph(54,135,200,10,28.0,31.0,6,0.1,0xf100,"%0.1f","%0.2f");
  TFT_HbarGraph_draw(hTempGauge);
  TFT_HbarGraph_draw(hDP);
  TFT_HbarGraph_draw(hrh);
  TFT_HbarGraph_draw(hbaro);

}
void setup()
{
  TFT_fillScreen(0);
#ifndef BAR
  weatherSetupGauges();
#else
  weatherSetupBars();
#endif;
  
}

void loop()
{
  print("Doing Environment Update\n");
  float temp=BME_readTemp();
  float rh=BME_readRH(); 
  float pressure=BME_readPressure();
  float tc=(temp-32)*5/9;
  //DP =  243.04*( LN(RH/100)+( 17.625*T  )/ (243.04 + T ))  / (17.625 -  LN(RH / 100) -  (17.625*T ) / (243.04 + T ))
  float dp = (243.04*(log(rh/100)+((17.625*tc) / (243.04 + tc))) / (17.625 - log(rh / 100) - ((17.625*tc) / (243.04 + tc))))*9.0 / 5 + 32;
#ifndef BAR
  TFT_Gauge_setPosition(tempGauge,temp);
  TFT_Gauge_setPosition(rhGauge,rh);
  TFT_Gauge_setPosition(dpGauge,dp);
  TFT_VbarGraph_setPosition(baroGauge,pressure);
#else
  TFT_HbarGraph_setPosition(hTempGauge ,temp); 
  TFT_HbarGraph_setPosition(hrh ,rh);
  TFT_HbarGraph_setPosition(hDP ,dp);
  TFT_HbarGraph_setPosition(hbaro ,pressure);
#endif
  screenCapture();
  delay(3*60000); // three minutes between readings
}
