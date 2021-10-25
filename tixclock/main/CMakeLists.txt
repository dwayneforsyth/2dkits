idf_component_register(SRCS
   "disk_system.c"
   "sha_file.c"
   "global.c"
   "tixclock_main.c"
   "led_driver.c"
   "web_server.c"
   "captdns.c"
   "pattern_engine.c"
   "dfu.c"
   "console.c"
   "download_file.c"
   "upgrade_disk.c"
   "file_sha.c"
   "cmd_dfu.c"
   "cmd_system.c"
   "cmd_tixclock.c"
   "cmd_wifi.c"
   "cmd_download.c"
   INCLUDE_DIRS "")

spiffs_create_partition_image(storage ../disk_image FLASH_IN_PROJECT)

# root for dfu http
target_add_binary_data(${COMPONENT_TARGET} "trustid_x3_root.pem" TEXT)

