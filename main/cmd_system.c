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

#include "cmd_system.h"
#include "version.h"
#include "disk_system.h"

#define WITH_TASKS_INFO 1

static const char *TAG = "cmd_system";

static void register_heap(void);
static void register_version(void);
static void register_restart(void);
static void register_tasks(void);
static void register_time(void);
static void register_dir(void);

/*******************************************************************************

    PURPOSE: adds system commands and callbacks to the menu

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/

void register_system(void)
{
    register_heap();
    register_version();
    register_restart();
    register_tasks();
    register_time();
    register_dir();
}

/*******************************************************************************

    PURPOSE: give hardware and software version info

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static int get_version(int argc, char **argv)
{
    esp_chip_info_t info;
    esp_chip_info(&info);
    ESP_LOGW(TAG, "IDF Version:%s", esp_get_idf_version());
    ESP_LOGW(TAG, "Chip info:");
    ESP_LOGW(TAG, "\tmodel:%s", info.model == CHIP_ESP32 ? "ESP32" : "Unknow");
    ESP_LOGW(TAG, "\tcores:%d", info.cores);
    ESP_LOGW(TAG, "\tfeature:%s%s%s%s%d%s",
           info.features & CHIP_FEATURE_WIFI_BGN ? "/802.11bgn" : "",
           info.features & CHIP_FEATURE_BLE ? "/BLE" : "",
           info.features & CHIP_FEATURE_BT ? "/BT" : "",
           info.features & CHIP_FEATURE_EMB_FLASH ? "/Embedded-Flash:" : "/External-Flash:",
           spi_flash_get_chip_size() / (1024 * 1024), " MB");
    ESP_LOGW(TAG, "\trevision number:%d", info.revision);
    ESP_LOGW(TAG, "\tsoftware version: %d.%d.%d",MAJOR, MINOR, BUILD);
    return 0;
}

/*******************************************************************************

    PURPOSE: register 'version' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static void register_version(void)
{
    const esp_console_cmd_t cmd = {
        .command = "version",
        .help = "Get version of chip and SDK",
        .hint = NULL,
        .func = &get_version,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/*******************************************************************************

    PURPOSE: time command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static int get_time(int argc, char **argv)
{
    time_t      now;
    char        timeBuf[64];
    struct tm   timeinfo;

    gmtime_r(&now, &timeinfo);
    strftime(timeBuf, sizeof(timeBuf), "%c", &timeinfo);
    ESP_LOGW(TAG, "%s\n", timeBuf);

    return 0;
}

/*******************************************************************************

    PURPOSE: register 'time' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static void register_time(void)
{
    const esp_console_cmd_t cmd = {
        .command = "time",
        .help = "Get system time",
        .hint = NULL,
        .func = &get_time,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/*******************************************************************************

    PURPOSE: 'restart' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/

static int restart(int argc, char **argv)
{
    ESP_LOGW(TAG, "Restarting");
    esp_restart();
}

/*******************************************************************************

    PURPOSE: register 'restart' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static void register_restart(void)
{
    const esp_console_cmd_t cmd = {
        .command = "reset",
        .help = "Software reset of the chip",
        .hint = NULL,
        .func = &restart,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/*******************************************************************************

    PURPOSE: 'heap' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/

static int heap_size(int argc, char **argv)
{
    uint32_t heap_size = heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT);
    ESP_LOGW(TAG, "min heap size: %u\n", heap_size);
    ESP_LOGW(TAG, "current size:  %u\n", esp_get_free_heap_size());
    return 0;
}

/*******************************************************************************

    PURPOSE: register 'heap' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static void register_heap(void)
{
    const esp_console_cmd_t heap_cmd = {
        .command = "heap",
        .help = "Get minimum size of free heap memory that was available during program execution",
        .hint = NULL,
        .func = &heap_size,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&heap_cmd) );

}

/*******************************************************************************

    PURPOSE: tasks command - give freeRTOS task info

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static int tasks_info(int argc, char **argv)
{
    const size_t bytes_per_task = 40; /* see vTaskList description */
    char *task_list_buffer = malloc(uxTaskGetNumberOfTasks() * bytes_per_task);
    if (task_list_buffer == NULL) {
        ESP_LOGE(TAG, "failed to allocate buffer for vTaskList output");
        return 1;
    }
    fputs("Task Name\tStatus\tPrio\tHWM\tTask#\tAffinity", stdout);
    fputs("\n", stdout);
    vTaskList(task_list_buffer);
    fputs(task_list_buffer, stdout);
    free(task_list_buffer);
    return 0;
}

/*******************************************************************************

    PURPOSE: register 'tasks' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static void register_tasks(void)
{
    const esp_console_cmd_t cmd = {
        .command = "tasks",
        .help = "Get information about running tasks",
        .hint = NULL,
        .func = &tasks_info,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}


/*******************************************************************************

    PURPOSE: 'dir' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/

static int dir(int argc, char **argv)
{
    disk_dir_list("/spiffs",NULL);
    return 0;
}

/*******************************************************************************

    PURPOSE: register 'heap' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static void register_dir(void)
{
    const esp_console_cmd_t dir_cmd = {
        .command = "dir",
        .help = "list contents of flash drive",
        .hint = NULL,
        .func = &dir,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&dir_cmd) );

}
