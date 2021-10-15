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

#include <stdint.h>
#include <stdbool.h>

typedef struct wifiData_t {
   char ssid[64];
   char passwd[128];
} wifiData_t;

#define WIFI_TABLE_SIZE 10
#define MAX_SSID_LENGTH 64
#define MAX_PASSWD_LENGTH 128

typedef struct blinkieAppData_t {
   char *ipName;
   char apSsid[MAX_SSID_LENGTH+1];
   char apPasswd[MAX_PASSWD_LENGTH+1];
   uint8_t apChan;
   uint8_t wifiCount;
   wifiData_t wifi[WIFI_TABLE_SIZE];
   uint8_t apMac[6];
   uint8_t staMac[6];
   bool testRelease;
   uint8_t tzone;
} blinkieAppData_t;

void loadSettings();
void initSettings();
void storeSettings();

extern blinkieAppData_t xAppData;

void addWifiGateway( char * ssid, char * password);

char * getWifiSsid( uint8_t index);
char * getWifiPasswd( uint8_t index);
char * getHSSsid( void);
char * getHSPasswd( void);
uint8_t getHSChan( void);
bool getSystemType( void);

void setHSSsid( char * hssid);
void setHSPasswd( char * hpasswd);
void setHSChan( uint8_t hchan);
void setTZ( char * wpasswd);
void setSystemType( bool);

void setWifiSsid( uint8_t index, char * wssid);
void setWifiPasswd( uint8_t index, char * wpasswd);
void WifiCleanup( void );


#endif
