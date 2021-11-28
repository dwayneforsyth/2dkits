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

const uint8_t neededFileCount = 10;
neededFiles_t  files[] = {
    {.name = "about.html",       .sha="a9d468e568c8e2323b4985e27958452a088a2d6b"},
    {.name = "patterns.html",    .sha="1c6413e42a4d20853cca0c71d92639019af149ab"},
    {.name = "header.html",      .sha="d067a220eda0a69389818b21e2397211922e60d6"},
    {.name = "footer.html",      .sha="0289f31c9e872fd865eafa54636483a19d1b5018"},
    {.name = "index.html",       .sha="80cae09b90cbfc881687c71bb126803b0becb8d1"},
    {.name = "settings.html",    .sha="9c1ce9f3d4cc12ff8e1e111e0553cad603429868"},
    {.name = "content-bg.jpg",.   sha="8bb99c548e90ddefb21e1889fdbb5eac2376121b"},
    {.name = "styles.css",       .sha="cfdbd4de3f1a040c8a383e4cf53976df8ca793b4"},
    {.name = "header-bg.jpg",    .sha="7b4d322d9e6b84143ff974b9052bdcd9369f09a1"},
    {.name = "box-title-bg.jpg", .sha="423c796eb3986460384521d82d345b416fe47a67"},

};

