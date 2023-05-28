void setup()
{
   EPD_setFont(2);
   EPD_fillScreen(0);
   EPD_setTextSize(1);
   EPD_doPartial(0);
   EPD_drawCircle(100,100,50,1); 
   EPD_fillCircle(100,300,50,1);
   EPD_setCursor(40,170);
   EPD_print("draw Circle");
   EPD_setCursor(40,370);
   EPD_print("fill Circle");  
   EPD_drawRect(240,50,100,100,1);
   EPD_fillRect(240,250,100,100,1);
   EPD_setCursor(240,170);
   EPD_print("draw Rect");
   EPD_setCursor(240,370);
   EPD_print("fill Rect"); 
   EPD_drawRoundRect(430,50,100,100,10,1);
   EPD_fillRoundRect(430,250,100,100,10,1);
   EPD_setCursor(410,170);
   EPD_print("draw Round Rect");
   EPD_setCursor(410,370);
   EPD_print("fill Round Rect"); 
   EPD_drawTriangle(630,50,730,50,730,140,1);
   EPD_fillTriangle(630,250,730,250,730,340,1);
   EPD_setCursor(610,170);
   EPD_print("draw Triangle");
   EPD_setCursor(610,370);
   EPD_print("fill Triangle");
   EPD_drawLine(30,450,170,450,1);
   EPD_drawLine(40,470,160,470,1);
   EPD_drawLine(50,490,150,490,1);
   EPD_drawLine(60,510,140,510,1);
   EPD_drawLine(70,530,130,530,1);
   EPD_setCursor(50,560);
   EPD_print("draw Lines");
   EPD_drawThickLine(230,450,370,450,1,10);
   EPD_drawThickLine(240,470,360,470,1,8);
   EPD_drawThickLine(250,490,350,490,1,6);
   EPD_drawThickLine(260,510,340,510,1,4);
   EPD_drawThickLine(270,530,330,530,1,2);
   EPD_setCursor(220,560);
   EPD_print("draw Thick Lines");
   EPD_setTextSize(2);
   EPD_setCursor(440,490);
   EPD_print("Bigger Text");
   EPD_setTextSize(3);
   EPD_setCursor(370,540);
   EPD_print("Biggest Text!"); 
   EPD_display();
   Sleep(10000);
   EPD_fillScreen(0);
   EPD_setCursor(2,10);
   EPD_setTextSize(2);
   EPD_setFont(0);
   EPD_print("Native Font size 2\n");EPD_print("\n");
   EPD_setFont(1);EPD_setTextSize(1);                         
   EPD_print("FreeMonoBold 9pt Font Size 1");EPD_setTextSize(2);
   EPD_print("\nFreeMonoBold 9pt Font Size 2\n");  EPD_setFont(2);
   EPD_setTextSize(1);EPD_print("FreeSansBold 9pt Font Size 1\n\n");
   EPD_setTextSize(2);
   EPD_setFont(3);EPD_print("FreeSansBold 9pt Font Size 2");  EPD_setFont(3);EPD_setTextSize(1);
   EPD_print("\n\nFreeSerifBold 9pt Font Size 1\n\n");
   EPD_setTextSize(2);EPD_print("FreeSansBold 9pt Font Size 2\n\n");
   EPD_setFont(4);  EPD_setTextSize(1);
   EPD_print("FreeMonoBold 24pt Font");
   EPD_setFont(1);EPD_print("\n");EPD_setFont(5);
   EPD_setFont(5);EPD_print("\n");  
   EPD_print("FreeSansBold 24pt Font");
   EPD_setFont(1);EPD_print("\n");
   EPD_setFont(6);EPD_print("\n");
   EPD_print("FreeSerifBold 24pt Font");
   EPD_setFont(7);EPD_setTextSize(2);
   EPD_print("\n\n\n\n   TomThumb Font size 2");
   EPD_display();

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