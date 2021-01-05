idf_component_register(SRCS
   "disk_system.c"
   "sha_file.c"
   "global.c"
   "led_driver.c"
   "numbers_led.c"
   "pattern_engine.c"
   "tower_main.c"
   "web_server.c"
   "dfu.c"
   "download_file.c"
   "console.c"
   "cmd_system.c"
   "cmd_dfu.c"
   "cmd_pattern.c"
   "cmd_wifi.c"
   "cmd_download.c"
   INCLUDE_DIRS "")

spiffs_create_partition_image(storage ../disk_image FLASH_IN_PROJECT)

# root for ota http
target_add_binary_data(${COMPONENT_TARGET} "trustid_x3_root.pem" TEXT)

