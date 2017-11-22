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

/*  读FLASH
ProgramCounter 		= *(__IO uint32_t*)(ProgramRUNcounter_Mode_FLASH_DATA_ADDRESS);
*/

/******************************************
写内部FLASH
*******************************************/
void WriteFlash(uint32_t addr,uint32_t data)
//void Write(FLASH_CHANNEL Channel,uint32_t data)
{	
		
	FLASH_Unlock(); //解锁FLASH编程擦除控制器
	FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);//清除标志位
	FLASH_ErasePage(addr); //擦除指定地址页
	FLASH_ProgramHalfWord(addr ,data); //从指定页的0地址开始写
	
	FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);//清除标志位
	FLASH_Lock(); //锁定FLASH编程擦除控制器
}




