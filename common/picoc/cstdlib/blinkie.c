/*  */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_driver.h"
#include "pattern_engine.h"

#include "../interpreter.h"



void BlinkieGetLed(struct ParseState *Parser, struct Value *ReturnValue,
    struct Value **Param, int NumArgs)
{
    getLed(Param[0]->Val->Integer, Param[1]->Val->Integer, Param[2]->Val->Integer,
           Param[3]->Val->Pointer, Param[4]->Val->Pointer, Param[5]->Val->Pointer);
}

void BlinkieSetLed(struct ParseState *Parser, struct Value *ReturnValue,
    struct Value **Param, int NumArgs)
{
    setLed(Param[0]->Val->Integer, Param[1]->Val->Integer, Param[2]->Val->Integer,
           Param[3]->Val->Integer, Param[4]->Val->Integer, Param[5]->Val->Integer);
}

void BlinkieEndFrame(struct ParseState *Parser, struct Value *ReturnValue,
    struct Value **Param, int NumArgs)
{
    delay_and_buttons(Param[0]->Val->Integer);
}

void BlinkieSetPatternRun(struct ParseState *Parser, struct Value *ReturnValue,
    struct Value **Param, int NumArgs)
{
    setPatternRun(Param[0]->Val->Integer);
}

void BlinkieTaskDelay(struct ParseState *Parser, struct Value *ReturnValue,
    struct Value **Param, int NumArgs)
{
    vTaskDelay(Param[0]->Val->Integer / portTICK_PERIOD_MS);
}

/* all math.h functions */
struct LibraryFunction BlinkieFunctions[] =
{
     {BlinkieSetLed, "void setLed(int,int,int, int,int,int);"},
     {BlinkieGetLed, "void getLed(int,int,int, int *, int *, int *);"},
     {BlinkieEndFrame, "void endFrame( int );"},
     {BlinkieSetPatternRun, "void setPatternRun( int );"},
     {BlinkieTaskDelay, "void vTaskDelay( int );"},
     {NULL,  NULL }
};

/* creates various system-dependent definitions */
void BlinkieSetupFunc(Picoc *pc)
{
}
