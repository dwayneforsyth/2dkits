//   Copyright (C) 2023 Dwayne Forsyth
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

#define ENC_PUSH 33
#define ENC_A    34
#define ENC_B    35

typedef encoder_t struct {
    bool direction;
    int8_t count;
}

void encoder_tick_callback(void) {
    int A = gpio_get_level( ENC_A );
    int B = gpio_get_level( ENC_B );

    if ((A == 0 && B == 1) || ((A == 1 && B == 0)))
    {
        drv_inst->invert_dir ? drv_inst->steps_count-- : drv_inst->steps_count++;
        return;
    }
    if ((A == 0 && B == 0) || ((A == 1 && B == 1)))
    {
        drv_inst->invert_dir ? drv_inst->steps_count++ : drv_inst->steps_count--;
        return;
    }
}

void encoder_push_callback(void) {
    int push = gpio_get_level( ENC_PUSH );
}

void init_encoder_knob( void ) {
    gpio_pad_select_gpio(ENC_A);
    gpio_set_direction(ENC_A, GPIO_MODE_INPUT);
    gpio_set_pull_mode(ENC_A, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(ENC_A, GPIO_INTR_ANYEDGE);
    gpio_isr_register(ENC_A, &encoder_tick_callback, null);
    gpio_intr_enable(ENC_A);

    gpio_pad_select_gpio(ENC_B);
    gpio_set_direction(ENC_B, GPIO_MODE_INPUT);
    gpio_set_pull_mode(ENC_B, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(ENC_B, GPIO_INTR_ANYEDGE);
    gpio_isr_register(ENC_B, &encoder_tick_callback, null);
    gpio_intr_enable(ENC_B);

    gpio_pad_select_gpio(ENC_PUSH);
    gpio_set_direction(ENC_PUSH, GPIO_MODE_INPUT);
    gpio_set_pull_mode(ENC_PUSH, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(ENC_PUSH, GPIO_INTR_ANYEDGE);
    gpio_isr_register(ENC_PUSH, &encoder_push_callback, null);
    gpio_intr_enable(ENC_PUSH);
}
