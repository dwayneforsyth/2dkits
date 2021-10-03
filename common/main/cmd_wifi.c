#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "esp_log.h"
#include "esp_console.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "esp_spi_flash.h"
#include "driver/rtc_io.h"
#include "driver/uart.h"
#include "argtable3/argtable3.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

//#include "cmd_system.h"
//#include "tixclock_main.h"
#include "global.h"

#define WITH_TASKS_INFO 1

static const char *TAG = "cmd_system";

static void register_winfo(void);
static void register_wssid(void);
static void register_addssid(void);

/*******************************************************************************

    PURPOSE: adds system commands and callbacks to the menu

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/

void commands_wifi(void)
{
    register_winfo();
    register_addssid();
}

/*******************************************************************************

    PURPOSE: 'debugled' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static int winfo(int argc, char **argv)
{
    ESP_LOGW(TAG, "winfo");
    ESP_LOGW(TAG, " Wifi Hotspot");
    ESP_LOGW(TAG, "  SSID: %s",getHSSsid());
    ESP_LOGW(TAG, "  Password: %s", getHSPasswd());
    ESP_LOGW(TAG, "  Channel: %d", getHSChan());
    ESP_LOGW(TAG, "  ip: 192.168.4.1");
    ESP_LOGW(TAG, "  MAC: %2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X",
           xAppData.apMac[0], xAppData.apMac[1], xAppData.apMac[2],
           xAppData.apMac[3], xAppData.apMac[4], xAppData.apMac[5]);
    ESP_LOGW(TAG, " Wifi AP");
    if (xAppData.ipName != NULL) {
        ESP_LOGW(TAG, "  IP: %s",xAppData.ipName);
    }
    ESP_LOGW(TAG, "  Gateways %d entries",xAppData.wifiCount);
    for (uint8_t i=0;i<xAppData.wifiCount;i++) {
	if (strlen(xAppData.wifi[i].ssid)!=0) {
            ESP_LOGW(TAG,"   %d: %s p=>%s<",i,xAppData.wifi[i].ssid, xAppData.wifi[i].passwd);
	}
    }

    return(0);
}

/*******************************************************************************

    PURPOSE: register 'debugled' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static void register_winfo(void)
{
    const esp_console_cmd_t cmd = {
        .command = "winfo",
        .help = "display wifi info",
        .hint = NULL,
        .func = &winfo,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/*******************************************************************************

    PURPOSE: 'addssid' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static struct {
    struct arg_str *ssid;
    struct arg_str *passwd;
    struct arg_end *end;
} addssid_args;

static int addssid(int argc, char **argv)
{
    ESP_LOGW(TAG, "add ssid");

    int nerrors = arg_parse(argc, argv, (void **) &addssid_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, addssid_args.end, argv[0]);
        return 1;
    }

    printf("add %s %s\n", addssid_args.ssid->sval[0], addssid_args.passwd->sval[0]);
    addWifiGateway(addssid_args.ssid->sval[0], addssid_args.passwd->sval[0]);

    if (xAppData.ipName == NULL) {
        ESP_LOGW(TAG, "Restarting");
        esp_restart();
    }
    return(0);
}

/*******************************************************************************

    PURPOSE: register 'addssid' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static void register_addssid(void)
{
    const esp_console_cmd_t cmd = {
        .command = "addssid",
        .help = "add ssid to database",
        .hint = NULL,
        .func = &addssid,
	.argtable = &addssid_args,
    };

    addssid_args.ssid = arg_str1(NULL, NULL, "ssid", "wifi ssid to add"),
    addssid_args.passwd = arg_str0("p", "passcode", "passcode", "wifi passcode if needed"),
    addssid_args.end    = arg_end(2);

    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}
