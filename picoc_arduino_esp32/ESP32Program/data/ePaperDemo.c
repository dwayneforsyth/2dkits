void setup();
void loop();
char buf[20];
int main(int argc,char ** argv)
{
  setup();
  //for (;;) loop();
  // Uncomment to run loop forever.
  //  When running loop forever,
  //  you'll get no response to web events
  return 0;
}
int vertOffset=20;
void formatPrint(int x,int y,char * fmt,float f)
{
  sprintf((char *)&buf,fmt,f);
  ePaper_setCursor(x,y+vertOffset);
  ePaper_println((char *)&buf);
}
void gauge(int x,int y,int w, int h,int bw,int divs,float min,float max,char * frmt)
{
  char gbuf[10];int i;
  ePaper_drawRect(x,y+vertOffset,w+1,h,0);
  ePaper_fillRect(x,y+vertOffset,bw,h,0);  
  for (i=1;i<=(divs*5);i++) 
     ePaper_drawLine(i*w/(divs*5),y+vertOffset,i*w/(divs*5),y+vertOffset+h+3,0);
  for(i=0;i<divs;i++) 
     ePaper_drawLine(1.0*i*w/divs,y+vertOffset,i*w/(divs*5),y+vertOffset+h+6,0);
  for (i=0;i<=divs;i++)
  {
    int hh=y+28;
    int l=-12+i*198/divs;
    if (l<0) l=1;
    float val=min+i*(max-min)/(divs);
    sprintf((char *)&gbuf,frmt,val);
    if (l>200-strlen((char *)&gbuf)*12) l=199-strlen((char *)&gbuf)*12;
    formatPrint(l,hh,frmt,val);   
  }
}
void setup()
{
	ePaper_init();
	ePaper_fillRect(0,0,200,200,0xffff);
	ePaper_setTextColor(0xffff);
	ePaper_setCursor(20,80);
	ePaper_fillRect(20,68,130,20,0);
	ePaper_println("Hello World");
	ePaper_setTextColor(0);
	ePaper_setCursor(20,110);
	ePaper_drawRect(20,95,130,20,0);
	ePaper_println("Hello World");
	ePaper_update();
	delay(5000);
	ePaper_fillRect(0,0,200,200,0xffff);
	ePaper_update();
	float temp=77.3,rh=55.2,dp=69,pressure=30.08;
	ePaper_setTextColor(0);
	sprintf((char *)&buf,"Temp %0.1fF RH %0.0f%%", temp,rh); 
	ePaper_setCursor(0,20);ePaper_println((char *)&buf);
	sprintf((char *)&buf,"DP %0.0f F %0.2f inHg",dp,pressure);ePaper_println((char *)&buf);
	int tl,th;
	tl=65.0;th=85.0;
	int bw=2+(temp-tl)*198/20;
	gauge(0,30,198,10,bw,4,65,85,"%0.0f");
	tl=40;
	th=60;
	bw=2+(rh-tl)*198/20;
	gauge(0,64,198,10,bw,4,40,60,"%0.0f"); 
	tl=50.0;
	th=80.0;
	bw=2+(dp-tl)*198/30;
	gauge(0,98,198,10,bw,3,50,80,"%0.0f");
	tl=280.0;pressure*10-1;
	th=310.0;
	bw=2+(pressure*10-tl)*198/30;
	gauge(0,98+34,198,10,bw,3,28,31,"%0.0f"); 
	ePaper_update();  
    ePaper_powerDown();
   
}

void loop()
{

}