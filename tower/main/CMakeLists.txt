idf_component_register(SRCS
   "../../common/main/disk_system.c"
   "../../common/main/sha_file.c"
   "file_sha.c"
   "global.c"
   "led_driver.c"
   "../../common/main/queued_i2s_parallel.c"
   "numbers_led.c"
   "pattern_engine.c"
   "tower_main.c"
   "../../common/main/web_server.c"
   "../../common/main/captdns.c"
   "../../common/main/dfu.c"
   "../../common/main/download_file.c"
   "../../common/main/upgrade_disk.c"
   "../../common/main/console.c"
   "../../common/main/cmd_system.c"
   "../../common/main/cmd_dfu.c"
   "../../common/main/cmd_pattern.c"
   "../../common/main/cmd_wifi.c"
   "../../common/main/cmd_download.c"
   INCLUDE_DIRS "../../common/main" .)

spiffs_create_partition_image(storage ../disk_image FLASH_IN_PROJECT)

# root for ota http
target_add_binary_data(${COMPONENT_TARGET} "trustid_x3_root.pem" TEXT)

