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

static const char *TAG = "updsk";

typedef struct {
    char *name;
    char *sha;
    uint8_t status;
} neededFiles_t;

#define NEEDED_FILE_COUNT 6
neededFiles_t  files[NEEDED_FILE_COUNT] = {
    {.name = "patterns.html", .sha="3964c504825ac56c1a69c542cbd53fa8689feb85"},
    {.name = "header.html",   .sha="faa0c2619ba7f07d159ebb3c85c91739931ad7b4"},
    {.name = "footer.html",   .sha="0289f31c9e872fd865eafa54636483a19d1b5018"},
    {.name = "about.html",    .sha="1131b80c280cdd797ec06810210e586b6c8278ff"},
    {.name = "index.html",    .sha="31582d37d13612e94df1751f7925e1372ebaad07"},
    {.name = "settings.html", .sha="2ff4835fbc295af7d91ac03c5a8db1575e905fdb"},
};

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
    uint8_t ret = 0;

    for (int8_t i=0; i < NEEDED_FILE_COUNT; i++) {
        if (strcmp(files[i].name, name)==0) {
            if (strcmp(files[i].sha, sha)==0) {
                ret = 1;
                files[i].status = 1;
		break;
            } else {
                files[i].status = 2;
                ret = 2;
		break;
            }
        }
    }
    ESP_LOGI(TAG, "file %-20s hash=>%s< check=%d", name, sha, ret );
}

/*******************************************************************************
    PURPOSE: check each file found

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/

void auditDiskFiles( void ) {
    char * url;
    char * file;

    diskDirCfg_t req = {
        .path = "/spiffs",
        .line_cb = auditFile_cb
    };

    disk_dir(req);

    for (int8_t i=0; i < NEEDED_FILE_COUNT; i++) {
        if (files[i].status != 1) {
            asprintf( &url, "http://www.2dkits.com/kits/kit25/%02d\.%02d\.%02d%s/%s",MAJOR, MINOR, BUILD, (getSystemType())?"_test":"", files[i].name); 
            asprintf( &file, "/spiffs/%s", files[i].name);
            printf("Need to download %s %s\n", url, files[i].name);
            download_file( file, url);
	    free(url);
	    free(file);
        }
    }
}


