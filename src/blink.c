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

#include "em_device.h"
#include <stdint.h>
#include "em_chip.h"
#include "em_cmu.h"
#include "bsp.h"
#include "em_i2c.h"


#define 	 LSM9DS1_REGISTER_WHO_AM_I_XG 	      0x0F
#define      LSM9DS1_REGISTER_STATUS_REG          0x17
#define      LSM9DS1_REGISTER_CTRL_REG4           0x1E
#define      LSM9DS1_REGISTER_CTRL_REG5_XL        0x1F
#define      LSM9DS1_REGISTER_CTRL_REG6_XL        0x20
#define      LSM9DS1_REGISTER_CTRL_REG7_XL        0x21
#define      LSM9DS1_REGISTER_CTRL_REG8           0x22
#define      LSM9DS1_REGISTER_CTRL_REG9           0x23
#define      LSM9DS1_REGISTER_CTRL_REG10          0x24


#define		 LSM9DS1_REGISTER_INT_GEN_THS_X_XL	  0x07
#define		 LSM9DS1_REGISTER_INT_GEN_THS_Y_XL	  0x08
#define		 LSM9DS1_REGISTER_INT_GEN_THS_Z_XL	  0x09
#define 	 LSM9DS1_REGISTER_INT_GEN_CFG_XL	  0x06
#define 	 LSM9DS1_REGISTER_INT1_CTRL			  0x0C

#define      LSM9DS1_REGISTER_OUT_X_L_XL          0x28
#define      LSM9DS1_REGISTER_OUT_X_H_XL          0x29
#define      LSM9DS1_REGISTER_OUT_Y_L_XL          0x2A
#define      LSM9DS1_REGISTER_OUT_Y_H_XL          0x2B
#define      LSM9DS1_REGISTER_OUT_Z_L_XL          0x2C
#define      LSM9DS1_REGISTER_OUT_Z_H_XL          0x2D

#define		 CHIP_EN_PIN						  11// Pin 11
#define		 CHIP_INT_PIN						  1 //pin 1
#define		 CHIP_EN_PORT						  1 //Port B
#define		 CHIP_INT_PORT						  0 //Port A


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
    GPIO_PinModeSet (CHIP_EN_PORT, CHIP_EN_PIN ,gpioModePushPull , 0 ); //PB11 chip select onboar Pin 11

    GPIO_PinModeSet (CHIP_INT_PORT, CHIP_INT_PIN ,gpioModeInputPull , 0); //PA1 chip select onboard Pin13
    GPIO_ExtIntConfig(CHIP_INT_PORT , CHIP_INT_PIN , CHIP_INT_PIN , true, false, true);

    GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
    GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);

    GPIO_IntClear(1<<CHIP_INT_PIN);
    GPIO_IntClear(1<<BSP_GPIO_PB0_PIN);

    GPIO_PortOutClear(CHIP_EN_PORT, CHIP_EN_PIN);
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
	GPIO_PinOutClear(CHIP_EN_PORT, CHIP_EN_PIN);
	uint8_t adres;
	adres = (0x00 << 7) | adr;
	USART_SpiTransfer(USART0 , adres); // send the 8 adress bits
	USART_SpiTransfer(USART0 , data); // sent the 8 data bits
	GPIO_PinOutSet(CHIP_EN_PORT, CHIP_EN_PIN);
	}

uint8_t Chip_read_8_bit_data (uint8_t adr){
	 USART0->CTRL |= USART_CTRL_MSBF;
	GPIO_PinOutClear(CHIP_EN_PORT, CHIP_EN_PIN);
	uint8_t adres;
	adres = (0x01 << 7) | adr;
	USART_SpiTransfer(USART0 , adres);
	uint8_t data = USART_SpiTransfer(USART0 , 0x00); // dummy bit to send
	GPIO_PinOutSet(CHIP_EN_PORT, CHIP_EN_PIN);
	return  data;


}

void LSM9DS1_init(){
	Chip_write_8_bit_data(LSM9DS1_REGISTER_CTRL_REG5_XL, 0x38);
	Chip_write_8_bit_data(LSM9DS1_REGISTER_CTRL_REG6_XL, 0x48);// here you can set the amount of G you can input
	Chip_write_8_bit_data(LSM9DS1_REGISTER_CTRL_REG9,0x44);
	Chip_write_8_bit_data(LSM9DS1_REGISTER_INT1_CTRL,0x40);


}

void LSM9DS1_int_init(){
	Chip_write_8_bit_data(LSM9DS1_REGISTER_INT_GEN_CFG_XL,0x7F);
	Chip_write_8_bit_data(LSM9DS1_REGISTER_INT_GEN_THS_X_XL,0x14);
	Chip_write_8_bit_data(LSM9DS1_REGISTER_INT_GEN_THS_Y_XL,0x14);
	Chip_write_8_bit_data(LSM9DS1_REGISTER_INT_GEN_THS_Z_XL,0x14);

}

void GPIO_ODD_IRQHandler(void)
	{
	  uint32_t interruptMask = GPIO_IntGet();
	  GPIO_IntClear(interruptMask);

	  if (interruptMask & (1 << BSP_GPIO_PB0_PIN)) {
	      /* PB0 */
		  BSP_LedClear(0);
	    }

	  if (interruptMask & (1 << CHIP_INT_PIN)) {
	      /* PB1 */
	    	BSP_LedSet(0);
	  }

	}


/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main (void)
{

	CHIP_Init ();

	CMU_ClockSelectSet (cmuClock_HF, cmuSelect_HFRCO); //clock settings

	CMU_ClockEnable (cmuClock_HFPER, true); // enable the clock


	/* setup SysTick timer for 1 msec interrupts  */
	if (SysTick_Config (CMU_ClockFreqGet (cmuClock_CORE) / 1000)) while (1) ;

	usart0_Master_Setup (); // enable the spi port
	gpio_Setup ();// set the gpio pins
	NVIC_EnableIRQ(GPIO_ODD_IRQn); // enable interupt on odd port numbers
	BSP_LedsInit(); // enable the warining led
	LSM9DS1_init (); // enable the sensor
	Delay (10);
	LSM9DS1_int_init();


	while (1)
	{
		EMU_EnterEM2();
	}



}

