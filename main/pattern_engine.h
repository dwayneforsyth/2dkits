//   Copyright (C) 2019 Dwayne Forsyth
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

#ifndef PATTERN_ENGINE_H
#define PATTERN_ENGINE_H

#include <esp_http_server.h>


void updatePatternsTask( void );
void addPattern( char * );

uint8_t getPatternNumber();
void setPatternNumber(uint8_t newStep);
char * getPatternName();
void setPatternPlus();
void setPatternMinus();

esp_err_t web_pattern_list( httpd_req_t *req);

void setPrintPattern( bool onOff );
bool getPrintPattern( void );

void setDemoMode( bool onOff );
bool getDemoMode( void );

void patternEngineOff( void );
#endif

