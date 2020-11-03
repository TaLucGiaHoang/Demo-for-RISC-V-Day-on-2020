/*
 * demo_control_led.c
 *
 *  Created on: Nov 3, 2020
 *      Author: HoangSHC
 */
#include "platform.h"

#define LED1_ON()     DEV_GPIO->DATAOUT |= (1 << 5)  // Turn on LED1
#define LED2_ON()     DEV_GPIO->DATAOUT |= (1 << 6)  // Turn on LED2
#define LED3_ON()     DEV_GPIO->DATAOUT |= (1 << 7)  // Turn on LED3
#define LED4_ON()     DEV_GPIO->DATAOUT |= (1 << 8)  // Turn on LED4
#define LED1_OFF()    DEV_GPIO->DATAOUT &= ~(1 << 5) // Turn off LED1
#define LED2_OFF()    DEV_GPIO->DATAOUT &= ~(1 << 6) // Turn off LED2
#define LED3_OFF()    DEV_GPIO->DATAOUT &= ~(1 << 7) // Turn off LED3
#define LED4_OFF()    DEV_GPIO->DATAOUT &= ~(1 << 8) // Turn off LED4

static void led1(int val)
{
	if(val == 0) {
		LED1_OFF();
	} else {
		LED1_ON();
	}
}

static void led2(int val)
{
	if(val == 0) {
		LED2_OFF();
	} else {
		LED2_ON();
	}
}

static void led3(int val)
{
	if(val == 0) {
		LED3_OFF();
	} else {
		LED3_ON();
	}
}

static void led4(int val)
{
	if(val == 0) {
		LED4_OFF();
	} else {
		LED4_ON();
	}
}

void control_led_init(void)
{
	 DEV_GPIO->CHANNELDIR |= 0x1E0;    // Set GPIO5~8 direction to output mode.
	 DEV_GPIO->DATAOUT &= ~(0x1E0);    // Turn off all LEDs. Clear GPIO5~8.
}

int control_led(char c)
{
	// This demo only use LED1
	if(c == '0') {
//		led1(0);
		DEV_GPIO->DATAOUT &= ~(0x1E0);    // Turn off all LEDs. Clear GPIO5~8.
	} else if (c == '1') {
//		led1(1);
		DEV_GPIO->DATAOUT |= (0x1E0);    // Turn on all LEDs. Set GPIO5~8.
	} else {
		return -1;
	}
	return 0;
}

