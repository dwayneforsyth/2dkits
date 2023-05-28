//File System Stuff

void SaveDataToFile(String fileNameForSave, String DataToSave)
{
  //Serial.println(fileNameForSave);
  //SPIFFS.begin();
#ifdef ESP32
 fs::File f = SPIFFS.open(String((char *)"/data/" + fileNameForSave + ".dat"),(char *)"w");
#else
  fs::File f = SPIFFS.open(String((char *)"/data/" + fileNameForSave + ".dat"),(char *)"w");
#endif

  if (!f)
  {
    sendContent((char *)"file open failed");
  }
  else
  {
    f.println(String(DataToSave + String('\r')));
    f.close();
  }
  return;
}

void BasicProgramWriteLine(int LineNumberToLookUp, String DataToWriteForProgramLine);
String LoadDataFromFile(String fileNameForSave)
{
  String WhatIwillReturn;
  //SPIFFS.begin();
#ifdef ESP32
 fs::File f = SPIFFS.open(String((char *)"/data/" + fileNameForSave + ".dat"),(char *)"r");
#else
  fs::File f = SPIFFS.open(String((char *)"/data/" + fileNameForSave + ".dat"),(char *)"r");
#endif
  if (!f)
  {
    //Serial.print((char *)"file open failed  :");
    //Serial.println(fileNameForSave);
  }
  else
  {
    WhatIwillReturn =  f.readStringUntil('\r');
    WhatIwillReturn.replace((char *)"\n",(char *)"");
    f.close();
    return WhatIwillReturn;
  }
}

//////////// New file stuff by CiccioCB /////////////

#ifdef ESP32
static fs::File BasicFileToSave;
#else
static fs::File BasicFileToSave;
#endif
//static int  program_nb_lines = 0;
//static uint16_t  line_seeks[256];

bool OpenToWriteOnFlash(String fileNameForWrite)
{
  //Serial.printf((char *)"Opening SPIFFS file %s\n",(char *)fileNameForWrite.c_str());
  BasicFileToSave = SPIFFS.open(fileNameForWrite,(char *)"w");
  if (!BasicFileToSave)
  {
    Serial.println(F((char *)"file write open failed"));
	return false;
  }
  return true;
}

bool WriteBasicLineOnFlash(String codeLine)
{
	int ret;
  int i=1;
  int where=codeLine.lastIndexOf((char *)"\n");
  //Serial.printf((char *)"where = %d and length=%d\n",where,codeLine.length());
  while (codeLine.length()-where<=2) 
  {
    //Serial.printf((char *)"Dropped last line #%d\n",i++);
    codeLine=codeLine.substring(0,where);
    where=codeLine.lastIndexOf((char *)"\n");
    //Serial.printf((char *)"where = %d and length=%d\n",where,codeLine.length());
  }
  ret = BasicFileToSave.print(codeLine);
  if (ret == 0)
  {
    Serial.println(F((char *)"file write println failed"));
    return false; 
  } 
  return true;
}

void CloseWriteOnFlash(void)
{
	BasicFileToSave.close();
}


void LoadBasicProgramFromFlash(String fileNameForRead)
{
  fs::File f = SPIFFS.open(fileNameForRead,(char *)"r");
  f.read((uint8_t *)&buf, BUFSIZE);
  return;
}


String MakeSureFileNameStartsWithSlash(String FileNameToCheckForSlash)
{
  if (FileNameToCheckForSlash.startsWith((char *)"/"))
  {
    return FileNameToCheckForSlash;
  }
  else
  {
    return String((char *)"/" + FileNameToCheckForSlash);
  }
}
