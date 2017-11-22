#include "flash.h"

FLASH_CHANNEL OUT1_ModeFlashChannel;
FLASH_CHANNEL OUT1_ValueFlashChannel;
FLASH_CHANNEL CSVFlashChannel;
FLASH_CHANNEL Threshold_ModeFlashChannel;
FLASH_CHANNEL DACOUT_ModeFlashChannel;
FLASH_CHANNEL KEY_ModeFlashChannel;
FLASH_CHANNEL RegisterB_ModeFlashChannel;
FLASH_CHANNEL DEL_ModeFlashChannel;
FLASH_CHANNEL HI_ModeFlashChannel;
FLASH_CHANNEL LO_ModeFlashChannel;
FLASH_CHANNEL DETECT_ModeFlashChannel;

/*  ��FLASH
ProgramCounter 		= *(__IO uint32_t*)(ProgramRUNcounter_Mode_FLASH_DATA_ADDRESS);
*/

/******************************************
д�ڲ�FLASH
*******************************************/
void WriteFlash(uint32_t addr,uint32_t data)
//void Write(FLASH_CHANNEL Channel,uint32_t data)
{	
		
	FLASH_Unlock(); //����FLASH��̲���������
	FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);//�����־λ
	FLASH_ErasePage(addr); //����ָ����ַҳ
	FLASH_ProgramHalfWord(addr ,data); //��ָ��ҳ��0��ַ��ʼд
	
	FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);//�����־λ
	FLASH_Lock(); //����FLASH��̲���������
}




