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
//   This is the LED driver for an ESP32 based 4x4x8 tower.
//   It runs using the esp32 parallel i2s peripheral
//**********************************************************************
//   I used https://github.com/willz1200/i2s_parallel_queued for the
//   starting point of this driver.

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "queued_i2s_parallel.h"
#include "driver/gpio.h"
#include "led_driver.h"
#include "led_map.h"
#include "version.h"

#define BLINK_GPIO     2
#define PIN_NUM_RED   25
#define PIN_NUM_GREEN 26
#define PIN_NUM_BLUE  14
#define PIN_NUM_CLK   13
#define PIN_ENABLE    18

#define PIN_LATCH      4
#define COMSIG0       15
#define COMSIG1       23
#define COMSIG2       22
#define COMSIG3       19
#define COMSIG4       10
#define COMSIG5       21
#define COMSIG6        5
#define COMSIG7        9

//Queue for dummy data used to block main loop when all buffers are full
QueueHandle_t main_data_queue;

#define bufferFrameSize (1024) //Number of 16-bit samples per buffer
#define bufferMemorySize (bufferFrameSize*2) //Becuase 2 bytes to store each 16-bit sample

uint16_t *bufferToFill; //Pointer to buffer that is next to be filled

uint8_t active = 0;

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} led_t;

led_t LED[3][128] = { 0 };


/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES: This gets called from the I2S interrupt.
           This only removes dummy data from queue to unblock main loop

*******************************************************************************/
void IRAM_ATTR buffer_filler_fn(void *buf, int len, void *arg) {
    QueueHandle_t queueHanle=(QueueHandle_t)arg;
    portBASE_TYPE high_priority_task_awoken = 0;

    uint8_t dummy[1];

    //Check if theres a dummy byte in the queue, indecating the main loop has updated the buffer, if not set to 0
    if (xQueueReceiveFromISR(queueHanle, (void*)&dummy, &high_priority_task_awoken)==pdFALSE) {
        memset(buf, 0, bufferFrameSize*2); //Nothing in queue. Zero out data
    }

    //Wake thread blocking the queue
    if (high_priority_task_awoken == pdTRUE) { //Check if a context switch needs to be requested
        portYIELD_FROM_ISR();
    }
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void writeSample(uint16_t *buf, uint16_t data, uint16_t pos) {
    buf[pos^1] = data;
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void setLedNum(uint8_t z, uint8_t x, uint8_t y, uint8_t iR, uint8_t iG, uint8_t iB, uint8_t set) {
    if ((z > 7)||(x>3)||(y>3)) {
        printf("Error set LED range\n");
	return;
    }

    uint8_t tR, tG, tB;

    getLed(z,x,y, &tR, &tG, &tB);

    if (iR == LED_PLUS) { iR = (tR==15)? 15: tR+1; }
    if (iG == LED_PLUS) { iG = (tG==15)? 15: tG+1; }
    if (iB == LED_PLUS) { iB = (tB==15)? 15: tB+1; }

    if (iR == LED_MINUS) { iR = (tR==0)? 0: tR-1; }
    if (iG == LED_MINUS) { iG = (tG==0)? 0: tG-1; }
    if (iB == LED_MINUS) { iB = (tB==0)? 0: tB-1; }

    if (iR == LED_NOOP) { iR = tR; }
    if (iG == LED_NOOP) { iG = tG; }
    if (iB == LED_NOOP) { iB = tB; }

    uint8_t number = z*16+x*4+y;
    LED[set][number].red = iR;
    LED[set][number].green = iG;
    LED[set][number].blue = iB;
}

void setLed(uint8_t z, uint8_t x, uint8_t y, uint8_t iR, uint8_t iG, uint8_t iB) {
    setLedNum(z, x, y, iR, iG, iB, 0);
}

void setLed2(uint8_t z, uint8_t x, uint8_t y, uint8_t iR, uint8_t iG, uint8_t iB) {
    setLedNum(z, x, y, iR, iG, iB, 1);
}

void setLed3(uint8_t z, uint8_t x, uint8_t y, uint8_t iR, uint8_t iG, uint8_t iB) {
    setLedNum(z, x, y, iR, iG, iB, 2);
}

void transferBuffer( void ) {
   memcpy(LED[0], LED[1], 128*3);
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void getLed(uint8_t z, uint8_t x, uint8_t y, uint8_t *iR, uint8_t *iG, uint8_t *iB) {

    if ((z > 7)||(x>3)||(y>3)) {
        printf("Error get LED range\n");
	return;
    }
    uint8_t number = z*16+x*4+y;
    *iR = LED[0][number].red;
    *iG = LED[0][number].green;
    *iB = LED[0][number].blue;
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void changeBank( uint8_t select ) {

    active = (select==0)? 0 : 2;
    printf("need to implement change bank\n");
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void allLedsOff() {
    for (uint8_t i =0; i< 128; i++) {
        LED[0][i].red = 0;
        LED[0][i].green = 0;
        LED[0][i].blue = 0;
    }
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void allLedsOn() {
    for (uint8_t i =0; i< 128; i++) {
        LED[0][i].red = 15;
        LED[0][i].green = 15;
        LED[0][i].blue = 15;
    }
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void allLedsColor( uint8_t red, uint8_t green, uint8_t blue) {
    for (uint8_t i =0; i< 128; i++) {
        LED[0][i].red = red;
        LED[0][i].green = green;
        LED[0][i].blue = blue;
    }
}

void allLedsColor2( uint8_t red, uint8_t green, uint8_t blue) {
    for (uint8_t i =0; i< 128; i++) {
        LED[1][i].red = red;
        LED[1][i].green = green;
        LED[1][i].blue = blue;
    }
}

void allLedsColor3( uint8_t red, uint8_t green, uint8_t blue) {
    for (uint8_t i =0; i< 128; i++) {
        LED[2][i].red = red;
        LED[2][i].green = green;
        LED[2][i].blue = blue;
    }
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
uint8_t ledOnOff( uint8_t color, uint8_t led, uint8_t bright) {
    uint8_t red   = (bright < LED[active][ledMap[led]].red)?   1 : 0;
    uint8_t green = (bright < LED[active][ledMap[led]].green)? 1 : 0;
    uint8_t blue  = (bright < LED[active][ledMap[led]].blue)?  1 : 0;
    return(red<<1|green<<2|blue<<3);
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void mainLoop(void) {
    uint16_t i = 0;
    uint16_t strobe = 0;
    uint16_t strobeOut = 0;
    uint8_t count = 0;
    uint16_t out = 0;
    uint8_t clock = 0;
    uint8_t led = 0;
    uint8_t bright = 0;
    uint8_t oldStrobe = 32;

    while(1) {
        uint8_t dummy[1];

        //Send the data to the queue becuase we've filled the buffer
        if (i%bufferFrameSize == 0) {
            xQueueSend(main_data_queue, dummy, portMAX_DELAY); //Blocked when queue is full
        }
        //Fill the buffer here
        if (bufferToFill!=NULL) {
            if (count < (NUM_LAYER*4+1)) {
                out = clock | ledOnOff(1, led, bright)| strobeOut;
                if (clock==0) {
                    clock = 1;
                } else {
                    clock = 0;
                    led++;
                }
            } else { // count == 17 / 33
                strobe = (strobe+1)%128;
                led = (strobe/16)*16;
                strobeOut = 1<<(5+(strobe/16));
                count = 0;
                clock = 0;
                if (oldStrobe != strobe/16) {
                    oldStrobe = strobe/16;
                    bright = 0;
                    out = 0x10;
                } else {
                    bright++;
                    out = 0x10+strobeOut;
                }
            }

            writeSample(bufferToFill, out, i%bufferFrameSize);
            count++;
        }
        i++;
    }
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void init_LED_driver(void) {

    allLedsColor( 0, 0, 0);
    allLedsColor2( 0, 0, 0);
    allLedsColor3( 0, 0, 0);
    gpio_pad_select_gpio(PIN_ENABLE);
    gpio_set_direction(PIN_ENABLE, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_ENABLE, 0);

    //Create data queue
    main_data_queue=xQueueCreate(1, 1);//bufferFrameSize*2
    //Initialize I2S parallel device.
    i2s_parallel_config_t i2scfg= {
        .gpio_bus={
            PIN_NUM_CLK, PIN_NUM_RED, PIN_NUM_GREEN, PIN_NUM_BLUE, PIN_LATCH,
            COMSIG0, COMSIG1, COMSIG2, COMSIG3, COMSIG4, COMSIG5, COMSIG6, COMSIG7,
            -1, -1, -1
        },
        .bits=I2S_PARALLEL_BITS_16,
        .clkspeed_hz=1000000, //1.00 MHz
        .bufsz=bufferFrameSize*2,
        .refill_cb=buffer_filler_fn, //Function Called by I2S interrupt
        .refill_cb_arg=main_data_queue //Queue pointer
    };
    i2s_parallel_setup(&I2S1, &i2scfg);
    i2s_parallel_start(&I2S1);

    xTaskCreatePinnedToCore(mainLoop, "mainLoop", 1024*3, NULL, 23, NULL, 1); //Use core 1 for main loop, I2S interrupt on core 0

    return;
}

