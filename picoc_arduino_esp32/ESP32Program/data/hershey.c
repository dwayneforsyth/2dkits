int main(int argc,char ** argv)
{
  int i;
  char buf[60];
  TFT_fillScreen(0xffff);
  HFsetFont("sansbold");
  HFdrawCenteredString(80,1.0,0xf100,"Hello World");
  delay(1000);
  HFdrawCenteredString(110,1.0,0x07c0,"Hello World");
  delay(1000);
  HFdrawCenteredString(140,1.0,0x001f,"Hello World");
  delay(1000);
  HFdrawStringOpaque(50,170,1.0,0xffff,0,"Hello World !");
  delay(2000);
  scroll();
  delay(1000);
  scroll();
  delay(1000);
  TFT_fillScreen(0xffff);TFT_setTextSize(1);
  for (i=1;i<30;i++)
  {
    sprintf((char *)&buf,"Console line %2d is output here\n",i);
    consoleOut((char *)&buf);
  }
  TFT_setTextColor(0xffff,0);
  consoleOut("Console has 30 lines by 53 characters for listings !!\n");
  TFT_setTextColor(0,0xffff);
  consoleOut("This is a very long line of console output that will extend over multiple lines but this is accommodated by consoleOut and sufficient screen space will be created\n");
  TFT_setTextColor(0,0xffff);
  delay(10000);
  TFT_fillScreen(0xffff);
  HFdrawCenteredString(120,1.0,0,"Program Finished");
  for (i=1;i<5;i++) { TFT_invertDisplay(i%2); delay(500); }
  delay(2000);  
  HFdrawStringOpaque(30,150,0.60,0xf100,0xffff,"Program is REALLY Finished");
  delay(300);   
  HFdrawStringOpaque(30,150,0.60,0xffff,0xf100,"Program is REALLY Finished");
  delay(300);   
  HFdrawStringOpaque(30,150,0.60,0xf100,0xffff,"Program is REALLY Finished");
  delay(300);   
  HFdrawStringOpaque(30,150,0.60,0xffff,0xf100,"Program is REALLY Finished");
  delay(300);   
  HFdrawStringOpaque(30,150,0.60,0xf100,0xffff,"Program is REALLY Finished");
  delay(300); 
  return 1;
 }
