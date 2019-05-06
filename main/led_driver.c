


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
   This code drive the 2DKits.com 4x4x8 tower
*/

#define BLINK_GPIO   02
#define PIN_NUM_MISO 12
#define PIN_NUM_MOSI 14
#define PIN_NUM_CLK  13
#define PIN_NUM_CS   04

#define STROBE       18
#define COMSIG0      15 // 15
#define COMSIG1      23 // 22
#define COMSIG2      22 // 23
#define COMSIG3      19 // 19
#define COMSIG4      17 // 21
#define COMSIG5      21 // 05
#define COMSIG6      05 // 17
#define COMSIG7      16 // 16


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

static const uint8_t strobeGPIO[9] = {
        COMSIG0,
        COMSIG1,
        COMSIG2,
        COMSIG3,
        COMSIG4,
        COMSIG5,
        COMSIG6,
        COMSIG7,
	0
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

void IRAM_ATTR spi_pre_transfer_callback() {
    gpio_set_level(PIN_NUM_CS, 0);
    spi_done = false;
}

void IRAM_ATTR spi_post_transfer_callback() {
    gpio_set_level(strobeGPIO[oldStrobe], 0);
    gpio_set_level(STROBE, 1);
    gpio_set_level(STROBE, 0);
    gpio_set_level(PIN_NUM_CS, 1);
    // once spi is done, turn strobe on
    gpio_set_level(strobeGPIO[strobe], 1);
    spi_done = true;
}


uint16_t ledDataOut[9][16][4];

void setLed(uint8_t z, uint8_t x, uint8_t y, uint8_t iR, uint8_t iG, uint8_t iB) {
    uint8_t oStrobe = 0;
    uint16_t oOffset = 0;
    uint8_t ti = 0;


    switch(z) {
	case 0:
            oStrobe = x;
            oOffset = 1<<y;
	    break;
	case 1:
            oStrobe = x+4;
            oOffset = 1<<(y);
	    break;
	case 4:
            oStrobe = x;
            oOffset = 1<<(y+4);
	    break;
	case 5:
            oStrobe = x+4;
            oOffset = 1<<(y+4);
	    break;
	case 6:
            oStrobe = x;
            oOffset = 1<<(y+8);
	    break;
	case 7:
            oStrobe = x+4;
            oOffset = 1<<(y+8);
	    break;
	case 2:
            oStrobe = x;
            oOffset = 1<<(y+12);
	    break;
	case 3:
            oStrobe = x+4;
            oOffset = 1<<(y+12);
	    break;
        default:
            printf("out of range layer = %d\n",z);
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
	if (ti < iB) {
	    ledDataOut[oStrobe][ti][2] |= oOffset;
	} else {
	    ledDataOut[oStrobe][ti][2] &= ~oOffset;
	}
    }
};

//
// this is the highest pri task, and it is currently running full speed
// it releases controll 1/9 of time while all the leds are off.
//
// have a bug where strobe 0 is running brigher then the others
//
void updateLedTask(void *param) {
    static uint8_t intensity = 0;

    while (1) {
        intensity++;
        if (intensity >= 15) {
            intensity = 0;
	    oldStrobe = strobe;
            strobe++;
	    if (strobe >= 8) {
		 strobe = 0;
                 gpio_set_level(strobeGPIO[7], 0);
	         vTaskDelay(2); // all leds are off, release control
	    }
        }

        while (spi_done != true);
        updateMBI5026Chain( spi,
     	    ledDataOut[strobe][intensity][1], 
     	    ledDataOut[strobe][intensity][2], 
     	    ledDataOut[strobe][intensity][0]);
    }
}

//
// bypass all the math and sets the strob data to all zeros.
//
void allLedsOff() {
    uint8_t s,i,c;
    for (s=0;s<9;s++) {
        for (i=0;i<16;i++) {
            for (c=0;c<3;c++) {
                ledDataOut[s][i][c] = 0x0000;
}   }   }   } 

void allLedsOn() {
    uint8_t s,i,c;
    for (s=0;s<9;s++) {
        for (i=0;i<16;i++) {
            for (c=0;c<3;c++) {
                ledDataOut[s][i][c] = 0xffff;
}   }   }   } 

//
//
//
void allLedsColor( uint8_t red, uint8_t green, uint8_t blue) {
    uint8_t l,x,y;
    for (l=0;l<8;l++) {
        for (x=0;x<4;x++) {
            for (y=0;y<4;y++) {
               setLed(l,x,y, red,green,blue);
}   }   }   }

void init_LED_driver() {
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

    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(BLINK_GPIO, 0);

    gpio_pad_select_gpio(PIN_NUM_CS);
    gpio_set_direction(PIN_NUM_CS, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_NUM_CS, 1);

    gpio_pad_select_gpio(STROBE);
    gpio_set_direction(STROBE, GPIO_MODE_OUTPUT);
    gpio_set_level(STROBE, 0);

    for (i=0;i<8;i++) {
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

    xTaskCreate(updateLedTask, "updateLedTask", 4*1024, NULL, 24, NULL);
}
