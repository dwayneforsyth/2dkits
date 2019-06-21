
#include "stddef.h"
#include "global.h"
#include <stdio.h>

blinkieAppData_t xAppData = {
   .ipName = NULL,
   .wifi = {{ "dforsythnet","" },
            { "dforsythnet2","" },
            { "Optimal-LAN","wifiworks"}
           },
   .apSsid = "blinkie",
   .apPasswd = "",
   .apChan = 9,
   .apMac = {0,0,0,0,0,0},
   .staMac = {0,0,0,0,0,0},
};

void storeSettings() {

   FILE *ptr = fopen("/spiffs/setting.bin","wb");

   printf("store Settings\n");

   fwrite( &xAppData, sizeof(xAppData), 1, ptr);

   fclose( ptr);
}

void loadSettings() {

   FILE *ptr = fopen("/spiffs/setting.bin","rb");

   printf("Load Settings\n");

   if (!ptr) {
       storeSettings();
   }

   fread( &xAppData, sizeof(xAppData), 1, ptr);

   fclose( ptr);
}

