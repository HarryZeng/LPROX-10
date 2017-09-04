
///**
//  ********************************  STM32F0x1  *********************************
//  * @�ļ���     �� differentialDC.c
//  * @����       �� HarryZeng
//  * @��汾     �� V1.5.0
//  * @�ļ��汾   �� V1.0.0
//  * @����       �� 2017��05��11��
//  * @ժҪ       �� ���ݴ���
//  ******************************************************************************/
///*----------------------------------------------------------------------------
//  ������־:
//  2017-05-11 V1.0.0:��ʼ�汾
//  ----------------------------------------------------------------------------*/
///* ������ͷ�ļ� --------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "differentialDC.h"
#include "key.h"
#include "display.h"
#include "SelfStudy.h"
#include "menu.h"
#include "bsp_eeprom_24xx.h"

/*DSP��궨�壺ARM_MATH_CM0*/


extern uint8_t sample_finish;
uint32_t DealyBaseTime=23;
uint16_t DEL = 50;
int16_t Threshold=1000;
int16_t HI = 1000;
int16_t LO = 700;
bool timeflag = 0;
uint8_t RegisterA=0;
uint8_t RegisterB=1;
uint8_t OUT1=0;
uint8_t OUT2=0;
int16_t OUT2_TimerCounter=0;
uint16_t OUT2_Timer=0;
uint32_t ADCValue=0;
uint8_t ADCValueFlag=0;
uint32_t ADC_ROW_Value[4];
uint32_t ADC_Display_Value[258];
uint32_t ADC_Display=0;
uint16_t ADC_Display_Index=0;
int32_t DACOUT = 1000;
uint32_t CPV = 0;
Button_STATUS KEY=ULOC;
uint8_t 	ConfirmShortCircuit=0;
uint32_t   ShortCircuitCounter = 0;
uint32_t   ShortCircuitLastTime = 0;
uint8_t 	KeyMappingFlag=0;
uint8_t  	EventFlag=0x00; 
uint8_t 	ShortCircuit=0;
uint32_t 	ShortCircuitTimer=0;
uint8_t  displayModeONE_FLAG = 0; 

void GetADCValue(uint32_t *meanValue);
void DisplayMODE(void);
void DisplayModeONE(void);
void DisplayModeTWO(void);
void DisplayModeTHIRD(void);
void DisplayModeFour(void);
void ShortCircuitProtection(void);
void SetOUT1Status(void);
void SetOUT2Status(void);
void ButtonMapping(void);
void DEL_Set(void);
void DisplayModeONE_STD(void);
void DisplayModeONE_AREA(void);
/*----------------------------------�궨��-------------------------------------*/
extern uint32_t OUTADCValue;

/*���������Ĵ�*/
void GetADCValue(uint32_t *meanValue)
{
//	uint32_t TempValue=0;
//	uint8_t i=0;
//	
//	if(sample_finish)
//	{
//		sample_finish = 0;
//		for(i=0;i<4;i++)
//			TempValue += ADC_ROW_Value[i];
//	}
	
	*meanValue = OUTADCValue;
}

/******
д�ڲ�FLASH
*****/
void WriteFlash(uint32_t addr,uint32_t data)
{	
	FLASH_Unlock(); //����FLASH��̲���������
	FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);//�����־λ
	FLASH_ErasePage(addr); //����ָ����ַҳ
	FLASH_ProgramWord(addr,data); //��ָ��ҳ��0��ַ��ʼд
	FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPERR);//�����־λ
	FLASH_Lock(); //����FLASH��̲���������
}
/*����OUT1*/
/********************
*
*�жϳ�RegisterA״̬
*
**********************/
uint8_t GetRegisterAState(uint32_t ADCValue)
{
	uint8_t A;
		
	if(ADCValue>=Threshold)
		A = 1;
	else if(ADCValue<=Threshold-50)
		A = 0;
	return A;
}



void differenttialDC(void)
{
	GetEEPROM();
	while(1)
	{
//			/*������ʾ*/
//			if(EventFlag&ADVtimeFlag) //�ж��¼�
//			{
				EventFlag = EventFlag &(~ADVtimeFlag);  //�����־λ
				GetADCValue(&ADC_Display_Value[ADC_Display_Index]);		//��ʱADC����
				ADC_Display_Value[256]+=ADC_Display_Value[ADC_Display_Index++];//�ۼ�
				if(ADC_Display_Index>128)
				{
					ADC_Display_Index = 0;
					ADCValueFlag = 1;
				}			
				if(ADCValueFlag)
				{
					ADCValueFlag = 0;
					ADC_Display = ADC_Display_Value[256]/128;
					ADC_Display_Value[256] = 0;
				}
		//	}			
				//		ADC_Display++;
				/*��·����*/
				ShortCircuitProtection();

				while(ConfirmShortCircuit)
				{
						GPIO_WriteBit(OUT1_GPIO_Port,OUT1_Pin,Bit_RESET);
						GPIO_WriteBit(OUT2_GPIO_Port,OUT2_Pin,Bit_RESET);/*��������OUT*/ /*���ֶ�·����OUT��������*/
						if((ShortCircuitLastTime - ShortCircuitTimer)>=20000)
						{
								ConfirmShortCircuit = 0;
								ShortCircuitCounter = 0;
								ShortCircuit=0;
						}
				}
				
				/*������ʾģʽ*/
				DisplayMODE();
			
				/*��������*/
				ButtonMapping();
				
				if(KEY==ULOC)/*�жϰ����Ƿ�����*/
				{
				/*SET��ѧϰģʽ*/
					selfstudy();
				/*Mode�˵�ģʽ*/
					menu();
				}
	}
}


/*******************************
*
*��ʾģʽ�л�
*
*******************************/
uint8_t DisplayModeNo=0;

void DisplayMODE(void)
{
	if(KEY==ULOC)
	{
		if(ModeButton.PressCounter==0)
		{
				DisplayModeNo = 0;
		}
		else if(ModeButton.Effect==PressShort && ModeButton.PressCounter==1&&DownButton.Status==Release)
		{
			DisplayModeNo = 1;
		}
		else if(ModeButton.Effect==PressShort && ModeButton.PressCounter==2&&DownButton.Status==Release)
		{
				DisplayModeNo = 2;
		}
		else if(ModeButton.Effect == PressShort && ModeButton.PressCounter==3 &&DownButton.Status==Release)
		{
				DisplayModeNo = 3;
		}
		else if(ModeButton.Effect == PressShort && ModeButton.PressCounter==3 &&DownButton.Status==Release)
		{
			DisplayModeNo = 4;
		}
		/********mode ���� ѭ��*******/
		if(ModeButton.Effect == PressShort && ModeButton.PressCounter==4 &&DownButton.Status==Release&&displayModeONE_FLAG==0)
		{
				ModeButton.PressCounter = 0;
				DisplayModeNo = 0;
		}
		else if(ModeButton.Effect == PressShort && ModeButton.PressCounter==5 &&DownButton.Status==Release&&displayModeONE_FLAG==1)
		{
				ModeButton.PressCounter = 0;
				DisplayModeNo = 0;
		}
	}
	else
	{
		while(ModeButton.Status==Press||SetButton.Status==Press||DownButton.Status==Press||UpButton.Status==Press)
		{
			if(ModeButton.Effect==PressLong &&DownButton.Effect ==PressLong&&DownButton.Status ==Press)
			{
				if(KEY == ULOC)
						KEY = LOC;
				else
				{
					KEY = ULOC;
					KeyMappingFlag = 0;
				}
				ModeButton.PressCounter = 0;
				while((ReadButtonStatus(&ModeButton)) == Press&&(ReadButtonStatus(&DownButton) == Press))
				{
					ButtonMappingDisplay(1);
				}
				WriteFlash(KEY_FLASH_DATA_ADDRESS,KEY);
				ModeButton.Effect=PressNOEffect;
				ModeButton.PressCounter = 0;
				DownButton.PressCounter = 0;
				DownButton.Effect=PressNOEffect;
			}
			else
				ButtonMappingDisplay(1);
		}
	}
	/*��ʾ*/
	if((DisplayModeNo==0&&displayModeONE_FLAG==0)||(DisplayModeNo<=1&&displayModeONE_FLAG==1))
	{
		DisplayModeONE();
		//DisplayModeONE_STD(); /*��׼��ʾģʽ-1*/
	}
	else if((DisplayModeNo==1&&displayModeONE_FLAG==0)||(DisplayModeNo==2&&displayModeONE_FLAG==1))
	{
		DisplayModeTWO();
	}
	else if((DisplayModeNo==2&&displayModeONE_FLAG==0)||(DisplayModeNo==3&&displayModeONE_FLAG==1))
	{
		DisplayModeTHIRD();
	}
	else if((DisplayModeNo==3&&displayModeONE_FLAG==0)||(DisplayModeNo==4&&displayModeONE_FLAG==1))
	{
		DisplayModeFour();
	}
}

/*******************************
*
*��ʾģʽ1
*
*******************************/
void DisplayModeONE(void)
{
	if(displayModeONE_FLAG)
		DisplayModeONE_AREA();
	else
		DisplayModeONE_STD();
}


/*******************************
*
*��ʾģʽ1_DETECT_STD
*
*******************************/
void DisplayModeONE_STD(void)
{
	static uint8_t lastCounter;
	static int16_t LastThreshold;
		/*�������ʾ*/
		SMG_DisplayModeONE(Threshold,ADC_Display);
	
	if(ModeButton.Status==Release && KeyMappingFlag==0 && KEY==ULOC)
	{
		/*Up Button*/
		LastThreshold = Threshold;
		if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
		{
			lastCounter = UpButton.PressCounter;
			UpButton.PressCounter = 0;
			Threshold = Threshold+1;
		}
		else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
		{				/*�����Ű���������ʱ�䳬������ʱ��*/
			UpButton.PressCounter = 0;
			if(UpButton.PressTimer<=KEY_LEVEL_1)
			{
				if(UpButton.PressTimer%KEY_LEVEL_1_SET==0)
					Threshold = Threshold+1;
			}
			else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<=KEY_LEVEL_2)
			{
				if(UpButton.PressTimer%KEY_LEVEL_2_SET==0)
					Threshold = Threshold+1;
			}
			else 
			{
				if(UpButton.PressTimer%KEY_LEVEL_3_SET==0)
					Threshold = Threshold+1;
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
			Threshold = Threshold-1;
		}
		else if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
		{
			DownButton.PressCounter = 0;
			if(DownButton.PressTimer<KEY_LEVEL_1)
			{
				if(DownButton.PressTimer%KEY_LEVEL_1_SET==0)
					Threshold = Threshold-1;
			}
			else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
			{
				if(DownButton.PressTimer%KEY_LEVEL_2_SET==0)
					Threshold = Threshold-1;
			}
			else 
			{
				if(DownButton.PressTimer%KEY_LEVEL_3_SET==0)
					Threshold = Threshold-1;
			}
		}
		else
		{
			DownButton.Effect = PressShort;
		}
		if(LastThreshold!=Threshold && DownButton.Status==Release && UpButton.Status==Release)
		{
			WriteFlash(Threshold_FLASH_DATA_ADDRESS,Threshold);
		}
	}
		if(Threshold>=4000)
				Threshold = 4000;
		else if(Threshold<=0)
				Threshold = 0;
}


/*******************************
*
*��ʾģʽ1_DETECT_AREA
*
*******************************/
void DisplayModeONE_AREA(void)
{
			static uint8_t lastCounter;
			static int16_t LastHIValue;
			static int16_t LastLOValue;
			
			/*HI display mode*/
			if(DisplayModeNo==0)
			{
				SMG_DisplayModeONE_Detect_AREA_HI(timeflag,HI,ADC_Display);
				if(ModeButton.Status==Release && KeyMappingFlag==0 && KEY==ULOC)
				{
					/*Up Button*/
					LastHIValue = HI;
					if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
					{
						lastCounter = UpButton.PressCounter;
						UpButton.PressCounter = 0;
						HI = HI+1;
					}
					else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
					{				/*�����Ű���������ʱ�䳬������ʱ��*/
						UpButton.PressCounter = 0;
						if(UpButton.PressTimer<=KEY_LEVEL_1)
						{
							if(UpButton.PressTimer%KEY_LEVEL_1_SET==0)
								HI = HI+1;
						}
						else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<=KEY_LEVEL_2)
						{
							if(UpButton.PressTimer%KEY_LEVEL_2_SET==0)
								HI = HI+1;
						}
						else 
						{
							if(UpButton.PressTimer%KEY_LEVEL_3_SET==0)
								HI = HI+1;
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
						HI = HI-1;
					}
					else if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
					{
						DownButton.PressCounter = 0;
						if(DownButton.PressTimer<KEY_LEVEL_1)
						{
							if(DownButton.PressTimer%KEY_LEVEL_1_SET==0)
								HI = HI-1;
						}
						else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
						{
							if(DownButton.PressTimer%KEY_LEVEL_2_SET==0)
								HI = HI-1;
						}
						else 
						{
							if(DownButton.PressTimer%KEY_LEVEL_3_SET==0)
								HI = HI-1;
						}
					}
					else
					{
						DownButton.Effect = PressShort;
					}
					if(LastHIValue!=HI && DownButton.Status==Release && UpButton.Status==Release)
					{
						//WriteFlash(Threshold_FLASH_DATA_ADDRESS,Threshold);
					}
				}
				if(HI>=4000)
						HI = 4000;
				else if(HI<=100)
						HI = 100;
			}
			/*LO display mode*/
			else if(DisplayModeNo==1)
			{
				SMG_DisplayModeONE_Detect_AREA_HI(timeflag,LO,ADC_Display);
				if(ModeButton.Status==Release && KeyMappingFlag==0 && KEY==ULOC)
				{
					/*Up Button*/
					LastLOValue = LO;
					if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
					{
						lastCounter = UpButton.PressCounter;
						UpButton.PressCounter = 0;
						LO = LO+1;
					}
					else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
					{				/*�����Ű���������ʱ�䳬������ʱ��*/
						UpButton.PressCounter = 0;
						if(UpButton.PressTimer<=KEY_LEVEL_1)
						{
							if(UpButton.PressTimer%KEY_LEVEL_1_SET==0)
								LO = LO+1;
						}
						else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<=KEY_LEVEL_2)
						{
							if(UpButton.PressTimer%KEY_LEVEL_2_SET==0)
								LO = LO+1;
						}
						else 
						{
							if(UpButton.PressTimer%KEY_LEVEL_3_SET==0)
								LO = LO+1;
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
						LO = LO-1;
					}
					else if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
					{
						DownButton.PressCounter = 0;
						if(DownButton.PressTimer<KEY_LEVEL_1)
						{
							if(DownButton.PressTimer%KEY_LEVEL_1_SET==0)
								LO = LO-1;
						}
						else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
						{
							if(DownButton.PressTimer%KEY_LEVEL_2_SET==0)
								LO = LO-1;
						}
						else 
						{
							if(DownButton.PressTimer%KEY_LEVEL_3_SET==0)
								LO = LO-1;
						}
					}
					else
					{
						DownButton.Effect = PressShort;
					}
					if(LastLOValue!=LO && DownButton.Status==Release && UpButton.Status==Release)
					{
						//WriteFlash(Threshold_FLASH_DATA_ADDRESS,Threshold);
					}
				}
				if(LO>=4000)
						LO = 4000;
				else if(LO<=100)
						LO = 100;
			}
	
	
	
}

/*******************************
*
*��ʾģʽ2
*
*******************************/
void DisplayModeTWO(void)
{
	static uint8_t lastCounter;
		/*�������ʾ*/
		SMG_DisplayModeTWO(RegisterB);
	
		/*Up Button*/
		if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
		{
			lastCounter = UpButton.PressCounter;
			UpButton.PressCounter = 0;
			if(RegisterB==0)
				RegisterB = 1;
			else 
				RegisterB=0;
			WriteFlash(RegisterB_FLASH_DATA_ADDRESS,RegisterB);
		}

		/*Down Button*/
		if(DownButton.PressCounter !=lastCounter && DownButton.Effect==PressShort)
		{
			DownButton.PressCounter = 0;
			if(RegisterB==0)
				RegisterB = 1;
			else 
				RegisterB=0;
			WriteFlash(RegisterB_FLASH_DATA_ADDRESS,RegisterB);
		}
}

/*******************************
*
*��ʾģʽ3
*
*******************************/
void DisplayModeTHIRD(void)
{
		/*�������ʾ*/
		SMG_DisplayModeTHIRD(CPV);
}

/*******************************
*
*��ʾģʽ4
*
*******************************/
void DisplayModeFour(void)
{
	DEL_Set();
}

/*******************************
*
*OUT1_DEL Ӧ��ֵ����
*
*******************************/
void DEL_Set(void)
{
		static uint8_t lastCounter;
		uint16_t LastDEL;
//		if(EventFlag&Blink500msFlag) 
//		{
//			EventFlag = EventFlag &(~Blink500msFlag);  //�����־λ
//			
//			SMG_DisplayMenuTwo_DEL();
//		}
//		/*�̰�MODE�󣬽���SHOT�������Ӳ˵�*/
//		while(ModeButton.Effect==PressShort && ModeButton.PressCounter==4)
//		{
			/*��ʾSHOT value*/			
//			if(UpButton.Status==Release&&DownButton.Status==Release)
//			{
//				if(EventFlag&Blink500msFlag) 
//				{
//					EventFlag = EventFlag &(~Blink500msFlag);  //�����־λ
//					/*��ʾSHOT,��˸*/
					SMG_DisplayMenuTwo_DEL_SET(DEL,0);
//				}
//			}
//			else
//			{
//				EventFlag = EventFlag &(~Blink500msFlag);
//				SMG_DisplayMenuTwo_DEL_SET(DEL,0);
//			}
			
			/*Up Button*/
			if(UpButton.PressCounter !=lastCounter && UpButton.Effect==PressShort)
			{
				lastCounter = UpButton.PressCounter;
				UpButton.PressCounter = 0;
				DEL = DEL+1;
			}
			else 	if(UpButton.Status==Press&&(UpButton.Effect==PressLong))
			{				/*�����Ű���������ʱ�䳬������ʱ��*/
				UpButton.PressCounter = 0;
				if(UpButton.PressTimer<KEY_LEVEL_1)
				{
					if(UpButton.PressTimer%KEY_LEVEL_1_SET==0)
						DEL = DEL+1;
				}
				else if(UpButton.PressTimer>KEY_LEVEL_1&&UpButton.PressTimer<KEY_LEVEL_2)
				{
					if(UpButton.PressTimer%KEY_LEVEL_2_SET==0)
						DEL = DEL+1;
				}
				else 
				{
					if(UpButton.PressTimer%KEY_LEVEL_3_SET==0)
						DEL = DEL+1;
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
				DEL = DEL-1;
			}
			else if(DownButton.Status==Press&&(DownButton.Effect==PressLong))
			{				/*�����Ű���������ʱ�䳬������ʱ��*/
				DownButton.PressCounter = 0;
				if(DownButton.PressTimer<KEY_LEVEL_1)
				{
					if(DownButton.PressTimer%KEY_LEVEL_1_SET==0)
						DEL = DEL-1;
				}
				else if(DownButton.PressTimer>KEY_LEVEL_1&&DownButton.PressTimer<KEY_LEVEL_2)
				{
					if(DownButton.PressTimer%KEY_LEVEL_2_SET==0)
						DEL = DEL-1;
				}
				else 
				{
					if(DownButton.PressTimer%KEY_LEVEL_3_SET==0)
						DEL = DEL-1;
				}
			}	
			else
			{
				DownButton.Effect = PressShort;
			}
			if(LastDEL!=DEL && DownButton.Status==Release && UpButton.Status==Release)
			{
				WriteFlash(DEL_FLASH_DATA_ADDRESS,DEL);
			}
			if(DEL>=300)
				DEL = 300;
			else if(DEL<=30)
					DEL = 30;
	//	}
}

/*******************************
*
*�ж�OUT1�����״̬
*
*******************************/
uint8_t SHOTflag=0;
void SetOUT1Status(void)
{
	if(ShortCircuit!=1)/*���Ƕ�·����������²��ж�OUT1�����*/
	{
		/*ͬ������*/
		OUT1=!(RegisterB^RegisterA);
		if(OUT1_Mode.DelayMode==TOFF)
		{
			//GPIOA->ODR ^= GPIO_Pin_9;
			if(OUT1)
			{
				GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_SET);
				OUT1_Mode.DelayCounter = 0;
			}
			else
			{
				GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_RESET);
				OUT1_Mode.DelayCounter=0;
			}
		}
		/*OFFD*/
		else if(OUT1_Mode.DelayMode==OFFD)
		{
			if(OUT1)
			{
				GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_SET);
				OUT1_Mode.DelayCounter = 0;
			}
			else
			{
				if(OUT1_Mode.DelayCounter>(OUT1_Mode.DelayValue*DealyBaseTime))
				{
					GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_RESET);
				}
			}
		}
		/*ON_D*/
		else if(OUT1_Mode.DelayMode==ON_D)
		{
			if(OUT1)
			{
				if(OUT1_Mode.DelayCounter>(OUT1_Mode.DelayValue*DealyBaseTime))
				{
					GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_SET);
				}
			}
			else
			{
				GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_RESET);
				OUT1_Mode.DelayCounter = 0;
			}
		}
		/*SHOT*/
		else if(OUT1_Mode.DelayMode==SHOT)
		{
			if(OUT1 || SHOTflag == 1)
			{
				if(OUT1_Mode.DelayCounter<(OUT1_Mode.DelayValue*DealyBaseTime))
				{
					GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_SET);
					SHOTflag = 1;
				}
				else
				{
					GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_RESET);
					SHOTflag = 0;
				}
			}
			else
			{
				OUT1_Mode.DelayCounter = 0;
				GPIO_WriteBit(OUT1_GPIO_Port, OUT1_Pin, Bit_RESET);
			}
		}
	}
}
/*******************************
*
*�ж�OUT2�����״̬
*
*******************************/
void SetOUT2Status(void)
{
	if(ShortCircuit!=1)/*���Ƕ�·����������²��ж�OUT2�����*/
	{
		if(OUT2)
		{
			GPIO_WriteBit(OUT2_GPIO_Port,OUT2_Pin,Bit_SET);/*����*/
			//GPIO_WriteBit(OUT1_GPIO_Port,OUT1_Pin,Bit_SET);
		}
	 if(OUT2_TimerCounter >=250)
		{
			OUT2 = 0;
			OUT2_TimerCounter = 0;  /*��ȡ��ǰʱ��*/
			//GPIO_WriteBit(OUT1_GPIO_Port,OUT1_Pin,Bit_RESET);
			GPIO_WriteBit(OUT2_GPIO_Port,OUT2_Pin,Bit_RESET);/*80ms������*/
		}
	}
}

/*******************************
*
*��·����
*
*******************************/
void ShortCircuitProtection(void)
{
	uint8_t SCState;
	
	/*��ȡSC���ŵ�״̬*/
	if(ShortCircuit!=1)
	{
		SCState = GPIO_ReadInputDataBit(SC_GPIO_Port ,SC_Pin);
		if((BitAction)SCState == Bit_RESET)
		{
			/*����FB_SC*/
			ShortCircuit= 1;
		}
		else
		{
			ShortCircuit = 0;
			ConfirmShortCircuit = 0;
		}
	}
	if(ShortCircuit && ShortCircuitCounter>=100)
	{
		ConfirmShortCircuit=1;
		GPIO_WriteBit(OUT1_GPIO_Port,OUT1_Pin,Bit_RESET);
		GPIO_WriteBit(OUT2_GPIO_Port,OUT2_Pin,Bit_RESET);/*��������OUT*/
		ShortCircuitTimer = ShortCircuitLastTime;
	}	
}

/*******************************
*
*��������
*
*******************************/
void ButtonMapping(void)
{
	/*��������*/
	if(ModeButton.Effect==PressLong &&DownButton.Effect ==PressLong&&DownButton.Status ==Press)
	{
		if(KEY == ULOC)
				KEY = LOC;
		else
			KEY = ULOC;
		ModeButton.PressCounter = 0;
		while((ReadButtonStatus(&ModeButton)) == Press&&(ReadButtonStatus(&DownButton) == Press))
		{
			ButtonMappingDisplay(1);
			KeyMappingFlag = 1;
		}
		WriteFlash(KEY_FLASH_DATA_ADDRESS,KEY);
		ModeButton.Effect=PressNOEffect;
		ModeButton.PressCounter = 0;
		DownButton.PressCounter = 0;
		DownButton.Effect=PressNOEffect;

	}
	/*������ʼ��*/
	if(ModeButton.Effect==PressLong &&SetButton.Effect ==PressLong&&SetButton.Status ==Press)
	{
		CSV = 1000;
		Threshold = 1000;
		KEY = ULOC;
		OUT1_Mode.DelayMode = TOFF;
		OUT1_Mode.DelayValue = 10;
		DACOUT = 1000;
		DEL = 50;
		RegisterB = 1;
		while((ReadButtonStatus(&ModeButton)) == Press&&(ReadButtonStatus(&SetButton) == Press))
		{
			ButtonMappingDisplay(2);
			KeyMappingFlag = 1;
		}
	
	
		WriteFlash(OUT1_Mode_FLASH_DATA_ADDRESS,OUT1_Mode.DelayMode);
		WriteFlash(OUT1_Value_FLASH_DATA_ADDRESS,OUT1_Mode.DelayValue);
		WriteFlash(CSV_FLASH_DATA_ADDRESS,CSV);
		WriteFlash(Threshold_FLASH_DATA_ADDRESS,Threshold);
		WriteFlash(DACOUT_FLASH_DATA_ADDRESS,DACOUT);
		WriteFlash(KEY_FLASH_DATA_ADDRESS,KEY);
		WriteFlash(RegisterB_FLASH_DATA_ADDRESS,RegisterB);
		WriteFlash(DEL_FLASH_DATA_ADDRESS,DEL);

		ModeButton.Effect=PressNOEffect;
		ModeButton.PressTimer = 0;
		ModeButton.PressCounter = 0;
		SetButton.Effect=PressNOEffect;
		SetButton.PressCounter = 0;
		
		DAC_SetChannel1Data(DAC_Align_12b_R,(uint16_t)DACOUT);
		DAC_SoftwareTriggerCmd(DAC_Channel_1,ENABLE);

	}
	/*����������*/
	if(ModeButton.Effect==PressLong &&UpButton.Effect ==PressLong&&UpButton.Status ==Press)
	{
		CPV = 0;
		while((ReadButtonStatus(&ModeButton)) == Press&&(ReadButtonStatus(&UpButton) == Press))
		{
			ButtonMappingDisplay(3);
			KeyMappingFlag=1;
		}
		ModeButton.Effect=PressNOEffect;
		ModeButton.PressTimer = 0;
		ModeButton.PressCounter = 0;
		UpButton.PressCounter = 0;
		UpButton.Effect=PressNOEffect;
	}	
}

 void Test_Delay(uint32_t ms)
{
	uint32_t i;

	/*��
		CPU��Ƶ168MHzʱ�����ڲ�Flash����, MDK���̲��Ż�����̨ʽʾ�����۲Ⲩ�Ρ�
		ѭ������Ϊ5ʱ��SCLƵ�� = 1.78MHz (����ʱ: 92ms, ��д������������ʾ����̽ͷ���ϾͶ�дʧ�ܡ�ʱ��ӽ��ٽ�)
		ѭ������Ϊ10ʱ��SCLƵ�� = 1.1MHz (����ʱ: 138ms, ���ٶ�: 118724B/s)
		ѭ������Ϊ30ʱ��SCLƵ�� = 440KHz�� SCL�ߵ�ƽʱ��1.0us��SCL�͵�ƽʱ��1.2us

		��������ѡ��2.2Kŷʱ��SCL������ʱ��Լ0.5us�����ѡ4.7Kŷ����������Լ1us

		ʵ��Ӧ��ѡ��400KHz���ҵ����ʼ���
	*/
	for (i = 0; i < ms*10; i++);
}
/*******************************
*
*��ȡEEPROM����
*
*******************************/

//	uint32_t _test_send_buf[30];
//	uint32_t _test_recv_buf[30];
//	short _test_size = 30;

//	char statusW=2;
//	char statusR=2;

uint32_t ProgramRUNcounter=0;


void GetEEPROM(void)
{
		uint8_t tempKEY=0;
		
		ProgramRUNcounter 		= *(__IO uint32_t*)(ProgramRUNcounter_Mode_FLASH_DATA_ADDRESS);
		if(ProgramRUNcounter>0)	
		{
			OUT1_Mode.DelayMode 	= *(__IO uint32_t*)(OUT1_Mode_FLASH_DATA_ADDRESS);
			OUT1_Mode.DelayValue 	= *(__IO uint32_t*)(OUT1_Value_FLASH_DATA_ADDRESS);
			CSV 									= *(__IO uint32_t*)(CSV_FLASH_DATA_ADDRESS);
			Threshold 						= *(__IO uint32_t*)(Threshold_FLASH_DATA_ADDRESS);
			DACOUT 								= *(__IO uint32_t*)(DACOUT_FLASH_DATA_ADDRESS);
			KEY 									= *(__IO uint32_t*)(KEY_FLASH_DATA_ADDRESS);
			RegisterB 						= *(__IO uint32_t*)(RegisterB_FLASH_DATA_ADDRESS);
			DEL 									= *(__IO uint32_t*)(DEL_FLASH_DATA_ADDRESS);
		}
	
		ProgramRUNcounter++;
		//����������д���
		WriteFlash(ProgramRUNcounter_Mode_FLASH_DATA_ADDRESS,ProgramRUNcounter);

		DAC_SetChannel1Data(DAC_Align_12b_R,(uint16_t)DACOUT);
		DAC_SoftwareTriggerCmd(DAC_Channel_1,ENABLE);
		
}

