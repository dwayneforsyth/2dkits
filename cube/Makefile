#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := cube

include $(IDF_PATH)/make/project.mk

SPIFFS_IMAGE_FLASH_IN_PROJECT := 1
$(eval $(call spiffs_create_partition_image,storage,disk_image,FLASH_IN_PROJECT))

qflash:
	$(PYTHON) $(IDF_PATH)/components/esptool_py/esptool/esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 115200 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x10000 $(BUILD_DIR_BASE)/cube.bin

pattern: 
	#Want to force a build of storage.bin
	cd patterns; gcc *.c
	cd patterns; ./a.out
	cp $(PROJECT_PATH)/patterns/*.pat $(PROJECT_PATH)/disk_image/
	#$(PYTHON) $(IDF_PATH)/components/esptool_py/esptool/esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 115200 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x110000 $(BUILD_DIR_BASE)/storage.bin
