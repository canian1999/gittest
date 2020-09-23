#include "stm32f10x_lib.h"
#include "stdio.h"
#include "time.h"

u8 TimeBuf[16];

struct tm SysTime;  // 系统的时间
struct tm TimeSet;  // 当前设置的时间
time_t RTCTick;

//#define RTCClockSource_LSI   /* Use the internal 32 KHz oscillator as RTC clock source */
#define RTCClockSource_LSE   /* Use the external 32.768 KHz oscillator as RTC clock source */
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */
/*******************************************************************************
* Function Name  : RTC_Configuration
* Description    : Configures the RTC.		   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
  
  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
//  BKP_DeInit();

#ifdef RTCClockSource_LSI
  /* Enable LSI */ 
  RCC_LSICmd(ENABLE);
  /* Wait till LSI is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);

  /* Select LSI as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);  
#elif defined	RTCClockSource_LSE  
  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);  
#endif


  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);


#ifdef RTCClockOutput_Enable  
  /* Disable the Tamper Pin */
  BKP_TamperPinCmd(DISABLE); /* To output RTCCLK/64 on Tamper pin, the tamper
                               functionality must be disabled */
                               
  /* Enable RTC Clock Output on Tamper Pin */
  BKP_RTCCalibrationClockOutputCmd(ENABLE);
#endif 

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  
  /* Enable the RTC Second */  
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  
  /* Set RTC prescaler: set RTC period to 1sec */
#ifdef RTCClockSource_LSI
  RTC_SetPrescaler(31999); /* RTC period = RTCCLK/RTC_PR = (32.000 KHz)/(31999+1) */
#elif defined	RTCClockSource_LSE
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
#endif
  
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}

void RTCInit(void)
{
  if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    /* RTC Configuration */
    RTC_Configuration();

    //初始时间
    TimeSet.tm_year=2020-1900;
    TimeSet.tm_mon = 4;
    TimeSet.tm_mday=9;
    TimeSet.tm_hour=0;
    TimeSet.tm_min=0;
    TimeSet.tm_sec=0;
    TimeSet.tm_wday=6;

    RTC_SetCounter(mktime(&TimeSet));

    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);    
  }
  else
  {
    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Enable the RTC Second */  
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
  }

  /* Clear reset flags */
  RCC_ClearFlag();
}

u8 DispTime=1;

void RdRTC(int *addr)
{
  RTCTick = RTC_GetCounter();
  localtime_r(&RTCTick, &SysTime);
  *addr = SysTime.tm_min;         //分钟
  *(addr + 1) = SysTime.tm_hour;   //小时
  *(addr + 2) = SysTime.tm_mday;   //日
  *(addr + 3) = SysTime.tm_mon+1;   //月
  *(addr + 4) = SysTime.tm_year - 100;   //年
} 
void WtRTC(int *addr)
{

	TimeSet.tm_min	=  *addr;
	TimeSet.tm_hour	=  *(addr + 1);
	TimeSet.tm_mday =  *(addr + 2);
	TimeSet.tm_mon  =  *(addr + 3)-1;
	TimeSet.tm_year =  *(addr + 4)+100;
	TimeSet.tm_sec  = 0; 

    RTC_Configuration();
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask(); 
    RTC_SetCounter(mktime(&TimeSet));// 保存用户设置的时间到RTC
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();   
}

