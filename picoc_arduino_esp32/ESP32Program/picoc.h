/* picoc external interface. This should be the only header you need to use if
 * you're using picoc as a library. Internal details are in interpreter.h */
#ifndef PICOC_H
#define PICOC_H

//#define BLINKY
//#define NO_HASH_INCLUDE

//#define DEBUG_LEXER

/* picoc version number */
#ifdef VER
#define PICOC_VERSION "v2.2 beta r" VER         /* VER is the subversion version number, obtained via the Makefile */
#else
#define PICOC_VERSION "v2.2"
#endif
#define BUFSIZE 12000
#define HEAP_SIZE 45000
void sendContent(char *it); 
extern void send(char *it);
extern void sendc(char it);
extern void sendln(char * it);
extern void sprint(char * what);

/* handy definitions */
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif
#define UNIX_HOST
#ifdef WIN32
#include <setjmp.h>
/* mark where to end the program for platforms which require this */
extern jmp_buf PicocExitBuf;
/* this has to be a macro, otherwise errors will occur due to the stack being corrupt */
#define PicocPlatformSetExitPoint() setjmp(PicocExitBuf)
#endif

#ifdef UNIX_HOST

#define PICOC_STACK_SIZE 45000
#include <setjmp.h>
#define BUILTIN_MINI_STDLIB
/* mark where to end the program for platforms which require this */
extern jmp_buf PicocExitBuf;

/* this has to be a macro, otherwise errors will occur due to the stack being corrupt */
#define PicocPlatformSetExitPoint() setjmp(PicocExitBuf)
#endif

/* parse.c */
void PicocParse(const char *FileName, const char *Source, int SourceLen, int RunIt, int CleanupNow, int CleanupSource, int EnableDebugger);
void PicocParseInteractive();

/* platform.c */
void PicocCallMain(int argc, char **argv);
void PicocInitialise(int StackSize);
void PicocCleanup();
void PicocPlatformScanFile(const char *FileName);

extern int PicocExitValue;


/* include.c */
void PicocIncludeAllSystemHeaders(int inctmpl);

#endif /* PICOC_H */
