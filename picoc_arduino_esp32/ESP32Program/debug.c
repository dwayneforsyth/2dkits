/* picoc interactive debugger */

#ifndef NO_DEBUGGER
extern char SingleStep;
extern char DebugBar[];
extern char sendChunked;
#include "interpreter.h"
#include <stdio.h>

#define BREAKPOINT_TABLE_SIZE 21
#define BREAKPOINT_HASH(p) ( ((unsigned long)(p)->FileName) ^ (((p)->Line << 16) | (0 << 16)) )

struct Table BreakpointTable;
struct TableEntry *BreakpointHashTable[BREAKPOINT_TABLE_SIZE];
int BreakpointCount = 0;
int DebugManualBreak = FALSE;

int DebugStep(struct ParseState * Parser);


/* initialise the debugger by clearing the breakpoint table */
void DebugInit()
{
    TableInitTable(&BreakpointTable, &BreakpointHashTable[0], BREAKPOINT_TABLE_SIZE, TRUE);
    BreakpointCount = 0;
}

/* free the contents of the breakpoint table */
void DebugCleanup()
{
    struct TableEntry *Entry;
    struct TableEntry *NextEntry;
    int Count;
    
    for (Count = 0; Count < BreakpointTable.Size; Count++)
    {
        for (Entry = BreakpointHashTable[Count]; Entry != NULL; Entry = NextEntry)
        {
            NextEntry = Entry->Next;
            HeapFreeMem(Entry);
        }
    }
}

/* search the table for a breakpoint */
static struct TableEntry *DebugTableSearchBreakpoint(struct ParseState *Parser, int *AddAt)
{
    struct TableEntry *Entry;
    int HashValue = BREAKPOINT_HASH(Parser) % BreakpointTable.Size;
    
    for (Entry = BreakpointHashTable[HashValue]; Entry != NULL; Entry = Entry->Next)
    {
        if (Entry->p.b.FileName == Parser->FileName && Entry->p.b.Line == Parser->Line && Entry->p.b.CharacterPos == 0)
            return Entry;   /* found */
    }
    
    *AddAt = HashValue;    /* didn't find it in the chain */
    return NULL;
}

/* set a breakpoint in the table */
void DebugSetBreakpoint(struct ParseState *Parser)
{
    int AddAt;
    struct TableEntry *FoundEntry = DebugTableSearchBreakpoint(Parser, &AddAt);
    
    if (FoundEntry == NULL)
    {   
        /* add it to the table */
        struct TableEntry *NewEntry = HeapAllocMem(sizeof(struct TableEntry));
        if (NewEntry == NULL)
            ProgramFail(NULL,(char *)"out of memory");
            
        NewEntry->p.b.FileName = Parser->FileName;
        NewEntry->p.b.Line = Parser->Line;
        NewEntry->p.b.CharacterPos = 0;//Parser->CharacterPos;
        NewEntry->Next = BreakpointHashTable[AddAt];
        BreakpointHashTable[AddAt] = NewEntry;
        BreakpointCount++;
    }
}

/* delete a breakpoint from the hash table */
int DebugClearBreakpoint(struct ParseState *Parser)
{
    struct TableEntry **EntryPtr;
    int HashValue = BREAKPOINT_HASH(Parser) % BreakpointTable.Size;
    
    for (EntryPtr = &BreakpointHashTable[HashValue]; *EntryPtr != NULL; EntryPtr = &(*EntryPtr)->Next)
    {
        struct TableEntry *DeleteEntry = *EntryPtr;
        if (DeleteEntry->p.b.FileName == Parser->FileName && DeleteEntry->p.b.Line == Parser->Line && DeleteEntry->p.b.CharacterPos == Parser->CharacterPos)
        {
            *EntryPtr = DeleteEntry->Next;
            HeapFreeMem(DeleteEntry);
            BreakpointCount--;

            return TRUE;
        }
    }

    return FALSE;
}

extern int GCheckTrailingSemicolon;
int mybrk;

/* before we run a statement, check if there's anything we have to do with the debugger here */
void DebugCheckStatement(struct ParseState *Parser)
{
    int DoBreak = FALSE;
    int AddAt;
    char fbuf[11];
    /* has the user manually pressed break? */
    if (DebugManualBreak)
    {
        DoBreak = TRUE;
        DebugManualBreak = FALSE;
    }
    
    /* is this a breakpoint location? */
    if (BreakpointCount != 0 && DebugTableSearchBreakpoint(Parser, &AddAt) != NULL)
       DoBreak = TRUE;
    
    /* handle a break */
    mybrk=0;
    if (DoBreak)
    {
      sendContent((char *)"Handling a break<br>"); //<br>Enter c to continue, s to single step<br>?xxx to list variable xxx<br>bnnnn to set breakpoint, l to list program\n");
      sprint((char *)"Now going to CurLinePrint\n");
      CurLinePrint(Parser);
      Parser->Mode=RunModeRun;
      //sprint((char *)"DebugMode=");snprintf((char *)&fbuf,10,(char *)"%d",(int)Parser->DebugMode);//DebugPrintln((char *)&fbuf);
      Parser->DebugMode=1;
          DoBreak=TRUE;
          mybrk=1;

    }
    if (SingleStep) CurLinePrint(Parser);
}

extern struct StackFrame* TopStackFrame;
const char * Ident;

void PrintValue(struct Value * Var)
{
  char fbuf[351];
  struct ValueType * Typ=Var->Typ;
      switch (Typ->Base)
    {
        case TypeVoid:           snprintf((char *)&fbuf,350,(char *)"void %x", Var->Val->UnsignedInteger); break;
        case TypeInt:            snprintf((char *)&fbuf,350,(char *)"int %d", Var->Val->Integer); break;
        case TypeShort:          snprintf((char *)&fbuf,350,(char *)"short %d", Var->Val->ShortInteger); break;
        case TypeChar:           snprintf((char *)&fbuf,350,(char *)"char %c", (char )Var->Val->ShortInteger); break;
        case TypeLong:           snprintf((char *)&fbuf,350,(char *)"long %ld", Var->Val->LongInteger); break;
        case TypeUnsignedInt:    snprintf((char *)&fbuf,350,(char *)"unsigned int %d", Var->Val->UnsignedInteger); break;
        case TypeUnsignedShort:  snprintf((char *)&fbuf,350,(char *)"unsigned short %d", Var->Val->UnsignedShortInteger); break;
        case TypeUnsignedLong:   snprintf((char *)&fbuf,350,(char *)"unsigned long %lu", Var->Val->UnsignedLongInteger); break;
#ifndef NO_FP
        case TypeFP:             snprintf((char *)&fbuf,350,(char *)"double %f", Var->Val->FP); break;
#endif
        case TypePointer:       if (Typ->FromType)  snprintf((char *)&fbuf,350,(char *)"%s %lx",(char *)"pointer*");  break;
        case TypeArray:          snprintf((char *)&fbuf,350,(char *)"%s ",(char *)"array");  sendContent((char *)&fbuf); snprintf((char *)&fbuf,350,(char *)"[");sendContent((char *)&fbuf); if (Typ->ArraySize != 0)  {snprintf((char *)&fbuf,350,(char *)"%d]",Typ->ArraySize);sendContent((char *)&fbuf);strcpy((char *)&fbuf,(char *)"");} break;

        case TypeFunction:       snprintf((char *)&fbuf,350,(char *)"function");  break;
        case TypeMacro:          snprintf((char *)&fbuf,350,(char *)"macro"); break;
        case TypeStruct:         snprintf((char *)&fbuf,350,(char *)"struct");break;
        case TypeUnion:          snprintf((char *)&fbuf,350,(char *)"union"); break;
        case TypeEnum:           snprintf((char *)&fbuf,350,(char *)"enum"); break;
        case TypeGotoLabel:      snprintf((char *)&fbuf,350,(char *)"gotoLabel"); break;
        case Type_Type:          snprintf((char *)&fbuf,350,(char *)"type"); break;
    }
    if (strlen((char *)&fbuf)!=0) sendContent((char *)&fbuf);
}
void prntLines(struct ParseState * Parser);
extern char WebResponse;
extern char DebugProgramName[33];
int DebugStep(struct ParseState * Parser)
{
            //sprint((char *)"Line =");sPrintln(Parser->Line);
            CurLinePrint(Parser);
            return ParseResultOk;
          if (WebResponse==0) return;
          char  cln[128];
  rdchar:
          PlatformGets((char *)&cln[0],127,(char *)"debug-> ");
          if (cln[0]=='c')
          {
            Parser->DebugMode=0;
            DebugClearBreakpoint(Parser);
            mybrk=0;
            return ParseResultOk;
          }
          if (cln[0]=='s')
          {
            CurLinePrint(Parser);
            return ParseResultOk;
          }
          if (cln[0]=='?')
          {
            char * cp=strstr((char *)&cln[0],(char *)"\n");
            if (cp!=NULL) *cp=0;
            // print symbol
            struct Value *VariableValue = NULL;
            int rslt;
            Ident=&cln[1];
            char * mycp=TableStrRegister(Ident);
            //if (VariableDefined((char *)&cln[1]))
            {
                if (TopStackFrame!=NULL) rslt=TableGet(&TopStackFrame->LocalTable,mycp,&VariableValue,NULL,NULL,NULL);
                if (rslt==0) rslt=TableGet(&GlobalTable,mycp,&VariableValue,NULL,NULL,NULL);
            }

            if (rslt!=0) PrintValue(VariableValue);
            else printf((char *)"%s not defined",(char *)&cln[1]);

            goto rdchar;
          }
          if (cln[0]=='l')
          {
            // list code
            prntLines(Parser);
            goto rdchar;
          }
          fflush(stdin);
          goto rdchar;


         return ParseResultOk;
}


#endif /* !NO_DEBUGGER */
