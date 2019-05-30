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

#ifndef GLOBAL_H
#define GLOBAL_H

typedef struct wifiData_t {
   char ssid[64];
   char passwd[128];
} wifiData_t;

#define WIFI_TABLE_SIZE 3

typedef struct blinkieAppData_t {
   char *ipName;
   wifiData_t wifi[WIFI_TABLE_SIZE];
} blinkieAppData_t;

#endif
