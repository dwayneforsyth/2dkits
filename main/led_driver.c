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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include <string.h>
#include "queued_i2s_parallel.h"
#include "driver/gpio.h"
#include <stdio.h>

#define BLINK_GPIO     2
#define PIN_NUM_RED   26
#define PIN_NUM_GREEN 25
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

const uint8_t ledMap[128] = {
    2*16+0*4+0, 2*16+0*4+1, 2*16+0*4+2, 2*16+0*4+3,
    6*16+0*4+0, 6*16+0*4+1, 6*16+0*4+2, 6*16+0*4+3,
    4*16+0*4+0, 4*16+0*4+1, 4*16+0*4+2, 4*16+0*4+3,
    0*16+0*4+0, 0*16+0*4+1, 0*16+0*4+2, 0*16+0*4+3,
    2*16+1*4+0, 2*16+1*4+1, 2*16+1*4+2, 2*16+1*4+3,
    6*16+1*4+0, 6*16+1*4+1, 6*16+1*4+2, 6*16+1*4+3,
    4*16+1*4+0, 4*16+1*4+1, 4*16+1*4+2, 4*16+1*4+3,
    0*16+1*4+0, 0*16+1*4+1, 0*16+1*4+2, 0*16+1*4+3,
    2*16+2*4+0, 2*16+2*4+1, 2*16+2*4+2, 2*16+2*4+3,
    6*16+2*4+0, 6*16+2*4+1, 6*16+2*4+2, 6*16+2*4+3,
    4*16+2*4+0, 4*16+2*4+1, 4*16+2*4+2, 4*16+2*4+3,
    0*16+2*4+0, 0*16+2*4+1, 0*16+2*4+2, 0*16+2*4+3,
    2*16+3*4+0, 2*16+3*4+1, 2*16+3*4+2, 2*16+3*4+3,
    6*16+3*4+0, 6*16+3*4+1, 6*16+3*4+2, 6*16+3*4+3,
    4*16+3*4+0, 4*16+3*4+1, 4*16+3*4+2, 4*16+3*4+3,
    0*16+3*4+0, 0*16+3*4+1, 0*16+3*4+2, 0*16+3*4+3,
    3*16+0*4+0, 3*16+0*4+1, 3*16+0*4+2, 3*16+0*4+3,
    7*16+0*4+0, 7*16+0*4+1, 7*16+0*4+2, 7*16+0*4+3,
    5*16+0*4+0, 5*16+0*4+1, 5*16+0*4+2, 5*16+0*4+3,
    1*16+0*4+0, 1*16+0*4+1, 1*16+0*4+2, 1*16+0*4+3,
    3*16+1*4+0, 3*16+1*4+1, 3*16+1*4+2, 3*16+1*4+3,
    7*16+1*4+0, 7*16+1*4+1, 7*16+1*4+2, 7*16+1*4+3,
    5*16+1*4+0, 5*16+1*4+1, 5*16+1*4+2, 5*16+1*4+3,
    1*16+1*4+0, 1*16+1*4+1, 1*16+1*4+2, 1*16+1*4+3,
    3*16+2*4+0, 3*16+2*4+1, 3*16+2*4+2, 3*16+2*4+3,
    7*16+2*4+0, 7*16+2*4+1, 7*16+2*4+2, 7*16+2*4+3,
    5*16+2*4+0, 5*16+2*4+1, 5*16+2*4+2, 5*16+2*4+3,
    1*16+2*4+0, 1*16+2*4+1, 1*16+2*4+2, 1*16+2*4+3,
    3*16+3*4+0, 3*16+3*4+1, 3*16+3*4+2, 3*16+3*4+3,
    7*16+3*4+0, 7*16+3*4+1, 7*16+3*4+2, 7*16+3*4+3,
    5*16+3*4+0, 5*16+3*4+1, 5*16+3*4+2, 5*16+3*4+3,
    1*16+3*4+0, 1*16+3*4+1, 1*16+3*4+2, 1*16+3*4+3
};

uint8_t RED[128] = { 0 };
uint8_t GREEN[128] = { 0 };
uint8_t BLUE[128] = { 0 };


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
void setLed(uint8_t z, uint8_t x, uint8_t y, uint8_t iR, uint8_t iG, uint8_t iB) {
    if ((z > 7)||(x>3)||(y>3)) {
        printf("Error set LED range\n");
	return;
    }
    uint8_t number = z*16+x*4+y;
    RED[number] = iR;
    GREEN[number] = iG;
    BLUE[number] = iB;
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
    *iR = RED[number];
    *iG = GREEN[number];
    *iB = BLUE[number];
}

/*******************************************************************************
    PURPOSE:

    INPUTS:

    RETURN CODE:
        NONE

    NOTES:

*******************************************************************************/
void changeBank( uint8_t select ) {

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
        RED[i] = 0;
        GREEN[i] = 0;
        BLUE[i] = 0;
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
        RED[i] = 15;
        GREEN[i] = 15;
        BLUE[i] = 15;
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
        RED[i] = red;
        GREEN[i] = green;
        BLUE[i] = blue;
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
    uint8_t red = (bright < RED[ledMap[led]])? 1 : 0;
    uint8_t green = (bright < GREEN[ledMap[led]])? 1 : 0;
    uint8_t blue = (bright < BLUE[ledMap[led]])? 1 : 0;
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
            if (count < 33) {
                out = clock | ledOnOff(1, led, bright)| strobeOut;
                if (clock==0) {
                    clock = 1;
                } else {
                    clock = 0;
                    led++;
                }
            } else { // count == 33
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
int init_LED_driver(void) {
    printf("Hello World\n");
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

    xTaskCreatePinnedToCore(mainLoop, "mainLoop", 1024*16, NULL, 7, NULL, 1); //Use core 1 for main loop, I2S interrupt on core 0

    return 0;
}

