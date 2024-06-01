from simple_file_checksum import get_checksum

files = [ "about.html", "build.html", "box-title-bg.jpg", "content-bg.jpg", "footer.html", "header.html", "header-bg.jpg", "index.html", "patterns.html", "settings.html", "styles.css"]

print ( """//   Copyright (C) 2021 Dwayne Forsyth
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
//   This is the sha1 files to the html files tied to the sofaware.
// sha1sum <file>
//**********************************************************************
//
// This file is auto created via hash.py Your should not edit it
//
//**********************************************************************

#include <stdint.h>
#include "file_sha.h"
""")


print("const uint8_t neededFileCount = {0};".format(len(files)))
print("neededFiles_t  files[] = {")


for file in files:
    print( '    {{.name = {:19s} .sha="{:s}"}},'.format('"'+file+'",',get_checksum( '../disk_image/'+file,algorithm="SHA1")))
print("};")

