/*
 * hardware_ini.c - functions for HW initialisation
 *
 * Copyright 2014 Edward V. Emelianov <eddy@sao.ru, edward.emelianoff@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

/*
 * All hardware-dependent initialisation & definition should be placed here
 * and in hardware_ini.h
 *
 */

#include "main.h"
#include "hardware_ini.h"

/**
 * GPIO initialisaion: clocking + pins setup
 */
void GPIO_init(){
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_IOPAEN |
			RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_IOPDEN |
			RCC_APB2ENR_IOPEEN);
	/*
	 * Setup EXTI on PA4 (PPS input from GPS) - pull down
	 * EXTI on PA5 - also pull down (trigger for time measurement)
	 */
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO4 | GPIO5);
	//AFIO_EXTICR2 = 0;
	exti_enable_request(EXTI4 | EXTI5);
	// trigger on rising edge
	exti_set_trigger(EXTI4 | EXTI5, EXTI_TRIGGER_RISING);
	nvic_enable_irq(NVIC_EXTI4_IRQ);
	nvic_enable_irq(NVIC_EXTI9_5_IRQ);
/*
	// Buttons: pull-up input
	gpio_set_mode(BTNS_PORT, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN,
			BTN_S2_PIN | BTN_S3_PIN);
	// turn on pull-up
	gpio_set(BTNS_PORT, BTN_S2_PIN | BTN_S3_PIN);
	// LEDS: opendrain output
	gpio_set_mode(LEDS_PORT, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN,
			LED_D1_PIN | LED_D2_PIN);
	// turn off LEDs
	gpio_set(LEDS_PORT, LED_D1_PIN | LED_D2_PIN);*/
/*
	// USB_DISC: push-pull
	gpio_set_mode(USB_DISC_PORT, GPIO_MODE_OUTPUT_2_MHZ,
				GPIO_CNF_OUTPUT_PUSHPULL, USB_DISC_PIN);
	// USB_POWER: open drain, externall pull down with R7 (22k)
	gpio_set_mode(USB_POWER_PORT, GPIO_MODE_INPUT,
				GPIO_CNF_INPUT_FLOAT, USB_POWER_PIN);
*/
}

/*
 * SysTick used for system timer with period of 1ms
 */
void SysTick_init(){
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8); // Systyck: 72/8=9MHz
	STK_RVR = STK_RVR_DEFAULT_VAL; // 9000 pulses: 1kHz
	systick_interrupt_enable();
	systick_counter_enable();
}

/**
 * PA5 interrupt - print time at button/switch trigger
 */
void exti9_5_isr(){
	if(EXTI_PR & EXTI5){
		if(trigger_ms == DIDNT_TRIGGERED){ // prevent bounce
			trigger_ms = Timer;
			memcpy(&trigger_time, &current_time, sizeof(curtime));
		}
		EXTI_PR = EXTI5;
	}
}

/*
// check buttons S2/S3
void check_btns(){
	static uint8_t oldstate[2] = {1,1}; // old buttons state
	uint8_t newstate[2], i;
	static uint32_t Old_timer[2] = {0,0};
	newstate[0] = gpio_get(BTNS_PORT, BTN_S2_PIN) ? 1 : 0;
	newstate[1] = gpio_get(BTNS_PORT, BTN_S3_PIN) ? 1 : 0;
	for(i = 0; i < 2; i++){
		uint8_t new = newstate[i];
		// pause for 60ms
		uint32_t O = Old_timer[i];
		if(O){
			if(Timer - O > 60 || O > Timer){
				P("Button S");
				usb_send('2' + i);
				if(new) P("released");
				else P("pressed");
				newline();
				oldstate[i] = new;
				Old_timer[i] = 0;
			}
		}
		else if(new != oldstate[i]){
			Old_timer[i] = Timer;
		}
	}
}
*/
