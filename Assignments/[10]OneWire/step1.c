// Nu_LB-002: L09_01_DS1820_Timer3.c
/*
	Onewire : GPE8
*/

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* written by dejwoot.kha@mail.kmutt.ac.th (2014/3/31)                      */
/* edited: june 2014: dejwoot.kha@mail.kmutt.ac.th                          */
/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include "NUC1xx.h"
#include "LCD_Driver.h"
#include "DrvSYS.h"
#include "Driver\DrvGPIO.h"
		// The maximal delay time is 335000 us.

#include "Driver\DrvI2C.h"
#include "Seven_Segment.h"

#define DELAY300ms	300000
#define scanDelay 4000

#define SEG_N0   0x82
#define SEG_N1   0xEE
#define SEG_N2   0x07
#define SEG_N3   0x46
#define SEG_N4   0x6A
#define SEG_N5   0x52
#define SEG_N6   0x12
#define SEG_N7   0xE6
#define SEG_N8   0x02
#define SEG_N9   0x62
#define SEG_Na   0x22
#define SEG_Nb   0x1A
#define SEG_Nc   0x93
#define SEG_Nd   0x0E
#define SEG_Ne   0x13
#define SEG_Nf   0x33

static uint16_t Timer0Counter = 0;

int8_t hex;
int8_t dec;

unsigned char SEG_BUF_HEX[16]={SEG_N0, SEG_N1, SEG_N2, SEG_N3, SEG_N4, SEG_N5, SEG_N6, SEG_N7, SEG_N8, SEG_N9, SEG_Na, SEG_Nb, SEG_Nc, SEG_Nd, SEG_Ne, SEG_Nf};

void show_seven_segment_Hex(unsigned char no, unsigned char number)
{
    unsigned char temp,i;
	temp=SEG_BUF_HEX[number];

	for(i=0;i<8;i++)
	    {
		if((temp&0x01)==0x01)
		   DrvGPIO_SetBit(E_GPE,i);
		   else
		   DrvGPIO_ClrBit(E_GPE,i);
		   temp=temp>>1;
		}
		DrvGPIO_SetBit(E_GPC,4+no);

}

void seg_display(int16_t value) {
	int8_t digit;


	digit = value / 10;
	close_seven_segment();
	show_seven_segment(1,digit);
	DrvSYS_Delay(scanDelay);


	digit = value % 10;
	close_seven_segment();
	show_seven_segment(0,digit);
	DrvSYS_Delay(scanDelay);
	}

void seg_display_Hex(int16_t value)
{
  int8_t digit;
// 		digit = value / 256;
// 		close_seven_segment();
// 		show_seven_segment_Hex(3,digit);
// 		DrvSYS_Delay(scanDelay);

// 		value = value - digit * 256;
		digit = value / 16;
		close_seven_segment();
		show_seven_segment_Hex(3,digit);
		DrvSYS_Delay(scanDelay);

		value = value - digit * 16;
		digit = value;
		close_seven_segment();
		show_seven_segment_Hex(2,digit);
		DrvSYS_Delay(scanDelay);

// 		value = value - digit * 10;
// 		digit = value;
// 		close_seven_segment();
// 		show_seven_segment(2,digit);
// 		DrvSYS_Delay(scanDelay);
}


static uint16_t Timer3Counter=0;

//------------------------------------------------OneWireReadByteTemperature
int8_t OneWireReadByteTemperature(void) {
	int8_t i;
	int8_t dataByte = 0xCC; // skip ROM

	GPIOE->DOUT &= 0xFEFF;	// Master send Reset
	DrvSYS_Delay(500);
	GPIOE->DOUT |= 0x0100;
	DrvSYS_Delay(200);			// wait for presence pulse

	for (i=0;i<8;i++) {			// skip ROM
		if ((dataByte&0x01 == 0x01)) {
			GPIOE->DOUT &= 0xFEFF;	// send '1'
			DrvSYS_Delay(3);				// low > 1 microsec.
			GPIOE->DOUT |= 0x0100;
			DrvSYS_Delay(60);
			} else {
			GPIOE->DOUT &= 0xFEFF;	// send '0'
			DrvSYS_Delay(60);				// low > 60 microsec.
			GPIOE->DOUT |= 0x0100;
			DrvSYS_Delay(2);
			}
		dataByte >>= 1;
		}

	dataByte = 0xBE;				// ReadScratchpad
	for (i=0;i<8;i++) {
		if ((dataByte&0x01 == 0x01)) {
			GPIOE->DOUT &= 0xFEFF;	// send '1'
			DrvSYS_Delay(3);				// low > 1 microsec.
			GPIOE->DOUT |= 0x0100;
			DrvSYS_Delay(60);
			} else {
			GPIOE->DOUT &= 0xFEFF;	// send '0'
			DrvSYS_Delay(60);				// low > 60 microsec.
			GPIOE->DOUT |= 0x0100;
			DrvSYS_Delay(2);
			}
		dataByte >>= 1;
		}

	// read 8 bits (byte0 scratchpad)
	DrvSYS_Delay(100);
	for (i=0;i<8;i++) {
		GPIOE->DOUT &= 0xFEFF;	//
		DrvSYS_Delay(2);				// low > 1 microsec.
		GPIOE->DOUT |= 0x0100;
		// Read
		DrvSYS_Delay(12);
		if ((GPIOE->PIN &= 0x0100) == 0x0100) {
			dataByte >>= 1;
			dataByte |= 0x80;
		} else {
			dataByte >>= 1;
			dataByte &= 0x7F;
		}
		DrvSYS_Delay(60);
		}
	dataByte >>= 1;
	return dataByte;
	}
//---------------------------------------------------OneWireTxSkipROMConvert
void OneWireTxSkipROMConvert(void) {
	int8_t i;
	uint8_t dataByte = 0xCC; // skip ROM

	GPIOE->DOUT &= 0xFEFF;	// Master send Reset
	DrvSYS_Delay(500);
	GPIOE->DOUT |= 0x0100;
	DrvSYS_Delay(200);

	for (i=0;i<8;i++) {		// skip ROM
		if ((dataByte&0x01 == 0x01)) {
			GPIOE->DOUT &= 0xFEFF;	// send '1'
			DrvSYS_Delay(3);				// low > 1 microsec.
			GPIOE->DOUT |= 0x0100;
			DrvSYS_Delay(60);
			} else {
			GPIOE->DOUT &= 0xFEFF;	// send '0'
			DrvSYS_Delay(60);				// low > 60 microsec.
			GPIOE->DOUT |= 0x0100;
			DrvSYS_Delay(2);
			}
		dataByte >>= 1;
	}

	dataByte = 0x44;	// convert Temperature
	for (i=0;i<8;i++) {
		if ((dataByte&0x01 == 0x01)) {
			GPIOE->DOUT &= 0xFEFF;	// send '1'
			DrvSYS_Delay(3);				// low > 1 microsec.
			GPIOE->DOUT |= 0x0100;
			DrvSYS_Delay(60);
			} else {
			GPIOE->DOUT &= 0xFEFF;	// send '0'
			DrvSYS_Delay(60);				// low > 60 microsec.
			GPIOE->DOUT |= 0x0100;
			DrvSYS_Delay(2);
			}
		dataByte >>= 1;
	}
}
//--------------------------------------------------------------------Timer3
void InitTIMER3(void)
{
	/* Step 1. Enable and Select Timer clock source */
	SYSCLK->CLKSEL1.TMR3_S = 0;	// Select 12Mhz for Timer0 clock source
	// 0 = 12 MHz, 1 = 32 kHz, 2 = HCLK, 7 = 22.1184 MHz
  SYSCLK->APBCLK.TMR3_EN = 1;	// Enable Timer0 clock source

	/* Step 2. Select Operation mode */
	TIMER3->TCSR.MODE = 1;				// 1 -> Select periodic mode
	// 0 = One shot, 1 = Periodic, 2 = Toggle, 3 = continuous counting mode

	/* Step 3. Select Time out period
	= (Period of timer clock input) * (8-bit Prescale + 1) * (24-bit TCMP)*/
	TIMER3->TCSR.PRESCALE = 11;	// Set Prescale [0~255]
	TIMER3->TCMPR = 1000000;		// Set TCMPR [0~16777215]
	// (1/12000000)*(11+1)*(1000000)= 1 sec

	/* Step 4. Enable interrupt */
	TIMER3->TCSR.IE = 1;
	TIMER3->TISR.TIF = 1;				// Write 1 to clear the interrupt flag
	NVIC_EnableIRQ(TMR3_IRQn);	// Enable Timer0 Interrupt

	/* Step 5. Enable Timer module */
	TIMER3->TCSR.CRST = 1;			// Reset up counter
	TIMER3->TCSR.CEN = 1;				// Enable Timer0
}
//----------------------------------------------------------------Timer3_IRQ
void TMR3_IRQHandler(void) 		// Timer0 interrupt subroutine
{
	int8_t ds1820Temp;
	//uint16_t Timer3Counter = 0;
	char lcd2_buffer[18] = "Timer3:";
	char lcd3_buffer[18] = "T =    C";

	sprintf(lcd2_buffer+7," %d",Timer3Counter);
	print_lcd(2, lcd2_buffer);
	Timer3Counter++;

	// to initiate a temperature measurement and A-to-D conversion
	OneWireTxSkipROMConvert();
	DrvSYS_Delay(100);
	ds1820Temp = OneWireReadByteTemperature();
	hex = ds1820Temp;
	sprintf(lcd3_buffer+4,"%d, %x C",ds1820Temp,ds1820Temp);
	print_lcd(3, lcd3_buffer);

 	TIMER3->TISR.TIF = 1;				// Write 1 to clear the interrupt flag
}

//----------------------------------------------------------------------MAIN
int32_t main (void) {

	UNLOCKREG();
	DrvSYS_Open(48000000);
	LOCKREG();

	Initial_pannel();  //call initial pannel function
	clr_all_pannal();
	print_lcd(0, "DS1820 Onewire");

	DrvGPIO_Open(E_GPE, 8, E_IO_QUASI);
	InitTIMER3();

	while (1) {
// 		__NOP();
		seg_display_Hex(hex);
			seg_display(hex);
		}
	}
