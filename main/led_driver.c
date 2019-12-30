//   Copyright (C) 2019 Dwayne Forsyth
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
//   This is the LED driver for an ESP32 based 4x4x8 tower.
//   It runs the GPIO and SPI bus to drive the physical LEDs.
//**********************************************************************
//
//   The 1st version is using a high level spi abstraction layer. I
//   see too much jitter in the LEDs so a more low level approch will
//   be required. We need a timer interupt, push 6 bytes down the SPI
//   then turn the LED bank strobe off, latch the LED driver chips, then
//   turn on the LED bank strobe.
//
//   This is a link to some of the low level spi info.
//   http://iot-bits.com/esp32/esp32-spi-tutorial-part-1/
//   https://www.elecrow.com/download/ESP32_technical_reference_manual.pdf

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "soc/spi_reg.h"
#include "driver/gpio.h"
#include "led_driver.h"
/*
   This code drive the 2DKits.com 4x4x8 tower
*/

//#define PROTO1

#define BLINK_GPIO   02
#define PIN_NUM_MISO 12 //12
#define PIN_NUM_MOSI 14 //13
#define PIN_NUM_CLK  13 //14
#define PIN_NUM_CS   18 //04?

#define LATCH        04 // 18
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

static const uint8_t strobeGPIO[8] = {
        COMSIG0,
        COMSIG1,
        COMSIG2,
        COMSIG3,
        COMSIG4,
        COMSIG5,
        COMSIG6,
        COMSIG7,
    };


// https://github.com/loboris/ESP32_NEW_SPI_MASTER_EXAMPLE/blob/master/main/spi_master_demo.c

/*******************************************************************************
    PURPOSE: 

    INPUTS:

    OUTPUTS:
        NONE

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void updateMBI5026Chain(spi_device_handle_t spi, uint16_t red, uint16_t green, uint16_t blue, uint8_t strobe) {

    static uint8_t buffer[129][(NUMBER_OF_LEDS)+1];
    static uint8_t mark1 = 0xAA;
    static spi_transaction_t t[129];
    static uint8_t mark2 = 0xAA;
    memset(&t[strobe], 0, sizeof(t[0]));       //Zero out the transaction


    if ((mark1 != 0xAA)||(mark2 != 0xAA)) {
	    ESP_ERROR_CHECK(1);
    }

    if (strobe > 127) strobe = 128;

#if (1)
    buffer[strobe][0] = red >> 8;
    buffer[strobe][1] = red & 0xff;
    buffer[strobe][2] = green >> 8;
    buffer[strobe][3] = green & 0xff;
    buffer[strobe][4] = blue >> 8;
    buffer[strobe][5] = blue & 0xff;
#else
    buffer[strobe][0] = 0xff;
    buffer[strobe][1] = 0xff;
    buffer[strobe][2] = strobe>>4;
    buffer[strobe][3] = strobe & 0x0F;
    buffer[strobe][4] = 0xAA;
    buffer[strobe][5] = 0xff;
#endif

    t[strobe].tx_buffer=&buffer[strobe];               //The data is the cmd itself
    t[strobe].length=6*8; // bits?
    t[strobe].user = strobe;

    spi_device_queue_trans(spi, &t[strobe], portMAX_DELAY);  //Transmit!
}

/*******************************************************************************
    PURPOSE:
        the ledDataOut is in the format streamed out the SPI port. We access
	the data the most to send it out the SPI, so keeping it in this format
	optimized the data for speed.

	For this description, a single RGB package LED is 3 LEDs, a Red, Green,
       	and Blue.

	The tower is a 4x4x8x3 (x,y,layer,color) LED structure. The driver is
       	(3x16)x8 matrix.  The 3x16 is three colors of 16 bits of data. so every
       	1/8 cycle up to 48 LEDs are light. The SPI buss pumps out 16 copies of
       	the 48 LEDs per the 1/8 cycle, giving the intensity or brightness.

	Cycle 0 - up to 48 LEDs on
	Cycle 0 - intensity 14 or higher
	Cycle 0 - bringness 13 or higher
	  ...
	Cycle 0
	Cycle 0 - brightness 1 or higher
	Cycle 1 - up to 48 LEDs on
	        Cycle 0 - intensity 14 or higher
        Cycle 1 - bringness 13 or higher
          ...
        Cycle 1
        Cycle 1 - brightness 1 or higher
	  ...
	Cycle 7 - up to 48 LEDs on
	        Cycle 0 - intensity 14 or higher
        Cycle 7 - bringness 13 or higher
          ...
        Cycle 7
        Cycle 7 - brightness 1 or higher

	We have two copies of the strobe data. When the user pushes the button
	a second copy of the LEDs structor is used to display the pattern number
	once done the data is switched back.

*******************************************************************************/

static uint16_t ledDataOut[2][8][16][4];
static uint8_t bank = 0;

/*******************************************************************************
    PURPOSE: 

    INPUTS:

    OUTPUTS:
        NONE

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void changeBank( uint8_t select ) {

    if (select < 2) {
        bank = select;
    }
    printf("running LED strobe bank = %d",bank);
}

/*******************************************************************************
    PURPOSE: 

    INPUTS:

    OUTPUTS:
        NONE

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void getStrobeOffset(uint8_t z, uint8_t x, uint8_t y, uint8_t *oStrobe, uint16_t *oOffset ) {

    switch(z) {
	case 0:
            *oStrobe = x;
            *oOffset = 1<<y;
	    break;
	case 1:
            *oStrobe = x+4;
            *oOffset = 1<<(y);
	    break;
	case 4:
            *oStrobe = x;
            *oOffset = 1<<(y+4);
	    break;
	case 5:
            *oStrobe = x+4;
            *oOffset = 1<<(y+4);
	    break;
	case 6:
            *oStrobe = x;
            *oOffset = 1<<(y+8);
	    break;
	case 7:
            *oStrobe = x+4;
            *oOffset = 1<<(y+8);
	    break;
	case 2:
            *oStrobe = x;
            *oOffset = 1<<(y+12);
	    break;
	case 3:
            *oStrobe = x+4;
            *oOffset = 1<<(y+12);
	    break;
        default:
            printf("out of range layer = %d\n",z);
	    break;
    }

}
#define RED 0
#define GREEN 1
#define BLUE 2
/*******************************************************************************
    PURPOSE: We have different physical LEDs with the pins driving different
             colors. This maps the RGB values to the correct pins. We are
             going to assume all the LEDs on a given level have the same
             pinout.

    INPUTS:
             level = which layer of the cube/tower 0 .. 7, 0 is base
             color = 0 .. 2, Red, Green, Blue

    OUTPUTS:
        NONE

    RETURN CODE:
             which pin is the right one to toggle

    NOTES:

*******************************************************************************/
uint8_t ledAdjust(uint8_t level, uint8_t color) {

#ifdef PROTO1
    uint8_t colorMap[8][3] = {
        {RED, GREEN, BLUE},
        {BLUE, RED, GREEN},
        {BLUE, RED, GREEN},
        {BLUE, RED, GREEN},
        {BLUE, RED, GREEN},
        {BLUE, RED, GREEN},
        {BLUE, RED, GREEN},
        {BLUE, RED, GREEN},
    };
#else 
    uint8_t colorMap[8][3] = {
        {RED, GREEN, BLUE},
        {RED, GREEN, BLUE},
        {RED, GREEN, BLUE},
        {RED, GREEN, BLUE},
        {RED, GREEN, BLUE},
        {RED, GREEN, BLUE},
        {RED, GREEN, BLUE},
        {RED, GREEN, BLUE},
    };
#endif

    return(colorMap[level][color]);
}

/*******************************************************************************
    PURPOSE: 

    INPUTS:

    OUTPUTS:
        NONE

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void getLed(uint8_t z, uint8_t x, uint8_t y, uint8_t *iR, uint8_t *iG, uint8_t *iB) {
    uint8_t oStrobe = 0;
    uint16_t oOffset = 0;
    uint8_t ti = 0;

    *iR=*iG=*iB=0;

    getStrobeOffset( z, x, y, &oStrobe, &oOffset );

    for (ti=0;ti<15;ti++) {
	if (ledDataOut[bank][oStrobe][ti][ledAdjust(z,RED)] & oOffset) *iR+=1;
	if (ledDataOut[bank][oStrobe][ti][ledAdjust(z,GREEN)] & oOffset) *iG+=1;
	if (ledDataOut[bank][oStrobe][ti][ledAdjust(z,BLUE)] & oOffset) *iB+=1;
    }
}

/*******************************************************************************
    PURPOSE: 

    INPUTS:

    OUTPUTS:
        NONE

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void setLed(uint8_t z, uint8_t x, uint8_t y, uint8_t iR, uint8_t iG, uint8_t iB) {
    uint8_t oStrobe = 0;
    uint16_t oOffset = 0;
    uint8_t ti = 0;
    uint8_t tR, tG, tB;

    if (iR == LED_PLUS) { iR = (tR=15)? 15: tR+1; }
    if (iG == LED_PLUS) { iG = (tG=15)? 15: tG+1; }
    if (iB == LED_PLUS) { iB = (tB=15)? 15: tB+1; }

    if (iR == LED_MINUS) { iR = (tR=0)? 0: tR-1; }
    if (iG == LED_MINUS) { iG = (tG=0)? 0: tG-1; }
    if (iB == LED_MINUS) { iB = (tB=0)? 0: tB-1; }

    if (iR == LED_NOOP) { iR = tR; }
    if (iG == LED_NOOP) { iG = tG; }
    if (iB == LED_NOOP) { iB = tB; }

    // save some time if no change
    getLed(z,x,y, &tR, &tG, &tB);
    if ((tR == iR)&&(tG == iG)&&(tB == iB)) return;

    getStrobeOffset( z, x, y, &oStrobe, &oOffset );

    for (ti=0;ti<15;ti++) {
	if (ti < iR) {
	    ledDataOut[bank][oStrobe][ti][ledAdjust(z,RED)] |= oOffset;
	} else {
	    ledDataOut[bank][oStrobe][ti][ledAdjust(z,RED)] &= ~oOffset;
	}
	if (ti < iG) {
	    ledDataOut[bank][oStrobe][ti][ledAdjust(z,GREEN)] |= oOffset;
	} else {
	    ledDataOut[bank][oStrobe][ti][ledAdjust(z,GREEN)] &= ~oOffset;
	}
	if (ti < iB) {
	    ledDataOut[bank][oStrobe][ti][ledAdjust(z,BLUE)] |= oOffset;
	} else {
	    ledDataOut[bank][oStrobe][ti][ledAdjust(z,BLUE)] &= ~oOffset;
	}
    }
}

/*******************************************************************************
    PURPOSE: 
        Hope was to turn off the TD62783APG "old" row line earlier (72us) to
	reduce the ghosting. It should shut off in 1.8us, do not think this is
        a cause of GHosting. The Saleae analysis shows the output of the TD62783APG
        longer then it should be (and does not reflect when the LEDs are going. I
        think nothing drains the power level after the LEDs cut out.
       
	I found the cause of most of the ghosting so and do not think the early
        row cutoff is adding value.

    INPUTS:
        SPI transaction about to start. THe user file has the strobe and intensity
	setting stored in it. (It is not a pointer to user data)

    OUTPUTS:
        NONE

    RETURN CODE:
        NONE

    NOTES:
        This is running under the interupt context, keep is short and sweet.        

*******************************************************************************/
#ifdef ROW_GAP
void IRAM_ATTR spi_pre_transfer_callback(spi_transaction_t *curTrans) {

    uint8_t strobe = ((uint8_t) curTrans->user) >>4;

    static uint8_t oldStrobe = 0;
    if (oldStrobe != strobe) {
	if (oldStrobe <8) {
	    gpio_set_level(strobeGPIO[oldStrobe], 0); // row off
	} 
	oldStrobe = strobe;
    }
}
#endif

/*******************************************************************************
    PURPOSE: 
        Latches the SPI data just loaded into the driver of the MBI5026 Column
        Changes the driver line that is activive ont he TD62783 chip. Row	

    INPUTS:
        SPI transaction just finished. The user file has the strobe and intensity
	setting stored in it. (It is not a pointer to user data)
	The strobe is used for the TD62783 line to make active.

    OUTPUTS:
        NONE

    RETURN CODE:
        NONE

    NOTES:
        This is running under the interupt context, keep is short and sweet.

        I do not think the "oldStrobe" logic is adding any value.	

*******************************************************************************/
void IRAM_ATTR spi_post_transfer_callback(spi_transaction_t *curTrans) {

    uint8_t strobe = ((uint8_t) curTrans->user) >>4;

#ifndef ROW_GAP
    static uint8_t oldStrobe = 0;
    if (oldStrobe <8) {
        gpio_set_level(strobeGPIO[oldStrobe], 0); // row off
    } 
    oldStrobe = strobe;
#endif
    gpio_set_level(LATCH, 1); //clear latch;
    asm volatile ("nop");
    gpio_set_level(LATCH, 0); //Latch in col

    if (strobe < 8) {
        gpio_set_level(strobeGPIO[strobe], 1); // row on
    }
}

/*******************************************************************************
    PURPOSE: 
        This will queue up 129 spi messages, one display cycle, then sleep for
        8 ms.

        8 cycles (columns) * 16 intensities + 1 all LEDs off.	

    INPUTS:
        NONE

    OUTPUTS:
        NONE

    RETURN CODE:
        NONE

    NOTES:
        this is the highest pri task.

*******************************************************************************/

void IRAM_ATTR updateLedTask(void *param) {
    uint8_t intensity = 0;
    uint8_t strobe = 0;
    spi_transaction_t *lastSpi;

    while (1) {

	// start loading next frame
	for (strobe=0;strobe<8; strobe++) {
            for (intensity=0;intensity<16;intensity++) {
                updateMBI5026Chain( spi,
                    ledDataOut[bank][strobe][intensity][1],
                    ledDataOut[bank][strobe][intensity][2],
                    ledDataOut[bank][strobe][intensity][0],(strobe<<4)+intensity);
	    }
	}

	// this is dumb, need to read the result of each request to see when the
	// queue is empty. we will get blocked each time a "done" entry is still
	// in the queue.
	uint8_t index;
	for (index=0;index<(8*16);index++) {
	     spi_device_get_trans_result(spi, &lastSpi, portMAX_DELAY);
	}
    }
}

/*******************************************************************************
    PURPOSE: 

    INPUTS:

    OUTPUTS:
        NONE

    RETURN CODE:
        NONE

    NOTES: bypass all the math and sets the strob data to all zeros.

*******************************************************************************/
void allLedsOff() {
    uint8_t s,i,c;
    for (s=0;s<8;s++) {
        for (i=0;i<15;i++) {
            for (c=0;c<3;c++) {
                ledDataOut[bank][s][i][c] = 0x0000;
}   }   }   } 

/*******************************************************************************
    PURPOSE: 

    INPUTS:

    OUTPUTS:
        NONE

    RETURN CODE:
        NONE

    NOTES:
        bypass all the math and sets the strob data to all zeros.

*******************************************************************************/
void allLedsOn() {
    uint8_t s,i,c;
    for (s=0;s<8;s++) {
        for (i=0;i<15;i++) {
            for (c=0;c<3;c++) {
                ledDataOut[bank][s][i][c] = 0xffff;
}   }   }   } 

/*******************************************************************************
    PURPOSE: 

    INPUTS:

    OUTPUTS:
        NONE

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void allLedsColor( uint8_t red, uint8_t green, uint8_t blue) {
    uint8_t l,x,y;
    for (l=0;l<8;l++) {
        for (x=0;x<4;x++) {
            for (y=0;y<4;y++) {
               setLed(l,x,y, red,green,blue);
}   }   }   }

/*******************************************************************************
    PURPOSE: 

    INPUTS:

    OUTPUTS:
        NONE

    RETURN CODE:
        NONE

    NOTES:
    http://iot-bits.com/esp32/esp32-spi-tutorial-part-1/
    https://www.mouser.com/pdfdocs/ESP32-Tech_Reference.pdf

*******************************************************************************/
void init_LED_driver() {
    esp_err_t ret;
    uint8_t i;
    const spi_bus_config_t buscfg={
        .miso_io_num=PIN_NUM_MISO,
        .mosi_io_num=PIN_NUM_MOSI,
        .sclk_io_num=PIN_NUM_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
	.max_transfer_sz=6*8,
    };
    const spi_device_interface_config_t devcfg={
	.command_bits = 0,
	.address_bits = 0,
	.dummy_bits = 0,
        .clock_speed_hz=750*1000,           //Clock out at 750 KHz (Yes this is slow)
        .mode=0,                                //SPI mode 0
        .spics_io_num=-1,                       //CS pin
        .queue_size=129,                          //We want to be able to queue 129 transactions at a time
#ifdef ROW_GAP
	.pre_cb=spi_pre_transfer_callback,
#endif
	.post_cb=spi_post_transfer_callback,
    };

    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(BLINK_GPIO, 0);

    gpio_pad_select_gpio(PIN_NUM_CS);
    gpio_set_direction(PIN_NUM_CS, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_NUM_CS, 0);

    gpio_pad_select_gpio(LATCH);
    gpio_set_direction(LATCH , GPIO_MODE_OUTPUT);
    gpio_set_level(LATCH, 0);

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
