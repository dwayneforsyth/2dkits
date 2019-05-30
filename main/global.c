
#include "stddef.h"
#include "global.h"

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
    
