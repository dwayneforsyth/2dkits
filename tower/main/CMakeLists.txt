set (COMMON "../../common/main/")
set (PICOC "../../common/picoc/")

idf_component_register(SRCS
   "${COMMON}disk_system.c"
   "${COMMON}sha_file.c"
   "file_sha.c"
   "${COMMON}global.c"
   "${COMMON}led_driver.c"
   "led_map.c"
   "numbers_led.c"
   "${COMMON}pattern_engine.c"
   "builtin_patterns.c"
   "tower_main.c"
   "${COMMON}web_server.c"
#   "${COMMON}captdns.c"
   "${COMMON}dfu.c"
   "${COMMON}download_file.c"
   "${COMMON}upgrade_disk.c"
   "${COMMON}console.c"
   "${COMMON}cmd_system.c"
   "${COMMON}cmd_dfu.c"
   "${COMMON}cmd_pattern.c"
   "${COMMON}cmd_wifi.c"
   "${COMMON}cmd_download.c"
   "${COMMON}queued_i2s_parallel.c"
   "${PICOC}/clibrary.c"
   "${PICOC}debug.c"
   "${PICOC}expression.c"
   "${PICOC}heap.c"
   "${PICOC}include.c"
   "${PICOC}lex.c"
   "${PICOC}parse.c"
   "${PICOC}platform.c"
   "${PICOC}table.c"
   "${PICOC}type.c"
   "${PICOC}variable.c"
   "${PICOC}/cstdlib/ctype.c"
   "${PICOC}/cstdlib/errno.c"
   "${PICOC}/cstdlib/math.c"
   "${PICOC}/cstdlib/stdbool.c"
   "${PICOC}/cstdlib/stdio.c"
   "${PICOC}/cstdlib/stdlib.c"
   "${PICOC}/cstdlib/string.c"
   "${PICOC}/cstdlib/time.c"
   "${PICOC}/cstdlib/unistd.c"
   "${PICOC}/cstdlib/blinkie.c"
   "${PICOC}/platform/platform_esp32.c"
   "${PICOC}/platform/library_unix.c"
   INCLUDE_DIRS
       "../../common/main"
       "../../common/picoc" .
   )

spiffs_create_partition_image(storage ../disk_image FLASH_IN_PROJECT)

# root for ota http
target_add_binary_data(${COMPONENT_TARGET} "${COMMON}trustid_x3_root.pem" TEXT)

