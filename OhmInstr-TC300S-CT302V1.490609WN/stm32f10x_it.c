/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : stm32f10x_it.c
* Author             : MCD Application Team
* Version            : V2.0.2
* Date               : 07/11/2008
* Description        : Main Interrupt Service Routines.
*                      This file provides template for all exceptions handler
*                      and peripherals interrupt service routine.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "stm32f10x_lib.h"
#include "key2010.h"
#include "lcd2010.h"
#include "macro.h"
#include "system.h"

unsigned int LCDLED_RST = 0;

extern unsigned int uiCnt10ms;
extern unsigned int uiCnt1000ms;
extern	volatile unsigned int uiCnt700ms;
extern u8  sx1276_7_8SlaveAddress;
extern unsigned char flgWindRun;   //标志进风电机正在运行;
extern unsigned int uiWindCnt;  //风门电机运行计时
extern unsigned char flgDispBrush;       //控制刷新LCD
extern unsigned char flgKeyOk; //按键
extern unsigned char ucSecCnt;
extern unsigned char ucMinCnt;
extern unsigned char ucCurveNum;
extern unsigned char flgMinCnt;
extern unsigned char flgHourCnt;
extern unsigned char flgSelfSetPause;
extern unsigned char flgRdDandT; //启动读取日期和时间
extern unsigned char flgACADC;  //启动交流电流ADC
extern unsigned char flgRdDandT; //启动读取日期和时间
extern unsigned char flgRdTmp;
extern unsigned char ucLcdBkledCnt;
extern unsigned char enWorkMode;
extern unsigned char ucBakWorkMode;
extern unsigned char ucModeSwiTime;
extern unsigned char flgFlashShow;//指示哪一位闪烁
extern unsigned char flgUpDownFlashShow;  //控制传感器显示切换时上、下棚闪烁
extern unsigned char flgTmpDispGist;      //用于标志温湿度显示的依据，0: 显示上棚传感器值;1：显示下棚传感器值
extern unsigned char ucTmpDispGistCnt;    //控制显示切换时间
extern unsigned char flgMainSensor;  //0：以上棚为主传感器；1：以下棚为主传感器
extern unsigned char flgFmNonCnt;
extern unsigned int  uiFmNonCnt; 
extern unsigned char flgCycleTimCnt;
extern unsigned int i16CtrAmount0;
extern unsigned int i16CtrAmount1;
extern u8 u8Sx1278SendFlag;
extern volatile unsigned char   flgCoalFanOn;     //鼓风机通断标志
extern volatile unsigned char ucBakWorkModeDebug;

extern volatile unsigned int FengMenGufengCtr; 
extern volatile unsigned char flgSysCtrFlag; 
extern volatile unsigned int FengMenGufengCtr1; 
extern volatile unsigned char flgXunHuanGaoDiCntPre;
extern volatile unsigned int u16CoalMotorSecCnt;  //风门电机运行计时
extern volatile unsigned int u16CoalMotorOverLoadFlag;  //风门电机运行计时
extern volatile unsigned char flgCoalMotorRun;   //标志进风电机正在运行; 
extern volatile unsigned int u16CoalMotorSecCntPre;  //风门电机运行计时
extern volatile unsigned int i16CoalMotorIntervalSecCnt;
extern unsigned char vdGetFengmenCtrModStart;

unsigned int u16CoalMotorOverLoadFresh = 0;
unsigned int u16CoalMotorOverLoadCnt = 0;
//extern volatile unsigned int u16CoalMotorRunFlag;  //风门电机运行计时
extern volatile unsigned char   flgFanOn;     //鼓风机通断标志
unsigned int u16FanonSecCnt = 0;
unsigned int u16MotorCnt = 1;
unsigned int u16CoalMotorIntevalCnt = 0;
extern volatile unsigned char flgSensorErr;
extern volatile unsigned char TemperSensorErrFlag;
extern volatile unsigned char ucFengjiLackPhCnt;
extern volatile unsigned char flgXunHuanGaoDiPreDelay;
extern u8 SysTime_MicroSec;

extern unsigned int i16CtrAmount0;
extern unsigned int i16CtrAmount0Set;

extern unsigned int u16MasterAddress;
extern unsigned int u16MasterAddressSet;

extern unsigned int u16SlaveAddress;
extern unsigned int u16SlaveAddressSet;


extern volatile unsigned int FenmenActMore1dot5RunFlag;
extern volatile unsigned int FenmenActLeSS1RunFlag;

extern u8 GetRFFrame_1278(void);
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : NMIException
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMIException(void)
{ 
  NVIC_SETFAULTMASK();
  NVIC_GenerateSystemReset();
 }

/*******************************************************************************
* Function Name  : HardFaultException
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFaultException(void)
{
  NVIC_SETFAULTMASK();
  NVIC_GenerateSystemReset();
}

/*******************************************************************************
* Function Name  : MemManageException
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MemManageException(void)
{
  NVIC_SETFAULTMASK();
  NVIC_GenerateSystemReset();
}

/*******************************************************************************
* Function Name  : BusFaultException
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BusFaultException(void)
{
  NVIC_SETFAULTMASK();
  NVIC_GenerateSystemReset();
}

/*******************************************************************************
* Function Name  : UsageFaultException
* Description    : This function handles Usage Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UsageFaultException(void)
{
  NVIC_SETFAULTMASK();
  NVIC_GenerateSystemReset();
}

/*******************************************************************************
* Function Name  : DebugMonitor
* Description    : This function handles Debug Monitor exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DebugMonitor(void)
{  
  NVIC_SETFAULTMASK();
  NVIC_GenerateSystemReset();
}

/*******************************************************************************
* Function Name  : SVCHandler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVCHandler(void)
{ 
  NVIC_SETFAULTMASK();
  NVIC_GenerateSystemReset();
}

/*******************************************************************************
* Function Name  : PendSVC
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PendSVC(void)
{ 
  NVIC_SETFAULTMASK();
  NVIC_GenerateSystemReset();
}

/*******************************************************************************
* Function Name  : SysTickHandler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTickHandler(void)
{
  extern vu32 msTick;
	
  msTick++;
/****10ms*******************************************/
	if(msTick>=10)		//该if内10ms进入一次
	{
     msTick = 0;
	   uiCnt10ms++;
/****100ms******************************************/
	   if(uiCnt10ms >= 10)		//该if内100ms进一次
	   {
			 			REVERSE_RESETCPU;  //
		     uiCnt10ms = 0;
			 if(LCDLED_RST > 2)		//无传感器信号LCD复位
			 {
				 LCDLED_RST --;
			     CLR_SDS18B20;
			 }
			 else if(LCDLED_RST>1)
			 {
				 SET_SDS18B20;
				 vdInitLCD();
				 LCDLED_RST --;
			 }
			 else
			 {
				  SET_SDS18B20;
				  LCDLED_RST = 0;
			 }
		     uiCnt1000ms++;
/********1秒*************************************/
		     if(uiCnt1000ms>=10)	//该if内1s进一次
		     {
	   	         if((1 == flgSensorErr)||(1 == TemperSensorErrFlag))
		         {  
		             REVERSE_LCDLED;
		         }
			     uiCnt1000ms=0;
		         ucSecCnt++;//计秒
					if(flgXunHuanGaoDiPreDelay>1)
					 flgXunHuanGaoDiPreDelay --;
					if(ucFengjiLackPhCnt>1)
						ucFengjiLackPhCnt--;
				 if(ucSecCnt >= 60)  //分钟
				 {
					ucSecCnt = 0;
					ucMinCnt++;
					if(ucCurveNum != SELFSET)
					{
						flgMinCnt  = 0x01;
					}
					else
					{
						if(!flgSelfSetPause)
						flgMinCnt  = 0x01;
					}
					flgRdDandT = 0x01;  //读取日期和时钟
					if(ucMinCnt >= 60)  //小时
					{
							ucMinCnt = 0;
							flgHourCnt = 0x01;
					}		    
				}
			    else if(1 == (ucSecCnt%2))//2秒执行一次
				{
					 if(1 == flgCoalFanOn)
					 {
					      u16FanonSecCnt = u16FanonSecCnt + 2;
						  if(u16FanonSecCnt >= 120)				//两分钟
						  {
								u16FanonSecCnt = 120;
						  }
					 }
					 else
					 {
						 u16FanonSecCnt = 0;
					 }
		
					 if(u16FanonSecCnt >= 120)					
					 {
					 	   u16CoalMotorIntevalCnt = u16CoalMotorIntevalCnt + 2;
					 }
						
					 if(u16CoalMotorIntevalCnt >= 600)				//
					 {
						   u16CoalMotorIntevalCnt = 0;
						   u16MotorCnt = i16CtrAmount0 * 60;
						   if(i16CtrAmount0 >= 9)
							 u16MotorCnt = 610;
					 }


					 u16MotorCnt = u16MotorCnt - 2;
					 if(u16MotorCnt>=2)
					 {
						  JIAMEIJI_FORWARD;
					 }
					 else
					 {
						  JIAMEIJI_STOP;
					 	  u16MotorCnt = 3;
					 }
				}
				else 
				{     
					if(ucLcdBkledCnt > 2)
					    ucLcdBkledCnt = ucLcdBkledCnt - 2;
					else if((0 == flgSensorErr)&&(0 == TemperSensorErrFlag))
					{
						CLR_LCDLED;
					}
					if((enWorkMode == SET_MODE)||(enWorkMode == QUERY_MODE))
					{
						 if( ucModeSwiTime > 2)
							 ucModeSwiTime = ucModeSwiTime - 2;
						 else
						 {
							 enWorkMode = ucBakWorkMode;
							 flgFlashShow = 0;
							 CLR_SETLED; 
						 }
					 }
					 if((enWorkMode == DEBUG_MODE)||(enWorkMode == DEBUG_SlaveAddressSet)||(enWorkMode==DEBUG_MasterAddressSet))
					 {
						 if( ucModeSwiTime > 2)
							 ucModeSwiTime = ucModeSwiTime - 2;
						 else
						 {
							enWorkMode = ucBakWorkModeDebug;
							i16CtrAmount0Set = i16CtrAmount0;
							u16MasterAddressSet = u16MasterAddress;
							u16SlaveAddressSet  = u16SlaveAddress;
						 }
					 }
					 if( flgUpDownFlashShow != 0x00 )
					 {
						 if( ucTmpDispGistCnt > 2)
						     ucTmpDispGistCnt = ucTmpDispGistCnt - 2;
						 else
						 {
							 flgTmpDispGist  = flgMainSensor;
						     flgUpDownFlashShow  = 0x00;
						 } 
					 }
					 if(2 == (ucSecCnt%3))  
					{   
						flgSysCtrFlag = TRUE;
					}
				    flgRdTmp	 = TRUE;	 //启动温度转换
				 }
		  	  }
		      else if((uiCnt1000ms == 4)||(uiCnt1000ms == 8))
		      {
			      flgDispBrush = TRUE;
				  	vdGetFengmenCtrModFlag();
			    }
					else if((uiCnt1000ms == 2))
					{
					 flgACADC  = TRUE;
					}
					else
					{
							if(TRUE!=flgRdTmp)
							{
								 if(GetRFFrame_1278())
									u8Sx1278SendFlag = 1;
							}
						            SysTime_MicroSec++;
									if(SysTime_MicroSec >=4)
											SysTime_MicroSec = 4;
					 }
			}
	    else			//<100ms
			{
				if(( uiCnt10ms%2) == 1) 	//20ms扫描一次按键
				{
					 if(flgKeyOk == FALSE)
					 KeyScan();

					 /*控制进风门电机的运行时间*/
					 if(flgWindRun ==0x01)		//标志风门电机正在运转；
					 {
						 if(uiWindCnt >= 2)
							 uiWindCnt = uiWindCnt - 2;
						 else
						 {
							 WIND_STOP;
							 flgWindRun = 0x00;
							 FenmenActMore1dot5RunFlag = 0; //执行完不再检测
							 FenmenActLeSS1RunFlag     = 0; //执行完不再检测
						 }
					 } 			 
				}
			}
	}
}
/*******************************************************************************
* Function Name  : WWDG_IRQHandler
* Description    : This function handles WWDG interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WWDG_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : PVD_IRQHandler
* Description    : This function handles PVD interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PVD_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : TAMPER_IRQHandler
* Description    : This function handles Tamper interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TAMPER_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles RTC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : FLASH_IRQHandler
* Description    : This function handles Flash interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : RCC_IRQHandler
* Description    : This function handles RCC interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External interrupt Line 0 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI0_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : EXTI1_IRQHandler
* Description    : This function handles External interrupt Line 1 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI1_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : This function handles External interrupt Line 2 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI2_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : EXTI3_IRQHandler
* Description    : This function handles External interrupt Line 3 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI3_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : This function handles External interrupt Line 4 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI4_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : DMA1_Channel1_IRQHandler
* Description    : This function handles DMA1 Channel 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : DMA1_Channel2_IRQHandler
* Description    : This function handles DMA1 Channel 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel2_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : DMA1_Channel3_IRQHandler
* Description    : This function handles DMA1 Channel 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel3_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : DMA1_Channel4_IRQHandler
* Description    : This function handles DMA1 Channel 4 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel4_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : DMA1_Channel5_IRQHandler
* Description    : This function handles DMA1 Channel 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel5_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : DMA1_Channel6_IRQHandler
* Description    : This function handles DMA1 Channel 6 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel6_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : DMA1_Channel7_IRQHandler
* Description    : This function handles DMA1 Channel 7 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel7_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : ADC1_2_IRQHandler
* Description    : This function handles ADC1 and ADC2 global interrupts requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC1_2_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : USB_HP_CAN_TX_IRQHandler
* Description    : This function handles USB High Priority or CAN TX interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_HP_CAN_TX_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : USB_LP_CAN_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_CAN_RX0_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : CAN_RX1_IRQHandler
* Description    : This function handles CAN RX1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_RX1_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : CAN_SCE_IRQHandler
* Description    : This function handles CAN SCE interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN_SCE_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 9 to 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : TIM1_BRK_IRQHandler
* Description    : This function handles TIM1 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_BRK_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : TIM1_UP_IRQHandler
* Description    : This function handles TIM1 overflow and update interrupt
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_UP_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : TIM1_TRG_COM_IRQHandler
* Description    : This function handles TIM1 Trigger and commutation interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_TRG_COM_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : TIM1_CC_IRQHandler
* Description    : This function handles TIM1 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_CC_IRQHandler(void)
{
for(;;);
}

/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)
{
for(;;);
}

/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles TIM4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : I2C1_EV_IRQHandler
* Description    : This function handles I2C1 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_EV_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : I2C1_ER_IRQHandler
* Description    : This function handles I2C1 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_ER_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : I2C2_EV_IRQHandler
* Description    : This function handles I2C2 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_EV_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : I2C2_ER_IRQHandler
* Description    : This function handles I2C2 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_ER_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : SPI1_IRQHandler
* Description    : This function handles SPI1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : SPI2_IRQHandler
* Description    : This function handles SPI2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI2_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 15 to 10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : RTCAlarm_IRQHandler
* Description    : This function handles RTC Alarm interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTCAlarm_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : USBWakeUp_IRQHandler
* Description    : This function handles USB WakeUp interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBWakeUp_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : TIM8_BRK_IRQHandler
* Description    : This function handles TIM8 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_BRK_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : TIM8_UP_IRQHandler
* Description    : This function handles TIM8 overflow and update interrupt
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_UP_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : TIM8_TRG_COM_IRQHandler
* Description    : This function handles TIM8 Trigger and commutation interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_TRG_COM_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : TIM8_CC_IRQHandler
* Description    : This function handles TIM8 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_CC_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : ADC3_IRQHandler
* Description    : This function handles ADC3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC3_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : FSMC_IRQHandler
* Description    : This function handles FSMC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FSMC_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : SDIO_IRQHandler
* Description    : This function handles SDIO global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SDIO_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : TIM5_IRQHandler
* Description    : This function handles TIM5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM5_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : SPI3_IRQHandler
* Description    : This function handles SPI3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI3_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : UART4_IRQHandler
* Description    : This function handles UART4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART4_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : UART5_IRQHandler
* Description    : This function handles UART5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART5_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : TIM6_IRQHandler
* Description    : This function handles TIM6 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM6_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : TIM7_IRQHandler
* Description    : This function handles TIM7 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM7_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : DMA2_Channel1_IRQHandler
* Description    : This function handles DMA2 Channel 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel1_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : DMA2_Channel2_IRQHandler
* Description    : This function handles DMA2 Channel 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel2_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : DMA2_Channel3_IRQHandler
* Description    : This function handles DMA2 Channel 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel3_IRQHandler(void)
{for(;;);}

/*******************************************************************************
* Function Name  : DMA2_Channel4_5_IRQHandler
* Description    : This function handles DMA2 Channel 4 and DMA2 Channel 5
*                  interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel4_5_IRQHandler(void)
{for(;;);}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
