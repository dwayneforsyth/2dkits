
#include "interpreter.h"
#include "picoc.h"

/* mark where to end the program for platforms which require this */
jmp_buf PicocExitBuf;

#ifndef NO_DEBUGGER
#include <signal.h>

static void BreakHandler(int Signal)
{
    PlatformPrintf((char *)"break\n");
    DebugManualBreak = TRUE;
}

void PlatformInit()
{
    /* capture the break signal and pass it to the debugger */
 //   break_pc = pc;
 //   signal(SIGINT, BreakHandler);
}
#else
void PlatformInit()
{
}
#endif

void PlatformCleanup()
{
}


/* read a file into memory */
char *PlatformReadFile(const char *FileName);

void printLines(char * what);
void delay(int n);
void shortEspRestart();

/* read and scan a file for definitions */
void PicocPlatformScanFile(const char *filename)
{
  char fname[33];
  //Ssend((char *)"in PicocPlatformScanFile\n");
  sprintf((char *)&fname,(char *)"filename length is %d\n",strlen(filename));
  //Ssend((char *)&fname);
  if (*filename!='/') sprintf((char *)&fname,(char *)"/%s",filename);
  else sprintf((char *)&fname,(char *)"%s",filename);
  //Ssend((char *)"Running ");//Ssend((char *)&fname);//Ssend((char *)"\n");
    char *SourceStr = PlatformReadFile((char *)&fname);
    /* ignore "#!/path/to/picoc" .. by replacing the "#!" with "//" */
    if (SourceStr != NULL && SourceStr[0] == '#' && SourceStr[1] == '!') 
    { 
        SourceStr[0] = '/'; 
        SourceStr[1] = '/'; 
    }
    printLines(SourceStr);
    //Ssend((char *)"Done printLines\n");
    PicocParse(filename, SourceStr, strlen(SourceStr), TRUE, FALSE, TRUE, TRUE);
}
/* exit the program */
void PlatformExit(int RetVal)
{
    sprint((char *)"</textarea>");
    //for (int i=0;i<20;i++) { sendContent((char *)".");delay(10); }
    PicocExitValue = RetVal;
    sprint((char *)"<h2>Restarting ESP32 ....</h2>");
    delay(3000);
    shortEspRestart();
}
