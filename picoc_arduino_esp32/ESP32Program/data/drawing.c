void main()
{
  int i;
  //Debug();
  TFT_fillScreen(0xffff);
  TFT_setTextSize(2);
  TFT_setCursor(160,20);
  TFT_setTextColor(0xf100,0xffff);
  TFT_print("Hello World");
  delay(1000);
  TFT_fillScreen(0xffff);
  TFT_setTextSize(3);
  TFT_setCursor(100,120);
  TFT_print("Hello World");
  delay(1000);
  TFT_drawLine(10,10,310,230,0x1f);
  TFT_drawLine(10,230,310,10,0x1f);
  delay(1000);
  TFT_fillScreen(0xffff);
  TFT_drawRect(10,10,300,220,0);
  delay(1000);
  TFT_fillRect(10,10,300,220,0);
  TFT_fillCircle(160,120,100,0xffff);
  TFT_drawCircle(160,120,93,0xf100);
  TFT_fillCircle(160,120,85,0xf100);
  TFT_drawCircle(160,120,78,0x001f);
  TFT_fillCircle(160,120,70,0x001f);
  TFT_drawCircle(160,120,63,0x07c0);
  TFT_fillCircle(160,120,55,0x07c0);
  TFT_drawCircle(160,120,48,0);
  TFT_fillCircle(160,120,40,0);
  TFT_drawCircle(160,120,33,0xffff);
  TFT_fillCircle(160,120,25,0xffff);
  TFT_drawCircle(160,120,18,0xf100);
  TFT_fillCircle(160,120,10,0xf100);
  TFT_fillCircle(160,120,2,0xffff);
  screenCapture();
  TFT_fillScreen(0xf100);
  for (i=0;i<120;i+=5)
  {
    TFT_drawRect(5+i,5+i,310-i*2,230-i*2,0xffff);
  }  
  screenCapture();
  TFT_fillScreen(0xf100);
  for (i=0;i<110;i+=5)
  {
    TFT_drawEllipse(5+i,5+i,310-i*2,230-i*2,0xffff);
  }
  screenCapture();
  TFT_fillScreen(0xf100);
  for (i=0;i<110;i+=5)
  {
    TFT_drawRoundRect(5+i,5+i,310-i*2,230-i*2,12,0xffff);
  }
  screenCapture();
  char buf[100];
  sprintf((char *)&buf,"sin pi/4 is %0.6f\n",sin(3.14159267/4.0));
  TFT_setTextColor(0,0xffff);
  TFT_setCursor(10,40);
  TFT_setTextSize(2);
  TFT_print((char *)&buf);
  sprintf((char *)&buf,"sin(pi/4)^2 is %0.6f\n",pow(sin(3.14159267/4.0),2));
  TFT_setCursor(10,140);
  TFT_print((char *)&buf);
  screenCapture();
  fontShow();

}
