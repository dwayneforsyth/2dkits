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

// ============================================================================

// fnmatch defines
#define	FNM_NOMATCH     1       // Match failed.
#define	FNM_NOESCAPE	0x01	// Disable backslash escaping.
#define	FNM_PATHNAME	0x02	// Slash must be matched by slash.
#define	FNM_PERIOD		0x04	// Period must be matched by period.
#define	FNM_LEADING_DIR	0x08	// Ignore /<tail> after Imatch.
#define	FNM_CASEFOLD	0x10	// Case insensitive search.
#define FNM_PREFIX_DIRS	0x20	// Directory prefixes of pattern match too.
#define	EOS	            '\0'

//-----------------------------------------------------------------------
static const char * rangematch(const char *pattern, char test, int flags)
{
  int negate, ok;
  char c, c2;

  /*
   * A bracket expression starting with an unquoted circumflex
   * character produces unspecified results (IEEE 1003.2-1992,
   * 3.13.2).  This implementation treats it like '!', for
   * consistency with the regular expression syntax.
   * J.T. Conklin (conklin@ngai.kaleida.com)
   */
  if ( (negate = (*pattern == '!' || *pattern == '^')) ) ++pattern;

  if (flags & FNM_CASEFOLD) test = tolower((unsigned char)test);

  for (ok = 0; (c = *pattern++) != ']';) {
    if (c == '\\' && !(flags & FNM_NOESCAPE)) c = *pattern++;
    if (c == EOS) return (NULL);

    if (flags & FNM_CASEFOLD) c = tolower((unsigned char)c);

    if (*pattern == '-' && (c2 = *(pattern+1)) != EOS && c2 != ']') {
      pattern += 2;
      if (c2 == '\\' && !(flags & FNM_NOESCAPE)) c2 = *pattern++;
      if (c2 == EOS) return (NULL);

      if (flags & FNM_CASEFOLD) c2 = tolower((unsigned char)c2);

      if ((unsigned char)c <= (unsigned char)test &&
          (unsigned char)test <= (unsigned char)c2) ok = 1;
    }
    else if (c == test) ok = 1;
  }
  return (ok == negate ? NULL : pattern);
}

//--------------------------------------------------------------------
static int fnmatch(const char *pattern, const char *string, int flags)
{
  const char *stringstart;
  char c, test;

  for (stringstart = string;;)
    switch (c = *pattern++) {
    case EOS:
      if ((flags & FNM_LEADING_DIR) && *string == '/') return (0);
      return (*string == EOS ? 0 : FNM_NOMATCH);
    case '?':
      if (*string == EOS) return (FNM_NOMATCH);
      if (*string == '/' && (flags & FNM_PATHNAME)) return (FNM_NOMATCH);
      if (*string == '.' && (flags & FNM_PERIOD) &&
          (string == stringstart ||
          ((flags & FNM_PATHNAME) && *(string - 1) == '/')))
              return (FNM_NOMATCH);
      ++string;
      break;
    case '*':
      c = *pattern;
      // Collapse multiple stars.
      while (c == '*') c = *++pattern;

      if (*string == '.' && (flags & FNM_PERIOD) &&
          (string == stringstart ||
          ((flags & FNM_PATHNAME) && *(string - 1) == '/')))
              return (FNM_NOMATCH);

      // Optimize for pattern with * at end or before /.
      if (c == EOS)
        if (flags & FNM_PATHNAME)
          return ((flags & FNM_LEADING_DIR) ||
                    strchr(string, '/') == NULL ?
                    0 : FNM_NOMATCH);
        else return (0);
      else if ((c == '/') && (flags & FNM_PATHNAME)) {
        if ((string = strchr(string, '/')) == NULL) return (FNM_NOMATCH);
        break;
      }

      // General case, use recursion.
      while ((test = *string) != EOS) {
        if (!fnmatch(pattern, string, flags & ~FNM_PERIOD)) return (0);
        if ((test == '/') && (flags & FNM_PATHNAME)) break;
        ++string;
      }
      return (FNM_NOMATCH);
    case '[':
      if (*string == EOS) return (FNM_NOMATCH);
      if ((*string == '/') && (flags & FNM_PATHNAME)) return (FNM_NOMATCH);
      if ((pattern = rangematch(pattern, *string, flags)) == NULL) return (FNM_NOMATCH);
      ++string;
      break;
    case '\\':
      if (!(flags & FNM_NOESCAPE)) {
        if ((c = *pattern++) == EOS) {
          c = '\\';
          --pattern;
        }
      }
      break;
      // FALLTHROUGH
    default:
      if (c == *string) {
      }
      else if ((flags & FNM_CASEFOLD) && (tolower((unsigned char)c) == tolower((unsigned char)*string))) {
      }
      else if ((flags & FNM_PREFIX_DIRS) && *string == EOS && ((c == '/' && string != stringstart) ||
    		  (string == stringstart+1 && *stringstart == '/')))
              return (0);
      else return (FNM_NOMATCH);
      string++;
      break;
    }
  // NOTREACHED
  return 0;
}

// ============================================================================


/*******************************************************************************
    PURPOSE: get a disk dir in ascii

    INPUTS:

    RETURN CODE:
        NONE

    NOTES: should make printer cb to support ascii and html in one function

*******************************************************************************/
//-----------------------------------------
void disk_dir_list(char *path, char *match) {

    DIR *dir = NULL;
    struct dirent *ent;
    char type;
    char size[12];
    char tpath[255];
    char tbuffer[80];
    struct stat sb;
    struct tm *tm_info;
    char *lpath = NULL;
    int statok;

    printf("\nList of Directory [%s]\n", path);
    printf("-----------------------------------\n");
    // Open directory
    dir = opendir(path);
    if (!dir) {
        printf("Error opening directory\n");
        return;
    }

    // Read directory entries
    uint64_t total = 0;
    int nfiles = 0;
    printf("T  Size      Date/Time         Name\n");
    printf("-----------------------------------\n");
    while ((ent = readdir(dir)) != NULL) {
        sprintf(tpath, path);
        if (path[strlen(path)-1] != '/') strcat(tpath,"/");
        strcat(tpath,ent->d_name);
        tbuffer[0] = '\0';

        if ((match == NULL) || (fnmatch(match, tpath, (FNM_PERIOD)) == 0)) {
            // Get file stat
            statok = stat(tpath, &sb);

            if (statok == 0) {
                tm_info = localtime(&sb.st_mtime);
                strftime(tbuffer, 80, "%d/%m/%Y %R", tm_info);
            }
            else sprintf(tbuffer, "                ");

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
            }
            else {
                type = 'd';
                strcpy(size, "       -");
            }

            printf("%c  %s  %s  %s\r\n",
                type,
                size,
                tbuffer,
                ent->d_name
            );
	    // This is a kludge, we are going to check for files thatend with a ".pat" and when
	    // we find one, we added it to the pattern engine. If will ignore files it has in
	    // its database.
	    if (strcmp(".pat",&(ent->d_name[strlen(ent->d_name)-4])) == 0) {
		addPattern(ent->d_name);
	    }
        }
    }
    if (total) {
        printf("-----------------------------------\n");
    	if (total < (1024*1024)) printf("   %8d", (int)total);
    	else if ((total/1024) < (1024*1024)) printf("   %6dKB", (int)(total / 1024));
    	else printf("   %6dMB", (int)(total / (1024 * 1024)));
    	printf(" in %d file(s)\n", nfiles);
    }
    printf("-----------------------------------\n");

    closedir(dir);

    free(lpath);

	uint32_t tot=0, used=0;
    esp_spiffs_info(NULL, &tot, &used);
    printf("SPIFFS: free %d KB of %d KB\n", (tot-used) / 1024, tot / 1024);
    printf("-----------------------------------\n\n");
}

/*******************************************************************************
    PURPOSE: get disk drive output in htlp format

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
//-----------------------------------------
esp_err_t web_disk_dir_list(httpd_req_t *req) {

    char *path = "/spiffs";
    char *match = NULL;
    DIR *dir = NULL;
    struct dirent *ent;
    char type;
    char size[12];
    char tpath[255];
    char tbuffer[123];
    char tbuffer2[129];
    struct stat sb;
    struct tm *tm_info;
    char *lpath = NULL;
    int statok;
    const char *error_open = "Error opening directory\n";
    const char *dir_header = "<table><tr><th>T<th>Size<th>Date/Time<th>Name\n";
    const char *dir_footer = "</table></body></html>\n";
    const char *dir_heading = "</div></td> <td valign=\"top\"><div id=\"navBreadCrumb\">Disk Dir</div><div class=\"centerColumn\" id=\"indexDefault\"><h1 id=\"indexDefaultHeading\"></h1>\n";

    httpd_resp_set_hdr(req, "Content-type", "text/html");
    file_get_handler(req, "/spiffs/header.html",true);

    httpd_resp_send_chunk(req, dir_heading, strlen(dir_heading));
    snprintf(tbuffer,sizeof(tbuffer),"List of Directory [%s]\n", path);
    httpd_resp_send_chunk(req, tbuffer, strlen(tbuffer));

    // Open directory
    dir = opendir(path);
    if (!dir) {
        printf("Error opening directory\n");
        httpd_resp_send_chunk(req, error_open, strlen(error_open));
        return ESP_FAIL;
    }
    // Read directory entries
    uint64_t total = 0;
    int nfiles = 0;
    httpd_resp_send_chunk(req, dir_header, strlen(dir_header));

    while ((ent = readdir(dir)) != NULL) {
        sprintf(tpath, path);
        if (path[strlen(path)-1] != '/') strcat(tpath,"/");
        strcat(tpath,ent->d_name);
        tbuffer[0] = '\0';

        if ((match == NULL) || (fnmatch(match, tpath, (FNM_PERIOD)) == 0)) {
            // Get file stat
            statok = stat(tpath, &sb);

            if (statok == 0) {
                tm_info = localtime(&sb.st_mtime);
                strftime(tbuffer, 80, "%d/%m/%Y %R", tm_info);
            }
            else sprintf(tbuffer, "                ");

            if (ent->d_type == DT_REG) {
                type = 'f';
                nfiles++;
                if (statok) strcpy(size, "       ?");
                else {
                    total += sb.st_size;
                    if (sb.st_size < (1024*1024)) sprintf(size,"%8d", (int)sb.st_size);
                    else if ((sb.st_size/1024) < (1024*1024)) sprintf(size,"%6dKB", (int)(sb.st_size / 1024));
                    else sprintf(size,"%6dMB", (int)(sb.st_size / (1024 * 1024)));
                }
            }
            else {
                type = 'd';
                strcpy(size, "       -");
            }

            snprintf(tbuffer2, sizeof(tbuffer2), "<tr><td>%c<td align=\"right\">%.40s<td>%.40s<td>%.40s\n",
                type,
                size,
                tbuffer,
                ent->d_name
            );
            httpd_resp_send_chunk(req, tbuffer2, strlen(tbuffer2));
	}
    }
    if (total) {
    	if (total < (1024*1024)) snprintf(tbuffer, sizeof(tbuffer), "   %8d", (int)total);
    	else if ((total/1024) < (1024*1024)) snprintf(tbuffer, sizeof(tbuffer), "   %6dKB", (int)(total / 1024));
    	else snprintf(tbuffer, sizeof(tbuffer), "   %6dMB", (int)(total / (1024 * 1024)));
    	snprintf(tbuffer2, sizeof(tbuffer2), "<tr><td colspan=4>%.40s in %d file(s)\n", tbuffer, nfiles);
        httpd_resp_send_chunk(req, tbuffer2, strlen(tbuffer2));
    }

    closedir(dir);

    free(lpath);

	uint32_t tot=0, used=0;
    esp_spiffs_info(NULL, &tot, &used);
    sprintf(tbuffer, "<tr><td colspan=4>SPIFFS: free %d KB of %d KB\n", (tot-used) / 1024, tot / 1024);
    httpd_resp_send_chunk(req, tbuffer, strlen(tbuffer));
    httpd_resp_send_chunk(req, dir_footer, strlen(dir_footer));
    file_get_handler(req, "/spiffs/footer.html",false);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}
