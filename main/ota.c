
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "string.h"

#include "ota.h"
#include "version.h"
#include "pattern_engine.h"

static const char *TAG = "ota";

#define MAX_FILE_NAME_LEN 54
#define MAX_OTA_CONTROL_LEN 80
char otaBuffer[MAX_OTA_CONTROL_LEN];

#define CONFIG_FIRMWARE_STATUS_URL "http://www.2dkits.com/kits/kit25/current_firmware.txt"
#define CONFIG_TEST_FIRMWARE_STATUS_URL "http://www.2dkits.com/kits/kit25/current_test_firmware.txt"
#define CONFIG_BASE_FIRMWARE_URL "http://www.2dkits.com/"

extern const uint8_t server_cert_pem_start[] asm("_binary_trustid_x3_root_pem_start");

/*******************************************************************************

    PURPOSE: http event handler used when downloading the ota control file.
             on "data" events we store the data in a otaBuffer.

    INPUTS: event struct

    RETURN CODE:

*******************************************************************************/
esp_err_t _http_event_handler2(esp_http_client_event_t *evt)
{
    int8_t length = 0;

    switch (evt->event_id) {
    case HTTP_EVENT_ON_DATA:
        //DDF we could get this in parts?
//        ESP_LOGW(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
//        printf("%.*s", evt->data_len, (char*)evt->data);
        length = (evt->data_len < MAX_OTA_CONTROL_LEN)? evt->data_len : MAX_OTA_CONTROL_LEN;
        memcpy( otaBuffer, evt->data, length);
        break;
    default:
        break;
    }
    return ESP_OK;
}

/*******************************************************************************

    PURPOSE: Checks server if download is needed, if so it returns true, and
             a URL to the OTA bin file.

    INPUTS: pointer to store the firmware url

    RETURN CODE: bool - is the ota needed

*******************************************************************************/

bool check_ota(char * firmware_file, char * url) {
    uint8_t major, minor, build;
    char file[MAX_FILE_NAME_LEN+1];
    bool update = false;

    esp_http_client_config_t configOTAStatus = {
        .url = url,
	.cert_pem = (char *)server_cert_pem_start,
//        .auth_type = HTTP_AUTH_TYPE_NONE,
        .event_handler = _http_event_handler2,
    };

    otaBuffer[0] = '\0';
    esp_http_client_handle_t client = esp_http_client_init(&configOTAStatus);
    esp_err_t err = esp_http_client_perform(client);
    if ((err == ESP_OK) && (esp_http_client_get_status_code(client) == 200)) {
        if ( 4 != sscanf(otaBuffer,"%hhd.%hhd.%hhd %54s", &major, &minor, &build, file)) {
            ESP_LOGW(TAG, "parsing of control fail failed");
            major=minor=build=0;
            file[0]='\0';
        }

        snprintf(firmware_file,MAX_OTA_CONTROL_LEN,"%s%s",CONFIG_BASE_FIRMWARE_URL,file);
//        ESP_LOGW(TAG, ">%d.%d.%d %s<", major, minor, build, firmware_file);

        if (major > MAJOR) {
            update = true;
        } else if ((major == MAJOR) && (minor > MINOR)) {
            update = true;
        } else if ((major == MAJOR) && (minor == MINOR)) {
            update = (build > BUILD)? true : false;
        }
    } else {
        ESP_LOGW(TAG, "fetch control file failed, esp_err=%d http_code=%d",
             err, esp_http_client_get_status_code(client));
    }
    esp_http_client_cleanup(client);

    return(update);
}

/*******************************************************************************

    PURPOSE: calls routine to see if download is needed, performs the OTA if needed.

    INPUTS: none

    RETURN CODE: none

*******************************************************************************/

#define OTA_TRIES 3

void ota_task(void *pvParameter)
{
    uint8_t flags = (uint32_t) pvParameter;
    uint8_t tries = OTA_TRIES;


    char firmware_file[MAX_OTA_CONTROL_LEN];

    // this call returns the file name.
    bool needed = check_ota( firmware_file, (flags==2)? CONFIG_TEST_FIRMWARE_STATUS_URL :  CONFIG_FIRMWARE_STATUS_URL);
    ESP_LOGW(TAG, "%s %s",(needed)?"Downloading":"No update Needed", (flags!=0)?"Forced":"");

    if ((strlen(firmware_file) != 0) && (needed || (flags != 0))) {

        // do the ota
        esp_http_client_config_t configOTA = {
            .url = firmware_file,
	    .cert_pem = (char *)server_cert_pem_start,
//            .auth_type = HTTP_AUTH_TYPE_NONE,
//	    .skip_cert_common_name_check = true,
        };

        while (tries != 0) {
	    setPatternNumber(0); // give visual feedback we are downloading
            ESP_LOGW(TAG, "Downloading >%s<",firmware_file);
            esp_err_t ret = esp_https_ota(&configOTA);
            if (ret == ESP_OK) {
                ESP_LOGW(TAG, "OTA Done");
		patternEngineOff();
                esp_restart();
            } else {
                ESP_LOGE(TAG, "Firmware upgrade failed, retry left=%d",tries);
                tries--;
                vTaskDelay(500 / portTICK_PERIOD_MS); // wait 1/2 a second
            }
        }
    }

    ESP_LOGE(TAG, "OTA Failed");
    vTaskDelete(NULL);
}

/*******************************************************************************

    PURPOSE: creates the ota task

    INPUTS: none

    RETURN CODE: none

*******************************************************************************/

void perform_ota(uint8_t flags) {
    ESP_LOGW(TAG, "Starting OTA flags=%d", flags);
    xTaskCreate(ota_task, "ota_task", OTA_TASK_STACK_SIZE, (void *) flags, OTA_TASK_PRIORITY, NULL);
}
