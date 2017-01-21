//  UART0 Tx(B1) -> UART1 Rx(B4)

// Nu_LB-002: L08_02_UART0_8bytes.c
/*
	connect GPB0 to GPB1

 GPB0 - Rx
 GPB1 - Tx
*/

//
// Smpl_UART0 : while loop for UART0-TX keep transmitting 8 bytes string
//            : IRQ routine for UART0-RX keep receiving 8 bytes string & print to LCD
//						: (TX & RX at the same time)

/*--------------------------------------------------------------------------*/
/*                                                                          */
/* Copyright (c) Nuvoton Technology Corp. All rights reserved.              */
/* edited: june 2014: dejwoot.kha@mail.kmutt.ac.th                          */
/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include "Driver\DrvUART.h"
#include "Driver\DrvGPIO.h"
#include "Driver\DrvSYS.h"
#include "NUC1xx.h"
#include "NUC1xx-LB_002\LCD_Driver.h"

volatile uint8_t comRbuf[16];
volatile uint16_t comRbytes = 0;

#define DELAY300ms	300000 // The maximal delay time is 335000 us.
uint16_t iin2 =0;

char TEXT1[16] = "TX: sending...  ";
char TEXT2[16] = "RX:             ";

//-------------------------------------------------------------UART_Callback
void UART_INT_HANDLE(void) {
	uint8_t bInChar[1] = {0xFF};

	while (UART0->ISR.RDA_IF == 1) {
		DrvUART_Read(UART_PORT0, bInChar, 1);
		if (comRbytes < 8) {	// check if Buffer is full
			comRbuf[comRbytes] = bInChar[0];
			comRbytes++;
			}
		else if (comRbytes == 8) {
			comRbuf[comRbytes] = bInChar[0];
			comRbytes = 0;
			sprintf(TEXT2+4,"%s",comRbuf);
			print_lcd(2,TEXT2);
			}
		}

	while (UART1->ISR.RDA_IF == 1) {
		DrvUART_Read(UART_PORT1, bInChar, 1);
		if (comRbytes < 8) {	// check if Buffer is full
			comRbuf[comRbytes] = bInChar[0];
			comRbytes++;
			}
		else if (comRbytes == 8) {
			comRbuf[comRbytes] = bInChar[0];
			comRbytes = 0;
			sprintf(TEXT2+4,"%s",comRbuf);
			print_lcd(2,TEXT2);
			}
		}

	}

	void EINT1Callback(void) {
	DrvGPIO_DisableEINT1();
	clr_all_pannal();
	iin2++;
	DrvSYS_Delay(DELAY300ms);		// delay
	DrvGPIO_EnableEINT1(E_IO_BOTH_EDGE, E_MODE_EDGE, EINT1Callback);
	}

	void Init_LED() {
	// initialize GPIO pins
	DrvGPIO_Open(E_GPC, 12, E_IO_OUTPUT); // GPC12 pin set to output mode
	DrvGPIO_Open(E_GPC, 13, E_IO_OUTPUT); // GPC13 pin set to output mode
	DrvGPIO_Open(E_GPC, 14, E_IO_OUTPUT); // GPC14 pin set to output mode
	DrvGPIO_Open(E_GPC, 15, E_IO_OUTPUT); // GPC15 pin set to output mode
	// set GPIO pins to output Low
	DrvGPIO_SetBit(E_GPC, 12); // GPC12 pin output Hi to turn off LED
	DrvGPIO_SetBit(E_GPC, 13); // GPC13 pin output Hi to turn off LED
	DrvGPIO_SetBit(E_GPC, 14); // GPC14 pin output Hi to turn off LED
	DrvGPIO_SetBit(E_GPC, 15); // GPC15 pin output Hi to turn off LED
	}

//----------------------------------------------------------------------MAIN
int32_t main() {
	uint8_t  i = 0;
	uint8_t  dataout[9] = "NuMicro0";

	STR_UART_T sParam;
	STR_UART_T sParam1;
	STR_UART_T sParam2;

	UNLOCKREG();
  DrvSYS_Open(48000000);
	LOCKREG();
	Initial_pannel();  //call initial pannel function
	clr_all_pannal();
	print_lcd(0, "Smpl_UART0&1  ");

	Init_LED();

	/* Configure general GPIO interrupt */
	DrvGPIO_Open(E_GPB, 15, E_IO_INPUT);

	/* Configure external interrupt */
	DrvGPIO_EnableEINT1(E_IO_BOTH_EDGE, E_MODE_EDGE, EINT1Callback);

	/* Set UART0 Pin */
	DrvGPIO_InitFunction(E_FUNC_UART0);

	/* UART Setting */
	sParam.u32BaudRate = 9600;
	sParam.u8cDataBits = DRVUART_DATABITS_8;
	sParam.u8cStopBits = DRVUART_STOPBITS_1;
	sParam.u8cParity = DRVUART_PARITY_NONE;
	sParam.u8cRxTriggerLevel = DRVUART_FIFO_1BYTES;

	/* Set UART Configuration */
 	if (DrvUART_Open(UART_PORT0,&sParam) != E_SUCCESS);

	DrvUART_EnableInt(UART_PORT0, DRVUART_RDAINT, UART_INT_HANDLE);

	/* Set UART1 Pin */
	DrvGPIO_InitFunction(E_FUNC_UART1);

	/* UART Setting */
	sParam1.u32BaudRate = 9600;
	sParam1.u8cDataBits = DRVUART_DATABITS_8;
	sParam1.u8cStopBits = DRVUART_STOPBITS_1;
	sParam1.u8cParity = DRVUART_PARITY_NONE;
	sParam1.u8cRxTriggerLevel = DRVUART_FIFO_1BYTES;

	/* Set UART Configuration */
 	if (DrvUART_Open(UART_PORT1,&sParam1) != E_SUCCESS);

	DrvUART_EnableInt(UART_PORT1, DRVUART_RDAINT, UART_INT_HANDLE);

	/* Set UART2 Pin */
	DrvGPIO_InitFunction(E_FUNC_UART2);

	/* UART Setting */
	sParam2.u32BaudRate = 9600;
	sParam2.u8cDataBits = DRVUART_DATABITS_8;
	sParam2.u8cStopBits = DRVUART_STOPBITS_1;
	sParam2.u8cParity = DRVUART_PARITY_NONE;
	sParam2.u8cRxTriggerLevel = DRVUART_FIFO_1BYTES;

	/* Set UART Configuration */
 	if (DrvUART_Open(UART_PORT2,&sParam2) != E_SUCCESS);

	DrvUART_EnableInt(UART_PORT2, DRVUART_RDAINT, UART_INT_HANDLE);

	while(1) {
 	  dataout[7] = 0x30 + i;
		DrvUART_Write(UART_PORT0, dataout, 8);
		DrvUART_Write(UART_PORT2, dataout, 8);
		i++;
		if (i >= 10) i = 0;
		sprintf(TEXT1+14, "%d", i);
		print_lcd(1, TEXT1);

		DrvSYS_Delay(300000);
		DrvSYS_Delay(300000);
		DrvSYS_Delay(300000);

		if(iin2%8==0){
			DrvGPIO_SetBit(E_GPC, 12);
			DrvGPIO_SetBit(E_GPC, 13);
			DrvGPIO_SetBit(E_GPC, 14);
			DrvGPIO_SetBit(E_GPC, 15);
		}

		if(iin2%8==1){
			DrvGPIO_SetBit(E_GPC, 12);
			DrvGPIO_SetBit(E_GPC, 13);
			DrvGPIO_SetBit(E_GPC, 14);
			DrvGPIO_ClrBit(E_GPC, 15);
		}

		if(iin2%8==2){
			DrvGPIO_SetBit(E_GPC, 12);
			DrvGPIO_SetBit(E_GPC, 13);
			DrvGPIO_ClrBit(E_GPC, 14);
			DrvGPIO_ClrBit(E_GPC, 15);
		}

		if(iin2%8==3){
			DrvGPIO_SetBit(E_GPC, 12);
			DrvGPIO_ClrBit(E_GPC, 13);
			DrvGPIO_ClrBit(E_GPC, 14);
			DrvGPIO_ClrBit(E_GPC, 15);
		}
		if(iin2%8==4){
			DrvGPIO_ClrBit(E_GPC, 12);
			DrvGPIO_ClrBit(E_GPC, 13);
			DrvGPIO_ClrBit(E_GPC, 14);
			DrvGPIO_ClrBit(E_GPC, 15);
		}
		if(iin2%8==5){
			DrvGPIO_SetBit(E_GPC, 12);
			DrvGPIO_ClrBit(E_GPC, 13);
			DrvGPIO_ClrBit(E_GPC, 14);
			DrvGPIO_ClrBit(E_GPC, 15);
		}
		if(iin2%8==6){
			DrvGPIO_SetBit(E_GPC, 12);
			DrvGPIO_SetBit(E_GPC, 13);
			DrvGPIO_ClrBit(E_GPC, 14);
			DrvGPIO_ClrBit(E_GPC, 15);
		}
		if(iin2%8==8){
			DrvGPIO_SetBit(E_GPC, 12);
			DrvGPIO_SetBit(E_GPC, 13);
			DrvGPIO_SetBit(E_GPC, 14);
			DrvGPIO_SetBit(E_GPC, 15);
			iin2=0;
		}

		if(iin2%8==7){
			DrvGPIO_SetBit(E_GPC, 12);
			DrvGPIO_SetBit(E_GPC, 13);
			DrvGPIO_SetBit(E_GPC, 14);
			DrvGPIO_ClrBit(E_GPC, 15);
		}

	}
	//DrvUART_Close(UART_PORT0);
}
