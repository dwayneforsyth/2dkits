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

#pragma once

#include "esp_http_server.h"

typedef struct {
   char *path;
   void (*header_cb)(char *, void *);
   void (*line_cb)(char, char *, char *, char *, void *);
   void (*footer_cb)(uint64_t, int, uint32_t, uint32_t, void *);
   void *data;
} diskDirCfg_t;


void initialise_disk(void);
void disk_dir( diskDirCfg_t req);
