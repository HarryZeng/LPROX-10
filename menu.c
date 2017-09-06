///**
//  ********************************  STM32F0x1  *********************************
//  * @文件名     ： menu.c
//  * @作者       ： HarryZeng
//  * @库版本     ： V1.5.0
//  * @文件版本   ： V1.0.0
//  * @日期       ： 2017年05月15日
//  * @摘要       ： 数据处理
//  ******************************************************************************/
///*----------------------------------------------------------------------------
//  更新日志:
//  2017-05-15 V1.0.0:初始版本
//  ----------------------------------------------------------------------------*/
///* 包含的头文件 --------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "differentialDC.h"
#include "menu.h"
#include "key.h"
#include "display.h"
#include "bsp_eeprom_24xx.h"

int32_t CSV = 1000;
OUT1_DELAY_MODE_STRUCT OUT1_Mode={TOFF,10};
int8_t DispalyNo=0;
void MenuOne_CounterSET(void);
void MenuTwo_OUT1_DelaySET(void);

void MenuTwo_OUT1_TOFF(void);
void MenuTwo_OUT1_OFFD(void);
void MenuTwo_OUT1_ON_D(void);	
void MenuTwo_OUT1_SHOT(void);

extern uint8_t  displayModeONE_FLAG;
void menu(void)
{
	static uint8_t lastCounter;
	/*Mode按键第一次按下,超过长按时间*/
	lastCounter = ModeButton.PressCounter;
	if((DownButton.Status==Press && ModeButton.Status ==Press )|| SetButton.Status == Press||UpButton.Status==Press)
	{
		
	}
	else
	{
		while(((ModeButton.Status==Press&&(ModeButton.PressTimer>=ModeButton.LongTime)) || (ModeButton.Effect==PressLong)))//&&DownButton.Status==Release)
		{
				ModeButton.PressCounter = 0;
				MenuOne_CounterSET();
				while(ModeButton.PressCounter==1)
				{
					MenuOne_CounterSET();
				}

				while(ModeButton.PressCounter==2)
				{
					/*DETECT*//*数码管显示*/
					SMG_DisplayModeDETECT(displayModeONE_FLAG);
					/*******************************/
					/*Up Button*/
					if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
					{
						lastCounter = UpButton.PressCounter;
						UpButton.PressCounter = 0;
						if(displayModeONE_FLAG==0)
							displayModeONE_FLAG = 1;
						else 
							displayModeONE_FLAG=0;
						WriteFlash(DETECT_FLASH_DATA_ADDRESS,displayModeONE_FLAG);
					}

					/*Down Button*/
					if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
					{
						DownButton.PressCounter = 0;
						if(displayModeONE_FLAG==0)
							displayModeONE_FLAG = 1;
						else 
							displayModeONE_FLAG=0;
						WriteFlash(DETECT_FLASH_DATA_ADDRESS,displayModeONE_FLAG);
					}
				}
					
				if(ModeButton.PressCounter==3 && ModeButton.Effect==PressShort)
				{
					if(OUT1_Mode.DelayMode == TOFF)
						DispalyNo = 0;
					else if(OUT1_Mode.DelayMode == OFFD)
						DispalyNo = 1;
					else if(OUT1_Mode.DelayMode == ON_D)
						DispalyNo = 2;
					else if(OUT1_Mode.DelayMode == SHOT)
						DispalyNo = 3;
				}
				while(ModeButton.Effect==PressShort && ModeButton.PressCounter==2)
				{
						MenuTwo_OUT1_DelaySET();
						/*再短按MODE，则退出菜单*/
						if(ModeButton.Effect==PressShort && ModeButton.PressCounter>=4) 
						{
							ModeButton.PressCounter = 0;
							ModeButton.Status = Release;
							
							WriteFlash(OUT1_Mode_FLASH_DATA_ADDRESS,OUT1_Mode.DelayMode);
							WriteFlash(OUT1_Value_FLASH_DATA_ADDRESS,OUT1_Mode.DelayValue);
							WriteFlash(CSV_FLASH_DATA_ADDRESS,CSV);
							
							break;
						}
				}
		}
	}
}

/*COUNTER设定菜单*/
void MenuOne_CounterSET(void)
{
	static uint8_t lastCounter;
	
	SMG_DisplayMenuOne(CSV);
	
	/*Up Button*/
	if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
	{
		lastCounter = UpButton.PressCounter;
		UpButton.PressCounter = 0;
		CSV = CSV+1;
	}
	else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
	{				/*还按着按键，并且时间超过长按时间*/
		UpButton.PressCounter = 0;
		if(UpButton.PressTimer<KEY_LEVEL_1)
		{
			if(UpButton.PressTimer%KEY_LEVEL_1_SET==0)
				CSV = CSV+1;
		}
		else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<KEY_LEVEL_2)
		{
			if(UpButton.PressTimer%KEY_LEVEL_2_SET==0)
				CSV = CSV+1;
		}
		else 
		{
			if(UpButton.PressTimer%KEY_LEVEL_3_SET==0)
				CSV = CSV+1;
		}
	}	
	else
	{
		UpButton.Effect = PressShort;
	}
	
	/*Down Button*/
	if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
	{
		DownButton.PressCounter = 0;
		CSV = CSV-1;
	}
	else 	if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
	{				/*还按着按键，并且时间超过长按时间*/
		DownButton.PressCounter = 0;
		if(DownButton.PressTimer<KEY_LEVEL_1)
		{
			if(DownButton.PressTimer%KEY_LEVEL_1_SET==0)
				CSV = CSV-1;
		}
		else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
		{
			if(DownButton.PressTimer%KEY_LEVEL_2_SET==0)
				CSV = CSV-1;
		}
		else 
		{
			if(DownButton.PressTimer%KEY_LEVEL_3_SET==0)
				CSV = CSV-1;
		}
	}	
	else
	{
		DownButton.Effect = PressShort;
	}
	
	if(CSV<=1)
			CSV =1;
	else if(CSV>=65535)
			CSV =65535;
}


/*OUT1 DELAY设定菜单*/

void MenuTwo_OUT1_DelaySET(void)
{
		static uint8_t lastCounter;
	
		/*TOFF Mode*/
		while(DispalyNo==0)
		{
			MenuTwo_OUT1_TOFF();
			if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
			{
				lastCounter = UpButton.PressCounter;
				UpButton.PressCounter = 0;
				DispalyNo++;
				if(DispalyNo>3)
					DispalyNo = 0;
			}
			if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
			{
				//lastCounter = DownButton.PressCounter;
				DownButton.PressCounter = 0;
				DispalyNo--;
				if(DispalyNo<0)
					DispalyNo = 3;
			}
			if(ModeButton.PressCounter>=4) break;
		}
		
		/*OFFD mode*/
		while(DispalyNo==1)
		{
				MenuTwo_OUT1_OFFD();
				if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
				{
					//lastCounter = UpButton.PressCounter;
					UpButton.PressCounter = 0;
					DispalyNo++;
					if(DispalyNo>3)
						DispalyNo = 0;
				}
				if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
				{
					//lastCounter = DownButton.PressCounter;
					DownButton.PressCounter = 0;
					DispalyNo--;
					if(DispalyNo<0)
						DispalyNo = 3;
				}
				if(ModeButton.PressCounter>4) break;
		}
		/*ON_D mode*/
		while(DispalyNo==2)
		{
				MenuTwo_OUT1_ON_D();
				if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
				{
					//lastCounter = UpButton.PressCounter;
					UpButton.PressCounter = 0;
					DispalyNo++;
					if(DispalyNo>3)
						DispalyNo = 0;
				}
				if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
				{
					//lastCounter = DownButton.PressCounter;
					DownButton.PressCounter = 0;
					DispalyNo--;
					if(DispalyNo<0)
						DispalyNo = 3;
				}
				if(ModeButton.PressCounter>4) break;
		}
		while(DispalyNo==3)
		{
				MenuTwo_OUT1_SHOT();
				if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
				{
					//lastCounter = UpButton.PressCounter;
					UpButton.PressCounter = 0;
					DispalyNo++;
					if(DispalyNo>3)
						DispalyNo = 0;
				}
				if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
				{
					//lastCounter = DownButton.PressCounter;
					DownButton.PressCounter = 0;
					DispalyNo--;
					if(DispalyNo<0)
						DispalyNo = 3;
				}
				if(ModeButton.PressCounter>4) break;
		}
		
//		while(DispalyNo==4)
//		{
//				MenuTwo_DEL();
//				if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
//				{
//					//lastCounter = UpButton.PressCounter;
//					UpButton.PressCounter = 0;
//					DispalyNo++;
//					if(DispalyNo>4)
//						DispalyNo = 0;
//				}
//				if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
//				{
//					//lastCounter = DownButton.PressCounter;
//					DownButton.PressCounter = 0;
//					DispalyNo--;
//					if(DispalyNo<0)
//						DispalyNo = 4;
//				}
//				if(ModeButton.PressCounter>4) break;
//		}

		
}


/*******************************
*
*OUT1_TOFF
*
*******************************/
void MenuTwo_OUT1_TOFF(void)
{
		if(EventFlag&Blink500msFlag) 
		{
			EventFlag = EventFlag &(~Blink500msFlag);  //清楚标志位
			SMG_DisplayMenuTwo_TOFF();
		}
		//if(ModeButton.Effect==PressShort && ModeButton.PressCounter==4)
			OUT1_Mode.DelayMode = TOFF;
}

/*******************************
*
*OUT1_OFFD
*
*******************************/
void MenuTwo_OUT1_OFFD(void)
{
		static uint8_t lastCounter;
	
		if(EventFlag&Blink500msFlag) 
		{
			EventFlag = EventFlag &(~Blink500msFlag);  //清楚标志位
			/*显示OFFD,闪烁*/
			SMG_DisplayMenuTwo_OFFD();
		}
		/*短按MODE后，进入OFFD的设置子菜单*/
		while(ModeButton.Effect==PressShort && ModeButton.PressCounter==4)
		{
			OUT1_Mode.DelayMode = OFFD;
			/*显示OFFD value*/
			if(UpButton.Status==Release&&DownButton.Status==Release)
			{
				if(EventFlag&Blink500msFlag) 
				{
					EventFlag = EventFlag &(~Blink500msFlag);  //清楚标志位
					/*显示OFFD,闪烁*/
					SMG_DisplayMenuTwo_OFFD_SET(OUT1_Mode.DelayValue,1);
				}
			}
			else
			{
				EventFlag = EventFlag &(~Blink500msFlag);
				SMG_DisplayMenuTwo_OFFD_SET(OUT1_Mode.DelayValue,0);
			}
			/*Up Button*/
			if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
			{
				lastCounter = UpButton.PressCounter;
				UpButton.PressCounter = 0;
				OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
			}
			else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
			{				/*还按着按键，并且时间超过长按时间*/
				UpButton.PressCounter = 0;
				if(UpButton.PressTimer<KEY_LEVEL_1)
				{
					if(UpButton.PressTimer%KEY_LEVEL_1_SET==0)
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
				}
				else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<KEY_LEVEL_2)
				{
					if(UpButton.PressTimer%KEY_LEVEL_2_SET==0)
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
				}
				else 
				{
					if(UpButton.PressTimer%KEY_LEVEL_3_SET==0)
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
				}
			}	
			else
			{
				UpButton.Effect = PressShort;
			}
				/*Down Button*/
			if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
			{
				DownButton.PressCounter = 0;
				OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
			}
			else if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
			{				/*还按着按键，并且时间超过长按时间*/
				DownButton.PressCounter = 0;
				if(DownButton.PressTimer<KEY_LEVEL_1)
				{
					if(DownButton.PressTimer%KEY_LEVEL_1_SET==0)
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
				}
				else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
				{
					if(DownButton.PressTimer%KEY_LEVEL_2_SET==0)
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
				}
				else 
				{
					if(DownButton.PressTimer%KEY_LEVEL_3_SET==0)
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
				}
			}	
			else
			{
				DownButton.Effect = PressShort;
			}
			if(OUT1_Mode.DelayValue>=9999)
				OUT1_Mode.DelayValue = 9999;
			else if(OUT1_Mode.DelayValue<=0)
					OUT1_Mode.DelayValue = 0;
		}
}

/*******************************
*
*OUT1_ON_D
*
*******************************/
void MenuTwo_OUT1_ON_D(void)
{
	static uint8_t lastCounter;
		if(EventFlag&Blink500msFlag) 
		{
			EventFlag = EventFlag &(~Blink500msFlag);  //清楚标志位
			
			SMG_DisplayMenuTwo_ON_Delay();
		}
		/*短按MODE后，进入ON_D的设置子菜单*/
		while(ModeButton.Effect==PressShort && ModeButton.PressCounter==4)
		{
			OUT1_Mode.DelayMode = ON_D;
			/*显示ON_D value*/
			if(UpButton.Status==Release&&DownButton.Status==Release)
			{
				if(EventFlag&Blink500msFlag) 
				{
					EventFlag = EventFlag &(~Blink500msFlag);  //清楚标志位
					/*显示ON_D,闪烁*/
					SMG_DisplayMenuTwo_ON_Delay_SET(OUT1_Mode.DelayValue,1);
				}
			}
			else
			{
				EventFlag = EventFlag &(~Blink100msFlag);
				SMG_DisplayMenuTwo_ON_Delay_SET(OUT1_Mode.DelayValue,0);
			}
			
			/*Up Button*/
			if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
			{
				lastCounter = UpButton.PressCounter;
				UpButton.PressCounter = 0;
				OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
			}
			else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
			{				/*还按着按键，并且时间超过长按时间*/
				UpButton.PressCounter = 0;
				if(UpButton.PressTimer<KEY_LEVEL_1)
				{
					if(UpButton.PressTimer%KEY_LEVEL_1_SET==0)
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
				}
				else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<KEY_LEVEL_2)
				{
					if(UpButton.PressTimer%KEY_LEVEL_2_SET==0)
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
				}
				else 
				{
					if(UpButton.PressTimer%KEY_LEVEL_3_SET==0)
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
				}
			}	
			else
			{
				UpButton.Effect = PressShort;
			}	
				/*Down Button*/
			if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
			{
				DownButton.PressCounter = 0;
				OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;

			}
			else if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
			{				/*还按着按键，并且时间超过长按时间*/
				DownButton.PressCounter = 0;
				if(DownButton.PressTimer<KEY_LEVEL_1)
				{
					if(DownButton.PressTimer%KEY_LEVEL_1_SET==0)
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
				}
				else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
				{
					if(DownButton.PressTimer%KEY_LEVEL_2_SET==0)
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
				}
				else 
				{
					if(DownButton.PressTimer%KEY_LEVEL_3_SET==0)
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
				}
			}	
			else
			{
				DownButton.Effect = PressShort;
			}
			
			if(OUT1_Mode.DelayValue>=9999)
				OUT1_Mode.DelayValue = 9999;
			else if(OUT1_Mode.DelayValue<=0)
					OUT1_Mode.DelayValue = 0;
		}
}


/*******************************
*
*OUT1_SHOT
*
*******************************/
void MenuTwo_OUT1_SHOT(void)
{
		static uint8_t lastCounter;
		if(EventFlag&Blink500msFlag) 
		{
			EventFlag = EventFlag &(~Blink500msFlag);  //清楚标志位
			
			SMG_DisplayMenuTwo_SHOT();
		}
		/*短按MODE后，进入SHOT的设置子菜单*/
		while(ModeButton.Effect==PressShort && ModeButton.PressCounter==4)
		{
			OUT1_Mode.DelayMode = SHOT;
			/*显示SHOT value*/			
			if(UpButton.Status==Release&&DownButton.Status==Release)
			{
				if(EventFlag&Blink500msFlag) 
				{
					EventFlag = EventFlag &(~Blink500msFlag);  //清楚标志位
					/*显示SHOT,闪烁*/
					SMG_DisplayMenuTwo_SHOT_SET(OUT1_Mode.DelayValue,1);
				}
			}
			else
			{
				EventFlag = EventFlag &(~Blink500msFlag);
				SMG_DisplayMenuTwo_SHOT_SET(OUT1_Mode.DelayValue,0);
			}
			
			/*Up Button*/
			if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
			{
				lastCounter = UpButton.PressCounter;
				UpButton.PressCounter = 0;
				OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
			}
			else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
			{				/*还按着按键，并且时间超过长按时间*/
				UpButton.PressCounter = 0;
				if(UpButton.PressTimer<KEY_LEVEL_1)
				{
					if(UpButton.PressTimer%KEY_LEVEL_1_SET==0)
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
				}
				else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<KEY_LEVEL_2)
				{
					if(UpButton.PressTimer%KEY_LEVEL_2_SET==0)
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
				}
				else 
				{
					if(UpButton.PressTimer%KEY_LEVEL_3_SET==0)
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue+1;
				}
			}	
			else
			{
				UpButton.Effect = PressShort;
			}	
				/*Down Button*/
			if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
			{
				DownButton.PressCounter = 0;
				OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
			}
			else if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
			{				/*还按着按键，并且时间超过长按时间*/
				DownButton.PressCounter = 0;
				if(DownButton.PressTimer<KEY_LEVEL_1)
				{
					if(DownButton.PressTimer%KEY_LEVEL_1_SET==0)
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
				}
				else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
				{
					if(DownButton.PressTimer%KEY_LEVEL_2_SET==0)
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
				}
				else 
				{
					if(DownButton.PressTimer%KEY_LEVEL_3_SET==0)
						OUT1_Mode.DelayValue = OUT1_Mode.DelayValue-1;
				}
			}	
			else
			{
				DownButton.Effect = PressShort;
			}
			
			if(OUT1_Mode.DelayValue>=9999)
				OUT1_Mode.DelayValue = 9999;
			else if(OUT1_Mode.DelayValue<=0)
					OUT1_Mode.DelayValue = 0;
		}
}



///*******************************
//*
//*OUT1_DEL 应差值设置
//*
//*******************************/
//void MenuTwo_DEL(void)
//{
//		static uint8_t lastCounter;
//		if(EventFlag&Blink500msFlag) 
//		{
//			EventFlag = EventFlag &(~Blink500msFlag);  //清楚标志位
//			
//			SMG_DisplayMenuTwo_DEL();
//		}
//		/*短按MODE后，进入SHOT的设置子菜单*/
//		while(ModeButton.Effect==PressShort && ModeButton.PressCounter==4)
//		{
//			/*显示SHOT value*/			
//			if(UpButton.Status==Release&&DownButton.Status==Release)
//			{
//				if(EventFlag&Blink500msFlag) 
//				{
//					EventFlag = EventFlag &(~Blink500msFlag);  //清楚标志位
//					/*显示SHOT,闪烁*/
//					SMG_DisplayMenuTwo_DEL_SET(DEL,1);
//				}
//			}
//			else
//			{
//				EventFlag = EventFlag &(~Blink500msFlag);
//				SMG_DisplayMenuTwo_DEL_SET(DEL,0);
//			}
//			
//			/*Up Button*/
//			if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
//			{
//				lastCounter = UpButton.PressCounter;
//				UpButton.PressCounter = 0;
//				DEL = DEL+1;
//			}
//			else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
//			{				/*还按着按键，并且时间超过长按时间*/
//				UpButton.PressCounter = 0;
//				if(UpButton.PressTimer<KEY_LEVEL_1)
//				{
//					if(UpButton.PressTimer%KEY_LEVEL_1_SET==0)
//						DEL = DEL+1;
//				}
//				else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<KEY_LEVEL_2)
//				{
//					if(UpButton.PressTimer%KEY_LEVEL_2_SET==0)
//						DEL = DEL+1;
//				}
//				else 
//				{
//					if(UpButton.PressTimer%KEY_LEVEL_3_SET==0)
//						DEL = DEL+1;
//				}
//			}	
//			else
//			{
//				UpButton.Effect = PressShort;
//			}	
//				/*Down Button*/
//			if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
//			{
//				DownButton.PressCounter = 0;
//				DEL = DEL-1;
//			}
//			else if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
//			{				/*还按着按键，并且时间超过长按时间*/
//				DownButton.PressCounter = 0;
//				if(DownButton.PressTimer<KEY_LEVEL_1)
//				{
//					if(DownButton.PressTimer%KEY_LEVEL_1_SET==0)
//						DEL = DEL-1;
//				}
//				else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
//				{
//					if(DownButton.PressTimer%KEY_LEVEL_2_SET==0)
//						DEL = DEL-1;
//				}
//				else 
//				{
//					if(DownButton.PressTimer%KEY_LEVEL_3_SET==0)
//						DEL = DEL-1;
//				}
//			}	
//			else
//			{
//				DownButton.Effect = PressShort;
//			}
//			
//			if(DEL>=300)
//				DEL = 300;
//			else if(DEL<=30)
//					DEL = 30;
//		}
//}



