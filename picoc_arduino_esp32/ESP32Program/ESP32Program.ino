#define ALWAYS_STATION
// define ALWAYS_STATION to restart EPS32 if it can't connect to the specified SSID
//                       when defined it will not enter AP mode on a Station Connect
//                       failure. It will restart until it successfully connects
//                       to the SSID specified in /data/WIFIname.dat using the
//                         password specified in /data/WIFIpass.dat
//#define TFT
// define TFT to enable use of an attached 320x240 TFT Display
//#define SSD1306OLED
// define SSD1306OLED if using the Wemos or Heltec ESP32 WROOM with OLED
//#define BME280
// defining BME 280 will include support for BME280 sensor
//#define NEO_PIXEL
// defining NEO_PIXEL includes Adafruit_NeoPixel support functions;
//#define ePAPER
// for WaveShape  ePAPER 1.54
#ifdef ESP32
#ifdef SSD1306OLED
#define BUILTIN_LED 25
#else
#define BUILTIN_LED 2
#endif
#endif // ESP32

#ifdef OTA
#include <ArduinoOTA.h>
#endif
#ifdef ePAPER
#include <GxEPD.h>
#include <GxGDEP015OC1/GxGDEP015OC1.h>    // 1.54" b/w
extern GxGDEP015OC1 display;    // 1.54" b/w display; //(io, -1, -1/*RST=D4*/ /*BUSY=D2*/); // default selection of D4(=2), D2(=4)
#endif

#define FS_NO_GLOBALS


#define EPDMONO
#ifdef EPDMONO
#define NOsdFAT
#include <Inkplate.h>            //Include Inkplate library in the sketch
Inkplate display(INKPLATE_1BIT); //Create object on Inkplate library and set library to work in black/white mode (1-bit)
#include <Fonts/FreeMonoBold9pt7b.h> // use mono bold 9pt font with Inkplate
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSerifBold9pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSerifBold24pt7b.h>
#include <Fonts/TomThumb.h>

GFXfont * fonts[8]={0,(GFXfont *)&FreeMonoBold9pt7b,(GFXfont *)&FreeSansBold9pt7b,(GFXfont *)&FreeSerifBold9pt7b,(GFXfont *)&FreeMonoBold24pt7b,(GFXfont *)&FreeSansBold24pt7b,(GFXfont *)&FreeSerifBold24pt7b,(GFXfont *)&TomThumb};



#endif

#define DBG_OUTPUT_PORT Serial
/*

   portions of the code are based upon the ESPBasic Arduino sketch developed by Michael Molinari
   and modified by Rick Lunglhofer for the ESP32 and to replace the Basic Interpreter with a C
   language interpreter along with enhancement to support additional displays and sensors.

   for ESP8266Basic
  The MIT License (MIT)

  Copyright (c) 2015 Michael Molinari
            (c) 2019 Rick Lunglhofer


  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  Significant speed improvements submited by cicciocb
  Uploading of bas files improvement added by cicciocb


  the C Language interpreter is based upon the picoc work of
  Zik Saleeba and Copyright (c) 2009-2011.
  The license is shown below:

  picoc is published under the "New BSD License".
  http://www.opensource.org/licenses/bsd-license.php
  Copyright (c) 2009-2011, Zik Saleeba
  All rights reserved.
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:
      Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
            Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.
            Neither the name of the Zik Saleeba nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#define DEBUGP Serial
#ifndef ESP32
#define FS_NO_GLOBALS
#include <FS.h>
#include <ESP8266WiFi.h>
#else
#include "shield.h"
#include <SPIFFS.h>
#include <WiFi.h>
#endif
#include <WiFiClient.h>
#include <WiFiServer.h>
#ifdef ESP32
#include <HTTPUpdate.h>
#endif
//#include <WiFiUdp.h>
#ifdef ESP32
#include <WebServer.h>
WebServer server(80);
#else
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);
#endif
char sendChunked = 0;
#ifdef ESP32
fs::File fsUploadFile;
#else
fs::File fsUploadFile;
#endif
#include <Wire.h>
#include <time.h>

String GetRidOfurlCharacters(String urlChars);
void LoadBasicProgramFromFlash(String fileNameForRead);
String BasicProgram(int linenum);
String programArgs;
String ProgramName;
String GlobalNames[200];
int GlobalCount = 0;
extern "C" {
#include "picoc.h"
#include "interpreter.h"
  extern char listIncludes;
  void IncludeFile(char *FileName);
  void idrop(char * what);
  void addGlobal(char * what)
  {
    GlobalNames[GlobalCount++] = what;
   // Serial.printf((char *)"Adding %s to global names at %d\n", what, GlobalCount-1);
  }
  void DeleteGlobals()
  {
    int i;
    char buf[66];
#ifdef ESP32
   fs::File fout=SPIFFS.open((char *)"/dropGlobals.h",(char *)"w");
#else
    fs::File fout=SPIFFS.open((char *)"/dropGlobals.h",(char *)"w");
#endif
    for (i = 0; i < GlobalCount; i++) 
    {
      snprintf((char *)&buf,65,(char *)"drop(\"%s\");\n",(char *)GlobalNames[i].c_str());
      fout.print((char *)&buf);
     // Serial.printf((char *)"drop(\"%s\");\n", (char *)GlobalNames[i].c_str());
    }
    fout.close();
    GlobalCount=0;
  }
  void espRestart()
  {
    //return;
    sendContent((char *)" ");
    sendContent((char *)"<meta http-equiv=\"refresh\" content=\"8; url=./filemng\" />");
    ESP.restart();
  }
  void shortEspRestart()
  {
    ESP.restart();
  }
  char * getProgramArgs()
  {
    return (char *)programArgs.c_str();
  }
  char SingleStep = 0;
  struct ParseState Parser;
  enum ParseResult Ok;
  char buf[BUFSIZE];
  extern char * Prompt;
  byte didit = 0;
  byte line_available = 0;
  char literal = 0;
  char toConsole = 0;
#ifdef TFT
  void iconsoleOut(char * what);
#endif
#ifdef EPDMONO
  void iconsoleOut(char * what);
#endif
  // send content allows server output to be sent as chunks if sendChunked==1
  void sendContent(char *it)  // send as Web Server output and to Serial - server out includes limited &nbsp; and <BR> for [CR] and 2 spaces for tabs
  {
    String hit;
    if (strlen(it) < 1) return;
    String CRLF = F("\r\n");
    int len = strlen(it);
    int i;
    for (i = 0; i < len; i++)
    {
      if (!literal || literal == 2)
      {
        if (it[i] != 13 && it[i] != ' ' && it[i] != 10 && it[i] != '\t' && it[i] != '<' && it[i] != '>' ) hit += it[i];
        //  else if (it[i]==13) hit+="<br/>";
        else if (it[i] == 10) hit += "<br/>";
        else if (it[i] == '>') {
          if (literal == 2) hit += "&gt;";
          else hit += ">";
        }
        else if (it[i] == '<') {
          if (literal == 2) hit += "&lt;";
          else hit += "<";
        }
        else if (it[i] == ' ') hit += "&nbsp;";
        else if (it[i] == '\t') hit += "&nbsp;&nbsp;";
      }
      else hit += it[i];
    }
    if (hit.length() == 0) return;
    if (sendChunked == 0) server.sendContent(hit);
    else
    {
      //Serial.println((char *)"\nsending Chunk");
      server.sendContent(String(String(hit).length(), 16) + CRLF);
      server.sendContent(String(hit) + CRLF);
    }
    if (toConsole > 0)
#if defined TFT || defined EPDMONO
      iconsoleOut((char *)it);
#else
      toConsole = toConsole;
#endif
  }

  /* get a line of interactive input */
  void sok();
  int hextoi(char i)
  {
    if (i >= 'A') i = 10 + i - 'A';
    else if (i >= 'a') i = 10 + i - 'a';
    else i = i - '0';
    return i;
  }
  void urldecode(char * src, char * dst) {
    char ch;
    int i, ii;
    for (i = 0; i < strlen(src); i++)
    {
      if (int(src[i]) == 37) {
        ii = 16 * hextoi(src[i + 1]) + hextoi(src[i + 2]);
        ch = (char) ii;
        *dst++ = ch;
        i = i + 2;
      }
      else *dst++ = src[i];
    }
    *dst = 0;
    return ;
  }
  char WebResponse = 0;
  char WebResponseBuf[400];
  char *PlatformGets(char *Buf, int MaxLen, const char *Prompt)
  {
    Buf[0] = 0;
    if (WebResponse == 1)
    {
      // line available in WebResponseBuf
      strncpy(Buf, (char *)&WebResponseBuf, MaxLen);
    }
  }
  char *PlatformGetLine(char *Buf, int MaxLen, const char *Prompt)
  {
    Buf[0] = 0;
#ifndef USE_SERIAL
    if (didit == 1) {
      didit = 0;
      sok();
    }
    if (!line_available) return 0;
    strncpy(Buf, (char *)&buf, MaxLen);
    urldecode(Buf, (char *)&buf);
    //Serial.println((char *)&buf);
    strncpy(Buf, (char *)&buf, MaxLen);
    didit = 1; line_available = 0;
    return Buf;
#else
    if (didit == 1) {
      didit = 0;
      sok();
    }
    if (!Serial.available()) {
      didit = 0;
      return 0;
    }
    didit = 1;
    int ccount = 0;
    int ch = Serial.read();
    while (ch != 13 && ch != 10 && ccount < MaxLen)
    {
      Buf[ccount++] = ch;
      ch = Serial.read();
    }
    Buf[ccount] = 0;
    //Serial.println((char *)Buf);
    return (char *)Buf;
#endif
  }
  void DebugPrintln(char * what)
  {
 // return;
    Serial.println(what);
  }
  void DebugPrintInt(int what)
  {
 //   return;
    Serial.println(what);
  }
  void sPrintln(char * what)
  {

    Serial.println(what);
  }
  void sPrintInt(int what)
  {

    Serial.println(what);
  }
  void sprint(char * what)
  {
    sendContent(what);
  }

  void ssend(char * what);


  char * PlatformReadFile(char * FileName)
  {
    //   //Serial.println(F("Entered PlatformReadFile"));
    char fbuf[33]; if (*FileName == '/') snprintf((char *)&fbuf, 32,(char *)"%s", FileName);
    else snprintf((char *)&fbuf, 32,(char *)"/%s", FileName);
    //Serial.print(F("opening file "));Serial.println((char *)&fbuf);
    if (!SPIFFS.exists((char *)&fbuf))
    {
      Serial.print(F("File ")); Serial.print((char *)&fbuf); Serial.println(F(" Does Not Exist!!"));
      return (char *)&buf;
    }
#ifdef ESP32
   fs::File fin = SPIFFS.open((char *)&fbuf,(char *)"r");
#else
    fs::File fin = SPIFFS.open((char *)&fbuf,(char *)"r");
#endif
    if (!fin) {
      Serial.print(F("file open failed for ")); Serial.println((char *)&fbuf);
      return (char *)&buf;
    }
    snprintf((char *)&fbuf, 32,(char *)", File size is %d\n", fin.size());
    //sprint((char *)&fbuf);
    int fread = fin.read((uint8_t *)&buf, BUFSIZE);
    snprintf((char *)&fbuf, 32,(char *)"read %d bytes from file\n", fread);
    //Serial.printf((char *)&fbuf);
    fin.close();
    buf[fread] = 0;
    //Serial.print((char *)&buf);
    return (char *)&buf;
  }

  /* get a character of interactive input */
  int PlatformGetCharacter()
  {
    int rd = Serial.read();;
    return rd;
  }
  void printLines(char * src);
  void sendc(char it);
  /* write a character to the console */
  void PlatformPutc(unsigned char OutCh, FILE * Stream)
  {
    sendc(OutCh);
  }
  extern int nwatches;


  void sok()
  {

    //Serial.println((char *)"End Command");//server.sendContent((char *)"");
    if (Prompt != 0) sendContent(Prompt);
    delay(50);
    if (Prompt != 0) Serial.print(Prompt);
  }

  void sendEOF()
  {
    server.sendContent(F("0\r\n\r\n"));
  }
  void send(char * it)
  {
    sendContent(it);
    //DebugP.printf((char *)"send output %s\n",it);
    //if (toConsole==1) iconsoleOut(it);
  }
  int RunningProgram = 0;
  void Ssend(char * it)
  {
    Serial.print(it);
  }
  void sendc(char it)
  {
    char bf[2];
    bf[0] = it; bf[1] = 0;
    sendContent((char *)&bf);
    //if (toConsole==1) iconsoleOut((char *)&bf);
  }
  void sendln(char * it)
  {
    char B[200];
    sprintf((char *)&B,(char *)"%s\n", it);
    sendContent((char *)&B);
  }
  char fileExists(char * fileName)
  {
    char exists = 0;
    char fname[34];
    if (*fileName != '/') {
      fname[0] = '/';
      fname[1] = 0;
      strncat((char *)&fname, fileName, 33);
    }
    else strncpy((char *)&fname, fileName, 33);
#ifdef ESP32
   fs::File f = SPIFFS.open((char *)&fname,(char *)"r");
#else
    fs::File f = SPIFFS.open((char *)&fname,(char *)"r");
#endif
    if (f > 0) {
      exists = 1;
      f.close();
    }
    return exists;
  }

} // end extern "C"

void ssend(char * what)
{
  if (strlen(what) == 0) return;
  if (sendChunked == 0)
  {
    server.sendContent(what);
    //Serial.print(what);
  }
  else
  {
    //Serial.print((char *)"\nSending Chunk");
    //Serial.print(what);
    String hit = what;
    String CRLF = F("\r\n");
    server.sendContent(String(String(hit).length(), 16) + CRLF);
    server.sendContent(String(hit) + CRLF);
  }
}
#ifdef TFT
#include <TFT_eSPI.h> // Hardware-specific library
#include <JPEGDecoder.h>
#include <Hershey.h>
TFT_eSPI tft = TFT_eSPI();
#define GFXFF 1
#define FF18 &FreeMonoBold9pt7b
#define CF_Y32  &Yellowtail_32
boolean screenServer(void);
boolean screenServer(String FileName);
Hershey HF((char *)"serif");
#include <Fonts/FreeMonoBold9pt7b.h> // use mono bold 9pt font with Inkplate
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSerifBold9pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSerifBold24pt7b.h>
#include <Fonts/TomThumb.h>

GFXfont * fonts[8]={0,(GFXfont *)&FreeMonoBold9pt7b,(GFXfont *)&FreeSansBold9pt7b,(GFXfont *)&FreeSerifBold9pt7b,(GFXfont *)&FreeMonoBold24pt7b,(GFXfont *)&FreeSansBold24pt7b,(GFXfont *)&FreeSerifBold24pt7b,(GFXfont *)&TomThumb};

#define minimum(a,b)     (((a) < (b)) ? (a) : (b))
void jpegInfo() {
 // return;  // delete return to show jpegInfo on Serial port
 // Serial.println((char *)"===============");
 // Serial.println("JPEG image info");
 // Serial.println("===============");
  Serial.print  ((char *)"Width      :"); Serial.println(JpegDec.width);
  Serial.print  ((char *)"Height     :"); Serial.println(JpegDec.height);
  Serial.print  ((char *)"Components :"); Serial.println(JpegDec.comps);
  Serial.print  ((char *)"MCU / row  :"); Serial.println(JpegDec.MCUSPerRow);
  Serial.print  ((char *)"MCU / col  :"); Serial.println(JpegDec.MCUSPerCol);
  Serial.print  ((char *)"Scan type  :"); Serial.println(JpegDec.scanType);
  Serial.print  ((char *)"MCU width  :"); Serial.println(JpegDec.MCUWidth);
  Serial.print  ((char *)"MCU height :"); Serial.println(JpegDec.MCUHeight);
 // Serial.println((char *)"===============");
 // Serial.println((char *)"");
}
//====================================================================================
//   Opens the image file and prime the Jpeg decoder
//====================================================================================
void drawJpeg(char *filename, int xpos, int ypos) {

  // Serial.println(F("==========================="));
  //Serial.print(F("Drawing file: ")); //Serial.println(filename);
  // Serial.println(F("==========================="));

  // Open the named file (the Jpeg decoder library will close it after rendering image)
 fs::File jpegFile = SPIFFS.open( filename,(char *)"r");    // File handle reference for SPIFFS
  //  File jpegFile = SD.open( filename, FILE_READ);  // or, file handle reference for SD library

  if ( !jpegFile ) {
    Serial.print(F("ERROR: File \"")); Serial.print(filename); Serial.println (F("\" not found!"));
    strncpy((char *)&buf, filename, BUFSIZE - 1);
    strncat((char *)&buf,(char *)" not found", BUFSIZE - 1);
    Serial.println((char *)buf);
    return;
  }

  // Use one of the three following methods to initialise the decoder:
  boolean decoded = JpegDec.decodeFsFile(jpegFile); // Pass a SPIFFS file handle to the decoder,
  //boolean decoded = JpegDec.decodeSdFile(jpegFile); // or pass the SD file handle to the decoder,
 // boolean decoded = JpegDec.decodeFsFile(filename);  // or pass the filename (leading / distinguishes SPIFFS files)
  // Note: the filename can be a String or character array type
  if (decoded) {
    // print information about the image to the serial port
 //   jpegInfo();

    // render the image onto the screen at given coordinates
    jpegRender(xpos, ypos);
  }
  else {
    Serial.println(F("Jpeg file format not supported!"));
  }
  strncpy((char *)&buf,(char *)"Processed ", BUFSIZE - 1);
  strncat((char *)&buf, filename, BUFSIZE - 1);
  //Serial.println((char *)&buf);
}

//====================================================================================
//   Decode and render the Jpeg image onto the TFT screen
//====================================================================================
void jpegRender(int xpos, int ypos) {

  // retrieve infomration about the image
  uint16_t  *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;

  // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
  // Typically these MCUs are 16x16 pixel blocks
  // Determine the width and height of the right and bottom edge image blocks
  uint32_t min_w = minimum(mcu_w, max_x % mcu_w);
  uint32_t min_h = minimum(mcu_h, max_y % mcu_h);

  // save the current image block size
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;

  // record the current time so we can measure how long it takes to draw an image
  uint32_t drawTime = millis();

  // save the coordinate of the right and bottom edges to assist image cropping
  // to the screen size
  max_x += xpos;
  max_y += ypos;

  // read each MCU block until there are no more
  while ( JpegDec.readSwappedBytes()) {

    // save a pointer to the image block
    pImg = JpegDec.pImage;

    // calculate where the image block should be drawn on the screen
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;
/* // using readSwappedBytes so this byte swapping code not required
    char * cImage=(char *)pImg;
    char tmpC;
    for (int i=0;i<mcu_w*mcu_h;i++)
    {
      if (*cImage!=*(cImage+1)) // not true for white and black, save a lot of time for them
      {
        tmpC=*cImage;
        *cImage=*(cImage+1);
        *(cImage+1)=tmpC;
      }
      cImage+=2;
    }
*/
    // check if the image block size needs to be changed for the right and bottom edges
    if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
    else win_w = min_w;
    if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
    else win_h = min_h;

    // calculate how many pixels must be drawn
    uint32_t mcu_pixels = win_w * win_h;

    // draw image MCU block only if it will fit on the screen
    if ( ( mcu_x + win_w) <= tft.width() && ( mcu_y + win_h) <= tft.height())
    {
      // Now set a MCU bounding window on the TFT to push pixels into (x, y, x + width - 1, y + height - 1)
//      tft.setAddrWindow(mcu_x, mcu_y, mcu_x + win_w - 1, mcu_y + win_h - 1);
      // Write all MCU pixels to the TFT window
//      while (mcu_pixels--) tft.pushColor(*pImg++);
      tft.pushRect(mcu_x, mcu_y, win_w , win_h ,pImg);
    }

    else if ( ( mcu_y + win_h) >= tft.height()) JpegDec.abort();

  }

  // calculate how long it took to draw the image
  drawTime = millis() - drawTime; // Calculate the time it took

  // print the results to the serial port
  Serial.printf((char *)"Total render time was %d ms\n",drawTime);
  // Serial.println("=====================================");

}
void myfillScreen(unsigned short color)
{
  tft.fillRect(0, 0, 320, 240, color);
}
void TFT_setup()
{
  tft.begin();
  // tft.setFreeFont(FF18);                 // Select the font
  tft.setTextSize(1);
  HF.setFont((char *)"sansbold");
  tft.setRotation(1);
  tft.fillScreen(0xffff); // while fill to TFT
  tft.setTextColor(0, 0xffff); // black on white text
}

#endif // TFT
#ifdef OLED
#include <U8g2lib.h>
// the OLED used
#ifndef WIFI_Kit_32
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8x8(U8G2_R0, /* reset=*/ -1,/* clock=*/ 4, /* data=*/ 5 ); // for WEMOS
#else
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8x8(U8G2_R0, /* reset=*/ 16,/* clock=*/ 15, /* data=*/ 4 ); // for HELTEC
#endif
#endif // OLED
#ifdef SSD1306OLED
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"
#ifndef WIFI_Kit_32
SSD1306Wire display(0x3c, 5/*SDA*/, 4/*SCL*/);  // Wemos
#else
SSD1306Wire display(0x3c, 4,15);  // HELTEC
#endif
OLEDDISPLAY_COLOR OLEDcolor;
#endif
#include <Time.h>

PROGMEM const char BasicVersion[] = "<h2>ESP32 picoc version v2.2 beta r C Interpreter</h2>";
/*   PICOC LICENSE INFO
      _________________
  Copyright
  ---------

  picoc is published under the "New BSD License".
  http://www.opensource.org/licenses/bsd-license.php


  Copyright (c) 2009-2011, Zik Saleeba
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

      Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

      Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the
      distribution.

      Neither the name of the Zik Saleeba nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  Modified for ESP32, debugging, printf enhancements, and WebServer access by Rick Lunglhofer 2019

*/
// SPI STUFF
#include <SPI.h>

//ILI9341 Stuff
#include <Adafruit_GFX.h>

// udp client
//WiFiUDP udp;

WiFiClient client;




//Web Server Variables
String HTMLout;
PROGMEM const char InputFormText[] = R"=====( <input type="text" id="myid" name="input"><input type="submit" value="Submit" name="inputButton"><hr>)=====";
PROGMEM const char TextBox[] = R"=====( <input type="text" id="myid" name="variablenumber" value="variablevalue">)=====";
PROGMEM const char passwordbox[] = R"=====( <input type="password" id="myid" name="variablenumber" value="variablevalue">)=====";
PROGMEM const char Slider[] = R"=====( <input type="range" id="myid" name="variablenumber" min="minval" max="maxval" value=variablevalue>)=====";
PROGMEM const char GOTObutton[] =  R"=====(<input type="submit" id="myid" value="gotonotext" name="gotonobranch">)=====";
PROGMEM const char GOTOimagebutton[] =  R"=====(<input type="image" id="myid" src="/file?file=gotonotext" value="gotonotext" name="gotonobranch">)=====";
PROGMEM const char normalImage[] =  R"=====(<img src="/file?file=name">)=====";
PROGMEM const char javascript[] =  R"=====(<script src="/file?file=name"></script>)=====";
PROGMEM const char CSSscript[] =  R"=====(<link rel="stylesheet" type="text/css" href="/file?file=name">)=====";
PROGMEM const char DropDownList[] =  R"=====(<select name="variablenumber" id="myid" size="theSize">options</select>
<script>document.getElementsByName((char *)"variablenumber")[0].value = (char *)"VARS|variablenumber";</script>)=====";
PROGMEM const char DropDownListOpptions[] =  R"=====(<option>item</option>)=====";

char DebugBar[] = R"=====( <meta name="viewport" class=text2" content="width=device-width, initial-scale=1.0">
<a class="buttong" href="./debugResponse?value=s">STEP</a><hr>
)=====";

PROGMEM const char MobileFreindlyWidth[] = R"=====(<meta name="viewport" class="text2" content="width=device-width, initial-scale=1.0">)=====";

PROGMEM const char AdminBarHTML[] = R"=====(
<style type="text/css">
.button {
    display: inline-block;
    text-align: center;
    vertical-align: middle;
    padding: 7px 2px;
    border: 5px solid #a12727;
    border-radius: 8px;
    background: #ff4a4a;
    background: -webkit-gradient(linear, left top, left bottom, from(#ff4a4a), to(#992727));
    background: -moz-linear-gradient(top, #ff4a4a, #992727);
    background: linear-gradient(to bottom, #ff4a4a, #992727);
    box-shadow: #ff5959 0px 0px 40px 0px;
    text-shadow: #591717 1px 1px 1px;
    font: normal normal bold 16px arial;
    color: #ffffff;
    text-decoration: none;
}
.button:hover,
.button:focus {
    border: 5px solid #ffffff;
    background: #ff5959;
    background: -webkit-gradient(linear, left top, left bottom, from(#ff5959), to(#b62f2f));
    background: -moz-linear-gradient(top, #ff5959, #b62f2f);
    background: linear-gradient(to bottom, #ff5959, #b62f2f);
    color: #ffffff;
    text-decoration: none;
}
.button:active {
    background: #982727;
    background: -webkit-gradient(linear, left top, left bottom, from(#982727), to(#982727));
    background: -moz-linear-gradient(top, #982727, #982727);
    background: linear-gradient(to bottom, #982727, #982727);
}
.buttong {
    display: inline-block;
    text-align: center;
    vertical-align: middle;
    padding: 7px 2px;
    border: 5px solid #0dfc21;
    border-radius: 8px;
    background: #4aff71;
    background: -webkit-gradient(linear, left top, left bottom, from(#4aff71), to(#7af0b9));
    background: -moz-linear-gradient(top, #4aff71, #7af0b9);
    background: linear-gradient(to bottom, #4aff71, #7af0b9);
    text-shadow: #591717 1px 1px 1px;
    font: normal normal bold 16px arial;
    color: #ffffff;
    text-decoration: none;
}
.buttong:hover,
.buttong:focus {
    border: 5px solid #ffffff;
    background: #59ff88;
    background: -webkit-gradient(linear, left top, left bottom, from(#59ff88), to(#92ffde));
    background: -moz-linear-gradient(top, #59ff88, #92ffde);
    background: linear-gradient(to bottom, #59ff88, #92ffde);
    color: #ffffff;
    text-decoration: none;
}
.buttong:active {
    background: #2c9944;
    background: -webkit-gradient(linear, left top, left bottom, from(#2c9944), to(#7af0b9));
    background: -moz-linear-gradient(top, #2c9944, #7af0b9);
    background: linear-gradient(to bottom, #2c9944, #7af0b9);
}
.text1 {
            COLOR: #FFFFFF; 
            TEXT-DECORATION: none; 
            background: #000000;
            font-family: courier; 
}
.text2 {
            COLOR: #000000; 
            TEXT-DECORATION: none; 
            background: #FFFFFF;
            font-family: courier; 
}
.text3 {
            COLOR: #000000; 
            TEXT-DECORATION: none; 
            background: #FFFFFF;
            font-family: courier;
            width: 300px; 
}
* {
      font-family: courier;
      FONT-SIZE    14px;
    }
</style>
<meta name="viewport" class="text2" content="width=device-width, initial-scale=1.0">
<form  id="topform" action="./functionHelp">
<a class="button" href="./filemng">FILE MANAGER</a>
<a class="button" href="./settings">SETTINGS</a>
<a class="button" href="./codetmpl">CODE TEMPLATE</a>
<a class="button" href="http://www.microimagesys.com/ESP32_picoc_C_Language_Interpreter.pdf"  target="_blank">HELP</a>
<input class="button" type="submit" name='funcHelp' value="picoc Function Help"> &nbsp;<input type="text" class=\"text3\" name="function" value=""> 
</form>
<hr>)=====";

PROGMEM const char DebugBarHtml[] = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<a class="button" href="./vars">VARS</a>
<a class="button" href="./setvars">SET VARIABLE</a>
<a class="buttong" href="./step">STEP</a>
<a class="buttong" href="./continue">CONTINUE</a>
<a class="button" href="./setbreakpoint">SET BREAKPOINT</a>
<a class="buttong" href="./delbreakpoint">DELETE BREAKPOINT</a>
<a class="button" href="./quitdebug">QUIT DEBUG</a>
<hr>)=====";
#ifdef TFT
PROGMEM const char UploadPage[] = R"=====(
<form method='POST' action='/edit' enctype='multipart/form-data'>
<input class="button" type='file' name='Uploads' onclick="javascript:document.getElementById('Upload').type='submit'">
<input class="buttong" type='hidden' id="Upload" name="Upload" value='Upload'>
</form>
<form id="filelist"  method='POST' action='/filemng'>
<input class="button" type="submit" value="Delete" name="Delete">
<input class="button" type="submit" value="Edit" name="Edit">
<input class="buttong" type="submit" value="Run" name="Run">
<input class="button" type="submit" value="Rename" name="Rename">
 Program Arguments <input type="text" class=\"text3\" name="programArgs" value="*ProgramArgs*">
<input class="button" type="submit" value="NewFile" name="NewFile"> 
</form>
<select name="FileName" id="FileName" onDblClick="javascript:window.open('./edit?FileName='+this.value+'&open=Open','_parent');" size="50" form="filelist">*table*</select>
)=====";

PROGMEM const char EditorPageHTML[] =  R"=====(
<script>*edit.js*</script>
<form  id="usrform"  method='POST' action='/edit'>
<input type="text" id="FileName" class="text3" name="FileName" value="*programName*">
<input class="button" type="submit" value="Open" name="open">
<input class="buttong" type="hidden" value="Save" name="saving" id="saving">
<input class="buttong" type="submit" value="Run" name="run">
 Program Arguments <input class="text3" type="text" value="*ProgramArgs*", name="programArgs">
<a class="buttong" href="./ResetESP32">Reset ESP32</a>
<input class="buttong" type="submit" name="ScreenCapture" value="Screen Capture">
<br> 
)=====";
PROGMEM const char RunEditorPageHTML[] =  R"=====(
<script>*edit.js*</script>
<form  id="usrform"  method='POST' action='/edit'>
<input type="text" id="FileName" class="text3" name="FileName" value="*programName*">
<input class=button type="submit" value="Open" name="open">
<input class="buttong" type="submit" value="Run" name="run">
 Program Arguments <input class="text3" type="text" value="*ProgramArgs*", name="programArgs">
<a class="buttong" href="./ResetESP32">Reset ESP32</a>
<input class="buttong" type="submit" name="ScreenCapture" value="Screen Capture">
<br>
)=====";
#else
PROGMEM const char UploadPage[] = R"=====(
<form method='POST' action='/filemng' enctype='multipart/form-data'>
<input class="buttong" type='file' name='Upload' onclick="javascript:document.getElementById('Upload').type='submit'">
<input class="button" type='hidden' name="Upload" id="Upload" value='Upload'>
</form>
<form id="filelist" >
<input class="button" type="submit" value="Delete" name="Delete">
<input class="button" type="submit" value="Edit" name="Edit">
<input class="buttong" type="submit" value="Run" name="Run">
<input class="button" type="submit" value="Rename" name="Rename">
 Program Arguments <input type="text" class=\"text3\" name="programArgs" value="*ProgramArgs*">
<input class="button" type="submit" value="NewFile" name="NewFile"> 
</form>
<select name="FileName" id="FileName" onDblClick="javascript:window.open('./edit?FileName='+this.value+'&open=Open','_parent');" size="50" form="filelist">*table*</select>
)=====";

PROGMEM const char EditorPageHTML[] =  R"=====(
<script>*edit.js*</script>
<form  id="usrform" method='POST' action='/edit'>
<input type="text" id="FileName" class="text3" name="FileName" value="*programName*">
<input class="button" type="submit" value="Open" name="open">
<input class="buttong" type="hidden" value="Save" name="saving" id="saving">
<input class="buttong" type="submit" value="Run" name="run">
 Program Arguments <input class="text3" type="text" value="*ProgramArgs*", name="programArgs">
<a class="buttong" href="./ResetESP32">Reset ESP32</a>
<br>
)=====";
PROGMEM const char RunEditorPageHTML[] =  R"=====(
<script>*edit.js*</script>
<form  id="usrform" method='POST' action='/edit'>
<input type="text" id="FileName" class="text3" name="FileName" value="*programName*">
<input class=button type="submit" value="Open" name="open">
<input class="buttong" type="submit" value="Run" name="run">
 Program Arguments <input class="text3" type="text" value="*ProgramArgs*", name="programArgs">
<a class="buttong" href="./ResetESP32">Reset ESP32</a>
<br>
)=====";
#endif
PROGMEM const char editCodeJavaScript[] =  R"=====(
function changeSaveButtonColor()
{
  //alert("Save Needed");
  document.getElementById("saving").type="submit";
}
function ShowTheFileList(){
  var filelist;
  filelist = httpGet("/filelist?all=true");
  alert("List of the Files saved :\n\r" + filelist);
}
function httpGet(theUrl)
{
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.open( "GET", theUrl, false ); // false for synchronous request
    //alert(theUrl);
    xmlHttp.send( null );
    return xmlHttp.responseText;
}
function replaceAll(str, find, replace) {
  for (x = 0; x <= 10; x++) 
  {
    str = str.replace(find, replace);
  }
  return str;
}
)=====";


PROGMEM const char SettingsPageHTML[] =  R"=====(
<form action='settings' id="usrform"><table>
*BasicVersion*
<tr><th>
Station Mode (Connect to your router):</th></tr>
<tr><th><p align="right">Name:</p></th><th><input type="text" name="staName" value="*sta name*"></th></tr>
<tr><th><p align="right">Pass:</p></th><th><input type="password" name="staPass" value="*sta pass*"></th></tr>
<tr><th>
<br><br>Ap mode (ESP brocast out its own ap):</th></tr>
<tr><th><p align="right">Name:</p></th><th><input type="text" name="apName" value="*ap name*"></th></tr>
<tr><th><p align="right">Pass:<br>Must be at least 8 characters</p></th><th><input type="password" name="apPass" value="*ap pass*"></th></tr>
<tr><th>
<br><br>Log In Key (For Security):</th></tr>
<tr><th><p align="right">Log In Key:</p></th><th><input type="password" name="LoginKey" value="*LoginKey*"></th></tr>
<tr><th><p align="right">Display menu bar on index page:</p></th><th><input type="checkbox" name="showMenueBar" value="off" **checked**> Disable<br></th></tr>
<tr><th><p align="right">Run default.c at startup :</p></th><th><input type="checkbox" name="autorun" value="on" **autorun**> Enable<br></th></tr>
<tr><th><p align="right">Server listening port:</p></th><th><input type="text" name="listenport" value="*listenport*"></th></tr>
<tr><th><p align="right">OTA URL. Leave blank for default:</p></th><th><input type="text" name="otaurl" value="*otaurl*"></th></tr>
<tr><th>
<input class="button" type="submit" value="Save" name="save">
<input class="button" type="submit" value="Format" name="format">
<input class="button" type="submit" value="Update" name="update">
<input class="button" type="submit" value="Restart" name="restart">
</th></tr>
</table></form>
<br>
)=====";







PROGMEM const char LogInPage[] =  R"=====(
<form action='settings' id="usrform">
Log In Key
<input type="password" name="key" value="">
<input type="submit" value="login" name="login">
</form>
)=====";


//Graphics HTML CODE

PROGMEM const char GraphicsStartCode[] =  R"=====(<svg width="*wid*" height="*hei*">)=====";

PROGMEM const char GraphicsLineCode[] =  R"=====(<line x1="*x1*" y1="*y1*" x2="*x2*" y2="*y2*" stroke="*collor*"/>)=====";

PROGMEM const char GraphicsCircleCode[] =  R"=====(<circle cx="*x1*" cy="*y1*" r="*x2*" fill="*collor*"/>)=====";

PROGMEM const char GraphicsEllipseCode[] =  R"=====(<ellipse cx="*x1*" cy="*y1*" rx="*x2*" ry="*y2*" fill="*collor*"/>)=====";

PROGMEM const char GraphicsRectangleCode[] =  R"=====(<rect x="*x1*" y="*y1*" width="*x2*" height="*y2*" style="fill:*collor*"/>)=====";

byte numberButtonInUse = 0;
String ButtonsInUse[20];


String   msgbranch;
String   MsgBranchRetrnData;

int UdpBranchLine = 0;
String  UdpBuffer = "";
IPAddress UdpRemoteIP;
int UdpRemotePort;

void sendcontent(char * it);
// place where the program will jump on serial data reception
int SerialBranchLine  = 0;
int Serial2BranchLine = 0;


// Buffer to store incoming commands from serial port
String inData;

const int TotalNumberOfLines = 5000;            //this is the maximum number of lines that can be saved/loaded; it can go until 65535
int  program_nb_lines = 0;                      //this is the number of program lines read from the file

String VarialbeLookup(String VariableNameToFind);
void PrintAndWebOut(String what);
String evaluate(String expr);
void SetMeThatVar(String VariableNameToFind, String NewContents, int format);

int LoggedIn = 0;

//time Stff
int timezone = -5;
int dst = 1;

extern  unsigned char * HeapMemory;
void TableStrFree();
bool CheckIfLoggedIn();
void SaveDataToFile(String fileNameForSave, String DataToSave);
void CreateAP(String NetworkName, String NetworkPassword);
bool ConnectToTheWIFI(String NetworkName, String NetworkPassword, String NetworkStaticIP, String NetworkGateway, String NetworkSubnet);
String getContentType(String filename);
String RunningProgramGui();
void CloseWriteOnFlash(void);
bool WriteBasicLineOnFlash(String codeLine);
bool OpenToWriteOnFlash(String fileNameForWrite);
String LoadDataFromFile(String fileNameForSave);
void handleFileUpdate();
void DoSomeFileManagerCode();
String MakeSureFileNameStartsWithSlash(String it);
String SettingsPageHandeler();
void VariableCleanup();
char DebugProgramName[33];
//ESP8266WebServer server(80);
int runOnce = 0;
int oi = 1;
char GlobalScope = 0;
char didPlines = 0;

void runProgram()
{
  String WebOut = AdminBarHTML;
  toConsole = 0;
  String CRLF = F("\r\n");
  SingleStep = 0;
  ProgramName = server.arg((char *)"FileName");
  if (ProgramName.equals((char *)"")) ProgramName = F("/default.c");
  Serial.printf((char *)"Running program %s\n",(char *)ProgramName.c_str());
  programArgs = server.arg((char *)"programArgs");
  WebOut += F("<hr/>Program to run is ");
  WebOut += ProgramName + " " + programArgs;
  WebOut += "<hr>";
  WebOut += String(RunEditorPageHTML);
  WebOut += "<textarea readonly rows=\"22\" cols=\"180\" name=\"code\" id=\"code\" style=\"FONT-SIZE:14px; COLOR:#000000; LINE-HEIGHT:14px; FONT-FAMILY:Courier\">";
  WebOut.replace((char *)"*ProgramArgs*", programArgs);
  WebOut.replace((char *)"*programName*", ProgramName);
  // Serial.println(WebOut);
  server.sendContent(F("HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nConnection: keep-alive\r\nTransfer-Encoding: chunked\r\nAccess-Control-Allow-Origin: *\r\n\r\n"));
  sendChunked = 1;
  // each "chunk" is composed of :
  // the size of the block (in hex) terminated by \r\n
  server.sendContent(String(String(WebOut).length(), 16) + CRLF);
  // the block terminated by \r\n
  server.sendContent(String(WebOut) + CRLF);
  //  Serial.print(F("freeMemory prior to PicocInitialize ="));
  uint32_t free = ESP.getFreeHeap();
  //  Serial.println(free);
  //  Serial.println(F("Starting PicocInitialize"));
  GlobalScope = 1;
  if (runOnce == 0)
  {
    PicocInitialise(PICOC_STACK_SIZE);
      Serial.print(F("After Initialize, freeMemory="));
    free = ESP.getFreeHeap();
    Serial.println(free);
    //   Serial.println(F("Doing  PicocIncludeAllSystemHeaders"));
    PicocIncludeAllSystemHeaders(1);
  }
  //   Serial.println(F("Doing  PicocIncludeAllSystemHeaders"));
  PicocIncludeAllSystemHeaders(1);
  IncludeFile((char *)"/dropGlobals.h");
  runOnce = 1;    nwatches = 0;
  didPlines = 0; listIncludes = 1;
  //Serial.println(F("Starting PlatformScanFile"));
  literal = 1; sendChunked = 1;
  //   Serial.println((char *)"Going to PicoPlatfformScanFile");
  RunningProgram = 1;
  PicocPlatformScanFile((char *)ProgramName.c_str());
  literal = 0;
  //   Serial.println(F("Done PlatformScanFile"));
  GlobalScope = 0;
  WebOut = "</table><hr>Output from main( )<hr/><table frame=\"vsides\"  rules=\"cols\" cellpadding=\"2\">";
  server.sendContent(String(String(WebOut).length(), 16) + CRLF);
  server.sendContent(String(WebOut) + CRLF);
  strncpy((char *)&DebugProgramName, (char *)ProgramName.c_str(), 33);
  //   Serial.println((char *)&DebugProgramName);
  char * theArgv[20];
  int theArgc = 1;
  int nargs = 1;
  char * prog = (char *)ProgramName.c_str();
  theArgv[0] = (char *)prog;
  char argS[20][33];
  strncpy((char *)&argS[0][0], (char *)&prog, 32);
  String temp = programArgs;
  // Serial.println(temp);
  if (programArgs.equals((char *)""))
  {
    //Serial.println("Calling main();");
    PicocCallMain(0, 0);
  }
  else
  {
    temp.trim();
    int probe;
    probe = temp.indexOf((char *)" ");
    while (probe != -1)
    {
      if (probe == 0) temp.trim();
      else // have to separate args
      {
        strncpy((char *)&argS[nargs][0], (char *)temp.substring(0, probe).c_str(), 32);
        //Serial.printf((char *)"argv[%d]=%s\n",nargs,(char *)&argS[nargs][0]);
        theArgv[nargs] = (char *)&argS[nargs][0];
        nargs++;
        //Serial.printf((char *)"theArgv[%d]=%s\n",nargs,(char *)theArgv[nargs++]);
        temp = temp.substring(probe + 1);
        temp.trim();
      }
      probe = temp.indexOf((char *)" ");
    }
    if (temp.length() > 0)
    {
      strncpy((char *)&argS[nargs][0], (char *)temp.c_str(), 32);
      theArgv[nargs] = (char *)&argS[nargs][0];
      nargs++;
      //Serial.printf((char *)"theArgv[%d]=%s\n",nargs,(char *)theArgv[nargs++]);
      // Serial.printf((char *)"argv[%d]=%s\n",nargs,(char *)theArgv[nargs++]);
    }
    //Serial.printf((char *)"argc is %d\n",nargs);
    //for (int j=0;j<nargs;j++) Serial.printf((char *)"argv[%d] is %s\n",j,(char *)theArgv[j]);
    PicocCallMain(nargs, (char **)&theArgv[0]);
  }
  RunningProgram = 0; toConsole = 0;
  DeleteGlobals();
  
  char buf[200];
  free = ESP.getFreeHeap();
  sprintf((char *)&buf,(char *)"</table><hr>Program returned %d, free memory is %d<hr>", PicocExitValue, free);
  sendContent((char *)&buf);
  //Serial.print(F("freeMemory="));
  //Serial.println(free);
  server.sendContent(F("0\r\n\r\n"));
  server.sendContent(F("0\r\n\r\n"));
  sendChunked = 0;
 // PicocCleanup();
  free = ESP.getFreeHeap();
  Serial.print(F("freeMemory="));
  Serial.println(free);
#ifdef ESP32
  if (free<20000) ESP.restart();
#endif
}

String array[100];
int nFiles=0;
//Swap integer values by array indexes
void Sswap(int a, int b)
{
    String  tmp  = array[a];
    array[a] = array[b];
    array[b] = tmp;
}

//Partition the array into two halves and return the
//index about which the array is partitioned
int partition(int left, int right)
{
    int pivotIndex = left;
    String  pivotValue = array[pivotIndex];
    int index = left;
    int i;
 
    Sswap(pivotIndex, right);
    for(i = left; i < right; i++)
    {
      String a=array[i],b=pivotValue;
      a.toLowerCase();
      b.toLowerCase();
      if(strcmp((char *)a.c_str(),(char *)b.c_str()) < 0)
      {
          Sswap(i, index);
          index += 1;
      }
    }
    Sswap(right, index);
 
    return index;
}
 
//Quicksort the array

void quicksort(int left, int right)
{
    if(left >= right)
        return;
 
    int index = partition(left, right);
    quicksort(left, index - 1);
    quicksort(index + 1, right);
}
char doScreenCapture=0;
void ePaper_init(struct ParseState *Parser, struct Value *ReturnValue, struct Value **Param, int NumArgs);
void setup() {
  SPIFFS.begin();
#ifdef TFT
  Serial.begin(500000);
#else
#ifdef ePAPER
  Serial.begin(74880);
#else
  Serial.begin(115200);
#endif
#endif


  pinMode(26, OUTPUT); //VSPI SS
#ifdef TFT
  TFT_setup();
  Serial.println((char *)"\n/misLogo.jpg");
  drawJpeg((char *)"/misLogo.jpg", 0, 0);
#endif
#ifdef OLED
  u8x8.begin();
  u8x8.clearBuffer();
  u8x8.setFont(u8g2_font_amstrad_cpc_extended_8f);
  u8x8.drawStr(0, 10 * oi++,(char *)"  ESP32 picoc");
  u8x8.drawStr(0, 10 * oi++,(char *)" C Interpreter");
  u8x8.drawStr(14, 10 * oi++,(char *)"picoc@micro");
  u8x8.drawStr(0, 10 * oi++,(char *)" imagesys.com");
  u8x8.drawStr(4, 10 * oi++,(char *)"Running on ip");
#endif
#ifdef SSD1306OLED
 
  pinMode(16, OUTPUT); //  HELTEC OLED pin 16 is reset
  digitalWrite(16, 0);
  digitalWrite(16, 1);
  display.init();
  display.setContrast(20);
  display.setBrightness(10);
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0,(char *)"ESP32 picoc C Interpreter");
  display.drawString(0, 15,(char *)"picoc@microimagesys.com");
  display.drawString(0, 30,(char *)"picoc @ ");
  display.display();
#endif

#ifdef EPDMONO
    display.begin(); 
    display.clearDisplay(); //Clear any data that may have been in (software) frame buffer.
                            //(NOTE! This does not clean image on screen, it only clears it in the frame buffer inside ESP32).
    display.fillScreen(WHITE);
    display.setCursor(2,9);
    display.display();
    display.setFont(&FreeMonoBold9pt7b);
    display.setTextColor(BLACK,WHITE);
    display.setTextSize(1);

#endif

#ifdef ePAPER
  ePaper_init(0,0,0,0);
  display.setRotation(1);
  display.fillScreen(0xffff);
  display.setTextColor(0);
  display.update();
  display.setCursor(0, 20);
  display.println((char *)"  ESP32 picoc C");
  display.setCursor(0,45);
  display.println((char *)"Interpreter picoc@");
  display.setCursor(6,70);
  display.println((char *)"microimagesys.com");
#endif 
 
#ifdef OTA
    ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    //Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println(F("\nEnd"));
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.print("Progress: ");Serial.print((progress / (total / 100)));Serial.println(F("%"));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.print(F("Error["));Serial.print(error);Serial.println(F("]"));
    if (error == OTA_AUTH_ERROR) Serial.println(F("Auth Failed"));
    else if (error == OTA_BEGIN_ERROR) Serial.println(F("Begin Failed"));
    else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect Failed"));
    else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive Failed"));
    else if (error == OTA_END_ERROR) Serial.println(F("End Failed"));
  });
   ArduinoOTA.setHostname("ESP32 picoc"); 
  ArduinoOTA.begin();
  Serial.println("Finished OTA setup");
#endif //OTA

  // gets the listening port
  String listenport = LoadDataFromFile((char *)"listenport");
  // listening port - by default goes to 80 -
  if (listenport.toInt() == 0)
    listenport = F("80");
  WiFi.mode(WIFI_AP_STA);
  PrintAndWebOut(BasicVersion);
  server.on((char *)"/", []()
  {
    String WebOut;
    WebOut =    AdminBarHTML;
    //WebOut += RunningProgramGui();
    server.send(200, F("text/html"), WebOut);
  });

  server.on(F("/settings"), []()
  {

    server.send(200, F("text/html"), SettingsPageHandeler());
  });

  server.on(F("/vars"), []()
  {
    String WebOut = AdminBarHTML;
    server.send(200, F("text/html"), WebOut);
  });
  server.on(F("/debugResponse"), []()
  {
    WebResponse = 1;
    strncpy((char *)&WebResponseBuf, server.arg((char *)"value").c_str(), 32);
    return;
  });
  server.on(F("/espReset"), []()
  {
    server.send(200, F("text/html"),(char *)"<meta http-equiv=\"refresh\" content=\"5; url=./filemng\" /><h1>Restarting the ESP32 !!</h1>"); delay(100);
    ESP.restart();
  });

  server.on(F("/codetmpl"), []()
  {
    //Serial.println((char *)"codetmpl");
    String FileNameToView = "/default1.c";
    int i = 2;
    while (SPIFFS.exists(FileNameToView.c_str()))
    {
      char buf[33];
      sprintf((char *)&buf,(char *)"/default%d.c", i++);
      FileNameToView = (char *)&buf;
    }
    //Serial.println(FileNameToView);
#ifdef ESP32
   fs::File f = SPIFFS.open(FileNameToView,(char *)"w");
#else
    fs::File f = SPIFFS.open(FileNameToView,(char *)"w");
#endif
    f.println((char *)"\nvoid setup()\n{\n\n}");
    f.println((char *)"\nvoid loop()\n{\n  doLoop();\n}");
    f.println((char *)"int main(int argc,char ** argv)\n{");
    f.println((char *)"  setup();\n  //for (;;) loop();\n  //  Uncomment line above to run your loop() function forever.\n  //  When running loop forever,\n  //  you'll get no response to web events\n  //  unless you include doLoop();\n  //  in your interpreted loop() function\n ");
    f.println((char *)"  //  if you want to edit and rerun the program do ResetESP32\n  return 0;\n}");
    f.close();
    FileNameToView = GetRidOfurlCharacters(FileNameToView);
    String WholeUploadPage = F(R"=====(  <meta http-equiv="refresh" content="1; url=./edit?FileName=item&open=Open" />)=====");
    WholeUploadPage.replace((char *)"item", FileNameToView);
    //Serial.println(WholeUploadPage);
    server.send(200, F("text/html"), WholeUploadPage);
  });

  /* server.on(F("/run"), []()
    {
     programArgs=server.arg("programArgs");
     //Serial.printf("Program Args are %s\n",(char *)programArgs.c_str());
     ProgramName=server.arg("FileName");
     runProgram();
    });
  */
  server.onFileUpload(handleFileUpdate);

  server.on(F("/filemng"), []()
  {
    if (server.arg((char *)"Run").equals((char *)"Run")) {
      Serial.printf((char *)"/filemng Run\n");
      runProgram();
      return;
    }
    DoSomeFileManagerCode();
  });
  server.on(F("/functionHelp"), []()
  {
     char fbuf[20];     
       snprintf((char *)&fbuf,19,(char *)" %s",(char *)server.arg((char *)"function").c_str());
       if (strstr((char *)&fbuf,(char *)" *")!=0) strcat((char *)&fbuf,(char *)"(");
       else if (strstr((char *)&fbuf,(char *)"*")!=0) *strstr((char *)&fbuf,(char *)"*")=0;
     Serial.println((char *)&fbuf);
     if (strcmp((char *)fbuf,(char *)" *(")==0)
     {
         
        String WebOut=AdminBarHTML; WebOut+="<hr><p>"; WebOut+="<meta http-equiv=\"refresh\" content=\"0; url=./defs.txt\" />";
         Serial.println(WebOut);
         server.send(200, F("text/html"),WebOut);return;
     }
#ifdef ESP32
    fs::File fin=SPIFFS.open((char *)"/defs.txt",(char *)"r");
#else
     fs::File fin=SPIFFS.open((char *)"/defs.txt",(char *)"r");
#endif
     int fread = fin.read((uint8_t *)&buf, BUFSIZE);
     fin.close();
     buf[fread]=0;
     //Serial.println((char *)&buf);
     char * cp=(char *)&buf;
     char * ps=(char *)&buf;
     String helps=AdminBarHTML;
     helps+="<hr>";
     helps+="<h2>Help for function ";
     helps+=server.arg((char *)"function");
     helps+="</h2> &nbsp;&nbsp; <input class=\"buttong\" name=\"back\" value=\"GO Back\" onclick=\"javascript:window.history.back()\"><hr>\n";
     int foundIt=0;
     while (ps!=0)
     {
       ps=strchr(cp,10);
       if (ps==0) ps=strchr(cp,13);
       if (ps==0 && (strstr((char *)helps.c_str(),(char *)"<h3>")==0)) { 
        Serial.println((char *)"ps==0"); 
        String WebOut=AdminBarHTML; WebOut+="<hr><p>"; WebOut+="<meta http-equiv=\"refresh\" content=\"10; url=./filemng\" /><h3>Function not Found</h3>";
        server.send(400, F("text/html"),WebOut );
        return; 
       }
       if (ps==0) break;
       *ps=0;
       //Serial.println(cp);
       if (strstr(cp,(char *)&fbuf)!=0 && ((strstr(cp,(char *)&fbuf)-(char *)cp) <8) )
       {
         foundIt=1;
         Serial.println(cp);
         helps+="<h3>";
         helps+=cp;
         helps+="</h3>";
         for (int j=0;j<11;j++) helps.replace((char *)"  ",(char *)" ");
       }
       *ps=10;cp=ps+1;
     } 
     if (foundIt==1) {
         server.send(200, F("text/html"),helps);
         return;
     }
     Serial.println((char *)"not found at end of while");
     String WebOut=AdminBarHTML; WebOut+="<hr><p>"; WebOut+="<meta http-equiv=\"refresh\" content=\"10; url=./filemng\" /><h3>Function not Found</h3>";server.send(400, F("text/html"),WebOut );return; 
      
  });
  server.on(F("/edit"), []()
  {
    //Serial.print((char *)"Entered /edit\n");
    String WebOut;
    String TextboxProgramBeingEdited;
    String CRLF = F("\r\n");
    WebOut = String(EditorPageHTML);
    WebOut.replace((char *)"*edit.js*", editCodeJavaScript);
    WebOut.replace((char *)"*ProgramArgs*", programArgs);
    //Serial.printf((char *)"arg(run) is %s\n",(char *)server.arg((char *)"run").c_str());
    Serial.println(server.arg((char *)"FileName"));
    if (server.arg((char *)"FileName") != "" ) ProgramName = server.arg((char *)"FileName");
    if (server.arg((char *)"ScreenCapture")!="")
    {
#ifdef TFT
      if (!screenServer()) doScreenCapture=1;
#endif
      server.send(200,(char *)"text/html",(char *)"<meta http-equiv=\"refresh\" content=\"3; url=./goback.html\" /><h2>Screen Capture ...</h2>");
    }
    if ( server.arg((char *)"run").equals((char *)"Run") || server.arg((char *)"run").equals((char *)"RUN") )
    {
      programArgs = server.arg((char *)"programArgs");
      ProgramName = server.arg((char *)"FileName");
      //Serial.printf((char *)"run=%s and ProgramName=%s and Program Args=%s\n",(char *)server.arg((char *)"run").c_str(),(char *)ProgramName.c_str(),(char *)programArgs.c_str());
      if (ProgramName.substring(0, 1).equals((char *)"%")) ProgramName = String((char *)"/") + ProgramName.substring(3);
      if (ProgramName.equals((char *)"")) ProgramName = F("/default.c");
      //Serial.printf((char *)"Program Name to run is  %s\n",(char *)ProgramName.c_str());
      runProgram();
      return;
    }
    if ( server.arg((char *)"open") == F("Open") || server.arg((char *)"edit") == F("Edit") )
    {
      // really takes
      ProgramName = server.arg((char *)"FileName");
      if (ProgramName == "") ProgramName = server.arg((char *)"fileName");
      if (ProgramName.length() < 2) ProgramName = F("/default.c");
    }

    
    if (server.arg((char *)"saving") != "")
    {
      //Serial.println((char *)"Doing Saving");
      ProgramName = server.arg((char *)"FileName");
      ProgramName = MakeSureFileNameStartsWithSlash(ProgramName );
#ifdef ESP32
     fs::File fout = SPIFFS.open((char *)ProgramName.c_str(),(char *)"w");
#else
      fs::File fout = SPIFFS.open((char *)ProgramName.c_str(),(char *)"w");
#endif
      //Serial.printf((char *)"fout = %x\n",fout);
      String code = server.arg((char *)"code");
      //Serial.printf((char *)"code is\n%s\n\n",(char *)code.c_str());
      fout.write((uint8_t *)code.c_str(), code.length());
      fout.close();
    }

    ProgramName  = MakeSureFileNameStartsWithSlash(ProgramName );
    //LoadBasicProgramFromFlash( ProgramName);
    // the goal here is to replace the server send function by an equivalent that
    // permit to handle big buffers; this is acheived using the "chunked transfer"
    WebOut = String(EditorPageHTML);
    WebOut.replace((char *)"*edit.js*", editCodeJavaScript);
    WebOut.replace((char *)"*ProgramArgs*", programArgs);
    WebOut.replace(F("*programName*"), ProgramName);
    WebOut.replace(F("*programName*"), ProgramName);
    WebOut += "<textarea oninput=\"changeSaveButtonColor()\" class=\"text2\" rows=\"45\" cols=\"180\" name=\"code\" id=\"code\">*program txt*";

    WebOut = WebOut.substring(0, WebOut.indexOf(F("*program txt*")) );

    server.sendContent(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection:close\r\nTransfer-Encoding: chunked\r\nAccess-Control-Allow-Origin: *\r\n\r\n"));
    //Serial.println(F("start sending"));
    // each "chunk" is composed of :
    // the size of the block (in hex) terminated by \r\n
    server.sendContent(String(String(AdminBarHTML).length(), 16) + CRLF);
    // the block terminated by \r\n
    server.sendContent(String(AdminBarHTML) + CRLF);
    /////// end of first chunk ///////////
    server.sendContent(String(WebOut.length(), 16) + CRLF);
    server.sendContent(WebOut + CRLF);
    int iii;
    int i;
    fs::File theCode = SPIFFS.open((char *)ProgramName.c_str(),(char *)"r");
    TextboxProgramBeingEdited = theCode.readStringUntil('\n');
    int doit = 1;
    String codeAccummulator = TextboxProgramBeingEdited; codeAccummulator += "\n";
    while (theCode.available() > 0)
    {
      String temp = theCode.readStringUntil('\n');
      codeAccummulator += temp; codeAccummulator += "\n";
      TextboxProgramBeingEdited = TextboxProgramBeingEdited + "\n" + temp ;
      if (TextboxProgramBeingEdited.length() > 2048)
      {
        server.sendContent(String(TextboxProgramBeingEdited.length(), 16) + CRLF);
        server.sendContent(TextboxProgramBeingEdited + CRLF);
        TextboxProgramBeingEdited = "";
      }
    }
    if (TextboxProgramBeingEdited.length() > 0)
    {
      server.sendContent(String(TextboxProgramBeingEdited.length(), 16) + CRLF);
      server.sendContent(TextboxProgramBeingEdited + CRLF);
    }
    //Serial.println("Doing save to /backup");
#ifdef ESP32
   fs::File fout = SPIFFS.open((char *)"/backup",(char *)"w");
#else
    fs::File fout = SPIFFS.open((char *)"/backup",(char *)"w");
#endif
    fout.print(codeAccummulator);
    fout.close();
    //Serial.println((char *)"Code saved is");
    //Serial.println(codeAccummulator);
    //Serial.println((char *)"Done save to /backup");
    WebOut = "</textarea></br></form>";
    server.sendContent(String(WebOut.length(), 16) + CRLF);
    server.sendContent(WebOut + CRLF);
    // end of transmission
    server.sendContent(F("0\r\n\r\n"));
    server.sendContent(F("0\r\n\r\n"));
    delay(0);
    //Serial.println(F("End of Open"));
  });
server.on(F("/ResetESP32"), []()
  {
      Serial.printf((char *)"Command to Restart %s in the picoc Interpreter received\n",(char *)ProgramName.c_str());
      server.send(200,(char *)"text/html", String((char *)"<meta http-equiv=\"refresh\" content=\"4; url=./edit?FileName=")+ProgramName+String((char *)"&open=OPEN\" /><h2>Restarting ...</h2>"));
      delay(1000);
      ESP.restart();
    });

  server.on((char *)"/editor.js", []() {
    //Serial.println((char *)"Sending editor.js");
    server.send(200,(char *)"text/html", editCodeJavaScript);
  });
#ifdef TFT
  server.on((char *)"/ScreenCapture", []() {
    screenServer();
 //     if (!screenServer()) {
 //     Serial.printf((char *)"Setting doScreenCapture to 1\n");
 //     doScreenCapture=1;
 //     }
    });
#endif

  server.on(F((char *)"/filelist"), []()
  {
    String ret = "";
    String fn;
#ifdef ESP32
   fs::File root = SPIFFS.open((char *)"/");
   fs::File file = root.openNextFile();
    while (file) {
      char bf[16];
      String space = "                             ";
      String fname = file.name();
      ret += fname;
      int spaces = 25 - fname.length(); // Tabulate nicely
      ret += space.substring(0, spaces);
      snprintf((char *)&bf, 15,(char *)"%6d bytes\n", file.size());
      ret += ((char *)&bf);
      file = root.openNextFile();
    }
    server.send(200, F("text/html"), ret);
    
#else
    fs::File root = SPIFFS.open((char *)"/",(char *)"r");
 //   fs::File file = root.openNextFile();
#endif
  });

  /*
    server.on(F("/codein"), []() {

      if (server.arg(F("SaveTheCode")) == F("start"))
      {
        inData = "end";
        //Serial.println(F("start save"));
        //Serial.printf("fileName is %s\n",(char *)server.arg("FileName").c_str());
        ProgramName = GetRidOfurlCharacters(server.arg("FileName"));
        if (ProgramName == "")
          ProgramName = F("/default.c");
        ProgramName.trim();
        if (ProgramName[0] != '/')
          ProgramName = "/" + ProgramName;
        OpenToWriteOnFlash( ProgramName );
      }

      if (server.arg(F("SaveTheCode")) != F("yes") & server.arg(F("SaveTheCode")) != F("start") & server.arg(F("SaveTheCode")) != F("end"))
      {
        String LineNoForWebEditorIn;
        LineNoForWebEditorIn = server.arg(F("line"));
        int y = LineNoForWebEditorIn.toInt();
        //Serial.println(ProgramName + F(" / ") + String(y));
        //Serial.printf("\"code\" = %s\n",(char *)server.arg("code").c_str());
        //Serial.println("------------------------------------------------");
        WriteBasicLineOnFlash(GetRidOfurlCharacters(server.arg("code")));
        noOfLinesForEdit = y;
      }
      if (server.arg(F("SaveTheCode")) == F("end"))
      {
        // terminate the save
        Serial.println(F("end of save!!"));
        CloseWriteOnFlash();
        //LoadBasicProgramFromFlash( ProgramName );
      }
      server.send(200, F("text/html"), F("good"));
    });


  */


  server.on(F("/msg"), []() {

    MsgBranchRetrnData = F("No MSG Branch Defined");

    if (msgbranch != "")
    {
      inData = String((char *)" goto " + msgbranch + " ");
    }


    server.send(200, F("text/html"), MsgBranchRetrnData);
  });





  server.on(F("/input"), []() {
    server.send(200, F("text/html"), RunningProgramGui());
  });

  server.onNotFound ( []() {
    String fileNameToServeUp;
    String msg = server.uri();
    msg=msg.substring(1);
  //  Serial.printf((char *)"Uri is %s\n",(char *)msg.c_str());
    fileNameToServeUp = GetRidOfurlCharacters(server.arg((char *)"file"));
  //  Serial.printf((char *)"Filename to Serve up is >%s<\n",(char *)fileNameToServeUp.c_str());
    if (fileNameToServeUp.length() == 0) fileNameToServeUp = msg;
  //  Serial.printf((char *)"onNotfound for %s\n",(char *)fileNameToServeUp.c_str());
  //  Serial.print(F("freeMemory="));
    uint32_t free = ESP.getFreeHeap();
  //  Serial.println(free);
    if (fileNameToServeUp.length() == 0) {
      server.send(200, F("text/html"), RunningProgramGui());
      return;
    }
    //Serial.println(F("\nIn not found"));
  //  Serial.printf("File Name -> %s\n",fileNameToServeUp.c_str());
    String doFile = "/" + fileNameToServeUp;
    //Serial.printf("file to open -> %s\n",(char *)doFile.c_str());
    fs::File mySuperFile = SPIFFS.open(doFile.c_str(),(char *)"r");
    if (mySuperFile)
    {
      //Serial.printf("In mySuperFile with content type = %s\n",getContentType(fileNameToServeUp).c_str());
      server.streamFile(mySuperFile, getContentType(fileNameToServeUp));
      server.send(200, getContentType(fileNameToServeUp), mySuperFile.readString());
      //Serial.printf("%s\n",mySuperFile.readString().c_str());

    }
    else
    {
      server.send(200, F("text/html"), RunningProgramGui());
    }
    mySuperFile.close();
  });

  //LoadBasicProgramFromFlash((char *)"");


  if (  ConnectToTheWIFI(LoadDataFromFile(F("WIFIname")), LoadDataFromFile(F("WIFIpass")),(char *)"",(char *)"",(char *)"") == 0)
  {
    if (LoadDataFromFile(F("APname")) == "")
    {
      CreateAP((char *)"",(char *)"");
    }
    else
    {
      CreateAP(LoadDataFromFile(F("APname")), LoadDataFromFile(F("APpass")));
    }
    //Serial.println("Done creating AP");
  }

  server.begin();
  //Serial.print("freeMemory=");
  uint32_t free = ESP.getFreeHeap();
  //Serial.println(free);
}

int gfh()
{
  ESP.getFreeHeap();
}
String SettingsPageHandeler()
{
  if ( server.arg((char *)"key") == LoadDataFromFile(F("LoginKey")))
  {
    LoggedIn = millis();
  }
  String WebOut = String(AdminBarHTML) + String(SettingsPageHTML);
  String staName = LoadDataFromFile(F("WIFIname"));
  String staPass = LoadDataFromFile(F("WIFIpass"));
  String apName = LoadDataFromFile(F("APname"));
  String apPass = LoadDataFromFile(F("APpass"));
  String LoginKey = LoadDataFromFile(F("LoginKey"));
  String ShowMenueBar = LoadDataFromFile(F("ShowMenueBar"));
  String otaUrl = LoadDataFromFile(F("otaUrl"));
  String autorun = LoadDataFromFile(F("autorun"));
  String listenport = LoadDataFromFile(F("listenport"));

  // listening port - by default goes to 80 -
  if (listenport.toInt() == 0)
    listenport = F("80");

  //Serial.print("Loading Settings Files");

  if (millis() > LoggedIn + 600000 || LoggedIn == 0 )
  {
    WebOut = LogInPage;
  }
  else
  {

    if ( server.arg((char *)"restart") == F("Restart") ) {
      server.send(200,(char *)"text/html",(char *)"<meta http-equiv=\"refresh\" content=\"8; url=./filemng\" /><h2>Restarting ...</h2>");
      delay(1000);
      ESP.restart();
    }

    /*
        if ( server.arg("update") == F("Update") )
        {

          //        Serial.println(BasicOTAupgrade());
          if (LoadDataFromFile("otaUrl") == "")
          {
            t_httpUpdate_return  ret = ESPhttpUpdate.update(F("esp8266basic.smbisoft.com"), 80, F("/4M/ESP8266Basic.cpp.bin"));
            if (ret == HTTP_UPDATE_FAILED ) Serial.println(F("Update failed"));
          }
          else
          {
            String URLtoGet = LoadDataFromFile("otaUrl");
            String ServerToConnectTo;
            String PageToGet;
            ServerToConnectTo = URLtoGet.substring(0, URLtoGet.indexOf("/"));
            PageToGet = URLtoGet.substring(URLtoGet.indexOf("/"));
            t_httpUpdate_return  ret = ESPhttpUpdate.update(ServerToConnectTo, 80, PageToGet);
            if (ret == HTTP_UPDATE_FAILED ) Serial.println(F("Update failed"));
          }
          //t_httpUpdate_return  ret = ESPhttpUpdate.update("cdn.rawgit.com", 80,(char *)"/esp8266/Basic/master/Flasher/Build/4M/ESP8266Basic.cpp.bin");

        }
    */

    if ( server.arg((char *)"save") == F("Save") )
    {
      staName      = GetRidOfurlCharacters(server.arg(F("staName")));
      staPass      = GetRidOfurlCharacters(server.arg(F("staPass")));
      apName       = GetRidOfurlCharacters(server.arg(F("apName")));
      apPass       = GetRidOfurlCharacters(server.arg(F("apPass")));
      LoginKey     = GetRidOfurlCharacters(server.arg(F("LoginKey")));
      ShowMenueBar = GetRidOfurlCharacters(server.arg(F("showMenueBar")));
      otaUrl       = GetRidOfurlCharacters(server.arg(F("otaurl")));
      autorun      = GetRidOfurlCharacters(server.arg(F("autorun")));
      listenport   = GetRidOfurlCharacters(server.arg(F("listenport")));

      SaveDataToFile(F("WIFIname") , staName);
      SaveDataToFile(F("WIFIpass") , staPass);
      SaveDataToFile(F("APname") , apName);
      SaveDataToFile(F("APpass") , apPass);
      SaveDataToFile(F("LoginKey") , LoginKey);
      SaveDataToFile(F("ShowMenueBar") , ShowMenueBar);
      SaveDataToFile(F("otaUrl") , otaUrl);
      SaveDataToFile(F("autorun") , autorun);
      SaveDataToFile(F("listenport") , listenport);

    }

    if ( server.arg(F("format")) == F("Format") )
    {
      // BasicFileOpened.close();
      Serial.println(F("Formating "));
      Serial.print(SPIFFS.format());
    }

    WebOut.replace(F("*sta name*"), staName);
    WebOut.replace(F("*sta pass*"), staPass);
    WebOut.replace(F("*ap name*"),  apName);
    WebOut.replace(F("*ap pass*"),  apPass);
    WebOut.replace(F("*LoginKey*"), LoginKey);
    WebOut.replace(F("*BasicVersion*"), BasicVersion);
    WebOut.replace(F("*otaurl*"), otaUrl);
    WebOut.replace(F("*listenport*"), listenport);

    if ( ShowMenueBar == F("off"))
    {
      WebOut.replace(F("**checked**"), F("checked"));
    }
    else
    {
      WebOut.replace(F("**checked**"),(char *)"");
    }

    if ( autorun == F("on"))
    {
      WebOut.replace(F("**autorun**"), F("checked"));
    }
    else
    {
      WebOut.replace(F("**autorun**"),(char *)"");
    }

  }
  return WebOut;
}



String getContentType(String filename) {
  if (filename.endsWith(F(".htm"))) return F("text/html");
  else if (filename.endsWith(F(".html"))) return F("text/html");
  else if (filename.endsWith(F(".htm"))) return F("text/html");
  else if (filename.endsWith(F(".css"))) return F("text/css");
  else if (filename.endsWith(F(".js"))) return F("application/javascript");
  else if (filename.endsWith(F(".png"))) return F("image/png");
  else if (filename.endsWith(F(".gif"))) return F("image/gif");
  else if (filename.endsWith(F(".jpg"))) return F("image/jpeg");
  else if (filename.endsWith(F(".ico"))) return F("image/x-icon");
  else if (filename.endsWith(F(".xml"))) return F("text/xml");
  else if (filename.endsWith(F(".pdf"))) return F("application/x-pdf");
  else if (filename.endsWith(F(".zip"))) return F("application/x-zip");
  else if (filename.endsWith(F(".gz"))) return F("application/x-gzip");
  return F("text/plain");
}

void StartUpProgramTimer()
{
  return;
}



void DoSomeFileManagerCode()
{
  String WholeUploadPage = UploadPage;
  String FileListForPage ;
  WholeUploadPage.replace((char *)"*ProgramArgs*", programArgs);
  if (CheckIfLoggedIn())
  {
    WholeUploadPage = LogInPage;
  }
  else
  {
    if (server.arg((char *)"Delete") != "")
    {
      programArgs = server.arg((char *)"programArgs");
      String FIleNameForDelete = server.arg((char *)"FileName");
      FIleNameForDelete = GetRidOfurlCharacters(FIleNameForDelete);
      //Serial.println(FIleNameForDelete);
      SPIFFS.remove(FIleNameForDelete);
      //Serial.println(SPIFFS.remove((char *)"uploads/settings.png"));
    }
#ifdef ESP32
   fs::File root = SPIFFS.open((char *)"/");
   fs::File file = root.openNextFile();
    while (file) {
      if (nFiles<100 && (strstr((char *)file.name(),(char *)".hrsh")==0) && (strstr((char *)file.name(),(char *)".ico")==0)&& (strstr((char *)file.name(),(char *)".dat")==0)) array[nFiles++]=file.name();
      //FileListForPage += String(F("<option>")) + file.name() + String(F("</option>"));
      file = root.openNextFile();
    }
    
#else
    fs::File root = SPIFFS.open((char *)"/",(char *)"r");
//    fs::File file = root.openNextFile();
#endif
    // Serial.println(FileListForPage);
    quicksort(0,nFiles-1);
    for (int j=0;j<nFiles;j++) {    
      FileListForPage += String(F("<option>")) + array[j] + String(F("</option>"));
      array[j]="";
    }
    for (int j=0;j<nFiles;j++) array[j]="";
    nFiles=0;
    WholeUploadPage.replace((char *)"*table*", FileListForPage);

    if (server.arg((char *)"View") != "")
    {
      programArgs = server.arg((char *)"programArgs");
      String FileNameToView = server.arg((char *)"FileName");
      FileNameToView = GetRidOfurlCharacters(FileNameToView);
      FileNameToView.replace((char *)"/uploads/",(char *)"");
      WholeUploadPage = F(R"=====(  <meta http-equiv="refresh" content="0; url=./file?file=item" />)=====");
      WholeUploadPage.replace((char *)"item", FileNameToView);
    }


    if (server.arg(F("Edit")) != "" || server.arg(F("open")) != "")
    {
      programArgs = server.arg((char *)"programArgs");
      String FileNameToView = server.arg((char *)"FileName");
      FileNameToView = GetRidOfurlCharacters(FileNameToView);
      //FileNameToView.replace((char *)"/uploads/",(char *)"");
      WholeUploadPage = F(R"=====(  <meta http-equiv="refresh" content="0; url=./edit?FileName=item&open=Open" />)=====");
      WholeUploadPage.replace((char *)"item", FileNameToView);
    }


    if (server.arg(F("NewFile")) != "")
    {
      Serial.println((char *)"In NewFile");
      programArgs = server.arg((char *)"programArgs");
      String FileNameToView = "/newfile1.txt";
      int i = 2;
      while (SPIFFS.exists(FileNameToView.c_str()))
      {
        char buf[33];
        sprintf((char *)&buf,(char *)"/newfile%d.txt", i++);
        FileNameToView = (char *)&buf;
      }
      WholeUploadPage = F(R"=====(  <meta http-equiv="refresh" content="0; url=./edit?fileName=item&open=Open" />)=====");
      WholeUploadPage.replace((char *)"item", FileNameToView);
    }


    if (server.arg(F("Rename")) != "")
    {
      programArgs = server.arg((char *)"programArgs");
      String FileNameToView = server.arg((char *)"FileName");
      FileNameToView = GetRidOfurlCharacters(FileNameToView);
      String newfileName = server.arg((char *)"newfileName");
      newfileName  = GetRidOfurlCharacters(newfileName );
      WholeUploadPage = F(R"=====(<form id="filelist"><table><tr><td>Old Name<br><input type="text" id="FileName" class="text3" name="FileName" value="*item name*"><br>New Name<br><input type="text" class="text3"name="newfileName" value="*item name*"></td><td>&nbsp;&nbsp;<input class="buttong" type="submit" value="Change Name" name="Rename"></td></tr></table></form>)=====");
      WholeUploadPage.replace(F("*item name*"), FileNameToView);
      if (newfileName != "" )
      {
        newfileName = MakeSureFileNameStartsWithSlash(newfileName);
        WholeUploadPage = F(R"=====(  <meta http-equiv="refresh" content="0; url=./filemng" />)=====");
        SPIFFS.rename(FileNameToView , newfileName);
      }
    }
  }
  server.send(200, F("text/html"),  String( AdminBarHTML + WholeUploadPage ));
}



void handleFileUpdate()
{
  //if (server.uri() != "/edit") return;
  HTTPUpload& upload = server.upload();
  DBG_OUTPUT_PORT.printf((char *)"upload.status is %d\n", (int)upload.status);
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    ProgramName=String((char *)"/uploads/")+filename;
    DBG_OUTPUT_PORT.print((char *)"Upload Name: "); DBG_OUTPUT_PORT.println(filename);
    fsUploadFile = SPIFFS.open(String((char *)"/uploads/" + filename),(char *)"w");
    filename = String();
  } else if (upload.status == 1) {
    DBG_OUTPUT_PORT.print((char *)"Upload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
    if (fsUploadFile) {
      fsUploadFile.write(upload.buf, upload.currentSize);
      DBG_OUTPUT_PORT.printf((char *)"wrote %d bytes to %s\n", upload.currentSize, (char *)upload.filename.c_str());
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile)
      fsUploadFile.close();
    DBG_OUTPUT_PORT.print((char *)"Upload Size: "); DBG_OUTPUT_PORT.println(upload.totalSize);
  }
}








String  getSerialInput()
{
  return "";
}




bool CheckIfLoggedIn()
{
  if (LoadDataFromFile((char *)"LoginKey") != "")
  {
    if ( millis() > LoggedIn + 600000 || LoggedIn == 0 )     return 1;
  }
  return 0;
}
#ifdef SSD1306OLED
extern int OLEDdidWhen;
#define DISPLAY_INVERTING
#endif
int lCount=0;
int OLEDdidWhen=0;
int OLEDdisplayOn;
void loop()
{
  server.handleClient();
#ifdef LIGHT_SLEEP
  //  Serial.println((char *)"light_sleep_enter");
    esp_sleep_enable_timer_wakeup(33000); // 1/30 seconds
    int ret = esp_light_sleep_start();
  //  Serial.printf((char *)"light_sleep: %d\n", ret);
#endif
#ifdef DISPLAY_INVERTING
  if (lCount%(10)==0) {
   int now=millis();
   if ((now-OLEDdidWhen)>5*60000 ) { if (OLEDdisplayOn==1) display.normalDisplay(); else display.invertDisplay(); display.display(); OLEDdidWhen=now; Serial.println((char *)"Inverted OLED display"); OLEDdisplayOn=++OLEDdisplayOn%2;} // invert display after 5 minutes of inactivity
  }
#endif
  delay(50);
  if (lCount++%(12000)==0) 
  {
    uint32_t free = ESP.getFreeHeap();
   Serial.printf((char *)"free heap is %d\n", free);
  }
  
}







String getValueforPrograming(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {
    0, -1
  };
  int maxIndex = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}




String getValue(String data, char separator, int index)
{
  data = String(data + "           ");
  int maxIndex = data.length() - 1;
  int j = 0;
  byte WaitingForQuote;
  String chunkVal = "";
  String ChunkReturnVal;
  for (int i = 0; i <= maxIndex && j <= index; i++)
  {
    if (data[i] == '\"' )
    {
      i++;
      while (i <= maxIndex && data[i] != '\"' ) {
        chunkVal.concat(data[i]);
        i++;
        delay(0);
      }
    }
    else if (data[i] == '|' )
    {
      i++;
      while (i <= maxIndex && data[i] != '|' ) {
        chunkVal.concat(data[i]);
        i++;
        delay(0);
      }
    }
    else
    {
      if (data[i] != separator) chunkVal.concat(data[i]);
    }

    if (data[i] == separator & data[i - 1] != separator)
    {
      j++;
      if (j > index)
      {
        //chunkVal.trim();
        if (chunkVal != String(separator))
        {
          ChunkReturnVal = chunkVal;
          break;
        }
      }
      chunkVal = "";
    }
  }
  //    Serial.println((char *)"index");
  //    Serial.println(index);
  //    Serial.println(j);

  if (j == index + 1)
  {

    return ChunkReturnVal;
  }
}

String FetchWebUrl(String URLtoGet, int PortNoForPage)
{
  if (PortNoForPage == 0) PortNoForPage = 80;
  String str = "             ";
  String ServerToConnectTo = URLtoGet.substring(0, URLtoGet.indexOf((char *)"/"));
  String PageToGet = URLtoGet.substring(URLtoGet.indexOf((char *)"/"));
  byte numberOwebTries = 0;
  // ServerToConnectTo ;
  //PageToGet = URLtoGet.substring(URLtoGet.indexOf((char *)"/"));

  //  Serial.println(ServerToConnectTo);
  //  Serial.println(PageToGet);


  if (client.connect(ServerToConnectTo.c_str() , PortNoForPage))
  {
    client.print(String((char *)"GET " + PageToGet + " HTTP/1.1\r\nHost: " +  ServerToConnectTo + "\r\n\r\n"));
    // wait for maximum 12 x 300msec = 3.6 seconds
    while ( ! client.available() && numberOwebTries < 12 ) {
      numberOwebTries++;
      delay(300);   // 300ms
    }

    while (client.available())
    {
      delay(0);
      if (str.endsWith(String((char *)"\r\n\r\n")))  str = "";

      str.concat( (const char)client.read());
      delay(0);
      if (client.available() == false)
      {
        // if no data, wait for 300ms hoping that new data arrive
        delay(300);
      }

    }



    client.stop();
    return str.substring(0, str.indexOf(String(String(char(10)) + "0" )  ));
  }
  client.stop();
  return "";
}



void serialFlush()
{

  while (Serial.available() > 0)
  {
    delay(0);
    char t = Serial.read();
  }
}
