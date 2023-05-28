void setup()
{
   EPD_fillScreen(0);
   EPD_doPartial(0);
   EPD_Gauge_dropGauges();
   // EpdGauge(float xc,float yc,float sang,float eang,float radius,float sval,float eval,int divisions,float increments,int color,char * fmt,char * valueFmt)
   EPD_Gauge(400,385,270,90+360,335,60,90,6,1,1,"%0.0f","%0.1f");
   EPD_Gauge_draw(0);
   EPD_Gauge_drawDangerByValue(0,80,90,1);
   EPD_Gauge_setPosition(0,72.2);
   EPD_setCursor(70,130);EPD_setTextSize(2);EPD_print("Temp");
   EPD_display();
   float t;
   for(t=63;t<85;t=t+5) {EPD_Gauge_setPosition(0,t);EPD_partialUpdate();}
   delay(3000);
   EPD_Gauge_dropGauges();
   EPD_fillScreen(1);
   EPD_Gauge(400,385,270,90+360,335,60,90,6,1,0,"%0.0f","%0.1f");
   EPD_Gauge_draw(0);
   EPD_Gauge_drawDangerByValue(0,80,90,0);
   EPD_Gauge_setPosition(0,72.2);
   EPD_setCursor(70,130);EPD_setTextSize(2);EPD_print("Temp");
   EPD_display();
   for(t=63;t<85;t=t+5) {EPD_Gauge_setPosition(0,t);EPD_partialUpdate();}
   delay(3000);
   EPD_Gauge_dropGauges();
   EPD_fillScreen(0);
   EPD_Gauge(400,305,360-170,360+170,260,0,100,10,1,1,"%0.0f","%0.0f");
   EPD_Gauge(400,325,270,360+90,270,0,1,8,0.125,1,"%0.3f","%0.2f");
   EPD_Gauge(480,348,0,180,210,0,1,4,0.125,0,"%0.3f","Fuel");
  EPD_Gauge_draw(0);EPD_display();float i;int j;
  for (j=0;j<1;j++)
  {
    for (i=0;i<=100;i+=4) 
    { 
      EPD_Gauge_setPosition(0,i);EPD_partialUpdate();
      delay(20);
    }
    for (i=100;i>=0;i-=4) 
    { 
      EPD_Gauge_setPosition(0,i);EPD_partialUpdate();
      delay(20);
    }
  }
  EPD_Gauge_setPosition(0,60);
  EPD_display();
  delay(3000);
  EPD_fillScreen(0);
  EPD_Gauge_draw(1);
  for (j=0;j<1;j++)
  {
    for (i=0;i<=1;i+=1/8.0) 
    { 
      EPD_Gauge_setPosition(1,i);EPD_partialUpdate();
      delay(100);
    }
    for (i=1-1/8.0;i>0;i-=1/8.0) 
    { 
      EPD_Gauge_setPosition(1,i);EPD_partialUpdate();
      delay(100);
    }
  }
  EPD_Gauge_setPosition(1,0.250);
  EPD_display();
  delay(3000);
  EPD_fillScreen(1);
  EPD_Gauge_draw(2);
  EPD_Gauge_drawDanger(2,0.0,22.75,0);
  for (j=0;j<1;j++)
  {
    for (i=0;i<=1;i+=1/16.0) 
    { 
      EPD_Gauge_setPosition(2,i);EPD_partialUpdate();
    }
    for (i=1-1/16.0;i>0;i-=1/16.0) 
    { 
      EPD_Gauge_setPosition(2,i);EPD_partialUpdate();
    }
  }
   
   EPD_Gauge_setPosition(2,0.6250);
   EPD_HbarGraph_dropGauges();
   EPD_HbarGraph(16,42,720,20,0,100,10,1,0,"%0.0f","%0.0f");
// // float x,float y,int width,int height,float sval,float eval,int divisions,float increments,int color,char * fmt,char * valueFmt
   EPD_HbarGraph_draw(0);
   EPD_HbarGraph_setPosition(0,60);
   EPD_VbarGraph_dropGauges();
   EPD_VbarGraph(10,140,20,450,0,100,10,2,0,"%0.0f","%0.0f");
   EPD_VbarGraph_draw(0);
   EPD_VbarGraph_setPosition(0,60);
   EPD_VbarGraph(85,140,20,450,0,100,10,2,0,"%0.0f","%0.0f");
   EPD_VbarGraph_draw(1);
   EPD_VbarGraph_setPosition(1,30);
   EPD_VbarGraph(170,140,20,450,0,100,10,2,0,"%0.0f","%0.0f");
   EPD_VbarGraph_draw(2);
   EPD_VbarGraph_setPosition(2,90);
   EPD_VbarGraph(245,140,20,450,0,100,10,2,0,"%0.0f","%0.0f");
   EPD_VbarGraph_draw(3);
   EPD_VbarGraph(320,140,20,450,0,100,10,2,0,"%0.0f","%0.0f");
   EPD_VbarGraph_draw(4);
   EPD_VbarGraph_setPosition(3,45);
   EPD_VbarGraph_setPosition(4,45);

   EPD_display();
   for (i=0;i<100;i+=3) { EPD_VbarGraph_setPosition(0,i);EPD_VbarGraph_setPosition(1,i-30); EPD_VbarGraph_setPosition(4,i-15);
      EPD_VbarGraph_setPosition(2,i+30);EPD_VbarGraph_setPosition(3,i+3);EPD_HbarGraph_setPosition(0,i); EPD_partialUpdate(); }
   for (i=99;i>56;i-=3) { EPD_VbarGraph_setPosition(0,i);EPD_VbarGraph_setPosition(1,i-30); EPD_VbarGraph_setPosition(4,i-15);
      EPD_VbarGraph_setPosition(3,i+3);EPD_VbarGraph_setPosition(2,i+30);EPD_HbarGraph_setPosition(0,i); EPD_partialUpdate(); }
}

void loop()
{
  doLoop();
}
int main(int argc,char ** argv)
{
  setup();
  //for (;;) loop();
  //  Uncomment line above to run your loop() function forever.
  //  When running loop forever,
  //  you'll get no response to web events
  //  unless you include doLoop();
  //  in your interpreted loop() function
 
  //  if you want to edit and rerun the program do ResetESP32
  return 0;
}



