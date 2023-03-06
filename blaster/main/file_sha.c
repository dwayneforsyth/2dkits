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


const uint8_t neededFileCount = 11;
neededFiles_t  files[] = {
    {.name = "patterns.html",    .sha="3964c504825ac56c1a69c542cbd53fa8689feb85"},
    {.name = "content-bg.jpg",   .sha="8bb99c548e90ddefb21e1889fdbb5eac2376121b"},
    {.name = "footer.html",      .sha="0289f31c9e872fd865eafa54636483a19d1b5018"},
    {.name = "styles.css",       .sha="cfdbd4de3f1a040c8a383e4cf53976df8ca793b4"},
    {.name = "header-bg.jpg",    .sha="7b4d322d9e6b84143ff974b9052bdcd9369f09a1"},
    {.name = "box-title-bg.jpg", .sha="423c796eb3986460384521d82d345b416fe47a67"},
    {.name = "header.html",      .sha="fd63482c606b3299741cc291b944ec0870385d09"},
    {.name = "about.html",       .sha="7e58556910da40573f748b2be3723d1c7e314a24"},
    {.name = "build.html",       .sha="594aaac299eba16854b34a18ce6d5cd261eadfb2"},
    {.name = "index.html",       .sha="d6dce707bc931da848a01c2ff6f5a5d2d8a02114"},
    {.name = "settings.html",    .sha="9c1ce9f3d4cc12ff8e1e111e0553cad603429868"},
    {.name = "esp32.jpeg",       .sha="d60809425af6d72693d40c1b229e56e1a7bf9692"},
    {.name = "esp_logo.png",     .sha="c4598765a01502978569ceb8abf35c53b401e456"},
    {.name = "ILGH16XB.FNT",     .sha="96a0c7215fbd4e4ffd4ab4a7fad85cff1114c864"},
    {.name = "ILGH24XB.FNT",     .sha="18a1150577bcae59736b0fdf11f0c2f55885ac8f"},
    {.name = "ILGH32XB.FNT",     .sha="48c289745bae84ee87674267e1c68408ac9e6f24"},
    {.name = "ILMH16XB.FNT",     .sha="8498504e2a6d2bfd4eb39630399c97c297adf3a7"},
    {.name = "ILMH24XB.FNT",     .sha="e27ba9404a3776ad49ab083c94287ce1928c9c8f"},
    {.name = "ILMH32XB.FNT",     .sha="65c0abdca00e29ca3891139913e4ea49cb72a316"},
    {.name = "image.bmp",        .sha="08e71076c9e5cabde20c336f97d97ae4c8a79619"},
    {.name = "LATIN32B.FNT",     .sha="7d69056fe1a17576c1c1645fbf7879bfe128f771"},
    {.name = "qrcode.bmp",       .sha="ca2b337c2cf7f583b7a6e82839fba369452b1a02"},
};
