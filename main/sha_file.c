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
//   generates a SHA of a flash file.
//
//   The sha1 can be generated on a unix machine with the sha1sum command
//
//   $ sha1sum footer.html 
//     0289f31c9e872fd865eafa54636483a19d1b5018  footer.html
//**********************************************************************

#include "stdint.h"
#include "string.h"
#include "stdio.h"

#include "mbedtls/md.h"

#include <esp_system.h>
 
esp_err_t fileSha( char *filename ){
  char   chunk[1024];
  uint16_t chunkSize = 0;
  uint8_t shaResult[20];

  FILE *f = fopen(filename, "r");
    if (f == NULL) {
        printf("File doesn't exist");
        return ESP_FAIL;
    }
 
  mbedtls_md_context_t ctx;
  mbedtls_md_type_t md_type = MBEDTLS_MD_SHA1;
 

  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
  mbedtls_md_starts(&ctx);

  do {
    chunkSize = fread(chunk, 1, sizeof(chunk), f);
    mbedtls_md_update(&ctx, (const unsigned char *) chunk, chunkSize);
  } while (chunkSize != 0);

  mbedtls_md_finish(&ctx, shaResult);
  mbedtls_md_free(&ctx);
  fclose(f);

  printf("Hash: ");
    for(int i= 0; i< sizeof(shaResult); i++){
      printf("%02x", (int)shaResult[i]);
    }
  printf("\n");
  return(ESP_OK);
}
