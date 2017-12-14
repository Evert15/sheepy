/***************************************************************************//**
 * @file
 * @brief Simple LED Blink Demo for SLSTK3400A_EFM32HG
 * @version 5.2.2
 *******************************************************************************
 * # License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/


#define 	 LSM9DS1_REGISTER_WHO_AM_I_XG 	      	  0x0F
#define      LSM9DS1_REGISTER_CTRL_REG1_G         0x10
#define      LSM9DS1_REGISTER_CTRL_REG2_G         0x11
#define      LSM9DS1_REGISTER_CTRL_REG3_G         0x12
#define      LSM9DS1_REGISTER_TEMP_OUT_L          0x15
#define      LSM9DS1_REGISTER_TEMP_OUT_H          0x16
#define      LSM9DS1_REGISTER_STATUS_REG          0x17
#define      LSM9DS1_REGISTER_OUT_X_L_G           0x18
#define      LSM9DS1_REGISTER_OUT_X_H_G           0x19
#define      LSM9DS1_REGISTER_OUT_Y_L_G           0x1A
#define      LSM9DS1_REGISTER_OUT_Y_H_G           0x1B
#define      LSM9DS1_REGISTER_OUT_Z_L_G           0x1C
#define      LSM9DS1_REGISTER_OUT_Z_H_G           0x1D
#define      LSM9DS1_REGISTER_CTRL_REG4           0x1E
#define      LSM9DS1_REGISTER_CTRL_REG5_XL        0x1F
#define      LSM9DS1_REGISTER_CTRL_REG6_XL        0x20
#define      LSM9DS1_REGISTER_CTRL_REG7_XL        0x21
#define      LSM9DS1_REGISTER_CTRL_REG8           0x22
#define      LSM9DS1_REGISTER_CTRL_REG9           0x23
#define      LSM9DS1_REGISTER_CTRL_REG10          0x24

#define      LSM9DS1_REGISTER_OUT_X_L_XL          0x28
#define      LSM9DS1_REGISTER_OUT_X_H_XL          0x29
#define      LSM9DS1_REGISTER_OUT_Y_L_XL          0x2A
#define      LSM9DS1_REGISTER_OUT_Y_H_XL          0x2B
#define      LSM9DS1_REGISTER_OUT_Z_L_XL          0x2C
#define      LSM9DS1_REGISTER_OUT_Z_H_XL          0x2D

#include "em_device.h"
#include <stdint.h>
#include "em_chip.h"
#include "em_cmu.h"
#include "bsp.h"
#include "em_i2c.h"


static volatile uint32_t msTicks; /* counts 1ms timeTicks */

/*
 * ISR for system tick counter
 */
void SysTick_Handler (void)
{
  msTicks++;       /* increment counter necessary in Delay()*/
}

/*
 * Delays number of msTick systicks (typically 1 ms)
 */
static void Delay (uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}

void gpio_Setup (void)
{
	CMU_ClockEnable (cmuClock_GPIO, true);

	/* Setup USART0 as the master */
    GPIO_PinModeSet (gpioPortE, 10, gpioModePushPull, 0);	  /* MOSI */
    GPIO_PinModeSet (gpioPortE, 11, gpioModeInputPull, 1);	/* MISO */
    GPIO_PinModeSet (gpioPortE, 12, gpioModePushPull, 0);	  /* CLK */
   // GPIO_PinModeSet (gpioPortE, 13, gpioModePushPull, 1);	  /* CS, initially inactive high */
    GPIO_PinModeSet (gpioPortB, 11 ,gpioModePushPull , 1 ); //PB11 chip select
    GPIO_PortOutClear(gpioPortB, 11);
}

void usart0_Master_Setup (void)
{
	CMU_ClockEnable (cmuClock_USART0, true);

	/*
	 * Use default configuration and specify clock and frame size.
	 */
	USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;

	init.databits = usartDatabits8;
	init.baudrate = 1000000;
	init.msbf = true;
	USART_InitSync (USART0, &init);

	/* Turn on automatic Chip Select control */
	USART0->CTRL |= USART_CTRL_AUTOCS;

	/* Enable SPI transmit and receive */
	USART_Enable (USART0, usartEnable);

	/* Route USART0 pins to port A locations and enable */
	USART0->ROUTE = USART_ROUTE_LOCATION_LOC0 |
	                USART_ROUTE_CLKPEN |
	                USART_ROUTE_CSPEN |
	                USART_ROUTE_TXPEN |
	                USART_ROUTE_RXPEN;

}


void Chip_write_8_bit_data (uint8_t adr , uint8_t data){
	 USART0->CTRL |= USART_CTRL_MSBF;
	GPIO_PinOutClear(gpioPortB, 11);
	uint8_t adres;
	adres = (0x00 << 7) | adr;
	USART_Tx(USART0 , adres); // send the 8 adress bits
	USART_Tx(USART0 , data); // sent the 8 data bits
	GPIO_PinOutSet(gpioPortB, 11);
	}

uint8_t Chip_read_8_bit_data (uint8_t adr){
	 USART0->CTRL |= USART_CTRL_MSBF;
	GPIO_PinOutClear(gpioPortB, 11);
	uint8_t adres;
	adres = (0x01 << 7) | adr;
	USART_Tx(USART0 , adres);
	uint8_t data = USART_SpiTransfer(USART0 , 0x00); // dummy bit to send
	GPIO_PinOutSet(gpioPortB, 11);
	return  data;


}

void LSM9DS1_init(){
	Chip_write_8_bit_data(LSM9DS1_REGISTER_CTRL_REG8 , 0x05);
	Delay (10);
	Chip_write_8_bit_data(LSM9DS1_REGISTER_CTRL_REG5_XL, 0x38);
	Chip_write_8_bit_data(LSM9DS1_REGISTER_CTRL_REG6_XL, 0xC0);// here you can set the amount of G you can input

}







/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main (void)
{
	uint8_t data;


	CHIP_Init ();

	CMU_ClockSelectSet (cmuClock_HF, cmuSelect_HFRCO);

	CMU_ClockEnable (cmuClock_HFPER, true);

	/* setup SysTick timer for 1 msec interrupts  */
	if (SysTick_Config (CMU_ClockFreqGet (cmuClock_CORE) / 1000)) while (1) ;

	usart0_Master_Setup ();

	gpio_Setup ();
	LSM9DS1_init ();
	Delay (10);


	while (1)
	{


		//Chip_write_8_bit_data( 0x0f,0x15);
		uint8_t data = Chip_read_8_bit_data(LSM9DS1_REGISTER_WHO_AM_I_XG);
		uint8_t data_X_L = Chip_read_8_bit_data(LSM9DS1_REGISTER_OUT_X_L_XL);
		uint8_t data_X_H = Chip_read_8_bit_data(LSM9DS1_REGISTER_OUT_X_H_XL);
		uint8_t data_Y_L = Chip_read_8_bit_data(LSM9DS1_REGISTER_OUT_Y_L_XL);
		uint8_t data_Y_H = Chip_read_8_bit_data(LSM9DS1_REGISTER_OUT_Y_H_XL);
		uint8_t data_Z_L = Chip_read_8_bit_data(LSM9DS1_REGISTER_OUT_Z_L_XL);
		uint8_t data_Z_H = Chip_read_8_bit_data(LSM9DS1_REGISTER_OUT_Z_H_XL);


		/* Disable SPI transmit and receive */




		Delay (1);
	}
}
