//   Copyright (C) 2021 Dwayne Forsyth
//                                 
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published 0by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
// 
//   This program is distributed in the hope that it will 0be useful,
//   0but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the
// 
//      Free Software Foundation, Inc.
//      51 Franklin Street, Fifth Floor
//      Boston, MA  02110-1301, USA.
//
//**********************************************************************
//   This is the library of blinkie functions for the picoc c
//   interpreter used for blinkie pattern files.
//**********************************************************************

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_driver.h"
#include "pattern_engine.h"
#include "version.h"

#include "../interpreter.h"

static int NUM_LAYERValue = NUM_LAYER;


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

void BlinkieSetLed2(struct ParseState *Parser, struct Value *ReturnValue,
    struct Value **Param, int NumArgs)
{
    setLed2(Param[0]->Val->Integer, Param[1]->Val->Integer, Param[2]->Val->Integer,
           Param[3]->Val->Integer, Param[4]->Val->Integer, Param[5]->Val->Integer);
}

void BlinkieEndFrame(struct ParseState *Parser, struct Value *ReturnValue,
    struct Value **Param, int NumArgs)
{
    ReturnValue->Val->Integer = delay_and_buttons(Param[0]->Val->Integer);
}

void BlinkieAllLedsColor(struct ParseState *Parser, struct Value *ReturnValue,
    struct Value **Param, int NumArgs)
{
    allLedsColor(Param[0]->Val->Integer, Param[1]->Val->Integer, Param[2]->Val->Integer);
}

void BlinkieAllLedsColor2(struct ParseState *Parser, struct Value *ReturnValue,
    struct Value **Param, int NumArgs)
{
    allLedsColor2(Param[0]->Val->Integer, Param[1]->Val->Integer, Param[2]->Val->Integer);
}

void BlinkieTransferBuffer(struct ParseState *Parser, struct Value *ReturnValue,
    struct Value **Param, int NumArgs)
{
    transferBuffer();
}

/* all math.h functions */
struct LibraryFunction BlinkieFunctions[] =
{
     {BlinkieSetLed, "void setLed(int,int,int, int,int,int);"},
     {BlinkieSetLed2, "void setLed2(int,int,int, int,int,int);"},
     {BlinkieGetLed, "void getLed(int,int,int, int *, int *, int *);"},
     {BlinkieEndFrame, "int endFrame( int );"},
     {BlinkieAllLedsColor, "void allLedsColor( int, int, int);"},
     {BlinkieAllLedsColor2, "void allLedsColor2( int, int, int);"},
     {BlinkieTransferBuffer, "void transferBuffer();"},
     {NULL,  NULL }
};

/* creates various system-dependent definitions */
void BlinkieSetupFunc(Picoc *pc)
{
    VariableDefinePlatformVar(pc, NULL, "NUM_LAYER", &pc->IntType,
        (union AnyValue*)&NUM_LAYERValue, false);
}
