///**
//  ********************************  STM32F0x1  *********************************
//  * @�ļ���     �� key.c
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

#define timer_period 0.10 //ms
#define shortKEY 	50
#define middleKEY	100
#define longKEY		150
#define SCAN_TIMER  htim3

//uint32_t key_counter;
ButtonStruct SetButton;
ButtonStruct ModeButton;
ButtonStruct UpButton;
ButtonStruct DownButton;

Button_Status ReadButtonStatus(ButtonStruct *Button);
void PressCallback(ButtonStruct* Button);

void Button_Init(void)
{
	 GPIO_InitTypeDef gpio_init_structure;  
	SetButton.ButtonPort = BUTTON_SET_GPIO_Port;
	SetButton.ButtonPin = BUTTON_SET_Pin;
	SetButton.Mode = ShortAndLong;
	SetButton.PressCounter = 0;
	SetButton.PressTimer = 0;
	SetButton.Status = Release;
	SetButton.WorkIn = WorkHigh;
	SetButton.Effect = PressNOEffect;
	SetButton.ShortTime = 1000*timer_period;
	SetButton.LongTime = 60000*timer_period;   
 
	ModeButton.ButtonPort = BUTTON_MODE_GPIO_Port;
	ModeButton.ButtonPin = BUTTON_MODE_Pin;
	ModeButton.Mode = ShortAndLong;
	ModeButton.PressCounter = 0;
	ModeButton.PressTimer = 0;
	ModeButton.Status = Release;
	ModeButton.WorkIn = WorkHigh;
	ModeButton.Effect = PressNOEffect;
	ModeButton.ShortTime = 1000*timer_period;  
	ModeButton.LongTime = 60000*timer_period;		
	
	UpButton.ButtonPort = BUTTON_UP_GPIO_Port;
	UpButton.ButtonPin = BUTTON_UP_Pin;
	UpButton.Mode = ShortAndLong;
	UpButton.PressCounter = 0;
	UpButton.PressTimer = 0;
	UpButton.Status = Release;
	UpButton.WorkIn = WorkHigh;
	UpButton.Effect = PressNOEffect;
	UpButton.ShortTime = 1000*timer_period;
	UpButton.LongTime = 20000*timer_period;
	
	DownButton.ButtonPort = BUTTON_DOWN_GPIO_Port;
	DownButton.ButtonPin = BUTTON_DOWN_Pin;
	DownButton.Mode = ShortAndLong;
	DownButton.PressCounter = 0;
	DownButton.PressTimer = 0;
	DownButton.Status = Release;
	DownButton.WorkIn = WorkHigh;
	DownButton.Effect = PressNOEffect;
	DownButton.ShortTime = 1000*timer_period;
	DownButton.LongTime = 20000*timer_period;
	
	gpio_init_structure.GPIO_Pin = UpButton.ButtonPin|DownButton.ButtonPin|SetButton.ButtonPin|ModeButton.ButtonPin;  
  gpio_init_structure.GPIO_Mode = GPIO_Mode_IN;                                
  gpio_init_structure.GPIO_Speed = GPIO_Speed_2MHz;              
  gpio_init_structure.GPIO_PuPd= GPIO_PuPd_DOWN; 
	GPIO_Init(UpButton.ButtonPort, &gpio_init_structure);
	GPIO_Init(DownButton.ButtonPort, &gpio_init_structure);
	GPIO_Init(ModeButton.ButtonPort, &gpio_init_structure);
	GPIO_Init(SetButton.ButtonPort, &gpio_init_structure);
	
}

void PressCallback(ButtonStruct* Button)
{	
	if(Button->Mode == ShortAndLong)		//����ģʽ������ʱ���жϳ����̰�
		{
			if(Button->PressTimer>=Button->LongTime) //��������ʱ���Ա�
			{
				Button->Effect = PressLong;  
				if(Button->Status==Release)
				{
					Button->PressTimer = 0; 
					Button->PressCounter++;				//��¼���������´���
				}
			}
			else// if((Button->PressTimer)<(Button->ShortTime)) 
			{
				Button->Effect = PressShort;       
				Button->PressTimer = 0; 
				Button->PressCounter++;				//��¼���������´���
			}
		}
		else 
		{	
				if(Button->Mode == Shortpress)
				{
						Button->Effect = PressShort;       //�̰�ģʽ
						Button->PressTimer = 0; 
						Button->PressCounter++;				//��¼���������´���
				}
				else if(Button->Mode == Longpress)		//����ģʽ
				{
					if(Button->PressTimer>=Button->LongTime) //�жϳ���ʱ��
					{
						Button->Effect = PressLong;       
						Button->PressTimer = 0; 
						Button->PressCounter++;				//��¼���������´���
					} 
				}
		}			 
}


/*����ɨ��*/  
void Key_Scan(void)  
{
	
			/*MODE BUTTON*/
		if(ReadButtonStatus(&ModeButton) == Press )     //UP�����£�һֱ���㰴��ʱ��,���ݶ�ʱ�������ԣ�����ʱ��									
    {
				ModeButton.PressTimer++;
				ModeButton.Status = Press;
    } 
		else if(ReadButtonStatus(&ModeButton) == Release)
		{
			ModeButton.Status = Release;
		}
		if(ModeButton.PressTimer>0 && ModeButton.Status == Release)  //�����ͷ�֮�����жϰ��µ�ʱ�䳤��
		{
			PressCallback(&ModeButton);
		}
		else if(ModeButton.PressTimer>ModeButton.LongTime && ModeButton.Status == Press) /*���������ڰ���״̬*/
		{
			PressCallback(&ModeButton);
		}
		
		
		/*SET BUTTON*/
		if(ReadButtonStatus(&SetButton) == Press )     //UP�����£�һֱ���㰴��ʱ��,���ݶ�ʱ�������ԣ�����ʱ��									
    {
				SetButton.PressTimer++;
				SetButton.Status = Press;
    } 
		else if(ReadButtonStatus(&SetButton) == Release)
		{
			SetButton.Status = Release;
		}
		if(SetButton.PressTimer>0 && SetButton.Status == Release)  //�����ͷ�֮�����жϰ��µ�ʱ�䳤��
		{
			PressCallback(&SetButton);
		}
		else if(SetButton.PressTimer>ModeButton.LongTime && SetButton.Status == Press) /*���������ڰ���״̬*/
		{
			PressCallback(&SetButton);
		}
		
		/*UP BUTTON*/
		if(ReadButtonStatus(&UpButton) == Press )     //UP�����£�һֱ���㰴��ʱ��,���ݶ�ʱ�������ԣ�����ʱ��									
    {
				UpButton.PressTimer++;
				UpButton.Status = Press;
    } 
		else if(ReadButtonStatus(&UpButton) == Release)
		{
			UpButton.Status = Release;
		}
		if(UpButton.PressTimer>0 && UpButton.Status == Release)  //�����ͷ�֮�����жϰ��µ�ʱ�䳤��
		{
			PressCallback(&UpButton);
		}
		else if(UpButton.PressTimer>UpButton.LongTime && UpButton.Status == Press) /*���������ڰ���״̬*/
		{
			PressCallback(&UpButton);
		}
		/*DOWN BUTTON*/
		if(ReadButtonStatus(&DownButton) == Press )     //DOWN�����£�һֱ���㰴��ʱ��,���ݶ�ʱ�������ԣ�����ʱ��									
    {
				DownButton.PressTimer++;
				DownButton.Status = Press;
    } 
		else if(ReadButtonStatus(&DownButton) == Release)
		{
			DownButton.Status = Release;
		}
		if(DownButton.PressTimer>0 && DownButton.Status == Release)  //�����ͷ�֮�����жϰ��µ�ʱ�䳤��
		{
			PressCallback(&DownButton);
		}
		else if(DownButton.PressTimer>UpButton.LongTime && DownButton.Status == Press) /*���������ڰ���״̬*/
		{
			PressCallback(&DownButton);
		}
}  

Button_Status ReadButtonStatus(ButtonStruct *Button)
{
	uint8_t PinState=0;
	PinState = GPIO_ReadInputDataBit(Button->ButtonPort ,Button->ButtonPin);
//	if(Button->WorkIn == WorkLow )
//	{
//		if(PinState==(uint8_t)Bit_RESET)
//			return Press;
//		else
//			return Release;
//	}
	if(Button->WorkIn == WorkHigh )
	{
		if(PinState==(uint8_t)Bit_SET)
			return Press;
		else
			return Release;
	}
}