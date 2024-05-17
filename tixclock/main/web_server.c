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
//   This is the web server, wifi, and internet stuff for:
//    * an ESP32 based 4x4x8 tower.
//    * an ESP32 based 4x4x4 cube
//    * an ESP32 based 4x4x8 tixClock
//   Why does a Blinkie need to talk to the internet? because it can!
//**********************************************************************
//   We are running a dual mode, we can connect to a network, and operate
//   as a hotspot too.
//**********************************************************************


/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <time.h>

#include <esp_http_server.h>
#include <esp_sntp.h>
#include "mdns.h"

#include "disk_system.h"
#include "pattern_engine.h"
#include "global.h"
#include "version.h"
#include "upgrade_disk.h"
#include "download_file.h"
#include "dfu.h"
#include "captdns.h"

extern blinkieAppData_t xAppData;

static const char *TAG="WEB";


/*******************************************************************************
    PURPOSE: pushes context to the web connection based on the token passed in.

    INPUTS:
        Web connection
        Token

    RETURN CODE:
        error code

    NOTES:
        sends data down the web connection.

*******************************************************************************/

esp_err_t lookupToken(httpd_req_t *req, char *token) {
    char tBuffer[110];
    time_t now;
    struct tm timeinfo;
    uint8_t i = 0;


    if (strcmp("%patternNumber",token)==0) {
#ifndef TIXCLOCK
        sprintf(tBuffer, "%d",getPatternNumber());
        httpd_resp_send_chunk(req, tBuffer, strlen(tBuffer));
    } else if (strcmp("%patternName",token)==0) {
        sprintf(tBuffer, "%s",getPatternName());
        httpd_resp_send_chunk(req, tBuffer, strlen(tBuffer));
#endif
    } else if (strcmp("%timeDate",token)==0) {
        time(&now);
        localtime_r(&now, &timeinfo);
        strftime(tBuffer, 80, "%c", &timeinfo);
        httpd_resp_send_chunk(req, tBuffer,strlen(tBuffer));
    } else if (strcmp("%hssid",token)==0) {
        printf(">%s<", getHSSsid());
        sprintf(tBuffer, "%s", getHSSsid());
        httpd_resp_send_chunk(req, tBuffer,strlen(tBuffer));
    } else if (strcmp("%hpasswd",token)==0) {
        sprintf(tBuffer,"[None]"); //DDF hard codded
        httpd_resp_send_chunk(req, tBuffer,strlen(tBuffer));
    } else if (strncmp("%wssid",token,6)==0) {
        i = token[6] - '0';
        if (i<10) {
            sprintf(tBuffer, "%s",getWifiSsid(i));
        } else {
            sprintf(tBuffer, "[software error]");
        }
        httpd_resp_send_chunk(req, tBuffer,strlen(tBuffer));
    } else if (strncmp("%wpasswd",token,8)==0) {
        i = token[8] - '0';
        if (i<10) {
            sprintf(tBuffer, "%s",getWifiPasswd(i));
        } else {
            sprintf(tBuffer, "[software error]");
        }
        httpd_resp_send_chunk(req, tBuffer,strlen(tBuffer));
    } else if (strncmp("%hchsel",token,7)==0) {
        //DDF send nothing - hard codded
    } else if (strncmp("%rgb",token,4)==0) {
        //DDF send nothing - hard codded
    } else if (strncmp("%tz",token,3)==0) {
	sprintf(tBuffer,"%%tz%02d", xAppData.tzone);
        if (strcmp(tBuffer, token)==0) {
            sprintf(tBuffer, " selected ");
            httpd_resp_send_chunk(req, tBuffer,strlen(tBuffer));
        }
    } else if (strncmp("%tf",token,3)==0) {
        if ((strcmp("%tf12", token)==0) && (xAppData.tformat == true)) {
            sprintf(tBuffer, " selected ");
            httpd_resp_send_chunk(req, tBuffer,strlen(tBuffer));
        } else if ((strcmp("%tf24", token)==0) && (xAppData.tformat == false)) {
            sprintf(tBuffer, " selected ");
            httpd_resp_send_chunk(req, tBuffer,strlen(tBuffer));
        } else {
            ESP_LOGI(TAG, "DDF time format error >%s<",token);
	}
    } else if (strcmp("%sasip",token)==0) {
        if (xAppData.ipName != NULL) {
            sprintf(tBuffer, "%s", xAppData.ipName);
        } else {
            sprintf(tBuffer, "[None]");
        }
        httpd_resp_send_chunk(req, tBuffer,strlen(tBuffer));
    } else if (strcmp("%apmac",token)==0) {
        sprintf(tBuffer, "%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X",
                xAppData.apMac[0], xAppData.apMac[1], xAppData.apMac[2],
                xAppData.apMac[3], xAppData.apMac[4], xAppData.apMac[5]);
        httpd_resp_send_chunk(req, tBuffer,strlen(tBuffer));
    } else if (strcmp("%stamac",token)==0) {
        sprintf(tBuffer, "%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X",
                xAppData.staMac[0], xAppData.staMac[1], xAppData.staMac[2],
                xAppData.staMac[3], xAppData.staMac[4], xAppData.staMac[5]);
        httpd_resp_send_chunk(req, tBuffer,strlen(tBuffer));
    } else if (strcmp("%upgrade",token)==0) {
	sprintf(tBuffer, "<li> software version = %d.%d.%d %s<br><br>",MAJOR, MINOR, BUILD, getSystemType()? "(test)":"");
        httpd_resp_send_chunk(req, tBuffer, strlen(tBuffer));

        if (checkForUpdate(0)) {
	    sprintf(tBuffer, "<button onclick=\"window.location.href = '/?dfu=0';\">Upgrade Firmware</button><br><br>");
            httpd_resp_send_chunk(req, tBuffer, strlen(tBuffer));
	}
	if (getSystemType()) {
	    sprintf(tBuffer, "<button onclick=\"window.location.href = '/?dfu=1';\">Forced Prod Upgrade Firmware</button><br>");
            httpd_resp_send_chunk(req, tBuffer, strlen(tBuffer));
	    sprintf(tBuffer, "<button onclick=\"window.location.href = '/?dfu=2';\">Forced Test Upgrade Firmware</button><br>");
            httpd_resp_send_chunk(req, tBuffer, strlen(tBuffer));
        }
    } else {
        sprintf(tBuffer, "%s%%",token);
        httpd_resp_send_chunk(req, tBuffer, strlen(tBuffer));
    }
    return(ESP_OK);
}

/*******************************************************************************
    PURPOSE:
        Sends a file on the intern file system to the html handler. If the file
	is not binary, it will scan for %tokens% and call a procedure to send
	other data in its place.

    INPUTS:
        http handler
	file name
	binary flag

    RETURN CODE:
        Error code

    NOTES:

*******************************************************************************/
esp_err_t file_get_handler(httpd_req_t *req, char *filename, bool binary)
{

    ESP_LOGI(TAG, "Reading file : %s", filename);

    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        const char * resp_str = "File doesn't exist";
        httpd_resp_send(req, resp_str, strlen(resp_str));
        return ESP_FAIL;
    }

    /* Read file in chunks (relaxes any constraint due to large file sizes)
     * and send HTTP response in chunked encoding */
    char   chunk[1024];
    char   token[20];
    size_t chunkSize;
    bool readingToken = false;
    uint16_t chunkIndex;
    uint16_t chunkHead = 0;
    uint8_t tokenIndex = 0;

    do {
        chunkSize = fread(chunk, 1, sizeof(chunk), f);
        chunkHead = 0;
        if (chunkSize != 0) {
            for (chunkIndex=0; chunkIndex<chunkSize; chunkIndex++) {
                if ((binary == false) && (chunk[chunkIndex] == '%')) {
                    if (readingToken == true) {
                        readingToken = false;
                        chunkHead = chunkIndex+1;
                        token[tokenIndex] = 0;
                        // perform token lookup
                        ESP_LOGI(TAG, "token = %s", token);
                        lookupToken(req, token);
                    } else {
                        readingToken = true;
                        token[0] = '%';
                        tokenIndex = 1;
                        // dump current buffer
                        if (httpd_resp_send_chunk(req, &chunk[chunkHead], chunkIndex-chunkHead) != ESP_OK) {
                            fclose(f);
                            return ESP_FAIL;
                        }
                    }
                } else if (readingToken == true) {
                    token[tokenIndex] = chunk[chunkIndex];
                    tokenIndex++;
                    // ok, it was not a token...
                    if (tokenIndex > 20) {
                        if (httpd_resp_send_chunk(req, token, tokenIndex) != ESP_OK) {
                            fclose(f);
                            return ESP_FAIL;
                        }
                        readingToken = false;
                        chunkHead = chunkIndex+1;
                        token[tokenIndex] = 0;
                    }
                }
            }
            if (readingToken == false) {
                // dump the buffer
                if (httpd_resp_send_chunk(req, &chunk[chunkHead], chunkIndex-chunkHead) != ESP_OK) {
                    fclose(f);
                    return ESP_FAIL;
                }
            }
        }
    } while (chunkSize != 0);

    // ok, it was not a token...
    if (readingToken == true) {
        token[tokenIndex] = 0;
        if (httpd_resp_send_chunk(req, token, tokenIndex) != ESP_OK) {
            fclose(f);
            return ESP_FAIL;
        }
    }

    fclose(f);
    // Adding this logging caused the code to start working.....
    ESP_LOGI(TAG, "done file : %s", filename);
    return ESP_OK;
}

void getFileName(char * file, char * line) {
    uint8_t i = strlen(line);
    while ((line[i] != '/') && (i != 0)) {i--;}
    sprintf(file, "/spiffs/%s", &line[i+1]);
}

void parseUrl(httpd_req_t *req) {
    char*  buf;
    size_t buf_len;

    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found URL query => %s", buf);
            char param[80];
            /* Get value of expected key from query string */
#ifndef TIXCLOCK
            if (httpd_query_key_value(buf, "pattern", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Pattern = %d", atoi(param));
                setPatternNumber(atoi(param));
            }
            if (httpd_query_key_value(buf, "download", param, sizeof(param)) == ESP_OK) {
		char file[40];
                ESP_LOGI(TAG, "Download = %s", param);
		getFileName(file, param);
  		download_file( file, param);
		addPattern(&file[8]); // this is a hack to remove the "/spiffs/"
            }
            if (httpd_query_key_value(buf, "delete", param, sizeof(param)) == ESP_OK) {
		char file[40];
                ESP_LOGI(TAG, "Delete = %s", param);
		getFileName(file, param);
                printf("file = >%s<\n",file);
  		remove(file);
		deletePattern(&file[8]); // this is a hack to remove the "/spiffs/"
            }
#endif
            if (httpd_query_key_value(buf, "dfu", param, sizeof(param)) == ESP_OK) {
//		char file[40];
                ESP_LOGI(TAG, "dfu = %d", atoi(param));
		perform_dfu(atoi(param));
            }
        }
        free(buf);
    }
}

/*******************************************************************************
    PURPOSE: sends a file through the web interface

    INPUTS: http request

    RETURN CODE:
        error code

    NOTES:

*******************************************************************************/
esp_err_t get_file_handler(httpd_req_t *req)
{
    uint8_t sLength = 0;
    char *filename = (char *) req->user_ctx;
    bool headFoot = false;

    // any inputs?
    parseUrl(req);

    ESP_LOGI(TAG, "ctx = %s", filename);
    sLength = strlen(filename);
    if (strcmp(".jpg",&filename[sLength-4]) == 0) {
        httpd_resp_set_hdr(req, "Content-type", "image/jpg");
        ESP_LOGI(TAG, "found jpg:");
    } else if (strcmp(".html",&filename[sLength-5]) == 0) {
        httpd_resp_set_hdr(req, "Content-type", "text/html");
        ESP_LOGI(TAG, "found html:");
        headFoot = true;
    } else if (strcmp(".css",&filename[sLength-4]) == 0) {
        httpd_resp_set_hdr(req, "Content-type", "text/html");
        ESP_LOGI(TAG, "found css:");
    }
    /* Send a simple response */
    if (headFoot) file_get_handler(req, "/spiffs/header.html", true);
    file_get_handler(req, req->user_ctx,!headFoot);
    if (headFoot) file_get_handler(req, "/spiffs/footer.html", false);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

bool isHexDigit( char in ) {

    if (((in >='0') && (in <= '9')) ||
            ((in >='A') && (in <= 'F')) ||
            ((in >='a') && (in <= 'f'))) {
        return( true );
    }
    return( false );
}

void processString( char * value ) {
    uint8_t strLength = strlen( value);
    char hex[3] = "00";

    if (strLength < 3) return;

    printf("in >%s<\n",value);

    for( uint8_t i=0; i< strLength-2; i++) {
        if (value[i] == '+') {
            value[i] = ' ';
        } else if ((value[i] == '%') &&
                   isHexDigit(value[i+1]) &&
                   isHexDigit(value[i+2]))
        {

            hex[0] = value[i+1];
            hex[1] = value[i+2];
            value[i] = strtol(hex,NULL,16);
            strcpy( &value[i+1], &value[i+3]);
        }
    }
    printf("out >%s<\n",value);
}

/*******************************************************************************
    PURPOSE: sends a file through the web interface

    INPUTS: http request

    RETURN CODE: NONE

    NOTES:

*******************************************************************************/
static void processVar( char * name, char * value) {
    uint8_t i;


    processString( value );

    if (strcmp("hssid",name)==0) {
        setHSSsid(value);
    } else if (strcmp("hpasswd", name)==0) {
        if (strcmp(value, "[None]")==0) {
            value[0] = 0;
            printf("erase\n");
        }
        setHSPasswd(value);
    } else if (strcmp("hchan", name)==0) {
        setHSChan( atoi(value));
    } else if (strncmp("wssid", name, 5)==0) {
        printf("processVr1 %s\n", value);
        ESP_LOGI(TAG, "DDF1 >%s< = >%s<", name, value);
        if (strcmp(value, "[Not+Used]")==0) {
            value[0] = 0;
            printf("erase\n");
        }
        if (strcmp(value, "[Not Used]")==0) {
            value[0] = 0;
            printf("erase\n");
        }
        ESP_LOGI(TAG, "DDF2 >%s< = >%s<", name, value);
        i = name[5] - '0';
        printf("processVr2 %d %s\n", i, value);
        setWifiSsid(i,value);
    } else if (strncmp("wpasswd", name, 7)==0) {
        ESP_LOGI(TAG, "DDF3 >%s< = >%s<", name, value);
        if (strcmp(value, "[None]")==0) {
            value[0] = 0;
        }
        if (strcmp(value, "[Not Used]")==0) {
            value[0] = 0;
        }
        ESP_LOGI(TAG, "DDF4 >%s< = >%s<", name, value);
        i = name[7] - '0';
        setWifiPasswd(i,value);
    } else if (strcmp("tz", name)==0) {
        setTZ(value);
#ifdef TIXCLOCK
    } else if (strcmp("tformat", name)==0) {
        setTFormat(value);
#endif
    } else {
        ESP_LOGI(TAG, "unknown >%s< = >%s<", name, value);
    }
}

/*******************************************************************************
    PURPOSE:  An HTTP POST handler

    INPUTS: http request

    RETURN CODE:
        error code

    NOTES:

*******************************************************************************/
static esp_err_t save_settings_handler(httpd_req_t *req)
{
    char dataName[100];
    char dataVar[100];
    size_t off=0;
    int ret = 0;

    char*  buf = malloc(req->content_len + 1);

    while (off < req->content_len) {
        /* Read data received in the request */
        ret = httpd_req_recv(req, buf + off, req->content_len - off);
        if (ret <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                httpd_resp_send_408(req);
            }
            free (buf);
            return ESP_FAIL;
        }
        off += ret;
//      ESP_LOGI(TAG, "/echo handler recv length %d", ret);
    }
    buf[off] = '\0';

    /* Log data received */
    ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
    uint16_t i = 0;
    uint16_t b = 0;
    while ( i < (req->content_len+1)) {
        if (buf[i] == '=') {
            strlcpy(dataName, &buf[b], i-b+1);
//           ESP_LOGI(TAG, ">%s< %d %d", dataName, b, i);
            b=i+1;
        } else if ((buf[i] == '&')||(buf[i] == 0)) {
            strlcpy(dataVar, &buf[b], i-b+1);
            ESP_LOGI(TAG, ">%s< = >%s<", dataName, dataVar);
            processVar( dataName, dataVar );
            b=i+1;
        }
        i++;
    }
    ESP_LOGI(TAG, "====================================");
    WifiCleanup();

    // End response
    /* Send a simple response */
    file_get_handler(req, "/spiffs/header.html", true);
//    httpd_resp_send_chunk(req, buf, ret);
    free(buf);
    file_get_handler(req, "/spiffs/settings.html", false);
    file_get_handler(req, "/spiffs/footer.html", false);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

/*******************************************************************************
    PURPOSE: dir html header callback

    INPUTS: path - dir path
            req - httpd connection

    RETURN CODE: NONE

*******************************************************************************/
void htmlHeader_cb( char *path, void * data) {
    httpd_req_t *req = (httpd_req_t *) data;
    char tbuffer[123];
    const char *dir_heading = "</div></td> <td valign=\"top\">"
                              "<div id=\"navBreadCrumb\">Disk Dir</div>"
                              "<div class=\"centerColumn\" id=\"indexDefault\">"
                              "<h1 id=\"indexDefaultHeading\"></h1>\n";
    const char *dir_header = "<table><tr><th>T<th>Name<th>Size<th>Sha1 hash\n";

    httpd_resp_set_hdr(req, "Content-type", "text/html");
    file_get_handler(req, "/spiffs/header.html",true);

    httpd_resp_send_chunk(req, dir_heading, strlen(dir_heading));
    snprintf(tbuffer,sizeof(tbuffer),"List of Directory [%s]\n", path);
    httpd_resp_send_chunk(req, tbuffer, strlen(tbuffer));
    httpd_resp_send_chunk(req, dir_header, strlen(dir_header));
}

/*******************************************************************************
    PURPOSE: dir html line callback. called for each file in dir.

    INPUTS: type - dir entry type (always 'f'?)
            size - size in bytes
            tbuffer - time file was created (not used?)
            name - file name
            req - httpd connection

    RETURN CODE: NONE

*******************************************************************************/
void htmlLine_cb( char type, char * size, char * tbuffer, char *name, void * data) {
    httpd_req_t *req = (httpd_req_t *) data;
    char tbuffer2[157];
    snprintf(tbuffer2, sizeof(tbuffer2),
             "<tr><td>%c<td align=\"right\">%.40s<td>%.40s<td>%.40s\n", type, name, size, tbuffer);
    httpd_resp_send_chunk(req, tbuffer2, strlen(tbuffer2));
}

/*******************************************************************************
    PURPOSE: dir html footer callback

    INPUTS: total - bytes in files (without blocking loss)
            nfile - number of files
            tot - total disk space
            used - total used disk space
            req - httpd connection

    RETURN CODE: NONE

*******************************************************************************/
void htmlFooter_cb(uint64_t total, int nfiles, uint32_t tot, uint32_t used, void *data) {
    httpd_req_t *req = (httpd_req_t *) data;
    const char *dir_footer = "</table></body></html>\n";
    char tbuffer[123];
    char tbuffer2[129];

    if (total) {
        if (total < (1024*1024)) {
            snprintf(tbuffer, sizeof(tbuffer), "   %8d", (int)total);
        } else if ((total/1024) < (1024*1024)) {
            snprintf(tbuffer, sizeof(tbuffer), "   %6dKB", (int)(total / 1024));
        } else {
            snprintf(tbuffer, sizeof(tbuffer), "   %6dMB", (int)(total / (1024 * 1024)));
            snprintf(tbuffer2, sizeof(tbuffer2), "<tr><td colspan=4>%.40s in %d file(s)\n", tbuffer, nfiles);
            httpd_resp_send_chunk(req, tbuffer2, strlen(tbuffer2));
        }
    }

    sprintf(tbuffer, "<tr><td colspan=4>SPIFFS: free %ld KB of %ld KB\n", (tot-used) / 1024, tot / 1024);
    httpd_resp_send_chunk(req, tbuffer, strlen(tbuffer));
    httpd_resp_send_chunk(req, dir_footer, strlen(dir_footer));
    file_get_handler(req, "/spiffs/footer.html",false);
    httpd_resp_send_chunk(req, NULL, 0);
}

/*******************************************************************************
    PURPOSE: get disk drive output in html format

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
esp_err_t web_disk_dir_list(httpd_req_t *req) {

    diskDirCfg_t dReq = {
        .path = "/spiffs",
        .header_cb = htmlHeader_cb,
        .line_cb = htmlLine_cb,
        .footer_cb = htmlFooter_cb,
        .data = req
    };

    disk_dir(dReq);
    return(ESP_OK);
}

/*******************************************************************************
    PURPOSE: mapping of web path to function to call

*******************************************************************************/
httpd_uri_t root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = get_file_handler,
    .user_ctx  = "/spiffs/index.html"
};

httpd_uri_t root2 = {
    .uri       = "/generate_204",
    .method    = HTTP_GET,
    .handler   = get_file_handler,
    .user_ctx  = "/spiffs/index.html"
};

httpd_uri_t root3 = {
    .uri       = "/gen_204",
    .method    = HTTP_GET,
    .handler   = get_file_handler,
    .user_ctx  = "/spiffs/index.html"
};

httpd_uri_t about = {
    .uri       = "/about.html",
    .method    = HTTP_GET,
    .handler   = get_file_handler,
    .user_ctx  = "/spiffs/about.html"
};

httpd_uri_t settings = {
    .uri       = "/settings.html",
    .method    = HTTP_GET,
    .handler   = get_file_handler,
    .user_ctx  = "/spiffs/settings.html"
};
#ifndef TIXCLOCK
httpd_uri_t patterns = {
    .uri       = "/patterns.html",
    .method    = HTTP_GET,
    .handler   = web_pattern_list,
    .user_ctx  = NULL
};
httpd_uri_t cloud = {
    .uri       = "/cloudpatterns.html",
    .method    = HTTP_GET,
    .handler   = cloud_pattern_list,
    .user_ctx  = NULL
};
#endif
httpd_uri_t logo = {
    .uri       = "/header-bg.jpg",
    .method    = HTTP_GET,
    .handler   = get_file_handler,
    .user_ctx  = "/spiffs/header-bg.jpg"
};

httpd_uri_t title = {
    .uri       = "/box-title-bg.jpg",
    .method    = HTTP_GET,
    .handler   = get_file_handler,
    .user_ctx  = "/spiffs/box-title-bg.jpg"
};

httpd_uri_t content = {
    .uri       = "/content-bg.jpg",
    .method    = HTTP_GET,
    .handler   = get_file_handler,
    .user_ctx  = "/spiffs/content-bg.jpg"
};

httpd_uri_t ccs = {
    .uri       = "/styles.css",
    .method    = HTTP_GET,
    .handler   = get_file_handler,
    .user_ctx  = "/spiffs/styles.css"
};

httpd_uri_t web_dir = {
    .uri       = "/dir",
    .method    = HTTP_GET,
    .handler   = web_disk_dir_list,
    .user_ctx  = NULL
};

httpd_uri_t save = {
    .uri       = "/form-settings",
    .method    = HTTP_POST,
    .handler   = save_settings_handler,
    .user_ctx  = NULL,
};

/*******************************************************************************
    PURPOSE: start the web server

    INPUTS: NONE

    RETURN CODE:
        web handle

    NOTES:

*******************************************************************************/
httpd_handle_t start_webserver(void) {

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 7 *1024;

    config.max_uri_handlers = 13;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: %d", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &root);
        httpd_register_uri_handler(server, &root2);
        httpd_register_uri_handler(server, &root3);
        httpd_register_uri_handler(server, &ccs);
        httpd_register_uri_handler(server, &web_dir);
        httpd_register_uri_handler(server, &logo);
        httpd_register_uri_handler(server, &title);
        httpd_register_uri_handler(server, &about);
        httpd_register_uri_handler(server, &settings);
#ifndef TIXCLOCK
        httpd_register_uri_handler(server, &patterns);
        httpd_register_uri_handler(server, &cloud);
#endif
        httpd_register_uri_handler(server, &content);
        httpd_register_uri_handler(server, &save);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

void start_mdns_service()
{
    //initialize mDNS service
    esp_err_t err = mdns_init();
    if (err) {
        printf("MDNS Init failed: %d\n", err);
        return;
    }

    //set hostname
    mdns_hostname_set(getHSSsid());
    //set default instance
    mdns_instance_name_set("2DKits Blinkie");
}

static bool serverInit = false;

static void ip_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
//    wifi_mode_t mode;
    static bool audit = false;

    switch (event_id) {
    case IP_EVENT_STA_GOT_IP: {

//        xAppData.ipName = ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip);
//        ESP_LOGI(TAG, "SYSTEM_EVENT_STA_GOT_IP");
//        ESP_LOGI(TAG, "Got IP: %s",
//                 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));

        /* Start the web server */
        if (serverInit == false) {
            start_webserver();
            start_mdns_service();
            serverInit = true;
        }
        /* Audit the disk for updates */
        if (audit == false) {
//	    auditDiskFiles(); // out of stack!!
	    xTaskCreate(auditDiskFiles, "audit", 4*1024, NULL, 23, NULL);
	    audit = true;
	}

        ESP_LOGI(TAG, "Initializing SNTP");
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setservername(0, "pool.ntp.org");
        sntp_init();
        break;
    }
    default:
        break;
    }
    return;
}


/*******************************************************************************
    PURPOSE: event handler for wifi

    INPUTS:
        handle
	event

    RETURN CODE:
        error code

    NOTES:

*******************************************************************************/
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
	
{
    wifi_sta_info_t *sta;
    static uint8_t wifiIndex = 0;

    wifi_config_t wifi_config_sta = {
        .sta = {
            .ssid = "",
            .password = "",
        },
    };

    switch(event_id) {
    case WIFI_EVENT_STA_START:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_START");
        if (xAppData.wifiCount != 0) ESP_ERROR_CHECK(esp_wifi_connect());
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_DISCONNECTED");
        xAppData.ipName = NULL;

        wifiIndex = (wifiIndex + 1) % WIFI_TABLE_SIZE;
        strcpy((char *) wifi_config_sta.sta.ssid, xAppData.wifi[wifiIndex].ssid);
        strcpy((char *) wifi_config_sta.sta.password, xAppData.wifi[wifiIndex].passwd);
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config_sta));

        ESP_LOGI(TAG, "Trying WiFi STA: SSID >%s< PASSWD >%s<", wifi_config_sta.sta.ssid, wifi_config_sta.sta.password);
        esp_wifi_connect();
        break;
    case WIFI_EVENT_STA_CONNECTED:
        ESP_LOGI(TAG, "WIFI_EVENT_STA_CONNECTED");
	break;
    case WIFI_EVENT_AP_STACONNECTED:

//	wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
//        ESP_LOGI(TAG,"station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);

        /* Start the web server */
        if (serverInit == false) {
            start_webserver();
            serverInit = true;
        }
        wifi_sta_list_t sta_list;
        ESP_ERROR_CHECK( esp_wifi_ap_get_sta_list(&sta_list));
        for(int i = 0; i < sta_list.num; i++)
        {
            //Print the mac address of the connected station
            sta =  &(sta_list.sta[i]);
            ESP_LOGI(TAG,"Station %d MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", i,
                     sta->mac[0], sta->mac[1], sta->mac[2], sta->mac[3], sta->mac[4], sta->mac[5]);
        }
        break;
    default:
        ESP_LOGI(TAG, "Network event %ld",event_id);
        break;
    }
    return;
}

/*******************************************************************************
    PURPOSE:  start the wifi interface part 1

    INPUTS: arg per task, not used

    RETURN CODE: NONE

    NOTES: clasic chicken / egg. We need the wifi running to get the mac. We
    need the mac to init the global settings. We need the global settings to
    init the wifi.

*******************************************************************************/
void initialise_wifi_p1(void *arg)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
}

static esp_netif_t *s_example_sta_netif = NULL;

/*******************************************************************************
    PURPOSE:  start the wifi interface part 2

    INPUTS: arg per task, not used

    RETURN CODE: NONE

    NOTES:

*******************************************************************************/
void initialise_wifi_p2(void *arg) {
    wifi_config_t wifi_config_sta = {
        .sta = {
            .ssid = "",
            .password = "",
        },
    };
    wifi_config_t wifi_config_ap = {
        .ap = {
            .ssid = "",
            .ssid_len = 0,
            .password = "",
            .authmode = WIFI_AUTH_OPEN,
            .beacon_interval = 400,
            .max_connection = 16,
        }
    };

    strcpy((char *) wifi_config_ap.ap.ssid, xAppData.apSsid);
    strcpy((char *) wifi_config_ap.ap.password, xAppData.apPasswd);
    // ap will show up to the same network it connects to via sta.
    wifi_config_ap.ap.channel = xAppData.apChan;
    if (xAppData.apPasswd[0] == 0) {
        wifi_config_ap.ap.authmode = WIFI_AUTH_OPEN;
    } else {
        wifi_config_ap.ap.authmode = WIFI_AUTH_WPA2_PSK;
    }

    strcpy((char *) wifi_config_sta.sta.ssid, xAppData.wifi[0].ssid);
    strcpy((char *) wifi_config_sta.sta.password, xAppData.wifi[0].passwd);

    ESP_LOGI(TAG, "Setting WiFi AP: SSID >%s< PASSWD >%s<", wifi_config_ap.ap.ssid, wifi_config_ap.ap.password);
    ESP_LOGI(TAG, "Trying WiFi STA: SSID >%s< PASSWD >%s<", wifi_config_sta.sta.ssid, wifi_config_sta.sta.password);

    esp_netif_inherent_config_t esp_netif_config = ESP_NETIF_INHERENT_DEFAULT_WIFI_STA();
    // Warning: the interface desc is used in tests to capture actual connection details (IP, gw, mask)
//    esp_netif_config.if_desc = EXAMPLE_NETIF_DESC_STA;
    esp_netif_config.route_prio = 128;
    s_example_sta_netif = esp_netif_create_wifi(WIFI_IF_STA, &esp_netif_config);



    esp_wifi_set_default_wifi_sta_handlers();
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &ip_event_handler, NULL));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config_ap) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config_sta));
    ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_AP, xAppData.apMac) );
    ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_STA, xAppData.staMac));

    ESP_ERROR_CHECK(esp_wifi_start());

//    captdnsInit();

//    start_webserver();
}
