//   Copyright (C) 2020 Dwayne Forsyth
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

#include <stdint.h>
#include "file_sha.h"

const uint8_t neededFileCount = 6;
neededFiles_t  files[] = {
    {.name = "patterns.html",    .sha="3964c504825ac56c1a69c542cbd53fa8689feb85"},
    {.name = "header.html",      .sha="528319cfcc31c238fd927e99f72b025753e48e8c"},
    {.name = "footer.html",      .sha="0289f31c9e872fd865eafa54636483a19d1b5018"},
    {.name = "about.html",       .sha="970212075306a9c7a4362695624552d7f8ef4d69"},
    {.name = "index.html",       .sha="67e10905b4aae5a03166045712ffab8bef37c8d5"},
    {.name = "settings.html",    .sha="9c1ce9f3d4cc12ff8e1e111e0553cad603429868"},
    {.name = "content-bg.jpg",.   sha="8bb99c548e90ddefb21e1889fdbb5eac2376121b"},
    {.name = "styles.css",       .sha="cfdbd4de3f1a040c8a383e4cf53976df8ca793b4"},
    {.name = "header-bg.jpg",    .sha="7b4d322d9e6b84143ff974b9052bdcd9369f09a1"},
    {.name = "box-title-bg.jpg", .sha="423c796eb3986460384521d82d345b416fe47a67"},
};

