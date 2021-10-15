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
//   This is the web server, wifi, and internet stuff for:
//    * an ESP32 based 4x4x8 tower.
//    * an ESP32 based 4x4x4 cube
//    * an ESP32 based 4x4x8 tixClock
//   Why does a Blinkie need to talk to the internet? because it can!
//**********************************************************************
//   We are running a dual mode, we can connect to a network, and operate
//   as a hotspot too. 
//**********************************************************************

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "string.h"

#include "disk_system.h"
#include "version.h"
#include "global.h"
#include "download_file.h"
#include "file_sha.h"

static const char *TAG = "updsk";

/*******************************************************************************
    PURPOSE: verify if needed file and correct sha

    INPUTS: file
            sha

    RETURN CODE:
           2 - found sha wrong
           1 - found sha match
           0 - not found

    NOTES: 
       

*******************************************************************************/
void auditFile_cb( char type, char * size, char * sha, char *name, void *data) {

    for (int8_t i=0; i < neededFileCount; i++) {
        if (strcmp(files[i].name, name)==0) {
            if (strcmp(files[i].sha, sha)==0) {
                files[i].status = 1;
		break;
            } else {
                files[i].status = 2;
		break;
            }
        }
    }
}

/*******************************************************************************
    PURPOSE: check each file found

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/

void auditDiskFiles( void* pvParameters ) {
    char * url;
    char * file;

    diskDirCfg_t req = {
        .path = "/spiffs",
        .line_cb = auditFile_cb
    };

    disk_dir(req);

    for (int8_t i=0; i < neededFileCount; i++) {
        if (files[i].status != 1) {
            asprintf( &url, "%s%02d.%02d.%02d%s/%s",CONFIG_BASE_CLOUD_URL, MAJOR, MINOR, BUILD, (getSystemType())?"_test":"", files[i].name);
            asprintf( &file, "/spiffs/%s", files[i].name);
            printf("Need to download %s %s\n", url, files[i].name);
            download_file( file, url);
	    free(url);
	    free(file);
        }
    }
    vTaskDelete( NULL );
}


