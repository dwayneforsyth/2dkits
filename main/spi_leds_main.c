/* SPI Master example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"

/*
 This code drives a chain of ata102 smart LED chains.
*/

#define BLINK_GPIO   02
#define PIN_NUM_MISO 12
#define PIN_NUM_MOSI 14
#define PIN_NUM_CLK  13
#define PIN_NUM_CS   04

#define STROBE       18
#define COMSIG0      03
#define COMSIG1      22
#define COMSIG2      23
#define COMSIG3      19
#define COMSIG4      21
#define COMSIG5      05
#define COMSIG6      17
#define COMSIG7      16


#define NUMBER_OF_LEDS 6

typedef struct {
   uint8_t red;
   uint8_t green;
   uint8_t blue;
} ledData_t;

spi_device_handle_t spi;
static uint8_t strobe = 0;
static uint8_t oldStrobe = 0;
bool spi_done = true;

static const uint8_t strobeGPIO[8] = {
        COMSIG0,
        COMSIG1,
        COMSIG2,
        COMSIG3,
        COMSIG4,
        COMSIG5,
        COMSIG6,
        COMSIG7
    };

void updateMBI5026Chain(spi_device_handle_t spi, uint16_t red, uint16_t green, uint16_t blue) {

    static uint8_t buffer[(NUMBER_OF_LEDS)+1];

    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction

    buffer[0] = red >> 8;
    buffer[1] = red & 0xff;
    buffer[2] = green >> 8;
    buffer[3] = green & 0xff;
    buffer[4] = blue >> 8;
    buffer[5] = blue & 0xff;

    t.tx_buffer=&buffer;               //The data is the cmd itself
    t.rx_buffer=NULL;
//    t.flags = SPI_TRANS_USE_TXDATA;
    t.length=6*8; // bits?
    spi_device_queue_trans(spi, &t, portMAX_DELAY);  //Transmit!
}

void spi_pre_transfer_callback() {
    gpio_set_level(PIN_NUM_CS, 0);
    spi_done = false;
    gpio_set_level(strobeGPIO[oldStrobe], 0);
}

void spi_post_transfer_callback() {
    gpio_set_level(strobeGPIO[oldStrobe], 0);
    gpio_set_level(strobeGPIO[oldStrobe], 0);
    gpio_set_level(strobeGPIO[oldStrobe], 0);
    gpio_set_level(strobeGPIO[oldStrobe], 0);
    gpio_set_level(STROBE, 1);
    gpio_set_level(STROBE, 0);
    gpio_set_level(PIN_NUM_CS, 1);
    // once spi is done, turn strobe on
    gpio_set_level(strobeGPIO[strobe], 1);
    spi_done = true;
}


uint16_t ledDataOut[8][16][4];

void setLed(uint8_t z, uint8_t x, uint8_t y, uint8_t iR, uint8_t iG, uint8_t iB) {
    uint8_t oStrobe = 0;
    uint16_t oOffset = 0;
    uint8_t ti = 0;


    switch(z) {
	case 0:
        default:
            oStrobe = x;
            oOffset = 1<<y;
	    break;
	case 1:
            oStrobe = x+4;
            oOffset = 1<<(y);
	    break;
	case 2:
            oStrobe = x;
            oOffset = 1<<(y+4);
	    break;
	case 3:
            oStrobe = x+4;
            oOffset = 1<<(y+4);
	    break;
    }

    for (ti=0;ti<15;ti++) {
	if (ti < iR) {
	    ledDataOut[oStrobe][ti][0] |= oOffset;
	} else {
	    ledDataOut[oStrobe][ti][0] &= ~oOffset;
	}
	if (ti < iG) {
	    ledDataOut[oStrobe][ti][1] |= oOffset;
	} else {
	    ledDataOut[oStrobe][ti][1] &= ~oOffset;
	}
    #if (1)
	if (ti < iB) {
	    ledDataOut[oStrobe][ti][2] |= oOffset;
	} else {
	    ledDataOut[oStrobe][ti][2] &= ~oOffset;
	}
#endif
    }
};

void updateLedTask(void *param) {
    static uint8_t intensity = 0;

    while (1) {
        intensity++;
        if (intensity >= 15) {
            intensity = 0;
	    oldStrobe = strobe;
            strobe++;
	    if (strobe >= 7) {
		 strobe = 0;
	         vTaskDelay(1);
	    }
        }

        while (spi_done != true);
        updateMBI5026Chain( spi,
     	    ledDataOut[strobe][intensity][1], 
     	    ledDataOut[strobe][intensity][2], 
     	    ledDataOut[strobe][intensity][0]);
    }
}

void allLedsOff() {
    uint8_t s,i,c;
    for (s=0;s<8;s++) {
        for (i=0;i<16;i++) {
            for (c=0;c<3;c++) {
                ledDataOut[s][i][c] = 0x0000;
}   }   }   } 


void app_main()
{
    esp_err_t ret;
    uint8_t i;
    const spi_bus_config_t buscfg={
        .miso_io_num=PIN_NUM_MISO,
        .mosi_io_num=PIN_NUM_MOSI,
        .sclk_io_num=PIN_NUM_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
	.max_transfer_sz=6*8
    };
    const spi_device_interface_config_t devcfg={
	.command_bits = 0,
	.address_bits = 0,
	.dummy_bits = 0,
        .clock_speed_hz=25*1000*1000,            //Clock out at 4 MHz
        .mode=3,                                //SPI mode 3
        .spics_io_num=-1,                       //CS pin
        .queue_size=7,                          //We want to be able to queue 7 transactions at a time
	.pre_cb=spi_pre_transfer_callback,
	.post_cb=spi_post_transfer_callback,
    };

    printf("Hello from Tower\n");

    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(BLINK_GPIO, 0);

    gpio_pad_select_gpio(PIN_NUM_CS);
    gpio_set_direction(PIN_NUM_CS, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_NUM_CS, 1);

    gpio_pad_select_gpio(STROBE);
    gpio_set_direction(STROBE, GPIO_MODE_OUTPUT);
    gpio_set_level(STROBE, 0);

    for (i=0;i<7l;i++) {
        gpio_pad_select_gpio(strobeGPIO[i]);
        gpio_set_direction(strobeGPIO[i], GPIO_MODE_OUTPUT);
        gpio_set_level(strobeGPIO[i], 0);
    }

    //Initialize the SPI bus
    ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    ESP_ERROR_CHECK(ret);
    //Attach the LEDs to the SPI bus
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);

    allLedsOff();
    setLed(0,0,0, 15,0,15);
    setLed(0,1,1, 8,0,0);
    setLed(0,2,2, 4,0,0);
    setLed(0,3,3, 1,0,0);
    setLed(0,3,3, 0,0,0);
 
    for (i=0;i<7;i++) {
        printf("i=%d R=%04X\n",i, ledDataOut[3][i][0]);
    }

    xTaskCreate(updateLedTask, "updateLedTask", 4*1024, NULL, 24, NULL);
    printf("about to loop\n");

    bool dr = true;
    bool db = true;
    bool dg = false;
    uint8_t j = 0;
    uint8_t r = 0;
    uint8_t b = 11;
    uint8_t g = 11;

    while(1) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
	if (db==true) {
	   b++;
	} else {
	   b--;
	}

	if (b==15) {
           db = false;
	} else if (b==1) {
	   db = true;
	}

	if (dr==true) {
	   r++;
	} else {
	   r--;
	}

	if (r==15) {
           dr = false;
	} else if (r==1) {
	   dr = true;
	}

	if (dg==true) {
	   g++;
	} else {
	   g--;
	}

	if (g==15) {
           dg = false;
	} else if (g==1) {
	   dg = true;
	}

        setLed(0,0,0, r,g,b);
        setLed(0,0,3, r,0,b);
        setLed(0,3,3, 0,g,b);
        setLed(0,3,0, r,g,0);
        setLed(0,2,1, g,g,g);
        setLed(0,1,2, r,r,r);

        printf("r=%d g=%d b=%d\n",r,g,b);
    }
}
