
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "global.h"

#include <esp_wifi.h>
#include <esp_log.h>
static const char *TAG="GLOBAL";
#include <esp_system.h>
#include <nvs_flash.h>

blinkieAppData_t xAppData = {
   .ipName = NULL,
#if(0)
   .apSsid = "blinkie",
   .wifiCount = 3,
   .wifi = {{ "dforsythnet","" },
            { "dforsythnet2","" },
            { "Optimal-LAN","wifiworks"}
           },
   .apPasswd = "",
   .apChan = 9,
   .apMac = {0,0,0,0,0,0},
   .staMac = {0,0,0,0,0,0},
#endif
};

void initSettings() {
   memset(&xAppData, 0, sizeof(blinkieAppData_t));
   ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_AP, xAppData.apMac) );
   ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_STA, xAppData.staMac));
   sprintf(xAppData.apSsid,"blinkie-%02X%02X",
           xAppData.apMac[4], xAppData.apMac[5]);
   xAppData.apChan = 7;
}

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
       initSettings();
       storeSettings();
   } else {
       fread( &xAppData, sizeof(xAppData), 1, ptr);
       fclose( ptr);
   }
}

void addWifiGateway( char * ssid, char * passwd) {
	printf("ddf0 %s %s", ssid, passwd);
   if ((xAppData.wifiCount+1) < WIFI_TABLE_SIZE) {
       strlcpy(xAppData.wifi[xAppData.wifiCount].ssid, ssid, MAX_SSID_LENGTH);
       strlcpy(xAppData.wifi[xAppData.wifiCount].passwd, passwd, MAX_PASSWD_LENGTH);
       xAppData.wifiCount++;
   }
   storeSettings();
}

char * getWifiSsid( uint8_t index) {
   if ((index < xAppData.wifiCount)&&(strlen(xAppData.wifi[index].ssid)!=0)) {
      return( xAppData.wifi[index].ssid);
   } else {
      return( "[Not Used]" );
   }
}

char * getHSSsid( void ) {
   if (strlen(xAppData.apSsid)==0) {
      return( "[None]");
   }
   return(xAppData.apSsid);
}

char * getHSPasswd( void ) {
   if (strlen(xAppData.apPasswd)==0) {
      return( "[None]");
   }
   return(xAppData.apPasswd);
}

uint8_t getHSChan( void ) {
   return(xAppData.apChan);
}

char * getWifiPasswd( uint8_t index) {
   if (index < xAppData.wifiCount) {
      if (strlen(xAppData.wifi[index].passwd)==0) {
         return( "[None]");
      } else {
         return( xAppData.wifi[index].passwd);
      }
   } else {
      return( "[Not Used]" );
   }
}

void setHSSsid( char * hssid) {
   strlcpy(xAppData.apSsid, hssid, MAX_SSID_LENGTH); 
   storeSettings();
}

void setHSPasswd( char * hpasswd) {
   strlcpy(xAppData.apPasswd, hpasswd, MAX_PASSWD_LENGTH); 
   storeSettings();
}

void setHSChan( uint8_t hchan) {
   xAppData.apChan = hchan;
   storeSettings();
}

void setSystemType( bool type) {
   xAppData.testRelease = type;
   storeSettings();
}

bool getSystemType( void ) {
   return(xAppData.testRelease);
}

void setWifiSsid( uint8_t index, char * wssid) {
    if (index > 10) {
        ESP_LOGE(TAG, "wifi index out of range i=%d", index);
        return;
    }
    printf("set wifi ssid[%d] = %s\n",index, wssid);
    strlcpy(xAppData.wifi[index].ssid, wssid, MAX_SSID_LENGTH);
}

void setWifiPasswd( uint8_t index, char * wpasswd) {
    if (index > 10) {
        ESP_LOGE(TAG, "wifi index out of range i=%d", index);
        return;
    }
	printf("set wifi passwd[%d] = %s\n",index, wpasswd);
    strlcpy(xAppData.wifi[index].passwd, wpasswd, MAX_PASSWD_LENGTH);
}

void WifiCleanup( void ) {
   xAppData.wifiCount = 0;
   for (uint8_t i=0;i<10;i++) {
       printf("cleanup3 %d >%s<\n",i,xAppData.wifi[i].ssid);
       if (xAppData.wifi[i].ssid[0] != 0) {
           if ( i > xAppData.wifiCount) {
               printf("shift %d -> %d\n", i, xAppData.wifiCount);
               //shift down
               strlcpy(xAppData.wifi[xAppData.wifiCount].ssid,
                       xAppData.wifi[i].ssid, MAX_PASSWD_LENGTH);
               strlcpy(xAppData.wifi[xAppData.wifiCount].passwd,
                       xAppData.wifi[i].passwd, MAX_PASSWD_LENGTH);
	   }
           xAppData.wifiCount++;
       } else {
           printf("empty i=%d c=%d\n", i, xAppData.wifiCount);
       }
   } 
   storeSettings();
}


void setTZ( char * timezone) {
   printf("set TZ >%s|%c<\n", timezone, timezone[1]);
   xAppData.tzone = timezone[1] - '0';
}

void setTFormat( char * tformat) {
   printf("set TFormat >%s|%c<\n", tformat, tformat[1]);
   if (tformat[1] == '2') {
       xAppData.tformat = true;
   } else {
       xAppData.tformat = false;
   }
}
