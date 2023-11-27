//   Copyright (C) 2021 Dwayne Forsyth
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
    {.name = "about.html",       .sha="3ed4bc08d16dae1adc78a4d4a27d612aa5f68224"},
    {.name = "build.html",       .sha="76c41da8f4dad90c01e8d3b106b4f125f32d11eb"},
    {.name = "box-title-bg.jpg", .sha="423c796eb3986460384521d82d345b416fe47a67"},
    {.name = "content-bg.jpg",.   sha="8bb99c548e90ddefb21e1889fdbb5eac2376121b"},
    {.name = "footer.html",      .sha="18c381b0e68956f77ffd140d73d0ae781e5234d2"},
    {.name = "header.html",      .sha="9c819ced212fe5f46941ff85b832f1098f51d4e4"},
    {.name = "header-bg.jpg",    .sha="7b4d322d9e6b84143ff974b9052bdcd9369f09a1"},
    {.name = "index.html",       .sha="67e10905b4aae5a03166045712ffab8bef37c8d5"},
    {.name = "patterns.html",    .sha="1c6413e42a4d20853cca0c71d92639019af149ab"},
    {.name = "settings.html",    .sha="9c1ce9f3d4cc12ff8e1e111e0553cad603429868"},
    {.name = "styles.css",       .sha="cfdbd4de3f1a040c8a383e4cf53976df8ca793b4"},
};

