#include "picoc.h"
#ifdef TFT
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include <NTPClient.h>
#include <Hershey.h>
extern TFT_eSPI tft;
extern char buf[BUFSIZE];
extern int indx;
extern float temp,pressure,dp,rh;
extern NTPClient timeClient;
extern Hershey HF;
#define SlideDelay 1000
#define EPDMONO
//void screenDumpToFile(String fname);
/*void myfillScreen(unsigned short color)
{
  int i;
  for (i=0;i<8;i++) {tft.fillRect(0,i*30,320,30,color);yield();}
}
*/
void drawGuagePointer(int xc,int yc,int sAngle,int eAngle,int rad,int sValue,int eValue,float pos)
{
   // draw guage pointer for guage at center xc,yc with radius rad
   // guage drawn clockwise from sAngle to eAngle with corresponding Range eValue-sValue
   // posinter at position pos
   //
   //calculated pointer angle is (eAngle-sAngle)*(pos-sValue/(eValue-sValue)+sAngle
   float angle=((eAngle-sAngle)*(pos-sValue)/(eValue-sValue)+sAngle)/180*3.14159;
   tft.drawLine(xc,yc,xc+cos(angle)*(rad-6),yc+sin(angle)*(rad-6),0xf100);  
   tft.drawLine(xc+1,yc+1,xc+cos(angle)*(rad-6),yc+sin(angle)*(rad-6),0xf100);  
   tft.drawLine(xc+1,yc-1,xc+cos(angle)*(rad-6),yc+sin(angle)*(rad-6),0xf100);  
   tft.drawLine(xc-1,yc+1,xc+cos(angle)*(rad-6),yc+sin(angle)*(rad-6),0xf100);  
   tft.fillCircle(xc-1,yc-1,3,0xf100);  
}


int nlines=8;
int curline=-1;
String screenarray[9];
void toScreen(String message,int * ln)
{
  tft.setTextSize(2);
  if (curline==-1) tft.fillScreen(0);
  curline++; 
  if (message.length()>22) message=message.substring(0,22);
  tft.fillRect(0,*ln*16,strlen(message.c_str())*14.1,16,0); 
  tft.setCursor(2,*ln*16);
  int pos=message.lastIndexOf(' ');
  if (pos>0) tft.print(message.substring(0,pos).c_str()); 
  else tft.print(message.c_str());
  if (pos>0) {tft.setCursor(-35+(pos+1)*16,*ln*16);tft.setTextColor(0x07ff);tft.print(message.substring(pos+1).c_str());}
 
  screenarray[*ln]=message;
  tft.setTextColor(0xffff);
}

void drawJpeg(char *filename, int xpos, int ypos);

void drawGlabels(int ht,int min,int max,int divs)
{
  tft.setTextSize(1);
  int range=max-min;
  float linc=1.0*(303-54)/divs;
  int intvl=1.0*range/divs;
  int i;
  tft.setTextColor(0xff70);
  for (i=0;i<=divs;i++) 
     { tft.fillRect(53+i*linc+0.5,ht-5,3,5,0xf100); tft.setCursor(49+i*linc+0.5,ht-13);tft.print(min+i*intvl);}
  linc=linc/5;
  tft.setTextColor(0xffff);
  if (linc>3) {
  for (i=0;i<=divs*5;i++) 
      if (i%5!=0) tft.drawLine(54+i*linc+0.5,ht,54+i*linc+0.5,ht-2,0xffff);
  }
  tft.setTextSize(2);
}
void drawGlabels2(int ht,int min,int max,int divs)
{
  char bup[33];
  tft.setTextSize(1);
  yield();
  float range=max-min;
  float linc=(303-54)/(1.0*divs);
  float intvl=1.0*range/divs;
  int i;
  tft.setTextColor(0xff70);
  for (i=0;i<=divs;i++) 
     { tft.fillRect(54+i*linc+0.5,ht-5,3,5,0xf100); tft.setCursor(44+i*linc+0.5,ht-14);
     int vi,vd;
     vi=(min+i*intvl);
     vd=((int)((100*(min+i*intvl)))%100)/10;
     snprintf((char *)&bup,31,(char *)"%d.%01d",vi,vd);
     tft.print((char *)&bup);yield();}
  linc=linc/5;
  tft.setTextColor(0xffff);
  if (linc>3) {
  for (i=0;i<=divs*5;i++) 
      if (i%5!=0) tft.drawLine(54+i*linc+0.5,ht,54+i*linc+0.5,ht-2,0xffff);
  }
  tft.setTextSize(2);
}
int oldHnd[3][2]={-1,-1,-1,-1,-1,-1};
void drawHnd(int xc,int yc,int x1,int y1,uint color,int indx)
{
  if (oldHnd[indx][0]!=-1)
  {    
    tft.fillTriangle(xc-3,yc,oldHnd[indx][0],oldHnd[indx][1],xc+3,yc,0xffff);
    tft.fillTriangle(xc,yc-3,oldHnd[indx][0],oldHnd[indx][1],xc,yc+3,0xffff);
  }
  oldHnd[indx][0]=x1;
  oldHnd[indx][1]=y1;
  tft.fillTriangle(xc-3,yc,x1,y1,xc+3,yc,color);
  tft.fillTriangle(xc,yc-3,x1,y1,xc,yc+3,color);
  tft.fillCircle(xc,yc,5,color);
}
void weatherGuage()
{
  // Serial.println((char *)"Doing weatherGauge()");
    if (oldHnd[0][0] == -1) { tft.fillScreen(0xffff); drawJpeg((char *)"/wthr240.jpg",0,0); }
    float angle;
    int x1,y1;
    angle=((temp-20)/(80.0)*195-195)/180*3.14159;
    x1=108+cos(angle)*(60-5);
    y1=77+sin(angle)*(60-5);
    drawHnd(108,77,x1,y1,0xf100,0);
    angle=((rh-20)/(80.0)*240-120)/180*3.14159;
    x1=45+cos(angle)*(48-5);
    y1=165+sin(angle)*(48-5);
    drawHnd(45,165,x1,y1,0xf100,1);
    angle=((dp-30)/(60.0)*240-120)/180*3.14159;
    x1=135+cos(angle)*(48-5);
    y1=158+sin(angle)*(48-5);
    drawHnd(135,158,x1,y1,0xf100,2);
    tft.fillRect(.6*337+1,.6*370-(31-28)/3*.6*330+1,4,(31-28)/3*.6*328,0xffff);
    tft.fillRect(.6*337+1,.6*370-(pressure-28)/3*.6*330+1,4,(pressure-28)/3*.6*328,0xf100);
    tft.setTextColor(0xf100,0xffff);
    tft.setCursor(250,5);tft.print((char *)"Temp");
    tft.setCursor(250,25);tft.printf((char *)"%3.1f",temp);
    tft.setCursor(250,65);tft.print((char *)" RH");
    tft.setCursor(250,85);tft.printf((char *)"%3.1f",rh);
    tft.setCursor(250,125);tft.print((char *)" DP");
    tft.setCursor(250,145);tft.printf((char *)"%3.1f",dp);
    tft.setCursor(250,185);tft.print((char *)" Baro");    
    tft.setCursor(250,205);tft.printf((char *)"%3.2f",pressure);
}
/*
  Guage tempg(180,130,-195,0,100,20,100,16,1,0xffa09000,(char *)"%3.1f F");
  Guage rhg(75, 275,-120.0,120.0,80,20,100,16,1,0xffa09000,(char *)"%3.1f %%");
  Guage dewPointg(225,265,-120.0,120.0,80,30,90,12,1,0xffa09000,(char *)"%3.1f F");
  VBarGraph barg(337,370,330,10,28,31,6,(float)0.1,(char *)"%4.2f in Hg",0xffa09000); 
 */
void graphWeather()
{
     tft.setTextSize(2);
     int hr=timeClient.getHours();
     if (hr>12) hr=hr-12;
     if (hr==0) hr=12;
     int min=timeClient.getMinutes();yield();
     tft.fillRect(0,78,303,87,0);
     drawGlabels(78,60,90,6);yield();
     tft.setCursor(0,78);tft.print((char *)"Temp");
     tft.setCursor(0,118);tft.print((char *)"  RH");
     tft.setCursor(0,158);tft.print((char *)"  DP");
     tft.setCursor(0,198);tft.print((char *)"Baro");
     tft.setTextColor(0xffff);yield();
     tft.drawRect(54,78,303-54,18,0xf100);
     long bh=(303-54)*(temp-60.00)/30.0;
     tft.fillRect(55,120,303-57,14,0x0); 
     if (bh>0) tft.fillRect(55,80,bh,14,0x07C0);
     drawGlabels(118,20,90,7);yield();
     tft.drawRect(54,118,303-54,18,0xf100);
     bh=(303-54)*(rh-20.00)/70.0;
     tft.fillRect(55,120,303-57,14,0x0); 
     if (bh>0) tft.fillRect(55,120,bh,14,0x07C0);
     drawGlabels(158,30,80,10);yield();
     tft.drawRect(54,158,303-54,18,0xf100);
     bh=(303-54)*(dp-30.00)/50.0;
     tft.fillRect(55,160,303-57,14,0x0); 
     if (bh>0) tft.fillRect(55,160,bh,14,0x07C0);     
     tft.setTextSize(2);
     drawGlabels2(198,29,31,4);yield();
     tft.drawRect(54,198,303-54,18,0xf100);
     bh=(303-54)*(pressure-29.00)/2.0;
     tft.fillRect(55,200,303-57,14,0x0); 
     if (bh>0) tft.fillRect(55,200,bh,14,0x07C0); 
     snprintf((char *)&buf,640,(char *)"%2d:%02d",hr,min);yield();
     HF.drawStringOpaque(190,30,1.33,0xf81f,0,(char *)buf);
     tft.setTextColor(0xffff);   
     tft.setTextSize(2);
}



void TFTFileDemo()
{
	  tft.fillScreen(0xffff);
  drawJpeg((char *)"/wthr240.jpg",40,0);
/*
  Guage tempg(108,78,-195,0,60,20,100,8,1,0xffa09000,(char *)"%3.1f F");
  Guage rhg(45, 165,-120.0,120.0,48,20,100,8,1,0xffa09000,(char *)"%3.1f %%");
  Guage dewPointg(135,159,-120.0,120.0,48,30,90,12,1,0xffa09000,(char *)"%3.1f F");
 */
  drawGuagePointer(148,78,-195,0,60,20,100,60);
  drawGuagePointer(85, 165,-120.0,120.0,48,20,100,60);
  drawGuagePointer(175,159,-120.0,120.0,48,30,90,60);
  tft.fillRect(243,122,4,100,0xf800);
  //screenDumpToFile((char *)"test.raw");
  screenServer();
//  while (!Serial.available()) yield();
  Serial.readBytes((char *)&buf,100);
  tft.fillScreen(0xffff);
  drawJpeg((char *)"/misLogo.jpg",0,0);
  screenServer();
  drawJpeg((char *)"/weMoLogo.jpg",0,0);
  screenServer();
  drawJpeg((char *)"/arduinoi.jpg",0,0);
  screenServer();
  tft.fillScreen(0xffff);
  drawJpeg((char *)"/mini.jpg",0,0);
  screenServer();
  tft.fillScreen(0xffff);
  drawJpeg((char *)"/ESP32.jpg",65,0);
  screenServer();
  tft.fillScreen(0xffff);
}
void FontShow()
{
  unsigned char i=0;  
  float a;
  char * hr[12]={"12",(char *)"1",(char *)"2",(char *)"3",(char *)"4",(char *)"5",(char *)"6",(char *)"7",(char *)"8",(char *)"9",(char *)"10",(char *)"11"};
  tft.fillScreen(0);
  tft.drawCircle(160,120,75,0xf100);
  float an;
  for (a=90;(a-90)<360;a+=30.0) {an=a/360.0*2*3.14159;yield();tft.drawLine(160+sin(an)*70,120-cos(an)*70,160+sin(an)*77,120-cos(an)*77,0xf100);
  HF.drawStringRotated(160,120,90,0.75,0xffff,a,hr[i++]);yield();}
  tft.drawLine(160,120,110,70,0xf100);tft.fillCircle(160,120,3,0xf100);
  tft.drawLine(160,120,206,80,0xf100);
  screenServer();
 // while (!Serial.available()) yield();
  Serial.readBytes((char *)&buf,100);
  tft.fillScreen(0);  
  HF.drawStringRotated(60,100,90,1.5,0xffff,135,(char *)"Hi");
  HF.drawStringRotated(260,220,90,1.5,0xffff,45,(char *)"Hi");
  HF.drawCenteredString(100,1.0,0xffff,(char *)"World");
#ifdef CURSIVE
  HF.setFont((char *)"cursive");
  HF.drawCenteredString(200,1,0xffff,(char *)"Hello World");
  screenServer();
#endif
#ifdef SERIF
  tft.fillScreen(0x001f);
  HF.setFont((char *)"serif");
  HF.drawStringOpaque(1,30,1,0x0,0xffff,(char *)"Black On White???");
  HF.drawStringOpaque(1,65,1,0xffff,0x0,(char *)"White On Black???");
  HF.drawStringOpaque(10,100,0.75,0x07E0,0xf100,(char *)"Christmas Anyone???");
  HF.setFont((char *)"cursive");
  int wd=HF.getStringSize((char *)"Hello World",1);
  HF.drawStringOpaque(160-wd/2,150,1,0xffff,0x000c,(char *)"Hello World");
 // while (!Serial.available())
  screenServer();
  tft.fillScreen(0xffff);
  HF.setCursor(10,20);
  HF.setFont((char *)"serif");
  for (i=32;i<127;i++) { HF.draw(i,1.0,0);}
  HF.drawCenteredString(215,1,0xf100,(char *)"     Serif Font");
  screenServer();
#endif
#ifdef SANS
  tft.fillScreen(0xffff);
  HF.setFont((char *)"sans");
  HF.setCursor(10,20);
  for (i=32;i<127;i++) { HF.draw(i,1.0,0);}
  HF.drawCenteredString(215,1,0xf100,(char *)"    Sans Font");
  screenServer();
#endif
#ifdef SANSB
  tft.fillScreen(0xffff);
  HF.setFont((char *)"sansbold");
  HF.setCursor(10,20);
  for (i=32;i<127;i++) { HF.draw(i,1.0,0);}
  HF.drawCenteredString(215,1,0xf100,(char *)"    Sans Font Bold");
  screenServer();
#endif
#ifdef GREEK
  tft.fillScreen(0xffff);
  HF.setFont((char *)"greek");
  HF.setCursor(10,20);
  for (i=32;i<127;i++) { HF.draw(i,1.0,0);}
  HF.setFont((char *)"serif");
  HF.drawCenteredString(215,1,0xf100,(char *)"Greek Font");
  screenServer();
#endif
#ifdef CURSIVE
  tft.fillScreen(0xffff);
  HF.setFont((char *)"cursive");
  HF.setCursor(10,20);
  for (i=32;i<127;i++) { HF.draw(i,1.0,0);}
  HF.drawCenteredString(215,1,0xf100,(char *)"Cursive Font");
  screenServer();
#endif
  HF.setFont((char *)"serif");
  tft.fillScreen(0xffff);
  HF.setCursor(10,20);
  for (i=32;i<'H';i++) { HF.draw(i,1.5,0);}
  HF.drawCenteredString(215,1,0xf100,(char *)"Serif at Size=1.5");  
  screenServer();
  tft.fillScreen(0xffff);
  HF.setCursor(10,20);
  HF.setFont((char *)"sans");
  HF.drawString(10,20,0.5,0,(char *)"Drawing at 20 strokes per second "); 
  HF.setFont((char *)"cursive");
  HF.setCursor(10,40);
  HF.setStrokeDelay(50);
  for (i=32;i<122;i++) { HF.draw(i,1,0);}
  HF.setStrokeDelay(0);
  HF.drawCenteredString(215,0.75,0xf100,(char *)"Cursive at Size=1.0"); 
  delay(SlideDelay);
  HF.setFont((char *)"serif");
  tft.fillScreen(0xffff);
  HF.setCursor(10,20);
  HF.setStrokeDelay(0);
  for (i=32;i<127;i++) { HF.draw(i,0.5,0);}
  HF.drawCenteredString(215,1,0xf100,(char *)"Serif at Size=0.5");
  screenServer();
  tft.fillScreen(0xffff);
  HF.setCursor(10,20);
  for (i=32;i<127;i++) { HF.draw(i,0.75,0);}  
  HF.drawCenteredString(215,1,0xf100,(char *)"Serif at Size=0.75");
  screenServer();
  tft.fillScreen(0);
  HF.setCursor(10,100);
  HF.draw('A',0.5,0xf100);delay(1);
  HF.draw('B',1.0,0xffE0);delay(1);
  HF.draw('C',1.5,0x0fff);delay(1);
  HF.draw('D',2,0xf81f);delay(1);
  HF.draw('E',2.5,0xffff);delay(1);
  HF.draw('W',3,0x70E7);delay(1);
  HF.draw('X',3.5,0x51C5);delay(1);
  screenServer();
  TFTFileDemo();
  delay(SlideDelay);
  tft.fillScreen(0xffff);
  HF.setFont((char *)"cursive");
  HF.setStrokeDelay(10);
  HF.drawString(60,115,1,0xf100,(char *)"That's All Folks");
  HF.drawString(61,116,1,0xf100,(char *)"That's All Folks");
  HF.setStrokeDelay(0);delay(500);
  HF.drawString(280,116,1,0xf100,(char *)"!");
  HF.drawString(281,115,1,0xf100,(char *)"!");
  HF.drawString(279,115,1,0xf100,(char *)"!");
  screenServer();
}
#endif //TFT
#ifdef EPDMONO

void drawGuagePointer(int xc,int yc,int sAngle,int eAngle,int rad,int sValue,int eValue,float pos)
{
   // draw guage pointer for guage at center xc,yc with radius rad
   // guage drawn clockwise from sAngle to eAngle with corresponding Range eValue-sValue
   // posinter at position pos
   //
   //calculated pointer angle is (eAngle-sAngle)*(pos-sValue/(eValue-sValue)+sAngle
   float angle=((eAngle-sAngle)*(pos-sValue)/(eValue-sValue)+sAngle)/180*3.14159;
   display.drawLine(xc,yc,xc+cos(angle)*(rad-6),yc+sin(angle)*(rad-6),1);  
   display.drawLine(xc+1,yc+1,xc+cos(angle)*(rad-6),yc+sin(angle)*(rad-6),1);  
   display.drawLine(xc+1,yc-1,xc+cos(angle)*(rad-6),yc+sin(angle)*(rad-6),1);  
   display.drawLine(xc-1,yc+1,xc+cos(angle)*(rad-6),yc+sin(angle)*(rad-6),1);  
   display.fillCircle(xc-1,yc-1,3,1);  
}


int nlines=8;
int curline=-1;
String screenarray[9];
void toScreen(String message,int * ln)
{
  display.setTextSize(2);
  if (curline==-1) display.fillScreen(0);
  curline++; 
  if (message.length()>22) message=message.substring(0,22);
  display.fillRect(0,*ln*16,strlen(message.c_str())*14.1,16,0); 
  display.setCursor(2,*ln*16);
  int pos=message.lastIndexOf(' ');
  if (pos>0) display.print(message.substring(0,pos).c_str()); 
  else display.print(message.c_str());
  if (pos>0) {display.setCursor(-35+(pos+1)*16,*ln*16);display.setTextColor(0x07ff);display.print(message.substring(pos+1).c_str());}
 
  screenarray[*ln]=message;
  display.setTextColor(1);
}

void drawGlabels(int ht,int min,int max,int divs)
{
  display.setTextSize(1);
  int range=max-min;
  float linc=1.0*(303-54)/divs;
  int intvl=1.0*range/divs;
  int i;
  display.setTextColor(1);
  for (i=0;i<=divs;i++) 
     { display.fillRect(53+i*linc+0.5,ht-5,3,5,1); display.setCursor(49+i*linc+0.5,ht-13);display.print(min+i*intvl);}
  linc=linc/5;
  display.setTextColor(1);
  if (linc>3) {
  for (i=0;i<=divs*5;i++) 
      if (i%5!=0) display.drawLine(54+i*linc+0.5,ht,54+i*linc+0.5,ht-2,1);
  }
  display.setTextSize(2);
}
void drawGlabels2(int ht,int min,int max,int divs)
{
  char bup[33];
  display.setTextSize(1);
  yield();
  float range=max-min;
  float linc=(303-54)/(1.0*divs);
  float intvl=1.0*range/divs;
  int i;
  display.setTextColor(0xff70);
  for (i=0;i<=divs;i++) 
     { display.fillRect(54+i*linc+0.5,ht-5,3,5,0); display.setCursor(44+i*linc+0.5,ht-14);
     int vi,vd;
     vi=(min+i*intvl);
     vd=((int)((100*(min+i*intvl)))%100)/10;
     snprintf((char *)&bup,31,(char *)"%d.%01d",vi,vd);
     display.print((char *)&bup);yield();}
  linc=linc/5;
  display.setTextColor(1);
  if (linc>3) {
  for (i=0;i<=divs*5;i++) 
      if (i%5!=0) display.drawLine(54+i*linc+0.5,ht,54+i*linc+0.5,ht-2,1);
  }
  display.setTextSize(2);
}
int oldHnd[3][2]={-1,-1,-1,-1,-1,-1};
void drawHnd(int xc,int yc,int x1,int y1,uint color,int indx)
{
  if (oldHnd[indx][0]!=-1)
  {    
    display.fillTriangle(xc-3,yc,oldHnd[indx][0],oldHnd[indx][1],xc+3,yc,0);
    display.fillTriangle(xc,yc-3,oldHnd[indx][0],oldHnd[indx][1],xc,yc+3,0);
  }
  oldHnd[indx][0]=x1;
  oldHnd[indx][1]=y1;
  display.fillTriangle(xc-3,yc,x1,y1,xc+3,yc,1);
  display.fillTriangle(xc,yc-3,x1,y1,xc,yc+3,1);
  display.fillCircle(xc,yc,5,1);
}


#endif
