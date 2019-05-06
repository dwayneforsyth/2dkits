#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := spi_leds

include $(IDF_PATH)/make/project.mk

$(eval $(call spiffs_create_partition_image,storage,disk_image,FLASH_IN_PROJECT))
