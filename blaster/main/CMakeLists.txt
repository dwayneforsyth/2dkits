set (COMMON "../../common/main/")

idf_component_register(SRCS
   "${COMMON}disk_system.c"
   "${COMMON}sha_file.c"
   "${COMMON}global.c"
   "blaster_main.c"
   "led_driver.c"
   "web_server.c"
#   "${COMMON}captdns.c"
   "pattern_engine.c"
   "${COMMON}dfu.c"
   "${COMMON}console.c"
   "${COMMON}download_file.c"
   "${COMMON}upgrade_disk.c"
   "file_sha.c"
   "${COMMON}cmd_dfu.c"
   "${COMMON}cmd_system.c"
   "${COMMON}cmd_wifi.c"
   "${COMMON}cmd_download.c"
   "display.c"
   "st7789.c"
   "fontx.c"
   "pngle.c"
   "decode_png.c"
   "decode_jpeg.c"
   INCLUDE_DIRS
       "../../common/main" .
   )

spiffs_create_partition_image(storage ../disk_image FLASH_IN_PROJECT)

# root for dfu http
target_add_binary_data(${COMPONENT_TARGET} "trustid_x3_root.pem" TEXT)

