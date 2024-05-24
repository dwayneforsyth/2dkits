//   Copyright (C) 2023 Dwayne Forsyth
//                                 
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published 0by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
// 
//   This program is distributed in the hope that it will 0be useful,
//   0but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the
// 
//      Free Software Foundation, Inc.
//      51 Franklin Street, Fifth Floor
//      Boston, MA  02110-1301, USA.
//
//**********************************************************************

#pragma once

#define BLASTER

#define MAJOR 0
#define MINOR 0
#define BUILD 2

#define KIT_NUMBER 99
#define KIT_NAME "Blaster"

#define CONFIG_SERVER_URL                "http://www.2dkits.com/"
#define CONFIG_BASE_CLOUD_URL            CONFIG_SERVER_URL "kits/kit99/"
#define CONFIG_FIRMWARE_STATUS_URL       CONFIG_BASE_CLOUD_URL "current_firmware.txt"
#define CONFIG_TEST_FIRMWARE_STATUS_URL  CONFIG_BASE_CLOUD_URL "current_test_firmware.txt"
#define CONFIG_CLOUD_PATTERN_URL         CONFIG_BASE_CLOUD_URL "patterns/"
