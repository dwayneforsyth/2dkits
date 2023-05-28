//wifi code here
#ifdef OLED
#include <U8g2lib.h>
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8x8;
extern int oi;
#endif // OLED
#ifdef SSD1306OLED
#include "SSD1306Wire.h"
extern SSD1306Wire display;
extern OLEDDISPLAY_COLOR OLEDcolor;
extern int OLEDdisplayOn;
#endif
#ifdef TFT
#include <TFT_eSPI.h> // Hardware-specific library
#include <Hershey.h>
extern Hershey HF;
extern TFT_eSPI tft;
#endif // TFT
#ifdef ePAPER
#include <GxEPD.h>
#include <GxGDEP015OC1/GxGDEP015OC1.h>    // 1.54" b/w
extern GxGDEP015OC1 display;    // 1.54" b/w display; //(io, -1, -1/*RST=D4*/ /*BUSY=D2*/); // default selection of D4(=2), D2(=4)
#endif
void CreateAP(String NetworkName, String NetworkPassword);
bool ConnectToTheWIFI(String NetworkName, String NetworkPassword, String NetworkStaticIP, String NetworkGateway, String NetworkSubnet)
{
  #ifdef ALWAYS_STATION
  WiFi.mode(WIFI_STA);
  #else
  WiFi.mode(WIFI_AP_STA);
  #endif
  byte numberOfAtempts = 0;
  int str_len = NetworkName.length() + 1;
  char ssid[str_len];
  NetworkName.toCharArray(ssid, str_len);

  str_len = NetworkPassword.length() + 1;
  char password[str_len];
  NetworkPassword.toCharArray(password, str_len);

  pinMode(BUILTIN_LED,OUTPUT);
  // Connect to WiFi network
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    numberOfAtempts = numberOfAtempts  + 1;
    digitalWrite(BUILTIN_LED,numberOfAtempts%2);
    delay(333);
    //Serial.println(numberOfAtempts);
    if (numberOfAtempts >= 24)
    {
      Serial.println((char *)"");
      Serial.println(F("Failed Wifi Connect "));
#ifdef ALWAYS_STATION
      ESP.restart();
#endif
      return 0;
    }
  }
  digitalWrite(BUILTIN_LED,0);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    //config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = (uint32_t)0x00000000, IPAddress dns2 = (uint32_t)0x00000000);
    IPAddress local(192,168,0,110),gatew(192,168,0,1),subnet(255,255,255,0),dns1(8,8,8,8),dns2(4,2,2,1);
    WiFi.config(local,gatew,subnet,dns1,dns2);
    WiFi.setSleep(true);
    WiFi.setTxPower(WIFI_POWER_5dBm);
  if (NetworkStaticIP != "" & NetworkGateway != "" & NetworkSubnet != "" )
  {

    NetworkStaticIP += ".";
    NetworkGateway += ".";
    NetworkSubnet += ".";
    IPAddress ip(     getValue(NetworkStaticIP, '.', 0).toInt(), getValue(NetworkStaticIP, '.', 1).toInt(), getValue(NetworkStaticIP, '.', 2).toInt(), getValue(NetworkStaticIP, '.', 3).toInt());
    IPAddress gateway(getValue(NetworkGateway,  '.', 0).toInt(), getValue(NetworkGateway, '.', 1).toInt(), getValue(NetworkGateway, '.', 2).toInt(), getValue(NetworkGateway, '.', 3).toInt());
    IPAddress subnet( getValue(NetworkSubnet,   '.', 0).toInt(), getValue(NetworkSubnet, '.', 1).toInt(), getValue(NetworkSubnet, '.', 2).toInt(), getValue(NetworkSubnet, '.', 3).toInt());
    WiFi.config(ip, gateway, subnet);
  }
  if (WiFi.localIP().toString().endsWith((char *)".0"))
  {
    Serial.println(WiFi.localIP());
    CreateAP((char *)"",(char *)"");
    return 0;
  }
  else
  {
    //configTime(3 * 3600, 0,(char *)"pool.ntp.org",(char *)"time.nist.gov");
    configTime(LoadDataFromFile((char *)"TimeZone").toFloat() * 3600, LoadDataFromFile((char *)"DaylightSavings").toInt(),(char *)"pool.ntp.org",(char *)"time.nist.gov");
    Serial.println((char *)"");
    Serial.print(F((char *)"Connected to "));
    Serial.println(ssid);
    Serial.print(F("IP address : "));
    Serial.println(WiFi.localIP());
#ifdef TFT
HF.setFont((char *)"serif");
    HF.drawString(156,32,0.67,0,(char *)WiFi.localIP().toString().c_str());
    HF.drawString(265,188,0.67,0xf1c3,PICOC_VERSION);
    HF.drawString(156,33,0.67,0,(char *)WiFi.localIP().toString().c_str());
    HF.drawString(265,189,0.67,0xf1c3,PICOC_VERSION);
    HF.drawString(157,32,0.67,0,(char *)WiFi.localIP().toString().c_str());
    HF.drawString(266,188,0.67,0xf1c3,PICOC_VERSION);
    HF.setFont((char *)"sansbold");
    HF.drawString(10,218,0.67,0xf1c3,(char *)"picoc@microimagesys.com");    
    HF.drawString(11,218,0.67,0xf1c3,(char *)"picoc@microimagesys.com"); 
    HF.drawString(10,217,0.67,0xf1c3,(char *)"picoc@microimagesys.com"); 
    tft.setTextSize(1);tft.setTextColor(TFT_GREEN,0);
#endif
#ifdef ePAPER
    display.setCursor(25,130);
    display.println((char *)WiFi.localIP().toString().c_str());
    display.setCursor(0,105);
    display.println((char *)"  Running at IP");
    display.update();
#endif
#ifdef OLED
    u8x8.drawStr(8,10*oi++,WiFi.localIP().toString().c_str());
    u8x8.sendBuffer();
#endif
#ifdef SSD1306OLED
   fs::File fin=SPIFFS.open((char *)"/data/OLEDcolor.txt",(char *)"r");
    if (fin==0) OLEDcolor=(OLEDDISPLAY_COLOR)0;
    else
    {
      char buf[1];
      int iread=fin.read((uint8_t *)&buf,10);
      buf[iread]=0;
      OLEDcolor=(OLEDDISPLAY_COLOR)atoi((char *)&buf);
      fin.close();
    }
    fin=SPIFFS.open((char *)"/data/OLEDcolor.txt",(char *)"w");
    OLEDcolor=(OLEDDISPLAY_COLOR)(((int)OLEDcolor+1)%2);
    Serial.printf((char *)"OLEDcolor is %d\n",(int)OLEDcolor);
    fin.print(OLEDcolor);
    OLEDdisplayOn=(int)OLEDcolor;
    fin.close();
    display.drawString(40,30,WiFi.localIP().toString().c_str());    
    if ((int)OLEDcolor==1) display.invertDisplay();
    display.display();
#endif  //SSD1306OLED
    SaveDataToFile((char *)"WIFIname" ,  NetworkName);
    SaveDataToFile((char *)"WIFIpass", NetworkPassword);
    String autorun=LoadDataFromFile((char *)"autorun");
    if (autorun.equals((char *)"on")) runProgram();
    return 1;
  }
}

void drawJpeg(char * filename, int xpos, int ypos); 

void CreateAP(String NetworkName, String NetworkPassword)
{
  WiFi.mode(WIFI_AP_STA);
  Serial.println(F("Creating WIFI access point"));


  if (NetworkName == "")
  {
    NetworkName = LoadDataFromFile((char *)"APname");
    NetworkPassword = LoadDataFromFile((char *)"APpass");

    if (NetworkName == "")
    {
      NetworkName = "ESP" + WiFi.softAPmacAddress();
      NetworkPassword = "";
    }
  }

  Serial.println(NetworkName);

  digitalWrite(BUILTIN_LED,1);
  int str_len = NetworkName.length() + 1;
  char ssid[str_len];
  NetworkName.toCharArray(ssid, str_len);

  str_len = NetworkPassword.length() + 1;
  char password[str_len];
  NetworkPassword.toCharArray(password, str_len);

  
  delay(2000);
  if (NetworkPassword.length() < 8)
  {
    WiFi.softAP(ssid);
  }
  else
  {
    WiFi.softAP(ssid, password);
  }
  delay(2000);

  SaveDataToFile((char *)"APname" ,  NetworkName);
  SaveDataToFile((char *)"APpass", NetworkPassword);
  digitalWrite(BUILTIN_LED,1);
  #ifdef TFT
  drawJpeg((char *)"/misLogoAP.jpg",0,0);
  #endif // TFT
}
