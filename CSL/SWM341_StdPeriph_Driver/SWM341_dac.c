/****************************************************************************************************************************************** 
* �ļ�����: SWM341_dac.c
* ����˵��:	SWM341��Ƭ����DACģ��������
* ����֧��:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
* ע������: 
* �汾����:	V1.1.0		2017��10��25��
* ������¼: 
*
*
*******************************************************************************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION 
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE 
* FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT 
* OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
* -ECTION WITH THEIR PRODUCTS.
*
* COPYRIGHT 2012 Synwit Technology
*******************************************************************************************************************************************/
#include "SWM341.h"
#include "SWM341_dac.h"


/****************************************************************************************************************************************** 
* ��������:	DAC_Init()
* ����˵��:	DAC ��ʼ��
* ��    ��: DAC_TypeDef * DACx	ָ��Ҫ�����õ�DAC�ӿڣ���Чֵ����DAC
*			uint32_t format 	���ݸ�ʽ����ȡֵDAC_FORMAT_LSB12B��DAC_FORMAT_MSB12B��DAC_FORMAT_8B
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void DAC_Init(DAC_TypeDef * DACx, uint32_t format)
{
	switch((uint32_t)DACx)
	{
	case ((uint32_t)DAC):
		SYS->CLKEN1 |= (0x01 << SYS_CLKEN1_DAC_Pos);
		break;
	}
	
	DACx->CR = (format << DAC_CR_DHRFMT_Pos);
}

/****************************************************************************************************************************************** 
* ��������:	DAC_Open()
* ����˵��:	DAC ����
* ��    ��: DAC_TypeDef * DACx	ָ��Ҫ�����õ�DAC�ӿڣ���Чֵ����DAC
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void DAC_Open(DAC_TypeDef * DACx)
{
	SYS->DACCR |= (1 << SYS_DACCR_EN_Pos);
}

/****************************************************************************************************************************************** 
* ��������:	DAC_Init()
* ����˵��:	DAC �ر�
* ��    ��: DAC_TypeDef * DACx	ָ��Ҫ�����õ�DAC�ӿڣ���Чֵ����DAC
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
void DAC_Close(DAC_TypeDef * DACx)
{
	SYS->DACCR &= ~(1 << SYS_DACCR_EN_Pos);
}