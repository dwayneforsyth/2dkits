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
//   This is the disk system for an ESP32 based 4x4x8 tower.
//   It handles all the applicaion communications with the disk or
//   file system.
//**********************************************************************

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include "time.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include <esp_http_server.h>

#include "pattern_engine.h"
#include "web_server.h"
#include "sha_file.h"
#include "disk_system.h"

static const char *TAG = "DISK";

/*******************************************************************************
    PURPOSE: init interface to flash disk

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void initialise_disk(void) {

    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 50,
      .format_if_mount_failed = false 
    };

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

}

/*******************************************************************************
    PURPOSE: get a disk dir

    INPUTS: diskDirCfg_t 

    RETURN CODE:
        NONE


*******************************************************************************/
void disk_dir(diskDirCfg_t req) {

    DIR *dir = NULL;
    struct dirent *ent;
    char type;
    char size[12];
    char tpath[255];
    char tbuffer[80];
    struct stat sb;
    struct tm *tm_info;
    int statok;

    // Open directory
    dir = opendir(req.path);
    if (!dir) {
        printf("Error opening directory\n");
        return;
    }

    // Read directory entries
    uint64_t total = 0;
    int nfiles = 0;

    if (req.header_cb !=NULL) req.header_cb(req.path, req.data);

    while ((ent = readdir(dir)) != NULL) {
        sprintf(tpath, req.path);
        if (req.path[strlen(req.path)-1] != '/') strcat(tpath,"/");
        strcat(tpath,ent->d_name);
        tbuffer[0] = '\0';

        // Get file stat
        statok = stat(tpath, &sb);

        if (statok == 0) {
            tm_info = localtime(&sb.st_mtime);
            strftime(tbuffer, 80, "%d/%m/%Y %R", tm_info);
        } else sprintf(tbuffer, "                ");

        if (ent->d_type == DT_REG) {
            type = 'f';
            nfiles++;
            if (statok) strcpy(size, "       ?");
            else {
                total += sb.st_size;
                if (sb.st_size < (1024*1024)) snprintf(size, sizeof(size), "%8d", (int)sb.st_size);
                else if ((sb.st_size/1024) < (1024*1024)) snprintf(size, sizeof(size), "%6dKB", (int)(sb.st_size / 1024));
                else snprintf(size, sizeof(size), "%6dMB", (int)(sb.st_size / (1024 * 1024)));
            }
	    fileSha(tpath, tbuffer);
        } else {
            type = 'd';
            strcpy(size, "       -");
        }
        if (req.line_cb != NULL) req.line_cb(type, size, tbuffer, ent->d_name, req.data);

    }

    uint32_t tot=0, used=0;
    esp_spiffs_info(NULL, &tot, &used);

    if (req.footer_cb != NULL) req.footer_cb(total, nfiles, tot, used, req.data);

    closedir(dir);
}
