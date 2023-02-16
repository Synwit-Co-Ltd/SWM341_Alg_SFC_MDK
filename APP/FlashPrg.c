#include "FlashOS.h"
#include "SWM341.h"     


/****************************************************************************************************************************************** 
* ��������: Init()
* ����˵��:	Initialize Flash Programming Functions
* ��    ��: unsigned long adr	Device Base Address
*			unsigned long clk	Clock Frequency (Hz)
*			unsigned long fnc	Function Code (1 - Erase, 2 - Program, 3 - Verify)
* ��    ��: int					0 - OK,  1 - Failed
* ע������: ��
******************************************************************************************************************************************/
int Init(unsigned long adr, unsigned long clk, unsigned long fnc)
{
	SFC_InitStructure SFC_initStruct;
	
	if(fnc == 1)
	{
		__disable_irq();
		NVIC->ICER[0] = 0xFFFFFFFF;
		NVIC->ICER[1] = 0xFFFFFFFF;
		NVIC->ICER[2] = 0xFFFFFFFF;
		
		SYS->PRSTEN = 0x55;
		SYS->PRSTR0 = 0xFFFFFFFF;
		SYS->PRSTR1 = 0xFFFFFFFF;
		__DSB();__ISB();__NOP();__NOP();
		SYS->PRSTR0 = 0;
		SYS->PRSTR1 = 0;
		SYS->PRSTEN = 0;
		
		SYS->CLKEN0 |= (1 << SYS_CLKEN0_ANAC_Pos);
		SYS->HRCCR |= (1 << SYS_HRCCR_DBL_Pos);
		CyclesPerUs = 40;
		
		/* SFCʹ��ר�õ�FSPI��Flash SPI���ӿ�����SPI Flash */
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

	return 0;
}


/****************************************************************************************************************************************** 
* ��������: UnInit()
* ����˵��:	De-Initialize Flash Programming Functions
* ��    ��: unsigned long fnc	Function Code (1 - Erase, 2 - Program, 3 - Verify)
* ��    ��: int					0 - OK,  1 - Failed
* ע������: ��
******************************************************************************************************************************************/
int UnInit(unsigned long fnc)
{
  	return 0;
}


/****************************************************************************************************************************************** 
* ��������: EraseSector()
* ����˵��:	Erase Sector in Flash Memory
* ��    ��: unsigned long adr	Sector Address
* ��    ��: int					0 - OK,  1 - Failed
* ע������: ��
******************************************************************************************************************************************/
int EraseSector(unsigned long adr)
{
	if((adr < 0x70000000) || (adr > 0x71000000))
		return 1;
	
	adr = adr - 0x70000000;
	
 	SFC_Erase(adr);
 	
 	return 0;
}


/****************************************************************************************************************************************** 
* ��������: ProgramPage()
* ����˵��:	Program Page in Flash Memory
* ��    ��: unsigned long adr	Page Start Address
*			unsigned long sz	Page Size
*			unsigned char *buf	Page Data
* ��    ��: int					0 - OK,  1 - Failed
* ע������: ��
******************************************************************************************************************************************/
int ProgramPage(unsigned long adr, unsigned long sz, unsigned char *buf)
{
	if((adr < 0x70000000) || (adr > 0x71000000))
		return 1;
	
	adr = adr - 0x70000000;
	
	SFC_Write(adr, (uint32_t *)buf, (sz + 3)/4);
	
  	return 0;
}


/****************************************************************************************************************************************** 
* ��������: BlankCheck()
* ����˵��:	Blank Check Block in Flash Memory
* ��    ��: unsigned long adr	Block Start Address
*			unsigned long sz	Block Size (in bytes)
*			unsigned char pat	Block Pattern
* ��    ��: int					0 - OK,  1 - Failed
* ע������: ��
******************************************************************************************************************************************/
int BlankCheck(unsigned long adr, unsigned long sz, unsigned char pat)
{		
	if((adr < 0x70000000) || (adr > 0x71000000))
		return 1;
		
	uint32_t lpat = (pat << 24) | (pat << 16) | (pat << 8) | pat;
	
	for(int i = 0; i < sz; i += 4)
		if(*((volatile unsigned int *)(adr+i)) != lpat)
			return 1;
	
	return 0;
}


/****************************************************************************************************************************************** 
* ��������: Verify()
* ����˵��:	Verify Flash Contents
* ��    ��: unsigned long adr	Block Start Address
*			unsigned long sz	Block Size (in bytes)
*			unsigned char *buf	Block Data
* ��    ��: int					(adr+sz) - OK,  other - Failed Address
* ע������: ��
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