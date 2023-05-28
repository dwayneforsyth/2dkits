/* picoc's interface to the underlying platform. most platform-specific code
 * is in platform/platform_XX.c and platform/library_XX.c */
 
#include "picoc.h"
#include "interpreter.h"
#include <stdarg.h>
#include <Arduino.h>
/* the value passed to exit() */
int PicocExitValue = 0;

/* initialise everything */
void PicocInitialise(int StackSize)
{
    //Ssend((char *)"Doing PlatformInit \n");
    PlatformInit();
    //Ssend((char *)"Done PlatformInit \n");
    BasicIOInit();
    //Ssend((char *)"Done BasicIOInit \n");
    HeapInit(StackSize);
    //Ssend((char *)"Done HeapInit \n");
    TableInit();
    //Ssend((char *)"Done TableInit \n");
    VariableInit();
    //Ssend((char *)"Done VariableInit \n");
    LexInit();
    //Ssend((char *)"Done PlatformInit \n");
    TypeInit();
    //Ssend((char *)"Done PlatformInit \n");
#ifndef NO_HASH_INCLUDE
    IncludeInit();
    //Ssend((char *)"Done IncludeInit \n");
#endif
    LibraryInit();
    //Ssend((char *)"Done LibraryInit \n");
#ifdef BUILTIN_MINI_STDLIB
    LibraryAdd(&GlobalTable,(char *)"c library", &CLibrary[0]);
    //Ssend((char *)"Done LibraryAdd \n");
  //  LibraryAdd(&GlobalTable,(char *)"stdlib", &StdlibFunctions[0]);
    CLibraryInit();
    //Ssend((char *)"Done CLibraryInit \n");
#endif
    //Ssend((char *)"Doing PlatformLibraryInit\n");
    PlatformLibraryInit();
    //Ssend((char *)"Done PlatformLibraryInit \n");
    DebugInit();
    //Ssend((char *)"Done DebugInit \n");
}
void sPrintInt(int what);
void sPrintln(char * what);
/* free memory */
int gfh();
void PicocCleanup()
{    
    int free = gfh();
    char fm[]="In PicocCleanup freeMemory=";
    Ssend((char *)&fm);
    DebugPrintInt(free);
    DebugCleanup(); 
  //Ssend((char *)"Done DebugCleanup\n");
#ifndef NO_HASH_INCLUDE
//    IncludeCleanup(); 
//  //Ssend((char *)"Done IncludeCleanup\n");
#endif
  Ssend((char *)"Doing ParseCleanup\n");
    //ParseCleanup(); 
  Ssend((char *)"Done ParseCleanup\n");
  free = gfh();
    sPrintln((char *)fm);
    sPrintInt(free);
  Ssend((char *)"Doing LexCleanup\n");
    LexCleanup(); 
  Ssend((char *)"Done LexCleanup\n");
  free = gfh();
    sPrintln((char *)fm);
    sPrintInt(free);
  Ssend((char *)"Doing VariableCleanup\n");
 //   VariableCleanup(); 
  Ssend((char *)"Done VariableCleanup\n");
  free = gfh();
    sPrintln((char *)fm);
    sPrintInt(free);
 //   TypeCleanup(); 
  Ssend((char *)"Done TypeCleanup\n");
  free = gfh();
    sPrintln((char *)fm);
    sPrintInt(free);
 //   TableStrFree(); 
  Ssend((char *)"Done TableStrFree\n");
  free = gfh();
    sPrintln((char *)fm);
    sPrintInt(free);
  //  HeapCleanup(); 
  Ssend((char *)"Done HeapCleanup\n");
  free = gfh();
    sPrintln((char *)fm);
    sPrintInt(free);
    PlatformCleanup(); 
  Ssend((char *)"Done PlatformCleanup\n");
  free = gfh();
    sPrintln((char *)fm);
    sPrintInt(free);
}

/* platform-dependent code for running programs */


#define CALL_MAIN_NO_ARGS_RETURN_VOID "main();"
#define CALL_MAIN_WITH_ARGS_RETURN_VOID "main(__argc,__argv);"
#define CALL_MAIN_NO_ARGS_RETURN_INT "__exit_value = main();"
#define CALL_MAIN_WITH_ARGS_RETURN_INT "__exit_value = main(__argc,__argv);"
void DeleteGlobals();
extern char DebugProgramName[33];
void PicocCallMain(int argc, char **argv)
{
    /* check if the program wants arguments */
    struct Value *FuncValue = NULL;
    
  
    if (!VariableDefined(TableStrRegister((char *)"main")))
        ProgramFail(NULL,(char *)"main() is not defined");
        
    VariableGet(NULL, TableStrRegister((char *)"main"), &FuncValue);
    if (FuncValue->Typ->Base != TypeFunction)
        ProgramFail(NULL,(char *)"main is not a function - can't call it");

    if (FuncValue->Val->FuncDef.NumParams != 0)
    {
        /* define the arguments */
        VariableDefinePlatformVar(NULL,(char *)"__argc", &IntType, (union AnyValue *)&argc, FALSE);
        VariableDefinePlatformVar(NULL,(char *)"__argv", CharPtrPtrType, (union AnyValue *)&argv, FALSE);
    }

    if (FuncValue->Val->FuncDef.ReturnType == &VoidType)
    {
        if (FuncValue->Val->FuncDef.NumParams == 0)
            PicocParse((char *)&DebugProgramName, CALL_MAIN_NO_ARGS_RETURN_VOID, strlen(CALL_MAIN_NO_ARGS_RETURN_VOID), TRUE, TRUE, FALSE, TRUE);
        else
            PicocParse((char *)&DebugProgramName, CALL_MAIN_WITH_ARGS_RETURN_VOID, strlen(CALL_MAIN_WITH_ARGS_RETURN_VOID), TRUE, TRUE, FALSE, TRUE);
    }
    else
    {
        VariableDefinePlatformVar(NULL,(char *)"__exit_value", &IntType, (union AnyValue *)&PicocExitValue, TRUE);
    
        if (FuncValue->Val->FuncDef.NumParams == 0)
            PicocParse((char *)&DebugProgramName, CALL_MAIN_NO_ARGS_RETURN_INT, strlen(CALL_MAIN_NO_ARGS_RETURN_INT), TRUE, TRUE, FALSE, TRUE);
        else
            PicocParse((char *)&DebugProgramName, CALL_MAIN_WITH_ARGS_RETURN_INT, strlen(CALL_MAIN_WITH_ARGS_RETURN_INT), TRUE, TRUE, FALSE, TRUE);
    }
}
extern char sendChunked;
extern char literal;
extern char toConsole;
void PrintSourceTextErrorLine(const char *FileName, const char *SourceText, int Line, int CharacterPos)
{
    int LineCount;
    char *LinePos;
    char *CPos;
    int CCount;
    char lb[428];
    literal=1;sendChunked=1;toConsole=0;
    sprint((char *)"</textarea><hr/>Program Error Information ...<hr/><textarea rows=\"6\" cols=\"128\" name=\"errors\" style=\"FONT-SIZE:14px; COLOR:#000000; LINE-HEIGHT:14px; FONT-FAMILY:Courier\">");
    if (strlen(FileName)==0) 
    {
      sprintf((char *)&lb,(char *)"Interactive input at line %d col %d\n",Line,CharacterPos);
      sprint((char *)&lb);return; 
    }
   if (SourceText != NULL)
    {
        //Ssend(SourceText); // for debug
        // find the source line 
        for (LinePos = SourceText, LineCount = 1; *LinePos != '\0' && LineCount < Line; LinePos++)
        {
            if (*LinePos == '\n')
                LineCount++;
        }
        strncpy((char *)&lb,LinePos,427);
        char * cp=strstr((char *)LinePos,(char *)"\n");
        *cp=0;
        sprintf((char *)&lb,(char *)"Error at Line %d and Character Pos %d\n>",Line,CharacterPos);sprint((char *)&lb);
        // display the line 
    /*    for (CPos = LinePos; *CPos != '\n' && *CPos != '\0'; CPos++)
            if (*CPos!=' ') PrintCh(*CPos);
            else sprint((char *)"&nbsp;");
            */
        //Ssend((char *)LinePos);
        literal=1;
        sprint((char *)LinePos);
        sprint((char *)"\n");
        //Ssend((char *)"\n");
        //Ssend((char *)"Finished print error source line\n");
        /*//display the error position
        for (CPos = LinePos, CCount = 0; *CPos != '\n' && *CPos != '\0' && (CCount < CharacterPos || *CPos == ' '); CPos++, CCount++)
        {
            if (*CPos == '\t')
                sprint((char *)"&nbsp;&nbsp;");
            else
                sprint((char *)"&nbsp;");
        }
        for (CPos=0;CPos<CharacterPos-1;CPos++) sprint((char *)"&nbsp;");
    */
    }
 //   else */
    {
        /* assume we're in interactive mode - try to make the arrow match up with the input text */
        for (CCount = 0; CCount < CharacterPos; CCount++)
            sprint((char *)"&nbsp");
    }
    sprintf((char *)&lb,(char *)"^\n%s:%d:%d ", FileName, Line, CharacterPos);sprint((char *)&lb);
}

/* display the source line and line number to identify an error */
void PlatformErrorPrefix(struct ParseState *Parser)
{
  //Ssend((char *)"In PlatformErrorPrefix\n");
    if (Parser != NULL)
       { PrintSourceTextErrorLine(Parser->FileName, Parser->SourceText, Parser->Line, Parser->CharacterPos);} //sprint((char *)"</textarea>"); }
}
extern void espRestart();
extern void shortEspRestart();
extern void sendEOF();
/* exit with a message */
extern char * getProgramArgs();
void ProgramFail(struct ParseState *Parser, const char *Message, ...)
{
    //Ssend((char *)"In ProgramFail\n");
    va_list Args;
    char lbuf[332];
    //sprintf((char *)&lbuf,(char *)"%RunMode is %d\n",Parser->Mode);
    //sprint((char *)&lbuf);
    PlatformErrorPrefix(Parser);
    //Ssend((char *)"Finished ErrorPrefix\n");
    va_start(Args, Message);
    PlatformVPrintf(Message, Args);
    va_end(Args);
    sprint((char *)"\n");
    literal=1;
    sprint((char *)"</textarea><h2>Restarting ESP32 .....</h2>");
    sprint((char *)"<h2>Redirecting to Edit Page for ");
    sprint((char *)Parser->FileName);
    sprint((char *)" in 30 seconds</h2>");
    sprint((char *)"<h2>You can also click Open button to go there quicker</h2>");
    //Ssend((char *)Parser->FileName);
    strncpy((char *)&lbuf,(char *)"<br><meta http-equiv=\"refresh\" content=\"30; url=./edit?FileName=",331);
    //Ssend((char *)&lbuf);
    strncat((char *)&lbuf,(char *)Parser->FileName,331);
    //Ssend((char *)&lbuf);
    strncat((char *)&lbuf,(char *)"&programArgs=",331);
    //Ssend((char *)&lbuf);
    strncat((char *)&lbuf,getProgramArgs(),331);
    //Ssend((char *)&lbuf);
    strncat((char *)&lbuf,(char *)"&edit=Edit&programArgs=\" />",331);
    //Ssend((char *)&lbuf);
    sprint((char *)&lbuf);
    sendEOF();
    sendEOF();
    delay(2500);
    shortEspRestart();
   // PlatformExit(1);
}

/* like ProgramFail() but gives descriptive error messages for assignment */
void AssignFail(struct ParseState *Parser, const char *Format, struct ValueType *Type1, struct ValueType *Type2, int Num1, int Num2, const char *FuncName, int ParamNo)
{
    sprint((char *)"In AssignFail\n");
  
    PlatformErrorPrefix(Parser);
    PlatformPrintf((char *)"can't %s ", (FuncName == NULL) ? "assign" : "set");   
   // sprint((char *)"in AssignFail, Format is ");sprint(Format);sprint((char *)"\n");
    //char lb[128];sprintf((char *)&lb,(char *)"Types are %d and %d\n",Type1->Base,Type2->Base);sprint((char *)&lb);
    if (Type1 != NULL)
        PlatformPrintf(Format, Type1, Type2);
    else
        PlatformPrintf(Format, Num1, Num2);
    
    if (FuncName != NULL)
        PlatformPrintf((char *)" in argument %d of call to %s()", ParamNo, FuncName);
        
    ProgramFail(NULL,(char *)"");
}

/* exit lexing with a message */
void LexFail(struct LexState *Lexer, const char *Message, ...)
{
    sprint((char *)"In LexFail\n");
    va_list Args;
    //Ssend((char *)"in LexFail\n");
    PrintSourceTextErrorLine(Lexer->FileName, Lexer->SourceText, Lexer->Line, Lexer->CharacterPos);
    va_start(Args, Message);
    PlatformVPrintf(Message, Args);
    va_end(Args);
    PlatformPrintf((char *)"\n");
    PlatformExit(1);
}

/* printf for compiler error reporting */
void PlatformPrintf(const char *Format, ...)
{
    va_list Args;
    
    va_start(Args, Format);
    PlatformVPrintf(Format, Args);
    va_end(Args);
}

void PlatformVPrintf(const char *Format, va_list Args)
{
    const char *FPos;
    char b[80];
    //Ssend(Format);//Ssend((char *)"\n");
    for (FPos = Format; *FPos != '\0'; FPos++)
    {
        if (*FPos == '%')
        {
            FPos++;
            switch (*FPos)
            {
            case 's': { char * arg =(char *)va_arg(Args, char *);sprint(arg); //Ssend((char *)"VPF for %s\n");
            break; }
            case 'd': { unsigned int ui=va_arg(Args,unsigned int);sprintf((char *)&b,(char *)"%d",ui);sprint((char *)&b);//Ssend((char *)"VPF for %d\n");
            break; }
            case 'c': { int ii=va_arg(Args, int);sprintf((char *)&b,(char *)"%c",ii); sprint((char *)&b);//Ssend((char *)"VPF for %c\n");
            break; }
            case 't': { struct ValueType * vt=(struct ValueType *)va_arg(Args,struct ValueType *);PrintType(vt);//Ssend((char *)"VPF for %t\n");
            break; }
#ifndef NO_FP
            case 'f': { float f=va_arg(Args, double); sprintf((char *)&b,(char *)"%f",f); sprint((char *)&b);//Ssend((char *)"VPF for %f\n");
            break; }
#endif
            case '%': { sprint((char *)"%"); //Ssend((char *)"VPF for %%\n");
            break; }
            case '\0': FPos--; break;
            }
        }
        else
            { sprintf((char *)&b,(char *)"%c",*FPos);b[1]=0;sprint((char *)&b);//Ssend((char *)&b);
            }
    }
}

/* make a new temporary name. takes a static buffer of char [7] as a parameter. should be initialised to "XX0000"
 * where XX can be any characters */
char *PlatformMakeTempName(char *TempNameBuffer)
{
    int CPos = 5;
    
    while (CPos > 1)
    {
        if (TempNameBuffer[CPos] < '9')
        {
            TempNameBuffer[CPos]++;
            return TableStrRegister(TempNameBuffer);
        }
        else
        {
            TempNameBuffer[CPos] = '0';
            CPos--;
        }
    }

    return TableStrRegister(TempNameBuffer);
}
