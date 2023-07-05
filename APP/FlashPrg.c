#include "FlashOS.h"
#include "SWM341.h"     


/****************************************************************************************************************************************** 
* 函数名称: Init()
* 功能说明:	Initialize Flash Programming Functions
* 输    入: unsigned long adr	Device Base Address
*			unsigned long clk	Clock Frequency (Hz)
*			unsigned long fnc	Function Code (1 - Erase, 2 - Program, 3 - Verify)
* 输    出: int					0 - OK,  1 - Failed
* 注意事项: 无
******************************************************************************************************************************************/
int Init(unsigned long adr, unsigned long clk, unsigned long fnc)
{
	SFC_InitStructure SFC_initStruct;
	
	if(fnc == 1)
	{
		__disable_irq();
		
		SYS->PRSTEN = 0x55;
		SYS->PRSTR0 = 0xFFFFFFFF;
		SYS->PRSTR1 = 0xFFFFFFFF;
		__DSB();__ISB();__NOP();__NOP();
		SYS->PRSTR0 = 0;
		SYS->PRSTR1 = 0;
		SYS->PRSTEN = 0;
		
		SystemInit();
		
		/* SFC使用专用的FSPI（Flash SPI）接口连接SPI Flash */
		PORT_Init(PORTD, PIN5, PORTD_PIN5_FSPI_SCLK,  0);
		PORT_Init(PORTD, PIN6, PORTD_PIN6_FSPI_SSEL,  0);
		PORT_Init(PORTD, PIN8, PORTD_PIN8_FSPI_MOSI,  1);
		PORT_Init(PORTD, PIN7, PORTD_PIN7_FSPI_MISO,  1);
		PORT_Init(PORTD, PIN3, PORTD_PIN3_FSPI_DATA2, 1);
		PORT_Init(PORTD, PIN4, PORTD_PIN4_FSPI_DATA3, 1);
		
		SFC_initStruct.ClkDiv = SFC_CLKDIV_2;
		SFC_initStruct.Cmd_Read = 0xEB;
		SFC_initStruct.Width_Read = SFC_RDWIDTH_4;
		SFC_initStruct.Cmd_PageProgram = 0x32;
		SFC_initStruct.Width_PageProgram = SFC_PPWIDTH_4;
		SFC_Init(&SFC_initStruct);
		
		SFC_QuadSwitch(1);
	}
	else if(fnc == 2)
	{
		// SFC 写入速度较慢，改用 GPIO 模拟 SPI 执行写入操作
		GPIO_SetBit(GPIOD, PIN6);
		GPIO_SetBit(GPIOD, PIN5);
		GPIO_INIT(GPIOD, PIN5, GPIO_OUTPUT);
		GPIO_INIT(GPIOD, PIN6, GPIO_OUTPUT);
		GPIO_INIT(GPIOD, PIN8, GPIO_OUTPUT);
		GPIO_INIT(GPIOD, PIN7, GPIO_INPUT);
	}
	else if(fnc == 3)
	{
		PORT_Init(PORTD, PIN5, PORTD_PIN5_FSPI_SCLK,  0);
		PORT_Init(PORTD, PIN6, PORTD_PIN6_FSPI_SSEL,  0);
		PORT_Init(PORTD, PIN8, PORTD_PIN8_FSPI_MOSI,  1);
		PORT_Init(PORTD, PIN7, PORTD_PIN7_FSPI_MISO,  1);
		PORT_Init(PORTD, PIN3, PORTD_PIN3_FSPI_DATA2, 1);
		PORT_Init(PORTD, PIN4, PORTD_PIN4_FSPI_DATA3, 1);
	}

	return 0;
}


/****************************************************************************************************************************************** 
* 函数名称: UnInit()
* 功能说明:	De-Initialize Flash Programming Functions
* 输    入: unsigned long fnc	Function Code (1 - Erase, 2 - Program, 3 - Verify)
* 输    出: int					0 - OK,  1 - Failed
* 注意事项: 无
******************************************************************************************************************************************/
int UnInit(unsigned long fnc)
{
  	return 0;
}


/****************************************************************************************************************************************** 
* 函数名称: EraseSector()
* 功能说明:	Erase Sector in Flash Memory
* 输    入: unsigned long adr	Sector Address
* 输    出: int					0 - OK,  1 - Failed
* 注意事项: 无
******************************************************************************************************************************************/
int EraseSector(unsigned long adr)
{
	if((adr < 0x70000000) || (adr > 0x71000000))
		return 1;
	
	adr = adr - 0x70000000;
	
 	SFC_EraseEx(adr, SFC_CMD_ERASE_BLOCK64KB, 0);
	while(SFC_FlashBusy()) __NOP();
 	
 	return 0;
}


/****************************************************************************************************************************************** 
* 函数名称: ProgramPage()
* 功能说明:	Program Page in Flash Memory
* 输    入: unsigned long adr	Page Start Address
*			unsigned long sz	Page Size
*			unsigned char *buf	Page Data
* 输    出: int					0 - OK,  1 - Failed
* 注意事项: 无
******************************************************************************************************************************************/
int ProgramPage(unsigned long adr, unsigned long sz, unsigned char *buf)
{
	if((adr < 0x70000000) || (adr > 0x71000000))
		return 1;
	
	adr = adr - 0x70000000;
	
	for(int i = 0; i < sz; i += 256)
	{
		SFC_GPIOWrite(adr + i, (uint32_t *)&buf[i], ((sz - i >= 256) ? 256 : (sz - i)) / 4);
	}
	
  	return 0;
}


/****************************************************************************************************************************************** 
* 函数名称: Verify()
* 功能说明:	Verify Flash Contents
* 输    入: unsigned long adr	Block Start Address
*			unsigned long sz	Block Size (in bytes)
*			unsigned char *buf	Block Data
* 输    出: int					(adr+sz) - OK,  other - Failed Address
* 注意事项: 无
******************************************************************************************************************************************/
unsigned long Verify(unsigned long adr, unsigned long sz, unsigned char *buf)
{	
	if((adr < 0x70000000) || (adr > 0x71000000))
		return adr;
		
	for(int i = 0; i < sz; i += 4)
		if(*((volatile unsigned int *)(adr+i)) != ((buf[i+3] << 24) | (buf[i+2] << 16) | (buf[i+1] << 8) | buf[i]))
			return (adr+i);
	
	return (adr+sz);
}
