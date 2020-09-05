/******************** (C) COPYRIGHT 2008 STMicroelectronics ***************
* File Name          : main.c
* Author             : MCD Application Team
* Version            : V2.0.2
* Date               : 07/11/2008
* Description        : Main program body
**************************************************************************/
#include "stm32f10x_lib.h"
#include "system.h"
#include "macro.h"
#include "lcd2010.h"
#include "key2010.h"
#include "iic.h"
#include "ds18b20.h"
#include "time.h"
#include "Curve.h"
#include "adc.h"
#include "yuyin.h"
#include "sx1276_7_8.h"
#include "spi.h"
/* Part 1: Macro definitions */
#define VOLSCALE     64       //(��ѹ����)  D/A     ��Чֵ  ȡ��
                              // 300/1.6 * 5/1023 * /1.414  * 100   = 65
                              //������   D/A  ��ЧVֵ  VtoC  AС����һλ  ȡ��
#define CURSCALE     86       //2000 * 5/1023 /1.414   /80   * 10        * 100  = 86

#define  SHENGWEN    0
#define  HENGWEN     1
#define  PIANWEN     2
#define  QUEXIANG    3
#define  GUOZAI      4
#define  ZHURAN      5
#define  PAISHI      6
#define  XUNZIDONG   7
#define  XUNGAO      8
#define  XUNDI       9

/*�������ߡ����ش���������Ϣ������eeprom�е�λ��*/

#define  IEEP_KAONUM        2008        //ucKaofangNum
#define  IEEP_MEIKAO_TOTALT 2010        //ÿ����ʱ�������׵�ַ��ucKaoTotalTime[0]-��һ����ʱ�䣬[1]-�ڶ�����ʱ�䣬[2]-��������ʱ��--��ֱ����ʮ��
                                        //��ĳ����ʱ��ֵΪ0��˵������û��,2010-2019
#define  IEEP_DATA_ADDRESS  30			//EEPROM��Ҫ�洢���š���������ߺţ�Сʱ����Ԥ��30��λ��
#define  IEEP_RESULT_ADDRESS 360 		//������320�ĳ��ȣ����ϳ�ʼ��30��λ�ã�Ԥ��360

#define  SEGTIME  16 
#define  CtrAmount0 18 
#define  MasterAddress  20
#define  SlaveAddress   22
#define  RUNSTATE_SAVE_ADDRESS 24

#define AT24CXXX_PAGE_SIZE 64     //AT24C512ҳ��С128��512ҳ��AT24C256ҳ��С64��512ҳ��
#define AT24CXXX_PAGE_NUM   512
#define KAONUM_PAGE_NUM     50     //ÿ��50ҳ�洢��������50 * 64(ҳ��С)/16(ÿСʱ��������ֽ���) = 200,ÿ������200��Сʱ


#define  SET_FJ GPIO_SetBits(GPIOA,GPIO_Pin_0)
#define  CLR_FJ GPIO_ResetBits(GPIOA,GPIO_Pin_0)

volatile unsigned char rstflag[5];

volatile unsigned char ucMaxKaoNum;
volatile unsigned char ucMaxTotalHour;
volatile unsigned char u8JiaMeiJiCnt;
//����ߵ���
volatile unsigned char flgXunHuanGaoDiCnt;
volatile unsigned char flgXunHuanGaoDiCntPre;

volatile unsigned char flgCycleTimCnt;
//ʱ���йر���
volatile unsigned char  ucSecCnt;
volatile unsigned char  ucMinCnt;
volatile unsigned int	i16CoalMotorIntervalSecCnt = 0;

//unsigned int   uiHourCnt                __attribute__ ((section (".noinit")));

volatile unsigned char  flgMinCnt;
volatile unsigned char  flgHourCnt;

volatile unsigned int   uiTotalRunHour;
volatile unsigned int   uiTotalQueryHour;         //__attribute__ ((section (".noinit")));

volatile unsigned int   ucSegCntTime;
volatile unsigned int   uiSegLastTime;  //��ʣ��ʱ��
volatile unsigned char  ucSixMinCnt;    //6���Ӽ���

volatile unsigned int uiDryTempVal;
volatile unsigned int uiWetTempVal;

volatile unsigned int uiAuxDryTempVal;    //�����������ɡ�ʪ��        
volatile unsigned int uiAuxWetTempVal;         

//Ŀ���¶ȼ������йر���
volatile unsigned int uiTargetDryTemp;
volatile unsigned int uiTargetWetTemp;

volatile unsigned int uiRiseDryTempRate;  
volatile unsigned int uiRiseWetTempRate;


volatile unsigned char enWorkMode;
volatile unsigned char ucBakWorkMode;
volatile unsigned char ucModeSwiTime;
volatile unsigned char ucBakWorkModeDebug;
volatile unsigned char ProtectCurrentMax;
volatile unsigned char ProtectCurrentError;
volatile unsigned int ProtectVoltageMax = 270;
volatile unsigned int ProtectVoltageMin = 160;
unsigned int ProtectVoltageMaxTmp;
volatile unsigned int uiAcVolVal;
volatile unsigned int uiAcVolValPre;
volatile unsigned int uiAcCurVal_A;
volatile unsigned int uiAcCurVal_APre;
volatile unsigned int uiAcCurVal_B;
volatile unsigned int uiAcCurVal_BPre;
volatile unsigned int uiAcCurVal_C;
volatile unsigned int uiAcCurVal_CPre;
volatile unsigned int uiAcVolValTemp;
volatile unsigned int u16CurrUpdateFlag = 0;

volatile unsigned int ucProtCurVal;
volatile unsigned int ucProtCurValErr;

volatile unsigned int uiFengmenCtrMod;

volatile unsigned int FengMenGufengCtr; 
volatile unsigned int FengMenGufengCtr1; 
//���������ʵ����ʪ��ֵ
unsigned int   uiTempBuff[4];
unsigned int   uiTempBuff_filter[4];
unsigned int   uiTempSensorErrFlagPre = 0;
unsigned char  uDryTmepUpFlag = 0;
unsigned char  uWetTmepUpFlag = 0;;

volatile unsigned char ucCurveNum;
volatile unsigned char ucCurveNumSet;

volatile unsigned char ucRealRunSegNum;   //ȡֵ��0-19
volatile unsigned char ucCurvRunSegNum;   //ȡֵ��0-9

volatile unsigned char ucSelfCurvRunSegMaxNumSet;   //ȡֵ��1-9 
volatile unsigned char ucSelfCurvRunSegMaxNum;   //ȡֵ��1-9   
volatile unsigned char flgSelfSetPause;   //����ģʽ����ͣ��־

volatile unsigned char   flgFlashShow;//ָʾ��һλ��˸
volatile unsigned char   ucSetupClassIndex;

volatile unsigned char   flgMainSensor;  //0��������Ϊ����������1��������Ϊ��������
volatile unsigned char   flgMainSensorSet;  //0��������Ϊ����������1��������Ϊ��������

volatile unsigned char   flgPianWen;
volatile unsigned char   flgPwrLackPhase;

volatile unsigned char   flgVolProt;  //��ѹ������(��ѹ��Ƿѹ)������־
volatile unsigned char   flgOverCurProt;  //����������־
volatile unsigned char   ucOverCurCnt1;  //����������־
volatile unsigned char   flgFengjiLackPh;  //���ȱ�ౣ����־
volatile unsigned char   ucOverCurCnt;
volatile unsigned char   ucFengjiLackPhCnt;   //4�����һ��
volatile unsigned char   flgFengjiNoCur;   //����޵�����־
volatile unsigned char   flgFengjiCurJudgeWait;   //����޵�����־

volatile unsigned char   flgDryHigh; 
volatile unsigned char   flgDryLow; 
volatile unsigned char   flgWetHigh; 
volatile unsigned char   flgWetLow; 

volatile unsigned char   flgTmpDispGist;      //���ڱ�־��ʪ����ʾ�����ݣ�0: ��ʾ���ﴫ����ֵ;1����ʾ���ﴫ����ֵ
volatile unsigned char   ucTmpDispGistCnt;    //������ʾ�л�ʱ��

volatile unsigned char   flgUpDownFlashShow;  //���ƴ�������ʾ�л�ʱ�ϡ�������˸

unsigned int  WetTempValCtrT,WetTempValCtr,WetBulbSuperscale;    //��Ƶ��ʪ���¶�Ŀ��ֵ���൱��LCD��ʾ��ʪ���¶�ֵ
unsigned int  FengmenAnglePre,FengmenAngleN; 

volatile unsigned char   flgDispBrush;       //����ˢ��LCD
volatile unsigned char   ucCurveDispBuff[4];      

//������־
volatile unsigned char   flgOverVol,flgUnderVol;        
volatile unsigned char flgSysCtrFlag; 
volatile unsigned char flgSysCtrCnt; 
//��ʾ�й�
unsigned int    uiDispBuff[8];          

extern struct SEGMENT   WorkSeg[10];
extern struct SEGMENT   WorkSegSet[10];
extern struct SEGMENT   WorkSegDispBuff[10];

volatile unsigned char   ucRTCDispBuff[5];
volatile unsigned char   ucSegTimRimDispBuff[20];  //0-19,rim,���
volatile unsigned char   ucDryTmpRimDispBuff[10];
volatile unsigned char   ucWetTmpRimDispBuff[10];
volatile unsigned char   ucDot5DispBuff[10];
volatile unsigned char   ucOtherDispBuff[10];    //0:����;1:����;2:ƫ��;3:ȱ��;4:����;5:��ȼ;6:��ʪ;7:(ѭ������)�Զ�;8:(ѭ������)��;9:(ѭ������)��;
volatile unsigned char   ucUpDownDispBuff[2];    //�ϡ�������ʾ����

int   ucDateTime[5];  
int   ucDateTimeSet[5];
unsigned int uiQueryData[4];

volatile unsigned int uiCnt10ms;
volatile unsigned int uiCnt1000ms;
volatile unsigned int uiCnt700ms =0;

//IIC����������ӵı���
unsigned int IICaddr,IICdatt,IICk;

unsigned int u16TimeMinFlag = 1;
unsigned int u16Over15CloseCnt = 0;

//���¿����йر���
volatile unsigned int uiMostDryTmp;
volatile unsigned int uiLastDryTmp;
volatile unsigned int uiPreDryTmp;  

//ʪ�¿����йر���
volatile unsigned int  uiMostWetTmp;
volatile unsigned int  uiLastWetTmp;
volatile unsigned int  uiPreWetTmp;

volatile unsigned char   flgFanOn;     //�ķ��ͨ�ϱ�־
volatile unsigned char   flgCoalFanOn;     //�ķ��ͨ�ϱ�־

volatile unsigned char   flgFanOnPre;     //�ķ��ͨ�ϱ�־
volatile unsigned char   flgXunOn;             //��־ѭ��������ƹܽ�״̬    
//��ʾѭ���������״̬��0��ͣ����1�����٣�2�����٣� 
volatile unsigned char   flgXunHuanGaoDi;  
volatile unsigned char   flgXunHuanGaoDiPre;  
volatile unsigned char   flgXunHuanGaoDiPreDelay = 30;  

volatile int uiTemp_filter_value;//�¶��˲���

volatile unsigned int  uiFmNonCnt; 
//��־�������¶ȳ�����Χʱ���Ų�������ʱ
volatile unsigned char flgFmNonCnt;
volatile unsigned char flgWindRun;   //��־��������������; 
volatile unsigned char flgCoalMotorRun;   //��־��������������; 

volatile unsigned char flgWindStatusCheck; //�����������������һ�ν����Լ�
volatile unsigned char flgSystemStatus;    //ϵͳ����״̬��

volatile unsigned int  uiPreMenTmpDot;  //��һ�η��Ŷ�����

volatile unsigned int  uiWindCnt;  //���ŵ�����м�ʱ

volatile unsigned char   ucKaofangNum;      //����
volatile unsigned char   ucKaofangNumQuery;


unsigned char  OvenOverFlag;
unsigned char  FengmenCtrMod;
unsigned char  FengmenCtrModPre;


volatile unsigned char vdVoiceAlarmCnt;

volatile unsigned char FENGJIRUNFLAG;// �����������У���ôRUNFLAG =1;
//����״̬

volatile unsigned char FlagPowerStatus;     //ϵͳ����״̬�ź�
volatile unsigned char FlagPowerStatusPre;     //ϵͳ����״̬�ź�
volatile unsigned char FlagPowerStatusCnt;  //��������˲��ź� 

volatile unsigned char FengjiProtFlag;      //�������״̬
volatile unsigned char SySFirstRunFlag;

volatile unsigned char OvenStartFlag;
volatile unsigned char FengJiRunTimeFlag;

volatile unsigned char DS18B20Flag;       //DS18B20��ȡ�ɹ���־
volatile unsigned char DS18B20FlagErrCnt; //DS18B20���ϴ���
volatile unsigned char DS18B20_Timer_Hour; //DS18B20���ϴ���


struct SEGMENT                   //�ṹ��
	{
	    unsigned int  ucSetRisTim;    //����ʱ��
	    unsigned int  uiSetDryTmp;    //���ø���
		unsigned int  uiSetWetTmp;    //����ʪ��
      	unsigned int  ucSetStaTim;    //����ʱ��
	};  

typedef struct tagFrameReq 
{
	u8 Serial;
	u8 Ftype;
	u8 Room;
	u8 check;	
}Frame_Req_S; //����������
	
typedef struct tagFrameRep 
{
	u8 Serial;
	u8 Ftype;
	u8 Room;
	u16 DryTmp;
	u16 WetTmp;
	u16 DryTmpAim;
	u16 WetTmpAim;
	u16 StageTime;
	u8 TotalTime;
	u8 OutputVol;
	u8 Check;	
}Frame_Rep_S; //�㱨����

Frame_Rep_S rep;

/*����Щ�жϣ������ϵͳ�����������иı䣬��������ѭ������״̬����ָʾΪvolatile */

volatile unsigned char ucKeyVal,flgKeyOk; //����
volatile unsigned char ucLcdBkledCnt;


//������ѹADC
volatile unsigned char flgACADC;  //������������ADC
volatile unsigned char flgRdDandT; //������ȡ���ں�ʱ��

volatile unsigned char flgRdTmp;   //������������
volatile unsigned char flgSensorErr,flgSensorWater;


volatile unsigned char ucPlayAddr,ucAlarmTimCnt;
volatile unsigned char flgPwrLackPhaseCnt;
volatile unsigned char flgFengjiLackPhCnt;
volatile unsigned char flgOverCurProtCnt;
volatile unsigned char flgOverVolCnt;
volatile unsigned char flgUnderVolCnt;
volatile unsigned char flgFengjiNoCurCnt;
volatile unsigned char flgSensorErrCnt;	
volatile unsigned char flgSensorWaterCnt;
volatile unsigned char flgDryHighCnt;
volatile unsigned char flgDryLowCnt;
volatile unsigned char flgWetHighCnt;
volatile unsigned char flgWetLowCnt;
volatile unsigned char RunStopState; //0 ��ʾ����ֹͣ״̬��1��ʾ��������״̬

volatile unsigned char over_cnt  = 0;    //4�����һ��
volatile unsigned char over_cnt1  = 0;   //4�����һ��
volatile unsigned char over_cnt2  = 0;    //4�����һ��
volatile unsigned char over_cnt3  = 0;   //4�����һ��
volatile unsigned char alarm_cnt  = 0;
volatile unsigned char PhaseLackCnt = 0; //ȱ�ౣ������
volatile unsigned char AlarmLedCnt = 0; //ȱ�ౣ������
//ע�ͣ�TotalHourNow ��ʾ��ǰ�������е���ʱ�䣬��������ʱ�����50Сʱ����ô��Ϊ��һ�������Ŀ������ڣ�
//ע��:TotalHourWrFlag��ʾ��ǰ�����Ƿ����Ӧ�Ĵ洢�ռ���й�+1������������й�+1���������ٽ���
unsigned char TotalHourNow = 0;
unsigned char TotalHourWrFlag = 0;
unsigned int PassWordCnt=0;
unsigned char PassFlag1;
unsigned char FlagRunMode;
unsigned char GufengjiOpenTest;
unsigned int u16CoalMotorCntrTable[3][4]=
{
{240,0,120,180},{300,0,180,300},{240,0,120,180}
};	

volatile unsigned int u16CoalMotorOverLoadFlag; 
volatile unsigned int u16CoalMotorSecCnt;  //���ŵ�����м�ʱ
volatile unsigned int u16CoalMotorSecCntPre;  //���ŵ�����м�ʱ
unsigned int i16GeneralAmount = 0;
unsigned int i16CompensateAmount = 0;
unsigned int i16DryTempScale = 0;
unsigned int i16DryTempErrScale = 0;
unsigned int i16CtrAmount0 = 0;
unsigned int i16CtrAmount0Set = 0;

unsigned int u16MasterAddress = 1;
unsigned int u16MasterAddressSet = 1;

unsigned int u16SlaveAddress = 1;
unsigned int u16SlaveAddressSet = 1;

volatile unsigned int i16CtrAmount3 = 8;
volatile unsigned int i16CtrAmount3Pre = 8;
//unsigned int i16CoalMotorMinCnt = 0;
unsigned int u16CoalMotorOverLoad;
unsigned int u16CoalMotorStopFlag = 1;


//SX1278
u8 mode;//lora--1/FSK--0
u8 Freq_Sel;//
u8 Power_Sel;//
u8 Lora_Rate_Sel;//
u8 BandWide_Sel;//
u8 rf_rx_packet_length;
u8 Fsk_Rate_Sel;//
u8 Fsk_Rate_Sel;//
u8 u8Sx1278SendFlag;
u8  sx1276_7_8SlaveAddress = 0x01;
u8 Frame_send_report[16] ;

unsigned char TemperSensorErrCnt = 0;
unsigned char TemperSensorErrFlag = 0;
unsigned int u16Sx1278FrameTemp = 0;
unsigned int u16Sx1278FrameCrcCheck = 0;


unsigned char ucReadKey(void);
unsigned char uiACProc(void);
extern unsigned int u16MotorCnt;
extern unsigned int u16CoalMotorIntevalCnt;
extern unsigned int LCDLED_RST;

void vdFillDispBuff(void);
void vdDeciToBcd(void);

void vdKeyProc(unsigned char keyval);  

void vdSerRKey(void);
void vdSerLKey(void);
void vdPlusKey(void);
void vdMinusKey(void);

void vdFengMenCntl(void);    //���ſ���
void vdGuoFengJiCntl(void);  //�ķ������
void vdDaFengJiCntl(void);   //ѭ���������

void vdReadDatFrEep(unsigned char quxianhao);
void vdSaveDatToEep(unsigned char quxianhao);

void vdSaveRunDatExEep(unsigned char kaohao,unsigned char uchour);
void vdReadRunDatFrExEep(unsigned char kaohao,unsigned char uchour);

void vdCalDryRate(void);
void vdCalWetRate(void);
void vdRenewTarDat(void);

void vdVolProc(void);
void vdCurProc(void);
void vdDryAlarm(void);
void vdWetAlarm(void);

void vdVoiceAlarm(void);
void vdChangeDatToOriginal(void);
void vdGetFengmenCtrModFlag(void);
void vdGetXunHuanGaodiFlag(void);
void vdSampCoalMotorOverload(void);
void AutoAddCoalMotorControlTimeChange(void);
/* Part 4: main() */
int main(void)
{
  unsigned char i,j;
 
  /*System Clocks Configuration */
  RCC_Configuration();

  /* NVIC Configuration */
  NVIC_Configuration();

  /* GPIO Configuration */
  GPIO_Configuration();

  /* SysTick end of count event each 1ms with input clock equal to 9MHz (HCLK/8, default) */
  SysTick_SetReload(9000);

  /*Enable SysTick interrupt */
  SysTick_ITConfig(ENABLE);

  /* Enable the SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Enable);
  
  SPI_Configuration();

  FENGJI_STOP;
  FAN_RUN;
  DS18B20Flag = 0;

  u16MotorCnt = 1;
  u16CoalMotorIntevalCnt = 120;
  flgCoalFanOn = 1;
  DS18B20_Timer_Hour = 0;
  LCDLED_RST = 0;

  IICaddr= CtrAmount0;
  WordFrom24C();
  _delay_ms(4);
  i16CtrAmount0 = IICdatt;
  if((i16CtrAmount0 >9)||(i16CtrAmount0 <1))
	 i16CtrAmount0  = 2;
  i16CtrAmount0Set = i16CtrAmount0;
  IICaddr= MasterAddress;
  WordFrom24C();
  _delay_ms(5);
  u16MasterAddress = (IICdatt)&0x3FF;
  if(u16MasterAddress > 999)
	u16MasterAddress = 1;
  u16MasterAddressSet = u16MasterAddress;
  IICaddr= SlaveAddress;
  WordFrom24C();
  _delay_ms(5);
  u16SlaveAddress = (IICdatt)&0x0F;
  if(u16SlaveAddress > 12)
	u16SlaveAddress = 1;
  u16SlaveAddressSet = u16SlaveAddress;

  //JIAMEIJI_FORWARD;
  JIAMEIJI_STOP;
	
  flgCoalMotorRun = 1;
  u16CoalMotorSecCnt = 60;
  vdInitLCD();
  RTCInit(); 
  ADCInit();
  for(i=0;i<4;i++)
	  uiTempBuff[i] = 0;
  for(i=0;i<20;i++)
  {
    (void)(ucStartRdTmp(uiTempBuff));
  }

  u16CoalMotorOverLoadFlag = 0;  

/*
  uiTotalRunHour = 160;
  ucRealRunSegNum = 18;
  IICaddr= RUNSTATE_SAVE_ADDRESS;//��ʼ��λ�洢�����š�����������ߺ�
  IICdatt=((ucRealRunSegNum)&0x7f)+(RunStopState<<7);
  IICdatt = IICdatt << 8;
  IICdatt = IICdatt + (uiTotalRunHour&0xff);
  WordTo24C();
  _delay_ms(5);
*/
  //�ж��Ƿ����ϵ縴λ
  for(j=0; j<5; j++)
  {
      if( rstflag[j] != j )
	       break;
  }
  if(j<5)
  {
	    //�ϵ縴λ,������
        for(j=0;j<5;j++)
		    rstflag[j]=j;
		//���Ź���
		IICaddr=0x0000;
		uiCnt1000ms=0;
		PhaseLackCnt = 0;
		FengjiProtFlag = 0;
		u8JiaMeiJiCnt = 0;
		WordFrom24C();
		ucKaofangNum =(unsigned char)(IICdatt>>8);
		if(ucKaofangNum >= 9)
		    ucKaofangNum = 0;
		ucCurveNum =(unsigned char)(IICdatt&0x0f);//��Ϊ��������
		if(ucCurveNum > 3)  //�ܹ��������ߣ���0��ʼ
            ucCurveNum = 1;
		flgMainSensor = (IICdatt&0xf0)>>4;//��Ϊ��������
		if(flgMainSensor > 1)  //ֻ����·����������0��ʼ
		    flgMainSensor = DWSENSOR;
        flgTmpDispGist = flgMainSensor;

        flgFlashShow      = 0;
        ucSetupClassIndex = 0;

    	flgSelfSetPause   = 0;
		ucSelfCurvRunSegMaxNum = 0;
		ucSelfCurvRunSegMaxNumSet = 0;

		//ϵͳ״̬
		FlagPowerStatus = 1;
		FlagPowerStatusPre = FlagPowerStatus;
		FlagPowerStatusCnt = 0; 
		for(i=0; i<10; i++)
		{
	    if(1== GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_10))
			FlagPowerStatusCnt++;
		 else
		 	FlagPowerStatusCnt = 0;
		}
		if(FlagPowerStatusCnt > 5 )
		   FlagPowerStatus = 1;
		else if(FlagPowerStatusCnt<3)
		   FlagPowerStatus = 0;


        PassFlag1 = 0;
		/*��ʼ����������*/
		vdReadDatFrEep(ucCurveNum);  //���ݶ�ȡ���������ݻ�����
		for(i=0; i<10; i++)
	    {
	        WorkSeg[i].ucSetRisTim = WorkSegSet[i].ucSetRisTim;
	        WorkSeg[i].uiSetDryTmp = WorkSegSet[i].uiSetDryTmp;
            WorkSeg[i].uiSetWetTmp = WorkSegSet[i].uiSetWetTmp;
		    WorkSeg[i].ucSetStaTim = WorkSegSet[i].ucSetStaTim;
        }
		if(PassFlag1==1)
		{
          for(i=0; i<10; i++)
	      {
	        WorkSeg[i].ucSetRisTim = 0;
			WorkSegSet[i].ucSetRisTim = 0;
	        WorkSeg[i].uiSetDryTmp = 0;
			WorkSegSet[i].uiSetDryTmp = 0;
            WorkSeg[i].uiSetWetTmp = 0;
			WorkSegSet[i].uiSetWetTmp = 0;
		    WorkSeg[i].ucSetStaTim = 0;
			WorkSegSet[i].ucSetStaTim = 0;
          }
		}
        if(1 == FlagPowerStatus)
	  	{
	  	  VoicePlay(WELCOME);   //������ӭ��
	  	  SET_LCDLED;
        }
		ucModeSwiTime = 0;

        flgMinCnt   = 0;
        flgHourCnt  = 0;
		ucSecCnt    = 0;
		ucMinCnt    = 0;
		uiTotalRunHour = 0;
		ucSegCntTime   = 0;
		ucSixMinCnt    = 0;

		flgKeyOk = FALSE;
	    ucKeyVal = KEY_NULL;
		over_cnt = 0;
		over_cnt1= 0;
		over_cnt2= 0;
		over_cnt3= 0;

        flgFanOn        = 0x00;
		flgFanOnPre     = 0x00;
		flgXunOn        = 0x00;
        flgXunHuanGaoDi = 0x00;
		flgXunHuanGaoDiCnt=0;
		flgWindRun      = 0x00;
		flgPianWen      = 0x00;

		uiAcVolVal = 0;
        uiAcVolValPre= 0;
        uiAcCurVal_A=0;
        uiAcCurVal_APre=0;
        uiAcCurVal_B=0;
        uiAcCurVal_BPre=0;
        uiAcCurVal_C=0;
        uiAcCurVal_CPre=0;
		alarm_cnt = 0;
		
		/*�ⲿeeprom�й�*/
		ucProtCurVal     = 400; 
		flgVolProt        =  0x00; 
		flgOverCurProt    =  0x00; 
		flgFengjiLackPh   =  0x00;
		ucOverCurCnt      =  0x00;
		ucFengjiLackPhCnt =  5;
		flgFengjiNoCur    =  0x00;

		flgDryHigh        =  0x00;
		flgDryLow         =  0x00;
		flgWetHigh        =  0x00;
	    flgWetLow         =  0x00;

		uiTargetDryTemp = 0;
		uiTargetWetTemp = 0;

		flgPwrLackPhaseCnt = 0;
        flgFengjiLackPhCnt = 0;
		flgOverCurProtCnt = 0;
		flgOverVolCnt = 0;
		flgUnderVolCnt = 0;
		flgFengjiNoCurCnt = 0;
		flgSensorErrCnt = 0;	
		flgSensorWaterCnt = 0;
		flgDryHighCnt = 0;
		flgDryLowCnt = 0;
		flgWetHighCnt = 0;
		flgWetLowCnt = 0;

		vdVoiceAlarmCnt = 0;
		TotalHourNow = 0;
		TotalHourWrFlag = 0;

		FengJiRunTimeFlag = 1;

		FengMenGufengCtr = 120;
		FengMenGufengCtr1 = 120;
		flgSysCtrCnt = 1;

		if(flgTmpDispGist == DWSENSOR)
	    {	
		uiDryTempVal =	uiTempBuff[DNDRY]; //��������
		uiWetTempVal =	uiTempBuff[DNWET];
	    uiDispBuff[0] = uiTempBuff[DNDRY];		 //��ʾ��������
		uiDispBuff[1] = uiTempBuff[DNWET];
        }
	    if(flgTmpDispGist == UPSENSOR)
	    {	
	
    	uiDryTempVal =	uiTempBuff[UPDRY]; //��������
	    uiWetTempVal =	uiTempBuff[UPWET];
		uiDispBuff[0] = uiTempBuff[UPDRY];		 //��ʾ��������
		uiDispBuff[1] = uiTempBuff[UPWET];
        }
		flgRdTmp  = TRUE;              //�����¶�ת��
		flgDispBrush = TRUE;
        ucLcdBkledCnt = 30;
		u16CoalMotorStopFlag = 1;

		//��ʾȫ��������ʱ500ms;
        lcdwc(CS_1,CMDLCDON);
        lcdwc(CS_2,CMDLCDON);
		lcdwc(CS_3,CMDLCDON); 
		lcdwc(CS_4,CMDLCDON);
		lcdwc(CS_5,CMDLCDON);
		lcdwc(CS_6,CMDLCDON);
		lcdwc(CS_7,CMDLCDON);
		lcdwc(CS_8,CMDLCDON);
        for(i=0;i<10;i++)
		    _delay_ms(50);
		OvenOverFlag = 0;
	}
	else    //������
	{
		if(enWorkMode  == RUN_MODE)
            SET_RUNLED;
        else if(enWorkMode  == SET_MODE)
		    SET_SETLED;
        
		if(flgXunOn)
		{
		    FENGJI_RUN;
			flgXunOn = 0x01;
		    FENGJIRUNFLAG = 1;
		}

		if((flgFanOn)&&(FENGJIRUNFLAG==1))
		{
		    FAN_RUN;
            flgFanOn = 0x01;
		}
	}
	//����ʱ�ȹط���
	WIND_CLOS;
	flgWindRun = 0x01; //��־���ŵ��������ת
	uiWindCnt  = 700;//FengmenAngleN+10;   //���ŵ�ǰ��λ�ö��10��֤��ȫ�ر� 
	uiFmNonCnt = 0;
    FengmenAngleN=0;
    FengmenAnglePre = 0;
	
	flgXunHuanGaoDi=0x00;
	FengjiProtFlag = 0;
	PhaseLackCnt = 0;
	
	for(i=0; i<8; i++)
	{
	    uiDispBuff[i] = 0;
	}
	RdRTC(ucDateTime);

	OvenStartFlag = 0;
    
	IICaddr=RUNSTATE_SAVE_ADDRESS;
	WordFrom24C();
	uiTotalRunHour = IICdatt&(0xff);
	RunStopState = (IICdatt>>15)&0x01;
	if(uiTotalRunHour>200)
	   uiTotalRunHour = 0;
	ucRealRunSegNum = ((IICdatt>>8)&0x7f);
	if(ucRealRunSegNum>19)
	   ucRealRunSegNum = 0;
	if(ucRealRunSegNum % 2 ==0)
	    ucCurvRunSegNum = ucRealRunSegNum / 2;
	else
	    ucCurvRunSegNum = (ucRealRunSegNum-1)/2;

	RunStopState = (IICdatt>>15)&0x01;
	
	if(RunStopState>1)
	   RunStopState = 0;
	
	IICaddr= SEGTIME;
	WordFrom24C();
	_delay_ms(3);
	IICdatt = IICdatt&(0xff);
	if(IICdatt<40)
	ucSegCntTime = IICdatt;
 	if((ucRealRunSegNum%2!=0)&&(ucSegCntTime<WorkSeg[ucCurvRunSegNum].ucSetRisTim))
	{
		ucSegCntTime = WorkSeg[ucCurvRunSegNum].ucSetRisTim;
		IICaddr = SEGTIME;
		IICdatt =((ucSegCntTime)&0xFF);
		WordTo24C();
		_delay_ms(4);
	}

    if( RunStopState == 1)
    {
		 enWorkMode = RUN_MODE;
		 ucBakWorkMode =STOP_MODE;
		 if(1 == FlagPowerStatus)
		 {
		    SET_RUNLED;
		    VoicePlay(START_RUN);
		 }
		 RunStopState = 1;
		 ucSecCnt = 0;
		 ucMinCnt = 0;
		 ucSixMinCnt    = 0;
		 flgMinCnt      = 0x00;
		 flgHourCnt     = 0x00;
				
		 if(ucCurvRunSegNum >= 1)
		 {
			 uiTargetDryTemp = WorkSeg[ucCurvRunSegNum -1].uiSetDryTmp *10;
    		 uiTargetWetTemp = WorkSeg[ucCurvRunSegNum -1].uiSetWetTmp *10; 				
		  }
		 vdCalDryRate();
		 vdCalWetRate();

		 flgXunOn = 0x01;
		 FENGJI_RUN;
		 flgFanOn =0x01;
		 FENGJIRUNFLAG =1;
        }
	    else
	    {
		   enWorkMode = STOP_MODE;
		   FlagRunMode = 0;
		}
		GufengjiOpenTest = 5;
    flgSysCtrFlag = 0;

	IICaddr= CtrAmount0;
	WordFrom24C();
	_delay_ms(5);
    i16CtrAmount0 = IICdatt;
	if((i16CtrAmount0 >9)||(i16CtrAmount0 <1))
	   i16CtrAmount0  = 2;
	i16CtrAmount0Set = i16CtrAmount0;
    ucProtCurVal = 400;
	ucProtCurValErr = 180;
	ProtectVoltageMax = 270;
	ProtectVoltageMin = 160;

	i16CoalMotorIntervalSecCnt = 0;

//SX1278
	Init1278(u16MasterAddress);
	sx1276_7_8_LoRaEntryRx();
	_delay_ms(500);
	
	
  while(1)
  {
      if((1 == u8Sx1278SendFlag)&&(TRUE!=flgRdTmp))
	  {
			 rep.Serial = u16MasterAddress;
			 rep.Ftype = 0x01;
			 rep.Room = (u16SlaveAddress & 0x1F)|(RunStopState<<7)|(ucCurveNum<<5);
			 if(1 == flgFengjiNoCur)
			 	u16Sx1278FrameTemp = 0;
			 else
			 	u16Sx1278FrameTemp = 0x4000;
			 if(flgFengjiLackPh)
				 u16Sx1278FrameTemp = 0x8000;
			 if(flgOverCurProt)
				 u16Sx1278FrameTemp = 0xC000;
			 rep.DryTmp =(uiDryTempVal & 0x3FF)|u16Sx1278FrameTemp;

			 if(0 == flgFanOn)
			 	u16Sx1278FrameTemp = 0;
			 else
			 	u16Sx1278FrameTemp = 0x4000;

			 rep.WetTmp = (uiWetTempVal & 0x3FF)|u16Sx1278FrameTemp;
			 rep.DryTmpAim = ((uiTargetDryTemp/10) & 0x3FF);
			 rep.WetTmpAim = ((uiTargetWetTemp/10) & 0x3FF);
			 rep.StageTime = ((ucRealRunSegNum&0x3F)<<10)|((uiDispBuff[4])&0x3FF);
			 rep.TotalTime = uiTotalRunHour;
			 if((uiAcVolVal>=100)&&(uiAcVolVal<350))  //�ϴ�����offset
			    rep.OutputVol = uiAcVolVal-100;
			 else
			 	rep.OutputVol = 0;
			 rep.Check =(u8)( ~(unsigned int)((rep.Serial+rep.Ftype+rep.Room+(u8)rep.DryTmp+(u8)(rep.DryTmp>>8)
			 +(u8)rep.WetTmp+(u8)(rep.WetTmp>>8)+(u8)rep.DryTmpAim+(u8)(rep.DryTmpAim>>8)
			 +(u8)rep.WetTmpAim+(u8)(rep.WetTmpAim>>8)+(u8)rep.StageTime+(u8)(rep.StageTime>>8)
			 +rep.TotalTime+rep.OutputVol)&0xff)+1);
			 Frame_send_report[0] = rep.Serial;
			 Frame_send_report[1] = rep.Ftype;
			 Frame_send_report[2] = rep.Room;
			 Frame_send_report[3] = (u8)(rep.DryTmp>>8);
			 Frame_send_report[4] = (u8)rep.DryTmp;
			 Frame_send_report[5] = (u8)(rep.WetTmp>>8);
			 Frame_send_report[6] = (u8)rep.WetTmp;
			 Frame_send_report[7] = (u8)(rep.DryTmpAim>>8);
			 Frame_send_report[8] = (u8)(rep.DryTmpAim);
			 Frame_send_report[9] = (u8)(rep.WetTmpAim>>8);
			 Frame_send_report[10] = (u8)rep.WetTmpAim;
			 Frame_send_report[11] = (u8)(rep.StageTime>>8);
			 Frame_send_report[12] = (u8)rep.StageTime;
			 Frame_send_report[13] = (u8)rep.TotalTime;
			 Frame_send_report[14] = rep.OutputVol;
			 u16Sx1278FrameCrcCheck = 
			 	Frame_send_report[0]+Frame_send_report[1]+Frame_send_report[2]
			   +Frame_send_report[3]+Frame_send_report[4]+Frame_send_report[5]
			   +Frame_send_report[6]+Frame_send_report[7]+Frame_send_report[8]
			   +Frame_send_report[9]+Frame_send_report[10]+Frame_send_report[11]
			   +Frame_send_report[12]+Frame_send_report[13]+Frame_send_report[14];
			 Frame_send_report[15] = (u8)(u16Sx1278FrameCrcCheck&0xFF);
			 Frame_send_report[15] = (~Frame_send_report[15])+1;
			 //����Frame_Rep_S rep ��ֵ
			 SendRF1278((u8 *)Frame_send_report,16);
		     u8Sx1278SendFlag = 0;
    }
	  if(flgKeyOk ==TRUE)            //��������
	  {
	      if(1 == FlagPowerStatus)
		  {    
			    SET_LCDLED;
		  }
			ucLcdBkledCnt = 30;
			ucModeSwiTime = 20;
			vdKeyProc(ucKeyVal);
			ucKeyVal = KEY_NULL;
			flgKeyOk = FALSE;
    }	    
	  if(flgRdDandT == TRUE)   //��ȡ���ں�ʱ��
	  {
		   flgRdDandT = FALSE;
		   RdRTC(ucDateTime);
	  }
	  if((flgDispBrush == TRUE)&&(1 != u8Sx1278SendFlag))    
	  {
		  vdFillDispBuff();
		  vdDispBrush();
		  flgDispBrush = FALSE;
     }
     if((flgRdTmp == TRUE)&&(0 == LCDLED_RST))
	 {
         if(!ucStartRdTmp(uiTempBuff))
		 {
		      if(flgSensorErr == 0)
			  {
				    if(!flgSensorErr)
					{
					    flgSensorErr = 0x01;
						VoicePlay(SENSOR_ERR);
				    }
					uiDryTempVal =  uiTempBuff[DNDRY]; //��������
					uiWetTempVal =  uiTempBuff[DNWET];
					uiAuxDryTempVal =  uiTempBuff[UPDRY]; //��������
					uiAuxWetTempVal =  uiTempBuff[UPWET];
					uiTempBuff_filter[DNDRY] = uiTempBuff[DNDRY];
					uiTempBuff_filter[DNWET] = uiTempBuff[DNWET];
					uiTempBuff_filter[UPDRY] = uiTempBuff[UPDRY];
					uiTempBuff_filter[UPWET] = uiTempBuff[UPWET];
               }
		  }
		  else
		  {
			    flgSensorErr = 0x00;
				if(flgMainSensor == DWSENSOR) //��������ʽ����
				{				
					uiTemp_filter_value = uiTempBuff_filter[DNDRY]+uiTempBuff_filter[DNWET]+uiTempBuff_filter[UPDRY]+uiTempBuff_filter[UPWET]-uiTempBuff[DNDRY]-uiTempBuff[DNWET]-uiTempBuff[UPDRY]-uiTempBuff[UPWET];
					if(uiTemp_filter_value<80&&uiTemp_filter_value>-80)
					{
		            	uiDryTempVal =  uiTempBuff[DNDRY]; //��������
		            	uiWetTempVal =  uiTempBuff[DNWET];
	                
						uiAuxDryTempVal =  uiTempBuff[UPDRY]; //��������
			            uiAuxWetTempVal =  uiTempBuff[UPWET];

						WetTempValCtr = uiWetTempVal;	  //DC����LXH����ģ�δ��֤
					}
					//�¶��˲�
					uiTempBuff_filter[DNDRY] = uiTempBuff[DNDRY];
					uiTempBuff_filter[DNWET] = uiTempBuff[DNWET];
					uiTempBuff_filter[UPDRY] = uiTempBuff[UPDRY];
					uiTempBuff_filter[UPWET] = uiTempBuff[UPWET];
		        }
				else if(flgMainSensor == UPSENSOR)   //�����½�ʽ����
				{	
				    uiTemp_filter_value = uiTempBuff_filter[DNDRY]+uiTempBuff_filter[DNWET]+uiTempBuff_filter[UPDRY]+uiTempBuff_filter[UPWET]-uiTempBuff[DNDRY]-uiTempBuff[DNWET]-uiTempBuff[UPDRY]-uiTempBuff[UPWET];
					if(uiTemp_filter_value<80&&uiTemp_filter_value>-80)
					{
					    uiDryTempVal =  uiTempBuff[UPDRY]; //��������
			            uiWetTempVal =  uiTempBuff[UPWET];
	                
						uiAuxDryTempVal =  uiTempBuff[DNDRY]; //��������
			            uiAuxWetTempVal =  uiTempBuff[DNWET];

						WetTempValCtr = uiWetTempVal;
					}
					uiTempBuff_filter[DNDRY] = uiTempBuff[DNDRY];
					uiTempBuff_filter[DNWET] = uiTempBuff[DNWET];
					uiTempBuff_filter[UPDRY] = uiTempBuff[UPDRY];
					uiTempBuff_filter[UPWET] = uiTempBuff[UPWET];
				}
		  }
			//���Ʊ��� 
		  if(uiTempBuff[DNDRY]>40&&uiTempBuff[DNWET]>40&&uiTempBuff[UPDRY]>40&&uiTempBuff[UPWET]>40)
		  {
				TemperSensorErrFlag = 0;
				DS18B20Flag = 1;
		  }
		  else
		  {
                if((++TemperSensorErrCnt) > 3)
				{
				   TemperSensorErrCnt = 0;
				   if(!TemperSensorErrFlag)
				   {
					   TemperSensorErrFlag = 1;
					   VoicePlay(SENSOR_ERR);
				   }
				}
		  }

		if((1 == flgSensorErr)||(1 == TemperSensorErrFlag))
		{
			if(DS18B20Flag)
			{
			   DS18B20FlagErrCnt++;
			   if(DS18B20FlagErrCnt <=3)
			   {
			      LCDLED_RST = 40;	

			   }
			   else
			   {
			      DS18B20FlagErrCnt = 4;
			   }
			}
		}
		if((850 == uiTempBuff[DNDRY])||(850 == uiTempBuff[DNWET])||(850 == uiTempBuff[UPDRY])||(850 == uiTempBuff[UPWET]))
		{
		   if(DS18B20Flag)
		   {
    		    DS18B20FlagErrCnt++;
    			if(DS18B20FlagErrCnt <=3)
    			{
    			   LCDLED_RST = 40;	
    			}
    			else
    			{
    			   DS18B20FlagErrCnt = 4;
    			}
			}
		}
		if((1 ==uiTempSensorErrFlagPre)&&(0 == TemperSensorErrFlag)&&(0 == flgSensorErr)&&(1 == FengmenCtrMod))
		{
			 WIND_CLOS;
			 flgWindRun = 0x01;  //��־���ŵ��������ת
			 uiWindCnt	= 700;	 //FengmenAngleN+10;   //���ŵ�ǰ��λ�ö��10��֤��ȫ�ر� 
			 uiFmNonCnt = 0;
			 FengmenAngleN=0;
		}
		uiTempSensorErrFlagPre = TemperSensorErrFlag|flgSensorErr;
	    for(i=0; i<4; i++)
		{
		    if(1== GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_10))
			   FlagPowerStatusCnt++;
		    else
		       FlagPowerStatusCnt = 0;
		}
		if( FlagPowerStatusCnt >= 2 )
		{
		      FlagPowerStatusPre = FlagPowerStatus;
		      FlagPowerStatus = 1;
		}
		else if(FlagPowerStatusCnt<2)
		{     
		      FlagPowerStatusPre = FlagPowerStatus;
		      FlagPowerStatus = 0;
		 }
		 flgRdTmp = FALSE;
	}
	if((flgACADC==TRUE)&&(1 == FlagPowerStatus))
    {   
    	    FengJiRunTimeFlag++;
    		if(GufengjiOpenTest>=10)
    			GufengjiOpenTest = 10;
    		GufengjiOpenTest++;
    		if(FengJiRunTimeFlag>=20)
    			FengJiRunTimeFlag = 20;
    	    if(uiACProc()&&(FengJiRunTimeFlag >=20))
    		{
    			vdVolProc();
    			vdCurProc();
    		}
    		else
        		flgPwrLackPhase = 0x00;
                    
    		if(enWorkMode == RUN_MODE)
    		{
    			vdDryAlarm();
    			vdWetAlarm();
            }
    		ucAlarmTimCnt++;
    		if(ucAlarmTimCnt > 60) //15*4 = 80s ��һ������
    		{
    			flgSensorWater = 0x00;    
    			ucAlarmTimCnt = 0;
    			if(( FlagPowerStatus == 1 )&&(enWorkMode == RUN_MODE ))
    			{
    			     vdVoiceAlarm();
    			}
    		}
    		if((flgOverCurProt == 1)||(flgFengjiLackPh == 1)||(flgVolProt ==1))
    		{
    		   AlarmLedCnt++;
    		   AlarmLedCnt = AlarmLedCnt % 2;
    		   if(AlarmLedCnt == 0)
    		   {
                  CLR_RUNLED;
    		   }
    		   else
    		   {
                  SET_RUNLED;
    		   }
            }
    		else
    		{
                if(enWorkMode  == RUN_MODE)
                    SET_RUNLED;
                else if(enWorkMode  == STOP_MODE)
    		        CLR_RUNLED;
    		}
    		flgACADC = FALSE;
    	}
		else if(0 == FlagPowerStatus)
		{
             uiAcVolVal = 0;
			 CLR_RUNLED;        //DTS201209081420,���е��Ϊ��ص�ʱ��RUNLED����
		}
		if((1 == FlagPowerStatus)&&(1== flgSysCtrFlag))
		{   
		    if(1 == flgSysCtrCnt)
			{
			   vdGetFengmenCtrModFlag();
			   flgSysCtrCnt = 0;
		    }
			else
			{
			   vdSampCoalMotorOverload();
			   flgSysCtrCnt = 1;
			}
			flgSysCtrFlag = 0;
		}
        if(1 == FlagPowerStatus)
		{
			vdGetXunHuanGaodiFlag();
        }
		if(1 == FlagPowerStatus)
		{    
			vdDaFengJiCntl();
		}
		
		switch(enWorkMode)
		{
		    case STOP_MODE:
			   flgXunOn = 0x00;
			   u8JiaMeiJiCnt = 0;
			   RunStopState = 0;
			   FENGJIRUNFLAG = 0 ;
			   flgWindStatusCheck = 0; //�����������������һ�ν����Լ�
               flgSystemStatus = 0;    //ϵͳ����״̬��
               RunStopState = 0;
			   FengjiProtFlag = 0;
			   flgFlashShow = 0;
			   FengJiRunTimeFlag = 1;
			   FlagRunMode = 0;
			   u16MotorCnt = 3;
			   u16CoalMotorStopFlag = 1;
			   if(GufengjiOpenTest>=10)
			   	{
			       FAN_STOP;                    //ֹͣ���
		           flgFanOn = 0x00;
				   flgCoalFanOn = 0x00;
				   GufengjiOpenTest = 10;
			   	}
			   i16CoalMotorIntervalSecCnt = 0;
			   if(flgHourCnt)          //Сʱʱ�䵽
			   {
				 flgHourCnt = 0x00;
	             vdInitLCD();
			   }
			   break;
           case RUN_MODE:
				 if((0 ==FlagPowerStatusPre)&&(1 == FlagPowerStatus)&&(1 == FengmenCtrMod)&&(flgWindRun == 0))
				 {
							WIND_CLOS;
							flgWindRun = 0x01;  //��־���ŵ��������ת
							uiWindCnt  = 700;   //FengmenAngleN+10;   //���ŵ�ǰ��λ�ö��10��֤��ȫ�ر� 
							uiFmNonCnt = 0;
							FengmenAngleN=0;
							FlagPowerStatusPre = 	FlagPowerStatus;
				 }
			   if(1 == u16CoalMotorStopFlag)				
			   {
					   u16MotorCnt = 3;										
                       u16CoalMotorStopFlag = 0;
			   }
				RunStopState = 1;
				flgFlashShow = 0;
				if((flgOverCurProt == 1)||(flgFengjiLackPh == 1)||(flgVolProt ==1))
				{
              FengjiProtFlag = 1;
        }
				if((flgSystemStatus == 0)&&(flgWindRun == 0))
				{
				    WIND_OPEN;
	          flgWindRun = 0x01; //��־���ŵ��������ת
	          uiWindCnt  = 700;  //FengmenAngleN+10;   //���ŵ�ǰ��λ�ö��10��֤��ȫ�ر� 
	          uiFmNonCnt = 0;
            FengmenAngleN=900;
					  flgWindStatusCheck = 2;
					  flgSystemStatus = 1;
				}
				if((flgWindStatusCheck == 2)&&(flgWindRun == 0))
				{
            WIND_CLOS;
	          flgWindRun = 0x01;  //��־���ŵ��������ת
	          uiWindCnt  = 700;   //FengmenAngleN+10;   //���ŵ�ǰ��λ�ö��10��֤��ȫ�ر� 
						uiFmNonCnt = 0;
            FengmenAngleN=0;
					  flgWindStatusCheck = 3;
				}
        if(flgMinCnt)    //����ʱ�䵽
				{
				   flgMinCnt    = 0x00;
				   if(ucMinCnt % 30 ==0x00)   //ÿ���Сʱ����Ŀ������
		                vdRenewTarDat();
				   if(FengjiProtFlag == 1)
				   {
                      PhaseLackCnt++;
                      if(PhaseLackCnt < 10)
					  {
						 FAN_STOP;  
						 FENGJI_STOP;
						 FENGJIRUNFLAG =0;
						 flgXunOn = 0x00;
						 flgFanOn = 0x00;
					  }
					  else
					  {
					    FengjiProtFlag = 0;
					    FengjiProtFlag = 0;
                        flgFengjiLackPh = 0; //���ȱ���־
						flgVolProt = 0;		//�����ѹ��־
						flgOverCurProt = 0;
						FENGJI_RUN;
						FAN_RUN;
						FENGJIRUNFLAG =1;
						flgXunOn = 0x01;
						flgFanOn = 0x01;
						PhaseLackCnt = 0;
					  }
				  }
				  if(ucMinCnt % 6 ==0x00)   //ÿ6���Ӹ���ʣ��ʱ��
				  {
					    ucSixMinCnt++;
					 if(ucSixMinCnt >= 10)//һ��Сʱ����
					{
						    ucSixMinCnt = 0;
                        	ucSegCntTime++;    //�����㣬�ܿأ��� uiTotalRunHour �����п�ʼ�������㣬���ܿ�
					}		  
				  } 
                }
				if(flgHourCnt)          //Сʱʱ�䵽
				{
				    flgHourCnt = 0x00;
					DS18B20_Timer_Hour++;
					if(DS18B20_Timer_Hour >= 4)
					{
					    DS18B20_Timer_Hour = 0;
						DS18B20FlagErrCnt = 0;
					}
	//				uiTotalRunHour++;
	//                vdInitLCD();
					if(OvenOverFlag ==0)
					{
    				     IICaddr= RUNSTATE_SAVE_ADDRESS;//��ʼ��λ�洢�����š�����������ߺ�
 	 				     IICdatt=((ucRealRunSegNum)&0x7f)+(RunStopState<<7);
					     IICdatt = IICdatt << 8;
					     IICdatt = IICdatt + (uiTotalRunHour&0xff);
					     WordTo24C();
					     _delay_ms(5);
					}
//					Init1278(u16MasterAddress);
//	                sx1276_7_8_LoRaEntryRx();
//	                _delay_ms(50);
    				/*�洢���ݣ��ⲿeeprom */
	                if(uiTotalRunHour<200)
				    {
					     vdSaveRunDatExEep(ucKaofangNum,uiTotalRunHour);
						 IICaddr=2*ucKaofangNum+2;//��ʼ��λ�洢�����š�����������ߺ�
  					     IICdatt=uiTotalRunHour;
					     WordTo24C();
						 _delay_ms(5);
						 IICaddr= SEGTIME;
  	                     IICdatt=((ucSegCntTime)&0xFF);
	                     WordTo24C();
	                     _delay_ms(4);
					}
					TotalHourNow++;
					flgFlashShow = 0;
					vdInitLCD();
					vdFillDispBuff();
			        vdDispBrush();
					
					WIND_CLOS;
	                flgWindRun = 0x01; //��־���ŵ��������ת
	                uiWindCnt  = 700;//FengmenAngleN+10;   //���ŵ�ǰ��λ�ö��10��֤��ȫ�ر� 
	                uiFmNonCnt = 0;
                    FengmenAngleN=0;
                   if((uiTargetDryTemp>=6800)&&(uiTotalRunHour>160))
					{
    		    		if( OvenOverFlag == 0)
				    	{
							IICaddr= RUNSTATE_SAVE_ADDRESS;//��ʼ��λ�洢�����š�����������ߺ�
							IICdatt=((ucRealRunSegNum)&0x7f)+(RunStopState<<7);
							IICdatt = IICdatt << 8;
							IICdatt = IICdatt + (uiTotalRunHour&0xff);
							WordTo24C();
							_delay_ms(5);
							
							IICaddr=0x0000;
    					    if(ucKaofangNum < 9)
	                          IICdatt=((ucKaofangNum+1)<<8)+((flgMainSensor)<<4)+ucCurveNum;
                            else
						     IICdatt=0+((flgMainSensor)<<4)+ucCurveNum;
						    WordTo24C();
						    _delay_ms(5);
		    			}
        				OvenOverFlag = 1;
				   }
				}/**/
            if(ucSegCntTime >= (WorkSeg[ucCurvRunSegNum].ucSetRisTim + WorkSeg[ucCurvRunSegNum].ucSetStaTim))  
            {
                    //��������ģʽ��ֹͣ������
					if((ucCurveNum == SELFSET)&&(ucCurvRunSegNum >= ucSelfCurvRunSegMaxNum))
					{
						flgSelfSetPause = 0x01;
					}
				    /*ǰһ���¡����´�׶ν����������һ���¡����´�׶�*/
                    else if(ucCurvRunSegNum < 9)
					{
					    ucSegCntTime = 0;    //��ʼ��һ�μ�ʱ
                    	IICaddr = SEGTIME;
                      	IICdatt =((ucSegCntTime)&0xFF);
                    	WordTo24C();
                    	_delay_ms(4);
     				    ucSecCnt     = 0;
				        ucMinCnt     = 0;
		                ucSixMinCnt  = 0;
				        flgMinCnt    = 0x00;
				        flgHourCnt   = 0x00;
					    
						if((ucRealRunSegNum % 2) != 0)  //ȷʵ�������½׶Σ���ת����һ���
						{
						    ucCurvRunSegNum++;
							ucRealRunSegNum++;    //��μ�С��ҲҪ�Ӽ�;
						}
                        else   //�������½׶Σ���������ʱ�޸����ݿ��ܻ������������
						{
						    ucRealRunSegNum++;
							ucSegCntTime  =  WorkSeg[ucCurvRunSegNum].ucSetRisTim;
							IICaddr = SEGTIME;
							IICdatt =((ucSegCntTime)&0xFF);
							WordTo24C();
							_delay_ms(4);
                       	}
       				  IICaddr= RUNSTATE_SAVE_ADDRESS;//��ʼ��λ�洢�����š�����������ߺ�
    	 				  IICdatt=((ucRealRunSegNum)&0x7f)+(RunStopState<<7);
   					  IICdatt = IICdatt << 8;
   					  IICdatt = IICdatt + (uiTotalRunHour&0xff);
   					  WordTo24C();
   					  _delay_ms(5);
   						/*���������ٶ�*/
   					  vdCalDryRate();
   					  vdCalWetRate();
					}
					else if(ucCurvRunSegNum >= 9) //�濾����
					{
					;
					}
             }
			else if(ucSegCntTime >= WorkSeg[ucCurvRunSegNum].ucSetRisTim )
			{
				    /*���½׶ν������������½׶�*/
                    if(ucRealRunSegNum % 2 ==0)   //ucRealRunSegNumΪż��˵�������½׶�,  
					{
				        ucRealRunSegNum++;
						ucSecCnt     = 0;
				        ucMinCnt     = 0;
		                ucSixMinCnt  = 0;

				        flgMinCnt      = 0x00;
				        flgHourCnt     = 0x00;

					    if(OvenOverFlag ==0 )
					    {
    				      IICaddr= RUNSTATE_SAVE_ADDRESS;//��ʼ��λ�洢�����š�����������ߺ�
 	 				      IICdatt=((ucRealRunSegNum)&0x7f)+(RunStopState<<7);
					      IICdatt = IICdatt << 8;
					      IICdatt = IICdatt + (uiTotalRunHour&0xff);
					      WordTo24C();
					      _delay_ms(5);
					    }
	                    /*���������ٶ�*/
						vdCalDryRate();
						vdCalWetRate();
					}
			}
            if(1 == FlagPowerStatus)//&&(FengMenGufengCtr>=120))
			{
			      vdGuoFengJiCntl();
				  if(flgFanOnPre!=flgFanOn)
				  {
				  		FengMenGufengCtr = 0;
						flgFanOnPre = flgFanOn;
				  }
            }
            if(1 == FlagPowerStatus)//&&(FengMenGufengCtr1>=120))
			{
			       vdFengMenCntl();
				   if(FengmenAnglePre!=FengmenAngleN)
                   {
                       FengMenGufengCtr1 = 0;
                       FengmenAnglePre = FengmenAngleN;
				   }
			}
			break;
			case SET_MODE:	  
				if(PassFlag1==1)
				{
								for(i=0; i<10; i++)
							{
									WorkSeg[i].ucSetRisTim = 0;
							WorkSegSet[i].ucSetRisTim = 0;
									WorkSeg[i].uiSetDryTmp = 0;
							WorkSegSet[i].uiSetDryTmp = 0;
										WorkSeg[i].uiSetWetTmp = 0;
							WorkSegSet[i].uiSetWetTmp = 0;
								WorkSeg[i].ucSetStaTim = 0;
							WorkSegSet[i].ucSetStaTim = 0;
								}
					}
				break;
				
            default:
			  break;
		}
	}
   
}

/**********************************************************
* Function Name  : vdCalDryRate                           *
* Description    : ���������ٶ�, ����uiRiseDryTempRate��  *
*                  �����ڶ�ʱ����ı�uiTargetDryTemp      *
*                  �ٸ���uiTargetDryTemp�����£�          *                          
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/

void vdCalDryRate(void)
{
    unsigned int uiNewTargetDryTemp;
	if(ucRealRunSegNum % 2 ==0)   //���½׶�
	{
	    if(WorkSeg[ucCurvRunSegNum].uiSetDryTmp > WorkSeg[ucCurvRunSegNum - 1].uiSetDryTmp)  //ucRealRunSegNum��1��ʼ�����Դ�ʱucCurvRunSegNumӦ�ô��ڵ���1
		{
            /*�����ÿ��Сʱ��Ҫ���Ӷ��ٶ�(*10*10)*/
		    uiRiseDryTempRate  = ((WorkSeg[ucCurvRunSegNum].uiSetDryTmp - WorkSeg[ucCurvRunSegNum - 1].uiSetDryTmp)*10)/(WorkSeg[ucCurvRunSegNum].ucSetRisTim * 2);
            uiNewTargetDryTemp = WorkSeg[ucCurvRunSegNum - 1].uiSetDryTmp * 10 + uiRiseDryTempRate * (1+ucSegCntTime * 2); 
		    

		    if(uiTargetDryTemp <= WorkSeg[ucCurvRunSegNum - 1].uiSetDryTmp * 10)
			{
		        uiTargetDryTemp = WorkSeg[ucCurvRunSegNum - 1].uiSetDryTmp * 10;
				uiTargetDryTemp += uiRiseDryTempRate * (1+ucSegCntTime * 2);  

			}
			else if(uiTargetDryTemp >= WorkSeg[ucCurvRunSegNum].uiSetDryTmp *10)
			{
			    uiTargetDryTemp = WorkSeg[ucCurvRunSegNum].uiSetDryTmp *10;
                uiRiseDryTempRate = 0;  //��ʾ����
			}
			else if(uiTargetDryTemp < uiNewTargetDryTemp) //��ԭ����Ŀ��ֵ��Ϊ����Ŀ��ֵ����㣬����������ʱ�������������,���ή��
			    uiTargetDryTemp = uiNewTargetDryTemp;
			
             uDryTmepUpFlag = 1;
		}
		else 
		{
            //�����ÿ��Сʱ��Ҫ���Ӷ��ٶ�(*10*10)
		    uiRiseDryTempRate  = ((WorkSeg[ucCurvRunSegNum-1].uiSetDryTmp - WorkSeg[ucCurvRunSegNum].uiSetDryTmp)*10)/(WorkSeg[ucCurvRunSegNum].ucSetRisTim * 2);
            uiNewTargetDryTemp = WorkSeg[ucCurvRunSegNum - 1].uiSetDryTmp * 10 -  uiRiseDryTempRate * (1+ucSegCntTime * 2);  

			if(uiTargetDryTemp >= WorkSeg[ucCurvRunSegNum - 1].uiSetDryTmp * 10)
			{
		        uiTargetDryTemp = WorkSeg[ucCurvRunSegNum - 1].uiSetDryTmp * 10;
				uiTargetDryTemp -=  uiRiseDryTempRate * (1+ucSegCntTime * 2);  
			}
			else if(uiTargetDryTemp <= WorkSeg[ucCurvRunSegNum].uiSetDryTmp *10)
			{
			    uiTargetDryTemp = WorkSeg[ucCurvRunSegNum].uiSetDryTmp *10;
                uiRiseDryTempRate = 0;  //��ʾ����
			}
			else
			{
				uiTargetDryTemp = uiNewTargetDryTemp;
			}
			uDryTmepUpFlag = 0;
	    }
	
	}
    else                        //���½׶�
	{
	    uiTargetDryTemp = WorkSeg[ucCurvRunSegNum].uiSetDryTmp *10;
		uiRiseDryTempRate = 0; 
	}
}
/**********************************************************
* Function Name  : vdCalWetRate                           *
* Description    : ���������ٶ�, ����uiRiseDryTempRate��  *
*                  �����ڶ�ʱ����ı�uiTargetDryTemp      *
*                  �ٸ���uiTargetDryTemp�����£�          *                          
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/

void vdCalWetRate(void)
{
    unsigned int uiNewTargetWetTemp;

    if(ucRealRunSegNum % 2 ==0)   //���½׶� if(ucRealRunSegNum = 0)ucCurvRunSegNum = 0; but ucRealRunSegNum from 1 start
	{
	    if(WorkSeg[ucCurvRunSegNum].uiSetWetTmp > WorkSeg[ucCurvRunSegNum - 1].uiSetWetTmp)  //ucRealRunSegNum��1��ʼ�����Դ�ʱucCurvRunSegNumӦ�ô��ڵ���1
		{
		    /*�����ÿ��Сʱ��Ҫ���Ӷ��ٶ�(*10 * 10)*/
		    uiRiseWetTempRate  = ((WorkSeg[ucCurvRunSegNum].uiSetWetTmp - WorkSeg[ucCurvRunSegNum - 1].uiSetWetTmp)*10)/(WorkSeg[ucCurvRunSegNum].ucSetRisTim * 2);
		    uiNewTargetWetTemp = WorkSeg[ucCurvRunSegNum - 1].uiSetWetTmp*10 +  uiRiseWetTempRate * (1+ucSegCntTime * 2); 
			
			if(uiTargetWetTemp <= WorkSeg[ucCurvRunSegNum - 1].uiSetWetTmp * 10)
			{
		        uiTargetWetTemp = WorkSeg[ucCurvRunSegNum - 1].uiSetWetTmp * 10 ;
				uiTargetWetTemp += uiRiseWetTempRate *(1+ucSegCntTime * 2);
			}
            else if(uiTargetWetTemp >= WorkSeg[ucCurvRunSegNum].uiSetWetTmp * 10)
			{
			    uiTargetWetTemp = WorkSeg[ucCurvRunSegNum].uiSetWetTmp * 10;
                uiRiseWetTempRate = 0;  //��ʾ����
			}
			else if(uiTargetWetTemp < uiNewTargetWetTemp)//��ԭ����Ŀ��ֵ��Ϊ����Ŀ��ֵ����㣬����������ʱ�������������,���ή��
			    uiTargetWetTemp = uiNewTargetWetTemp;     
		    uWetTmepUpFlag = 1;
		}
		else 
		{
		    /*�����ÿ��Сʱ��Ҫ���Ӷ��ٶ�(*10 * 10)*/
		    uiRiseWetTempRate  = ((WorkSeg[ucCurvRunSegNum-1].uiSetWetTmp - WorkSeg[ucCurvRunSegNum].uiSetWetTmp)*10)/(WorkSeg[ucCurvRunSegNum].ucSetRisTim * 2);
		    uiNewTargetWetTemp = WorkSeg[ucCurvRunSegNum - 1].uiSetWetTmp*10 - uiRiseWetTempRate * (1+ucSegCntTime * 2);

			if(uiTargetWetTemp >= WorkSeg[ucCurvRunSegNum-1].uiSetWetTmp * 10)
			{
		        uiTargetWetTemp = WorkSeg[ucCurvRunSegNum -1].uiSetWetTmp * 10;
				uiTargetWetTemp -= uiRiseWetTempRate * (1+ucSegCntTime * 2);
			}
            else if(uiTargetWetTemp <= WorkSeg[ucCurvRunSegNum].uiSetWetTmp * 10)
			{
			    uiTargetWetTemp = WorkSeg[ucCurvRunSegNum].uiSetWetTmp * 10;
                uiRiseWetTempRate = 0;  //��ʾ����
			}
			else
			{
			    uiTargetWetTemp = uiNewTargetWetTemp;
			}
			uWetTmepUpFlag = 0;
		}
	}
    else                        //���½׶�
	{
	    uiTargetWetTemp = WorkSeg[ucCurvRunSegNum].uiSetWetTmp * 10;
		uiRiseWetTempRate = 0; 
	}

} 

/**********************************************************
* Function Name  : vdRenewTarDat                          *
* Description    : ��ʱ����Ŀ����ʪ������                 *                         
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/
void vdRenewTarDat(void)
{
    if(1 == uDryTmepUpFlag)
    {
	 	uiTargetDryTemp += uiRiseDryTempRate;
	    if(uiTargetDryTemp > WorkSeg[ucCurvRunSegNum].uiSetDryTmp *10)//���ܸ��ڱ��ε�����
	      uiTargetDryTemp = WorkSeg[ucCurvRunSegNum].uiSetDryTmp *10;

	}
	else
	{
	    uiTargetDryTemp -= uiRiseDryTempRate;
	    if(uiTargetDryTemp < WorkSeg[ucCurvRunSegNum].uiSetDryTmp *10)//���ܸ��ڱ��ε�����
	    uiTargetDryTemp = WorkSeg[ucCurvRunSegNum].uiSetDryTmp *10;

	}

    if(1 == uWetTmepUpFlag)
    {
	 	uiTargetWetTemp += uiRiseWetTempRate;
	    if(uiTargetWetTemp > WorkSeg[ucCurvRunSegNum].uiSetWetTmp *10)//���ܸ��ڱ��ε�����
	    uiTargetWetTemp = WorkSeg[ucCurvRunSegNum].uiSetWetTmp *10;

	}
	else
	{
	    uiTargetWetTemp -= uiRiseWetTempRate;
	    if(uiTargetWetTemp < WorkSeg[ucCurvRunSegNum].uiSetWetTmp *10)//���ܸ��ڱ��ε�����
	      uiTargetWetTemp = WorkSeg[ucCurvRunSegNum].uiSetWetTmp *10;

	}


}

/**********************************************************
* Function Name  : vdGuoFengJiCntl                        *
* Description    : �ķ������, ��Ҫ���Ƹ���               *
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/

void vdGuoFengJiCntl(void)
{
    unsigned int uiBakTargetDryTemp;
    uiBakTargetDryTemp = uiTargetDryTemp/10;
   	//����1����С���趨�¶ȣ�������
    if((uiDryTempVal < uiBakTargetDryTemp)&&(uiDryTempVal >= 50)
		&&(1!=flgFengjiLackPh)&&(1!=flgOverCurProt)&&(uiDryTempVal<820))
    {
	     FAN_RUN; 
		 flgFanOn =0x01;
		 flgCoalFanOn = 0x01;
    }//����2���������趨�¶ȣ���ֹͣ 
    else //if(uiDryTempVal >= uiBakTargetDryTemp)
    {
		 FAN_STOP;                    //ֹͣ���
		 flgFanOn = 0x00;
		 flgCoalFanOn = 0x00;
    }
}

/**********************************************************
* Function Name  : vdFengMenCntl                          *
* Description    : �����ſ��ƣ���Ҫ���ʪ�¿���           *
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/
void vdFengMenCntl(void)
{ 

if((WetTempValCtr>5)&&(WetTempValCtr<800)&&(flgWindRun==0))
{
	if(WetTempValCtrT>=WetTempValCtr)
	{
     if(FengmenAngleN==900)
		{ 
			WIND_CLOS;
			flgWindRun = 0x01; //��־���ŵ��������ת
			uiWindCnt  = 550;//FengmenAngleN-300;  
			uiFmNonCnt = 0;
			FengmenAngleN =0;
		}
	    else if(FengmenAngleN==600)
		{ 
			WIND_CLOS;
			flgWindRun = 0x01; //��־���ŵ��������ת
			uiWindCnt  = 450;//FengmenAngleN-300;  
			uiFmNonCnt = 0;
			FengmenAngleN =0;
		}
		else if(FengmenAngleN==300)
		{
			WIND_CLOS;
			flgWindRun = 0x01; //��־���ŵ��������ת
			uiWindCnt  = 350;//FengmenAngleN-300;  
			uiFmNonCnt = 0;
			FengmenAngleN =0;
		}
		else 
	    {
			FengmenAngleN =0;
		}
	}
	else
	{
	if((WetTempValCtr-WetTempValCtrT>0)&&(WetTempValCtr-WetTempValCtrT<5))
	{
		if(FengmenAngleN==900)
		{ 
			WIND_CLOS;
			flgWindRun = 0x01; //��־���ŵ��������ת
			uiWindCnt  = 110;//FengmenAngleN-300;  
			uiFmNonCnt = 0;
			FengmenAngleN =300;
		}
		if(FengmenAngleN==600)
		{ 
			WIND_CLOS;
			flgWindRun = 0x01; //��־���ŵ��������ת
			uiWindCnt  = 40;//FengmenAngleN-300;  
			uiFmNonCnt = 0;
			FengmenAngleN =300;
		}
		if(FengmenAngleN==300)
		{
			FengmenAngleN=FengmenAngleN;
		}
		if(FengmenAngleN==0) 
		{
  	        WIND_OPEN;
			flgWindRun = 0x01; //��־���ŵ��������ת
			uiWindCnt  = 120;//300-FengmenAngleN;  
			uiFmNonCnt = 0;
			FengmenAngleN = 300;
		}
	}
	else if((WetTempValCtr-WetTempValCtrT>=5)&&(WetTempValCtr-WetTempValCtrT<10))
	{
		if(FengmenAngleN==900)
		  {  
		       WIND_CLOS;
			   flgWindRun = 0x01; //��־���ŵ��������ת
			   uiWindCnt  = 50;//FengmenAngleN-300;  
			   uiFmNonCnt = 0;
			   FengmenAngleN =600;
		   }
		  if(FengmenAngleN==600)
		  { 
               FengmenAngleN= 600;
		  }
		  if(FengmenAngleN==300)
		  {
			   WIND_OPEN;
			   flgWindRun = 0x01; //��־���ŵ��������ת
			   uiWindCnt  = 70;//300-FengmenAngleN;  
			   uiFmNonCnt = 0;
			   FengmenAngleN = 600;	
		  }
		  if(FengmenAngleN==0)
		  {
			   WIND_OPEN;
			   flgWindRun = 0x01; //��־���ŵ��������ת
			   uiWindCnt  = 180;//300-FengmenAngleN;  
			   uiFmNonCnt = 0;
			   FengmenAngleN = 600;	
		  }
	}
	else if((WetTempValCtr-WetTempValCtrT>=10)&&(WetTempValCtr-WetTempValCtrT<15))
  {
        if(FengmenAngleN==900)
			FengmenAngleN = 900;
		if(FengmenAngleN==600)
		{ 
     	    WIND_OPEN;
			flgWindRun = 0x01; //��־���ŵ��������ת
			uiWindCnt  = 350;//300-FengmenAngleN;  
			uiFmNonCnt = 0;
            FengmenAngleN= 900;
		}
		if(FengmenAngleN==300)
		{
			WIND_OPEN;
			flgWindRun = 0x01; //��־���ŵ��������ת
			uiWindCnt  = 450;//300-FengmenAngleN;  
			uiFmNonCnt = 0;
			FengmenAngleN = 900;	
		}
		if(FengmenAngleN==0)
		{
			WIND_OPEN;
			flgWindRun = 0x01; //��־���ŵ��������ת
			uiWindCnt  = 550;//300-FengmenAngleN;  
			uiFmNonCnt = 0;
			FengmenAngleN = 900;
		}
	}
	else
	{
        if(FengmenAngleN==600)
		{ 
			 FengmenAngleN=900;
	         WIND_OPEN;
			 flgWindRun = 0x01; //��־���ŵ��������ת
			 uiWindCnt  = 350;//1200-FengmenAngleN;  
			 uiFmNonCnt = 0;
		}
		else if(FengmenAngleN==300)
		{
			FengmenAngleN=900;
		    WIND_OPEN;
			flgWindRun = 0x01; //��־���ŵ��������ת
		    uiWindCnt  = 450;//1200-FengmenAngleN;  
			uiFmNonCnt = 0;
		}
		else if(FengmenAngleN ==0)
		{
			FengmenAngleN=900;
		    WIND_OPEN;
			flgWindRun = 0x01; //��־���ŵ��������ת
			uiWindCnt  = 550;//1200-FengmenAngleN;  
			uiFmNonCnt = 0;
		}
		else
		{
			FengmenAngleN=900;
		}
  }
  }
} 
}

/**********************************************************
* Function Name  : uiGaugeACV                             *
* Description    : ����������ѹ                           *                          
* Input          : None                                   *
* Output         : None                                   *
* Return         : ������ѹֵ                             *
***********************************************************/
void vdVoiceAlarm(void)
{
   vdVoiceAlarmCnt = vdVoiceAlarmCnt%12;
   vdVoiceAlarmCnt ++;

   	if((flgSensorErr||(TemperSensorErrFlag))&&(vdVoiceAlarmCnt == 1))
	{
		    VoicePlay(SENSOR_ERR);  //������û��
	}
	else if(((850 == uiTempBuff[DNDRY])||(850 == uiTempBuff[DNWET])||(850 == uiTempBuff[UPDRY])||(850 == uiTempBuff[UPWET]))
	&&(vdVoiceAlarmCnt == 1))
	{
		VoicePlay(SENSOR_ERR);	//������û��
	}
	else
	{

	}
	if((flgFengjiNoCur)&&(vdVoiceAlarmCnt == 2))
    {
		VoicePlay(FENGJI_NOCUR);   //"����޵���",����
//		flgFengjiNoCur = 0;
	}


    if((flgPwrLackPhase)&&(vdVoiceAlarmCnt == 3))
	{

	    VoicePlay(FEGNJI_LACK);   //��Դȱ��
	}

    if((flgFengjiLackPh)&&(vdVoiceAlarmCnt == 4))
	{
        VoicePlay(FEGNJI_LACK); //"���ȱ��",����
	}

    if((flgOverCurProt)&&(vdVoiceAlarmCnt == 5))      
	{
        VoicePlay(FENGJI_OVER); //"�������",����
    }
    if((flgOverVol)&&(vdVoiceAlarmCnt ==6))
	{
		  VoicePlay(VOL_HIGH);  //"��ѹ�쳣",����
		  flgOverVol = 0;
    }
    if((flgUnderVol)&&(vdVoiceAlarmCnt == 7))
	{
		  VoicePlay(VOL_LOW);  //"��ѹ�쳣",���� 
		  flgUnderVol = 0;
    }
    if((flgSensorWater)&&(vdVoiceAlarmCnt == 8))
    {
		VoicePlay(SENSOR_WATER);
		flgSensorWater = 0;
	}

    if((flgDryHigh)&&(vdVoiceAlarmCnt == 9))
	{
		VoicePlay(DRY_HIGH);  //"",����
		flgDryHigh = 0; 
    }

	if((flgDryLow)&&(vdVoiceAlarmCnt == 10))
    {
         VoicePlay(DRY_LOW);  //"",����
		 flgDryLow = 0;
	}

    if((flgWetHigh)&&(vdVoiceAlarmCnt == 11))
	{
		VoicePlay(WET_HIGH);  //"",����
		flgWetHigh = 0;
	}
	
	if((flgWetLow)&&(vdVoiceAlarmCnt == 12))
	{
		   VoicePlay(WET_LOW);  //"",����
		   flgWetLow = 0;
	}
}

/**********************************************************
* Function Name  : vdDaFengJiCntl                         *
* Description    : ����ѭ�������ͨ���ߡ�����ת����       *
*                  �ۺϿ��Ƹɡ�ʪ��                       *
*                1����ƽ׶Σ�ѭ�����һֱ���ٴ���        *
*                2����ɫ�׶Σ�ѭ�����2Сʱ���٣�2Сʱ����*
*                3���ɽ�׶Σ�ѭ�����һֱ���ٴ���        *                          
* Input          : None                                   *
* Output         : None                                   *
* Return         : ������ѹֵ                             *
***********************************************************/
void vdDaFengJiCntl(void)
{
 	if((flgFengjiLackPh == 0)&&(flgVolProt == 0)&&(flgOverCurProt == 0))
	{
		    FENGJI_RUN;
			flgXunOn=0x01;
            FENGJIRUNFLAG = 1;			
    }
	else //if(1 == FengmenCtrMod)
	{
            FENGJI_STOP;
			flgXunOn=0x00;
			FENGJIRUNFLAG = 0;
			FAN_STOP; 
		    flgFanOn =0x00;
	}
}

/**********************************************************
* Function Name  : uiGaugeACC                             *
* Description    : ���������ź�ֵ                         *
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/
unsigned char uiACProc(void)
{
    static unsigned char turn ;
    if(turn == 0x00)
	{
		  uiAcVolValTemp = uiGaugeACV(ACV_SAMP_CH0); //����������ѹ,ִ��һ��Լ26ms;
		  if(uiAcVolValPre==0)
		  {
		   uiAcVolValPre = 2200;
		   uiAcVolValTemp = (uiAcVolValPre*3+uiAcVolValTemp*7);
		  }
		  else 
		  {
		   uiAcVolValTemp = (uiAcVolValPre*3+uiAcVolValTemp*7);
		   uiAcVolValPre = uiAcVolValTemp*0.1;
		  }
		  uiAcVolVal  = uiAcVolValTemp*0.01;
		  
		  if(uiAcVolVal < 80)
		     uiAcVolVal = 0;
	      if(uiAcVolVal>240)
		     uiAcVolVal = uiAcVolVal+5;
		uiAcCurVal_A = uiGaugeACC(ACC_SAMP_CH0); 
		uiAcCurVal_A = (uiAcCurVal_APre*3+uiAcCurVal_A*7)/10;
	    uiAcCurVal_APre =  uiAcCurVal_A;
		u16CurrUpdateFlag = 0;
//		GPIO_SetBits(GPIOA,GPIO_Pin_8); 
	}
    else if(turn == 0x01)
	{
		uiAcCurVal_B = uiGaugeACC(ACC_SAMP_CH2);
		uiAcCurVal_B = (uiAcCurVal_BPre*3+uiAcCurVal_B*7)/10;
	    uiAcCurVal_BPre = uiAcCurVal_B;

        uiAcCurVal_C = uiGaugeACC(ACC_SAMP_CH1);
		uiAcCurVal_C = (uiAcCurVal_CPre*3+uiAcCurVal_C*7)/10;
	    uiAcCurVal_CPre = uiAcCurVal_C;
		u16CurrUpdateFlag = 1;
//  		GPIO_ResetBits(GPIOA,GPIO_Pin_8); 
	}
	turn++;
	if(turn > 0x01)
	{
	    turn = 0x00;
		return 1;
	}
	else
	    return 0;
}
/**********************************************************
* Function Name  : vdKeyProc                              *
* Description    : ��������                               *
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/
void vdKeyProc(unsigned char keyval)
{
    
	unsigned char i,j,k;

	switch(keyval)   
  {   
    case KEY_RUN:   
		{
			
    	if(enWorkMode==DEBUG_MODE)
			{
                 enWorkMode = ucBakWorkModeDebug;
	             IICaddr= CtrAmount0;
				 i16CtrAmount0 = i16CtrAmount0Set;
				 if((i16CtrAmount0 >9)||(i16CtrAmount0 <1))
     			   i16CtrAmount0  = 2;
				 
  	             IICdatt= i16CtrAmount0;
	             WordTo24C();
	             _delay_ms(5);


			}
			if(enWorkMode==DEBUG_MasterAddressSet)
			{
			     enWorkMode = ucBakWorkModeDebug;
				 IICaddr= MasterAddress;
				 u16MasterAddress = u16MasterAddressSet;
				 IICdatt= u16MasterAddress;
				 WordTo24C();
				 _delay_ms(5);
//				 Init1278(u16MasterAddress);
//            	 sx1276_7_8_LoRaEntryRx();
//            	 _delay_ms(500);
			}
			if(enWorkMode==DEBUG_SlaveAddressSet)
			{
			     enWorkMode = ucBakWorkModeDebug;
				 IICaddr = SlaveAddress;
				 u16SlaveAddress = u16SlaveAddressSet;
				 IICdatt = u16SlaveAddress;
				 WordTo24C();
				 _delay_ms(5);
			}
			CLR_SETLED;//DTS1209������ģʽ�������м�������ָʾ����
			if(enWorkMode != RUN_MODE)	
			{
				flgXunOn = 0x01;
				if((1 == FlagPowerStatus)&&(enWorkMode!=SET_MODE))
				{
				    SET_RUNLED;
				    VoicePlay(START_RUN);
				}
				enWorkMode = RUN_MODE;
				RunStopState = 1;
    			IICaddr= RUNSTATE_SAVE_ADDRESS;//��ʼ��λ�洢�����š�����������ߺ�
 	 			IICdatt=((ucRealRunSegNum)&0x7f)+(RunStopState<<7);
				IICdatt = IICdatt << 8;
				IICdatt = IICdatt + (uiTotalRunHour&0xff);
				WordTo24C();
				_delay_ms(5);
				if(ucRealRunSegNum % 2 ==0)
				    ucCurvRunSegNum = ucRealRunSegNum / 2;
				else //����
				    ucCurvRunSegNum = (ucRealRunSegNum-1)/2;

				ucSecCnt = 0;
				ucMinCnt = 0;
				ucSixMinCnt    = 0;

				flgMinCnt      = 0x00;
				flgHourCnt     = 0x00;
		 if(ucCurvRunSegNum >= 1)
		 {
			 uiTargetDryTemp = WorkSeg[ucCurvRunSegNum -1].uiSetDryTmp *10;
    		 uiTargetWetTemp = WorkSeg[ucCurvRunSegNum -1].uiSetWetTmp *10; 				
		  }
					
				vdCalDryRate();
			    vdCalWetRate();

				IICaddr=0x0000;
				WordFrom24C();
				ucKaofangNum =(unsigned char)(IICdatt>>8);
				if(ucKaofangNum >= 9)
				    ucKaofangNum = 0;
			}
		    break;
		}
		case KEY_STOP:
		{
			if(enWorkMode != STOP_MODE)
			{
			    enWorkMode = STOP_MODE;
				if(1 == FlagPowerStatus)
			    {
			         CLR_RUNLED;
				     VoicePlay(STOP_RUN);
				}
				RunStopState = 0;
                IICaddr= RUNSTATE_SAVE_ADDRESS;//��ʼ��λ�洢�����š�����������ߺ�
 	 			IICdatt=((ucRealRunSegNum)&0x7f)+(RunStopState<<7);
				IICdatt = IICdatt << 8;
				IICdatt = IICdatt + (uiTotalRunHour&0xff);
				WordTo24C();
				_delay_ms(5);
                FAN_STOP;        
                flgFanOn   = 0x00;
			}	
		    break;
		}
		case KEY_SETUP:  
		{
			switch(enWorkMode)
			{
			    case SET_MODE:
				   if(ucBakWorkMode == STOP_MODE)  //��ֹͣģʽ����
					 {
					    ucSetupClassIndex++;

				        if(ucSetupClassIndex > DORT_INDEX)    //0:����;1:;��ʪ�Ȳ���;2:�ϡ�����;3:ʱ��
				            ucSetupClassIndex = CURV_INDEX;

                        if(ucSetupClassIndex == CURV_INDEX) //����
				        {
						    
				            flgFlashShow = ucCurveNumSet + 1;  //flgFlashShow��1��ʼ��Ч��ucCurveNumSet��0��ʼ��Ч
				        }
						else if(ucSetupClassIndex == UPDW_INDEX)     //�ϡ�����
				        {
				            flgFlashShow = flgMainSensorSet + 1;
				        }
                        else
						    flgFlashShow = 0x01;

					}
					else if(ucBakWorkMode == RUN_MODE)  
					{  
						ucSetupClassIndex++;
						if(ucSetupClassIndex > DORT_INDEX)    //0:����;1:;��ʪ�Ȳ���;2:�ϡ�����;3:ʱ��
				            ucSetupClassIndex = PARA_INDEX;   //ֱ������0��CURV_INDEX��

                        if(ucSetupClassIndex == UPDW_INDEX)     //�ϡ�����
				        {
				            flgFlashShow = flgMainSensorSet + 1;
				        }
						else if(ucSetupClassIndex == PARA_INDEX)
						{
						    if(ucRealRunSegNum % 2 == 0x00)
					            flgFlashShow = ucCurvRunSegNum * 4;
                            else
					            flgFlashShow = ucCurvRunSegNum * 4 + 3;
     					}
                        else
						    flgFlashShow = 0x01;

					}
					
					break;
          case STOP_MODE:

                    /*��ֹͣģʽ�½�������ģʽ*/
				    ucBakWorkMode = enWorkMode;
				    enWorkMode = SET_MODE;
                    
					ucCurveNumSet = ucCurveNum;
					flgMainSensorSet = flgMainSensor;
					ucSelfCurvRunSegMaxNumSet = ucSelfCurvRunSegMaxNum;

					for(i=0; i<10; i++)
	                {
	                    WorkSegSet[i].ucSetRisTim = WorkSeg[i].ucSetRisTim;
	                    WorkSegSet[i].uiSetDryTmp = WorkSeg[i].uiSetDryTmp;
                        WorkSegSet[i].uiSetWetTmp = WorkSeg[i].uiSetWetTmp;
		                WorkSegSet[i].ucSetStaTim = WorkSeg[i].ucSetStaTim;
                    }
		
					for(i=0; i<5;i++)                     
	                    ucDateTimeSet[i] = ucDateTime[i];    //ʱ�䲿�����

                    SET_SETLED;
				    ucModeSwiTime = 35;
                    
					ucSetupClassIndex = CURV_INDEX;  //�ӹ������ÿ�ʼ
				    flgFlashShow = ucCurveNumSet + 1;  

				    break;
                case RUN_MODE:
                    
					/*������ģʽ�½�������ģʽ*/
					ucBakWorkMode = enWorkMode;
				    enWorkMode = SET_MODE;

				    ucCurveNumSet = ucCurveNum;
					flgMainSensorSet = flgMainSensor;
					ucSelfCurvRunSegMaxNumSet = ucSelfCurvRunSegMaxNum;

					for(i=0; i<10; i++)
	                {
	                    WorkSegSet[i].ucSetRisTim = WorkSeg[i].ucSetRisTim;
	                    WorkSegSet[i].uiSetDryTmp = WorkSeg[i].uiSetDryTmp;
                        WorkSegSet[i].uiSetWetTmp = WorkSeg[i].uiSetWetTmp;
		                WorkSegSet[i].ucSetStaTim = WorkSeg[i].ucSetStaTim;
                    }

					for(i=0; i<5;i++)                     
	                    ucDateTimeSet[i] = ucDateTime[i];  

					SET_SETLED;
				    ucModeSwiTime = 35;

					ucSetupClassIndex = PARA_INDEX;
					
					if(ucRealRunSegNum % 2 == 0x00)
					    flgFlashShow = ucCurvRunSegNum * 4;
                    else
					    flgFlashShow = ucCurvRunSegNum * 4 + 3;
					
					break;
                default:
				    
					break;			
			
			}
			
		    break;
		}
		case KEY_SELL:     
		{
		    vdSerLKey();
            
			if(enWorkMode == QUERY_MODE)
			{
				if(ucKaofangNumQuery > 0)
				    ucKaofangNumQuery--;
	            else
				    ucKaofangNumQuery = 9;

				IICaddr=2*ucKaofangNumQuery+2;//��ʼ��λ�洢�����š�����������ߺ�
  				WordFrom24C();
				ucMaxTotalHour = IICdatt&0xff;
				if(ucMaxTotalHour > 200)
				    ucMaxTotalHour = 0;	
				/*�����ݴ��ⲿeeprom*/
				uiTotalQueryHour  = 1;
	            vdReadRunDatFrExEep(ucKaofangNumQuery,uiTotalQueryHour);		    
			}
		    break;
		}
        case KEY_SELR:   
		{
            vdSerRKey();
    
			if(enWorkMode == QUERY_MODE)
			{
				ucKaofangNumQuery++;
				if(ucKaofangNumQuery >= 10)
				    ucKaofangNumQuery = 0;

				IICaddr=2*ucKaofangNumQuery+2;//��ʼ��λ�洢�����š�����������ߺ�
  				WordFrom24C();
				ucMaxTotalHour = IICdatt;
				if(ucMaxTotalHour > 200)
				    ucMaxTotalHour = 0;
				/*�����ݴ��ⲿeeprom*/
				uiTotalQueryHour  = 1;
	            vdReadRunDatFrExEep(ucKaofangNumQuery,uiTotalQueryHour);
            }
		    break;
		}
		case KEY_CSELL:   
		{
		    
			vdSerLKey();
		    break;
		}
		case KEY_CSELR:   
		{
            vdSerRKey();
		    break;
		}
		case KEY_PLUS:     
		{
		    /*��������*/
			if(enWorkMode == STOP_MODE)
			{
			    ucPlayAddr++;
				if(ucPlayAddr > 51)
				    ucPlayAddr = 0;
				if(1 == FlagPowerStatus)
				   VoicePlay(ucPlayAddr);
            }
			
			/*���ⲿeeprom������*/
			if(enWorkMode == QUERY_MODE)
			{
		        if(uiTotalQueryHour < ucMaxTotalHour)
				    uiTotalQueryHour++;
      				vdReadRunDatFrExEep(ucKaofangNumQuery,uiTotalQueryHour);
		    }
			vdPlusKey();
			if(enWorkMode == DEBUG_MODE)
			{
				i16CtrAmount0Set ++ ;
				if(i16CtrAmount0Set>9)
			 		i16CtrAmount0Set = 1;
			}
			if(enWorkMode == DEBUG_MasterAddressSet)
			{
				u16MasterAddressSet ++ ;
				if(u16MasterAddressSet>999)
			 		u16MasterAddressSet = 1;
			}
			if(enWorkMode == DEBUG_SlaveAddressSet)
			{
	
            	u16SlaveAddressSet ++ ;
            	if(u16SlaveAddressSet>16)
            		u16SlaveAddressSet = 1;
			}
			break;
		}
		case KEY_MINUS:  
		{
            /*��������*/
            if(enWorkMode == STOP_MODE)
			{
				if(ucPlayAddr > 0)
				    ucPlayAddr--;
	            else
				    ucPlayAddr = 51;
				if(1 == FlagPowerStatus)
				    VoicePlay(ucPlayAddr);
            }
		    
			/*�����ݴ��ⲿeeprom*/
			if(enWorkMode == QUERY_MODE)
	        {
	  
                if(uiTotalQueryHour > 1)
		            uiTotalQueryHour--;
				vdReadRunDatFrExEep(ucKaofangNumQuery,uiTotalQueryHour);
            }
			vdMinusKey();
			if(enWorkMode == DEBUG_MODE)
			{
				i16CtrAmount0Set -- ;
				if(i16CtrAmount0Set<1)
			 		i16CtrAmount0Set = 9;
			}
			if(enWorkMode == DEBUG_MasterAddressSet)
			{
				u16MasterAddressSet -- ;
				if(u16MasterAddressSet < 1)
			 		u16MasterAddressSet = 999;
			}
			if(enWorkMode == DEBUG_SlaveAddressSet)
			{
				u16SlaveAddressSet -- ;
				if(u16SlaveAddressSet < 1)
			 		u16SlaveAddressSet = 16;
			}
		    break;
		}
		case KEY_CPLUS:   
		{
			if(enWorkMode == QUERY_MODE)
			{
				if(uiTotalQueryHour < (ucMaxTotalHour+5))
				    uiTotalQueryHour = uiTotalQueryHour + 5;
      				vdReadRunDatFrExEep(ucKaofangNumQuery,uiTotalQueryHour);
		    }
		    vdPlusKey();
			if(enWorkMode == DEBUG_MasterAddressSet)
			{
				u16MasterAddressSet += 10 ;
				if(u16MasterAddressSet>=999)
			 		u16MasterAddressSet = 1;
			}
		    break;
		}
		case KEY_CMINUS:  
		{
			if(enWorkMode == QUERY_MODE)
			{
                if(uiTotalQueryHour >= 5)
				    uiTotalQueryHour = uiTotalQueryHour - 5;
      			vdReadRunDatFrExEep(ucKaofangNumQuery,uiTotalQueryHour);
		    }
		    vdMinusKey();
		    break;
		}
		case KEY_ENTER:  
		{
		    /*1���������*/
			if(flgOverCurProt)    //������������������Իָ�
			{
			    flgOverCurProt = 0x00;
				ucOverCurCnt   = 0x00;
				ucFengjiLackPhCnt = 6;            //20131208 03
				FengjiProtFlag = 0;
				flgOverCurProt = 0;
				FENGJI_RUN;
				FAN_RUN;
				flgFanOn =0x01;
				FENGJIRUNFLAG =1;
				flgXunOn = 0x01;
				PhaseLackCnt = 0;
			}
            if(flgFengjiLackPh)
			{
			    flgFengjiLackPh   = 0x00;
				ucFengjiLackPhCnt = 6;            //20131208 03
				FengjiProtFlag = 0;
				flgFengjiLackPh = 0;
				FENGJI_RUN;
				FAN_RUN;
				FENGJIRUNFLAG =1;
				flgXunOn = 0x01;
				flgFanOn = 0x01;
				PhaseLackCnt = 0;
			}
			if(flgVolProt == 0x01)
			{
			    flgVolProt = 0;
				alarm_cnt = 0;
  	            flgOverVol  = 0x00;
	            flgUnderVol = 0x00; 
				FengjiProtFlag = 0;
				FENGJI_RUN;
				FAN_RUN;
				flgFanOn = 0x01;
				FENGJIRUNFLAG =1;
				flgXunOn = 0x01;
				PhaseLackCnt = 0;
			}
		  if(enWorkMode == SET_MODE)
			{
                
			    enWorkMode = ucBakWorkMode;
				flgFlashShow = 0;
			    CLR_SETLED;
				/*����������ݸı䣬�������ߺ�*/
        if(ucCurveNum  != ucCurveNumSet)
				{
                    ucCurveNum  = ucCurveNumSet;
					IICaddr=0;
  					IICdatt=((ucKaofangNum)<<8)+((flgMainSensorSet)<<4)+ucCurveNumSet;
					WordTo24C();
					_delay_ms(5); 
				}

                /*������ش������ı䣬����*/
				if(flgMainSensor != flgMainSensorSet)
				{
                    flgMainSensor   = flgMainSensorSet;
				    flgTmpDispGist  = flgMainSensor;
					 
					IICaddr=0;
  					IICdatt=((ucKaofangNum)<<8)+((flgMainSensorSet)<<4)+ucCurveNumSet;
					WordTo24C();
					_delay_ms(5);
				}
                
				/*��������иı䣬������������*/
                j = 0;
        for(i=0;i<10;i++)
				{
				    if(WorkSeg[i].ucSetRisTim != WorkSegSet[i].ucSetRisTim)
					{
					    j = 1;
						break;
					}
					if(WorkSeg[i].uiSetDryTmp != WorkSegSet[i].uiSetDryTmp)
					{
					    j = 1;
						break;
					}
					if(WorkSeg[i].uiSetWetTmp != WorkSegSet[i].uiSetWetTmp)
					{
					    j = 1;
						break;
					}
					if(WorkSeg[i].ucSetStaTim != WorkSegSet[i].ucSetStaTim)
					{
					    j = 1;
						break;
					}
     			}
				if(j == 1)
				{
				    /*�����ǰ���ж�(���¶�)�����б仯�������¼���Ŀ����ʪ��*/
					k = 0;
					if(ucBakWorkMode == RUN_MODE)
				    {
					    
						if(WorkSeg[ucCurvRunSegNum].ucSetRisTim != WorkSegSet[ucCurvRunSegNum].ucSetRisTim)
						{
						    if(ucRealRunSegNum % 2 ==0)   //���½׶�
						        k = 1;
						}
						else if(WorkSeg[ucCurvRunSegNum].uiSetDryTmp != WorkSegSet[ucCurvRunSegNum].uiSetDryTmp)
						    k = 1;
						else if(WorkSeg[ucCurvRunSegNum].uiSetWetTmp != WorkSegSet[ucCurvRunSegNum].uiSetWetTmp)
						    k = 1;
                        				
					}

					/*j=1˵�������ݱ仯����������*/
                    for(i=0; i<10; i++)
	                {
	                    WorkSeg[i].ucSetRisTim = WorkSegSet[i].ucSetRisTim;
	                    WorkSeg[i].uiSetDryTmp = WorkSegSet[i].uiSetDryTmp;
                        WorkSeg[i].uiSetWetTmp = WorkSegSet[i].uiSetWetTmp;
		                WorkSeg[i].ucSetStaTim = WorkSegSet[i].ucSetStaTim;
                    }
				    vdSaveDatToEep(ucCurveNumSet);
					if(OvenOverFlag ==0 )
					{
    				  IICaddr= RUNSTATE_SAVE_ADDRESS;//��ʼ��λ�洢�����š�����������ߺ�
 	 				  IICdatt=((ucRealRunSegNum)&0x7f)+(RunStopState<<7);
					  IICdatt = IICdatt << 8;
					  IICdatt = IICdatt + (uiTotalRunHour&0xff);
					  WordTo24C();
					  _delay_ms(5);
					}
					if(k == 1)	  //�����޸ģ�������ʪ���ٶ�,������ʱ�䲻���
					{
						vdCalDryRate();
				        vdCalWetRate();
	                }
                }
				j = 0;
				for(i=0; i<5;i++)   
				{       
				    if(ucDateTime[i] == ucDateTimeSet[i])   
					    continue;
                    else
					{
					    j = 1;
						break;
					}	
			    }
				if( j == 1)
				{
				    for(i=0; i<5;i++)
				        ucDateTime[i] = ucDateTimeSet[i];    //ʱ�䲿�����
				    WtRTC(ucDateTime);
                }
				//������ģʽ��
			    if(ucCurveNumSet == SELFSET)
			    {
					ucSelfCurvRunSegMaxNum = ucSelfCurvRunSegMaxNumSet;
					if(ucCurvRunSegNum <= ucSelfCurvRunSegMaxNum)
					{
					    if(flgSelfSetPause == 0x01)
						{
						    flgSelfSetPause = 0x00;
					    }
					}
			    }
			}//end of if(enWorkMode == SET_MODE)
			else if(enWorkMode == QUERY_MODE)
			{
			    enWorkMode = ucBakWorkMode;
				flgFlashShow = 0;
			}
			if(enWorkMode==DEBUG_MODE)
			{
                 enWorkMode = ucBakWorkModeDebug;
	             IICaddr= CtrAmount0;
				 i16CtrAmount0 = i16CtrAmount0Set;
  	             IICdatt= i16CtrAmount0;
				 
				 if((i16CtrAmount0 >9)||(i16CtrAmount0 <1))
					i16CtrAmount0  = 2;
	             WordTo24C();
	             _delay_ms(4);
			}

			if(enWorkMode==DEBUG_MasterAddressSet)
			{
			     enWorkMode = ucBakWorkModeDebug;
				 IICaddr= MasterAddress;
				 u16MasterAddress = u16MasterAddressSet;
				 IICdatt= u16MasterAddress;
				 WordTo24C();
				 _delay_ms(5);
//				 Init1278(u16MasterAddress);
//            	 sx1276_7_8_LoRaEntryRx();
//            	 _delay_ms(500);
			}
			if(enWorkMode==DEBUG_SlaveAddressSet)
			{
			     enWorkMode = ucBakWorkModeDebug;
				 IICaddr = SlaveAddress;
				 u16SlaveAddress = u16SlaveAddressSet;
				 IICdatt = u16SlaveAddress;
				 WordTo24C();
				 _delay_ms(5);
			}
			break;
		}
		case KEY_RESET:
		{
			vdChangeDatToOriginal();
			vdReadDatFrEep(ucCurveNum);
			IICaddr=RUNSTATE_SAVE_ADDRESS;
	        WordFrom24C();
	        uiTotalRunHour = IICdatt&(0xff);
	        if(uiTotalRunHour>200)
	             uiTotalRunHour = 0;
	        ucRealRunSegNum = ((IICdatt>>8)&0x7f);
	        if(ucRealRunSegNum>19)
	              ucRealRunSegNum = 0;
	        if(ucRealRunSegNum % 2 ==0)
	           ucCurvRunSegNum = ucRealRunSegNum / 2;
	        else //����
	            ucCurvRunSegNum = (ucRealRunSegNum-1)/2;
			break;
		}	
		case KEY_SWIMA:    //0x01
		{
			if(flgTmpDispGist == flgMainSensor)  //����,����MAINSENSOR
	        {
	            flgTmpDispGist = ~flgMainSensor;

				ucTmpDispGistCnt = 4;  //�л���������������ʱ
			    flgUpDownFlashShow = 0x01;   //��˸λ����
			   
            }
	        else // if(flgTmpDispGist == AUXISENSOR) //����,����
            {
			    flgTmpDispGist = flgMainSensor;

				ucTmpDispGistCnt = 0;  //�л���������������ʱ
			    flgUpDownFlashShow = 0x00;   //��˸λ����
			   
 	        }/**/
			
		    break;
		}
		case KEY_QUERY:    //0x0F
		{
		    if((enWorkMode == STOP_MODE)||(enWorkMode == RUN_MODE))
			{
				ucBakWorkMode = enWorkMode;
				enWorkMode    = QUERY_MODE;
                ucModeSwiTime = 35;
                
				/*��˸Ч��*/
				ucSetupClassIndex = 0x04;
                flgFlashShow      = 0x01;

				uiTotalQueryHour = 1;               //�ӵ�һ����¼����
				ucKaofangNumQuery = ucKaofangNum;   //�ӵ�ǰ���β���
                
				/*�����ݴ��ⲿeeprom*/
                vdReadRunDatFrExEep(ucKaofangNumQuery,uiTotalQueryHour);
				/*���ڲ�eeprom�������ε����ʱ��*/
				IICaddr=2*ucKaofangNumQuery+2;//��ʼ��λ�洢�����š�����������ߺ�
  				WordFrom24C();
				ucMaxTotalHour = IICdatt&0xff;
				if(ucMaxTotalHour > 200)
				    ucMaxTotalHour = 0;
			}	
		    break;
		}
        case KEY_DEBUG:
		{
			if((enWorkMode==DEBUG_SlaveAddressSet)||(enWorkMode==DEBUG_MasterAddressSet)||(enWorkMode==DEBUG_MODE))
             return;
             ucBakWorkModeDebug=enWorkMode;
			 enWorkMode = DEBUG_MODE;
			 i16CtrAmount0Set = i16CtrAmount0;
			 ucModeSwiTime = 25;
			 break;
		}
		case KEY_MasterAddressSet:
		{
			
			if((enWorkMode==DEBUG_SlaveAddressSet)||(enWorkMode==DEBUG_MasterAddressSet)||(enWorkMode==DEBUG_MODE))
             return;
    		if(enWorkMode==DEBUG_MasterAddressSet)
			{
				enWorkMode = ucBakWorkModeDebug;
				IICaddr= MasterAddress;
				u16MasterAddress = u16MasterAddressSet;
				IICdatt= u16MasterAddress;
				WordTo24C();
				_delay_ms(5);
			 }
             ucBakWorkModeDebug=enWorkMode;
			 enWorkMode = DEBUG_MasterAddressSet;
			 u16MasterAddressSet = u16MasterAddress;
			 ucModeSwiTime = 25;
			 break;
		}
		case KEY_SlaveAddressSet:
		{
			if((enWorkMode==DEBUG_SlaveAddressSet)||(enWorkMode==DEBUG_MasterAddressSet)||(enWorkMode==DEBUG_MODE))
             return;
    		if(enWorkMode==DEBUG_SlaveAddressSet)
			{
				enWorkMode = ucBakWorkModeDebug;
				IICaddr= SlaveAddress;
				u16SlaveAddress = u16SlaveAddressSet;
				IICdatt= u16SlaveAddress;
				WordTo24C();
				_delay_ms(5);
			}
             ucBakWorkModeDebug=enWorkMode;
			 enWorkMode = DEBUG_SlaveAddressSet;
			 u16SlaveAddressSet = u16SlaveAddress;
			 ucModeSwiTime = 25;
			 break;
		}
		default:   
            break;   
    }
}

/**********************************************************
* Function Name  : vdSerLKey                              *
* Description    : ���Ƽ��������                         *
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/
void vdSerLKey(void)
{
  if(enWorkMode == SET_MODE)
	{
	    if(flgFlashShow > 0)
		    flgFlashShow--;
                
		if( ucSetupClassIndex == DORT_INDEX)  //ʱ��0x00 
		{
		    if(flgFlashShow == 0) 
		        flgFlashShow = 5;
	    }
		else if(ucSetupClassIndex == CURV_INDEX)  //����0x01
		{  
		    if(flgFlashShow == 0) 
			    flgFlashShow = 4;
					
            ucCurveNumSet = flgFlashShow - 1;//���ڲ�e2rom����������
		    vdReadDatFrEep(ucCurveNumSet);
		}
		else if(ucSetupClassIndex == PARA_INDEX)   //����0x02
		{
		    
			if(flgFlashShow == 0)           //39,�ܹ�39������
				flgFlashShow = 0x01;
		       

		}
		else if(ucSetupClassIndex == UPDW_INDEX)  //�ϡ�����0x03
		{
		    if(flgFlashShow == 0)   //�ܹ�2������flgMainSensor
		        flgFlashShow = 2;
            flgMainSensorSet = flgFlashShow - 1;
            
		}
	}
	else if(enWorkMode == STOP_MODE)
	{
	    if(ucRealRunSegNum > 1)
		    ucRealRunSegNum--;
		if(ucRealRunSegNum % 2 ==0)
	    	ucCurvRunSegNum = ucRealRunSegNum / 2;
		else
	    	ucCurvRunSegNum = (ucRealRunSegNum-1)/2;
        ucSegCntTime = 0;
		IICaddr = SEGTIME;
		IICdatt =((ucSegCntTime)&0xFF);
		WordTo24C();
		_delay_ms(5);
	}
	
}

/**********************************************************
* Function Name  : vdSerRKey                              *
* Description    : ���Ƽ��������                         *
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/
void vdSerRKey(void)
{
    if(enWorkMode == SET_MODE)
	{
	    flgFlashShow++;
		if( ucSetupClassIndex == DORT_INDEX )
	    {
		    if(flgFlashShow > 0x05)
			    flgFlashShow = 0x01;
		}
		else if(ucSetupClassIndex == CURV_INDEX)
		{
		    if(flgFlashShow > 0x04)
			    flgFlashShow = 0x01;
            ucCurveNumSet = flgFlashShow - 1;
		    vdReadDatFrEep(ucCurveNumSet);
		}
		else if(ucSetupClassIndex == PARA_INDEX)
		{
		    if(flgFlashShow > 0x27)   //39,�ܹ�39������
			    flgFlashShow = 0x27;
			//������ģʽ�£�ucSelfCurvRunSegMaxNumSet ֻ�����ӣ������С;
			if(ucCurveNumSet == SELFSET)
			{
			    if(ucSelfCurvRunSegMaxNumSet < flgFlashShow / 4)
                    ucSelfCurvRunSegMaxNumSet = flgFlashShow / 4;
			}/**/
		}
		else if(ucSetupClassIndex == UPDW_INDEX)  //�ϡ�����
		{
		    if(flgFlashShow > UPDW_INDEX)   //�ܹ�2������flgMainSensor
		        flgFlashShow = 0x01;
            flgMainSensorSet = flgFlashShow - 1;
		}
	}
	else if(enWorkMode == STOP_MODE)
	{
        if(ucCurveNum != SELFSET)
		{
		    if(ucRealRunSegNum < 19)
		    {   
		    	ucRealRunSegNum++;
	           	if(ucRealRunSegNum % 2 ==0)
	    			ucCurvRunSegNum = ucRealRunSegNum / 2;
				else
	    			ucCurvRunSegNum = (ucRealRunSegNum-1)/2;
           			ucSegCntTime = 0;
					
					IICaddr = SEGTIME;
					IICdatt =((ucSegCntTime)&0xFF);
					WordTo24C();
					_delay_ms(4);
		    }
	    }
		else //��������ģʽ��ֹͣ������
		{
		    if(ucRealRunSegNum % 2 ==0)
			    ucCurvRunSegNum = ucRealRunSegNum / 2;
			else //����
			    ucCurvRunSegNum = (ucRealRunSegNum-1) / 2;
			
			if(ucCurvRunSegNum <= ucSelfCurvRunSegMaxNumSet)
			{
			    if(ucRealRunSegNum < 19)
		        {
		        	ucRealRunSegNum++;
		           	ucSegCntTime = 0;
				if(ucRealRunSegNum % 2 ==0)
	    			ucCurvRunSegNum = ucRealRunSegNum / 2;
				else
	    			ucCurvRunSegNum = (ucRealRunSegNum-1)/2;
           			ucSegCntTime = 0;
					IICaddr = SEGTIME;
					IICdatt =((ucSegCntTime)&0xFF);
					WordTo24C();
					_delay_ms(4);
				}
			}
		}
	}
}

/**********************************************************
* Function Name  : vdPlusKey                              *
* Description    : �Ӽ��������                           *
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/
void vdPlusKey(void)
{
   
    if(enWorkMode == SET_MODE)
	{
	    if(ucSetupClassIndex == DORT_INDEX)
		{
		    if(flgFlashShow ==0x05)
			{
			    if(ucDateTimeSet[0] < 59)
			        ucDateTimeSet[0]++;
			    else
				ucDateTimeSet[0] = 0;
		    }
			else if(flgFlashShow ==0x04)
			{
			    if(ucDateTimeSet[1] < 23) 
			        ucDateTimeSet[1]++;
			    else
				ucDateTimeSet[1] = 0;
	        }
	        else if(flgFlashShow ==0x03)
			{
			    if(ucDateTimeSet[2] < 31) 
			        ucDateTimeSet[2]++;
			    else
				ucDateTimeSet[2] = 1;
	        }
	        else if(flgFlashShow ==0x02)
			{
			    if(ucDateTimeSet[3] < 12) 
				    ucDateTimeSet[3]++;
			    else
				ucDateTimeSet[3] = 1;
	        }
	        else if(flgFlashShow ==0x01)
			{
			    if(ucDateTimeSet[4] < 20)
			        ucDateTimeSet[4]++;
			    else
				    ucDateTimeSet[4] = 13;
	        }
        }//end of if(ucSetupClassIndex == 0x00)

		else if(ucSetupClassIndex == CURV_INDEX)
		{
		    ;
		}

		else if(ucSetupClassIndex == PARA_INDEX)
		{
            
			/*��һ��*/
		    if(flgFlashShow ==0x01)
			{
			    if(WorkSegSet[0].uiSetDryTmp < 880)   
			        WorkSegSet[0].uiSetDryTmp += 10;
				else
                   WorkSegSet[0].uiSetDryTmp = 280;
		    }
			else if(flgFlashShow ==0x02)
			{
			    if(WorkSegSet[0].uiSetWetTmp < 880)   
			        WorkSegSet[0].uiSetWetTmp +=  5;
				else
                   WorkSegSet[0].uiSetWetTmp = 280;
			}
			else if(flgFlashShow ==0x03)
			{
			    if(WorkSegSet[0].ucSetStaTim < 88)    
			        WorkSegSet[0].ucSetStaTim++;
				else
					WorkSegSet[0].ucSetStaTim = 1;
			}

			/*�ڶ���*/
			else if(flgFlashShow ==0x04)
			{
			    if(WorkSegSet[1].ucSetRisTim < 88)   
			        WorkSegSet[1].ucSetRisTim++;
				else
					WorkSegSet[1].ucSetRisTim = 1;
		    }
			else if(flgFlashShow ==0x05)
			{
			    if(WorkSegSet[1].uiSetDryTmp < 880)   
			        WorkSegSet[1].uiSetDryTmp += 10;
				else
					WorkSegSet[1].uiSetDryTmp = 280;
		    }
			else if(flgFlashShow ==0x06)
			{
			    if(WorkSegSet[1].uiSetWetTmp < 880)   
			        WorkSegSet[1].uiSetWetTmp +=  5;
				else
					WorkSegSet[1].uiSetWetTmp = 280;
			}
			else if(flgFlashShow ==0x07)
			{
			    if(WorkSegSet[1].ucSetStaTim < 88)    
			        WorkSegSet[1].ucSetStaTim++;
				else
					WorkSegSet[1].ucSetStaTim = 1;
			}

			/*������*/
			else if(flgFlashShow ==0x08)
			{
			    if(WorkSegSet[2].ucSetRisTim < 88)   
			        WorkSegSet[2].ucSetRisTim++;
				else
					WorkSegSet[2].ucSetRisTim = 1;				
		    }
			else if(flgFlashShow ==0x09)
			{
			    if(WorkSegSet[2].uiSetDryTmp < 880)   
			        WorkSegSet[2].uiSetDryTmp += 10;
				else
                   WorkSegSet[2].uiSetDryTmp = 280;
		    }
			else if(flgFlashShow ==0x0a)
			{
			    if(WorkSegSet[2].uiSetWetTmp < 880)   
			        WorkSegSet[2].uiSetWetTmp +=  5;
				else
                   WorkSegSet[2].uiSetWetTmp = 280;
			}
			else if(flgFlashShow ==0x0b)
			{
			    if(WorkSegSet[2].ucSetStaTim < 88)    
			        WorkSegSet[2].ucSetStaTim++;
				else
					WorkSegSet[2].ucSetStaTim = 1;
			}

			/*���Ķ�*/
			else if(flgFlashShow ==0x0c)
			{
			    if(WorkSegSet[3].ucSetRisTim < 88)   
			        WorkSegSet[3].ucSetRisTim++;
				else
					WorkSegSet[3].ucSetRisTim = 1;
		    }
			else if(flgFlashShow ==0x0d)
			{
			    if(WorkSegSet[3].uiSetDryTmp < 880)   
			        WorkSegSet[3].uiSetDryTmp += 10;
				else
                   WorkSegSet[3].uiSetDryTmp = 280;
		    }
			else if(flgFlashShow ==0x0e)
			{
			    if(WorkSegSet[3].uiSetWetTmp < 880)   
			        WorkSegSet[3].uiSetWetTmp +=  5;
				else
                   WorkSegSet[3].uiSetWetTmp = 280;
			}
			else if(flgFlashShow ==0x0f)
			{
			    if(WorkSegSet[3].ucSetStaTim < 88)    
			        WorkSegSet[3].ucSetStaTim++;
				else
					WorkSegSet[3].ucSetStaTim = 1;
			}

			/*�����*/
			else if(flgFlashShow ==0x10)
			{
			    if(WorkSegSet[4].ucSetRisTim < 88)   
			        WorkSegSet[4].ucSetRisTim++;
				else
					WorkSegSet[4].ucSetRisTim = 1;
		    }
			else if(flgFlashShow ==0x11)
			{
			    if(WorkSegSet[4].uiSetDryTmp < 880)   
			        WorkSegSet[4].uiSetDryTmp += 10;
				else
                   WorkSegSet[4].uiSetDryTmp = 280;
		    }
			else if(flgFlashShow ==0x12)
			{
			    if(WorkSegSet[4].uiSetWetTmp < 880)   
			        WorkSegSet[4].uiSetWetTmp +=  5;
				else
                   WorkSegSet[4].uiSetWetTmp = 280;
			}
			else if(flgFlashShow ==0x13)
			{
			    if(WorkSegSet[4].ucSetStaTim < 88)    
			        WorkSegSet[4].ucSetStaTim++;
				else
					WorkSegSet[4].ucSetStaTim = 1;
			}

			/*������*/
			else if(flgFlashShow ==0x14)
			{
			    if(WorkSegSet[5].ucSetRisTim < 88)   
			        WorkSegSet[5].ucSetRisTim++;
				else
					WorkSegSet[5].ucSetRisTim = 1;
		    }
			else if(flgFlashShow ==0x15)
			{
			    if(WorkSegSet[5].uiSetDryTmp < 880)   
			        WorkSegSet[5].uiSetDryTmp += 10;
				else
                   WorkSegSet[5].uiSetDryTmp = 280;
		    }
			else if(flgFlashShow ==0x16)
			{
			    if(WorkSegSet[5].uiSetWetTmp < 880)   
			        WorkSegSet[5].uiSetWetTmp +=  5;
				else
                   WorkSegSet[5].uiSetWetTmp = 280;
			}
			else if(flgFlashShow ==0x17)
			{
			    if(WorkSegSet[5].ucSetStaTim < 88)    
			        WorkSegSet[5].ucSetStaTim++;
				else
					WorkSegSet[5].ucSetStaTim = 1;
			}

			/*���߶�*/
			else if(flgFlashShow ==0x18)
			{
			    if(WorkSegSet[6].ucSetRisTim < 88)   
			        WorkSegSet[6].ucSetRisTim++;
				else
					WorkSegSet[6].ucSetRisTim = 1;
		    }
			else if(flgFlashShow ==0x19)
			{
			    if(WorkSegSet[6].uiSetDryTmp < 880)   
			        WorkSegSet[6].uiSetDryTmp += 10;
				else
                   WorkSegSet[6].uiSetDryTmp = 280;
		    }
			else if(flgFlashShow ==0x1a)
			{
			    if(WorkSegSet[6].uiSetWetTmp < 880)   
			        WorkSegSet[6].uiSetWetTmp +=  5;
				else
                   WorkSegSet[6].uiSetWetTmp = 280;
			}
			else if(flgFlashShow ==0x1b)
			{
			    if(WorkSegSet[6].ucSetStaTim < 88)    
			        WorkSegSet[6].ucSetStaTim++;
				else
					WorkSegSet[6].ucSetStaTim = 1;
			}

			/*�ڰ˶�*/
			else if(flgFlashShow ==0x1c)
			{
			    if(WorkSegSet[7].ucSetRisTim < 88)   
			        WorkSegSet[7].ucSetRisTim++;
				else
					WorkSegSet[7].ucSetRisTim = 1;
		    }
			else if(flgFlashShow ==0x1d)
			{
			    if(WorkSegSet[7].uiSetDryTmp < 880)   
			        WorkSegSet[7].uiSetDryTmp += 10;
				else
                   WorkSegSet[7].uiSetDryTmp = 280;
		    }
			else if(flgFlashShow ==0x1e)
			{
			    if(WorkSegSet[7].uiSetWetTmp < 880)   
			        WorkSegSet[7].uiSetWetTmp +=  5;
				else
                   WorkSegSet[7].uiSetWetTmp = 280;
			}
			else if(flgFlashShow ==0x1f)
			{
			    if(WorkSegSet[7].ucSetStaTim < 88)    
			        WorkSegSet[7].ucSetStaTim++;
				else
					WorkSegSet[7].ucSetStaTim = 1;
			}
			/*�ھŶ�*/
			else if(flgFlashShow ==0x20)
			{
			    if(WorkSegSet[8].ucSetRisTim < 88)   
			        WorkSegSet[8].ucSetRisTim++;
				else
					WorkSegSet[8].ucSetRisTim = 1;
		    }
			else if(flgFlashShow ==0x21)
			{
			    if(WorkSegSet[8].uiSetDryTmp < 880)   
			        WorkSegSet[8].uiSetDryTmp += 10;
				else
                   WorkSegSet[8].uiSetDryTmp = 280;
		    }
			else if(flgFlashShow ==0x22)
			{
			    if(WorkSegSet[8].uiSetWetTmp < 880)   
			        WorkSegSet[8].uiSetWetTmp +=  5;
				else
                   WorkSegSet[8].uiSetWetTmp = 280;
			}
			else if(flgFlashShow ==0x23)
			{
			    if(WorkSegSet[8].ucSetStaTim < 88)    
			        WorkSegSet[8].ucSetStaTim++;
				else
					WorkSegSet[8].ucSetStaTim = 1;
			}

			/*��ʮ��*/
			else if(flgFlashShow ==0x24)
			{
			    if(WorkSegSet[9].ucSetRisTim < 88)   
			        WorkSegSet[9].ucSetRisTim++;
				else
					WorkSegSet[9].ucSetRisTim = 1;
		    }
			else if(flgFlashShow ==0x25)
			{
			    if(WorkSegSet[9].uiSetDryTmp < 880)   
			        WorkSegSet[9].uiSetDryTmp += 10;
				else
                   WorkSegSet[9].uiSetDryTmp = 280;
		    }
			else if(flgFlashShow ==0x26)
			{
			    if(WorkSegSet[9].uiSetWetTmp < 880)   
			        WorkSegSet[9].uiSetWetTmp +=  5;
				else
                   WorkSegSet[9].uiSetWetTmp = 280;
			}
			else if(flgFlashShow ==0x27)
			{
			    if(WorkSegSet[9].ucSetStaTim < 88)    
			        WorkSegSet[9].ucSetStaTim++;
				else
					WorkSegSet[9].ucSetStaTim = 1;
			}
		}//end of else if(ucSetupClassIndex == 0x02)
	}//end of if(enWorkMode == SET_MODE)
}

/**********************************************************
* Function Name  : vdMinusKey                             *
* Description    : �����������                           *
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/

void vdMinusKey(void)
{
    
    if(enWorkMode == SET_MODE)
	{
	    if(ucSetupClassIndex == DORT_INDEX)
		{
		    if(flgFlashShow ==0x05)
		   {
		        if(ucDateTimeSet[0] > 0) 
			      ucDateTimeSet[0]--;
			else
			      ucDateTimeSet[0] = 59;
		    }
		    else if(flgFlashShow ==0x04)
		    {
		        if(ucDateTimeSet[1] > 0)
				ucDateTimeSet[1]--;
			else
		          ucDateTimeSet[1] = 23;
	         }
			else if(flgFlashShow ==0x03)
		     {
			    if(ucDateTimeSet[2] > 1)
			        ucDateTimeSet[2]--;
			    else
				ucDateTimeSet[2] = 31;
	              }
			else if(flgFlashShow ==0x02) 
			{
			    if(ucDateTimeSet[3] > 1)
				    ucDateTimeSet[3]--;
			    else
			       ucDateTimeSet[3] = 12;
	        }
			else if(flgFlashShow ==0x01) //?��
			{
			    if(ucDateTimeSet[4] > 13)
				    ucDateTimeSet[4]--;
				else
				    ucDateTimeSet[4] =20;
	        }
		}//end of if(ucSetupClassIndex == 0x00)

		else if(ucSetupClassIndex == CURV_INDEX)
		{
		    ;
		}

		else if(ucSetupClassIndex == PARA_INDEX)
		{
		    /*��һ��*/
		    if(flgFlashShow ==0x01)
			{
			    if(WorkSegSet[0].uiSetDryTmp > 280)   
			        WorkSegSet[0].uiSetDryTmp -= 10;
				else
					WorkSegSet[0].uiSetDryTmp = 880;
		    }
			else if(flgFlashShow ==0x02)
			{
			    if(WorkSegSet[0].uiSetWetTmp > 280)   
			        WorkSegSet[0].uiSetWetTmp -=  5;
				else
					WorkSegSet[0].uiSetWetTmp = 880;

			}
			else if(flgFlashShow ==0x03)
			{
			    if(WorkSegSet[0].ucSetStaTim > 1)    
			        WorkSegSet[0].ucSetStaTim--;
				else
					WorkSegSet[0].ucSetStaTim = 88;
			}

			/*�ڶ���*/
			else if(flgFlashShow ==0x04)
			{
			    if(WorkSegSet[1].ucSetRisTim > 1)   
			        WorkSegSet[1].ucSetRisTim--;
				else
					WorkSegSet[1].ucSetRisTim = 88;
		    }
			else if(flgFlashShow ==0x05)
			{
			    if(WorkSegSet[1].uiSetDryTmp > 280)   
			        WorkSegSet[1].uiSetDryTmp -= 10;
				else
					WorkSegSet[1].uiSetDryTmp = 880;
		    }
			else if(flgFlashShow ==0x06)
			{
			    if(WorkSegSet[1].uiSetWetTmp > 280)   
			        WorkSegSet[1].uiSetWetTmp -=  5;
				else
					WorkSegSet[1].uiSetWetTmp = 880;
			}
			else if(flgFlashShow ==0x07)
			{
			    if(WorkSegSet[1].ucSetStaTim > 1)    
			        WorkSegSet[1].ucSetStaTim--;
				else
					WorkSegSet[1].ucSetStaTim = 88;
			}

			/*������*/
			else if(flgFlashShow ==0x08)
			{
			    if(WorkSegSet[2].ucSetRisTim > 1)   
			        WorkSegSet[2].ucSetRisTim--;
				else
					WorkSegSet[2].ucSetRisTim = 88;
		    }
			else if(flgFlashShow ==0x09)
			{
			    if(WorkSegSet[2].uiSetDryTmp > 280)   
			        WorkSegSet[2].uiSetDryTmp -= 10;
				else
					WorkSegSet[2].uiSetDryTmp = 880;
		    }
			else if(flgFlashShow ==0x0a)
			{
			    if(WorkSegSet[2].uiSetWetTmp > 280)   
			        WorkSegSet[2].uiSetWetTmp -=  5;
				else
					WorkSegSet[2].uiSetWetTmp = 880;
			}
			else if(flgFlashShow ==0x0b)
			{
			    if(WorkSegSet[2].ucSetStaTim > 1)    
			        WorkSegSet[2].ucSetStaTim--;
				else
					WorkSegSet[2].ucSetStaTim = 88;
			}

			/*���Ķ�*/
			else if(flgFlashShow ==0x0c)
			{
			    if(WorkSegSet[3].ucSetRisTim > 1)   
			        WorkSegSet[3].ucSetRisTim--;
				else
					WorkSegSet[3].ucSetRisTim = 88;
		    }
			else if(flgFlashShow ==0x0d)
			{
			    if(WorkSegSet[3].uiSetDryTmp > 280)   
			        WorkSegSet[3].uiSetDryTmp -= 10;
				else
					WorkSegSet[3].uiSetDryTmp = 880;
		    }
			else if(flgFlashShow ==0x0e)
			{
			    if(WorkSegSet[3].uiSetWetTmp > 280)   
			        WorkSegSet[3].uiSetWetTmp -=  5;
				else
					WorkSegSet[3].uiSetWetTmp = 880;
			}
			else if(flgFlashShow ==0x0f)
			{
			    if(WorkSegSet[3].ucSetStaTim > 1)    
			        WorkSegSet[3].ucSetStaTim--;
				else
					WorkSegSet[3].ucSetStaTim = 88;
			}

			/*�����*/
			else if(flgFlashShow ==0x10)
			{
			    if(WorkSegSet[4].ucSetRisTim > 1)   
			        WorkSegSet[4].ucSetRisTim--;
				else
					WorkSegSet[4].ucSetRisTim = 88;
		    }
			else if(flgFlashShow ==0x11)
			{
			    if(WorkSegSet[4].uiSetDryTmp > 280)   
			        WorkSegSet[4].uiSetDryTmp -= 10;
				else
					WorkSegSet[4].uiSetDryTmp = 880;
		    }
			else if(flgFlashShow ==0x12)
			{
			    if(WorkSegSet[4].uiSetWetTmp > 280)   
			        WorkSegSet[4].uiSetWetTmp -=  5;
				else
					WorkSegSet[4].uiSetWetTmp = 880;
			}
			else if(flgFlashShow ==0x13)
			{
			    if(WorkSegSet[4].ucSetStaTim > 1)    
			        WorkSegSet[4].ucSetStaTim--;
				else
					WorkSegSet[4].ucSetStaTim = 88;
			}

			/*������*/
			else if(flgFlashShow ==0x14)
			{
			    if(WorkSegSet[5].ucSetRisTim > 1)   
			        WorkSegSet[5].ucSetRisTim--;
				else
					WorkSegSet[5].ucSetRisTim = 88;
		    }
			else if(flgFlashShow ==0x15)
			{
			    if(WorkSegSet[5].uiSetDryTmp > 280)   
			        WorkSegSet[5].uiSetDryTmp -= 10;
				else
					WorkSegSet[5].uiSetDryTmp = 880;
		    }
			else if(flgFlashShow ==0x16)
			{
			    if(WorkSegSet[5].uiSetWetTmp > 280)   
			        WorkSegSet[5].uiSetWetTmp -=  5;
				else
					WorkSegSet[5].uiSetWetTmp = 880;
			}
			else if(flgFlashShow ==0x17)
			{
			    if(WorkSegSet[5].ucSetStaTim > 1)    
			        WorkSegSet[5].ucSetStaTim--;
				else
					WorkSegSet[5].ucSetStaTim = 88;
			}

			/*���߶�*/
			else if(flgFlashShow ==0x18)
			{
			    if(WorkSegSet[6].ucSetRisTim > 1)   
			        WorkSegSet[6].ucSetRisTim--;
				else
					WorkSegSet[6].ucSetRisTim = 88;
		    }
			else if(flgFlashShow ==0x19)
			{
			    if(WorkSegSet[6].uiSetDryTmp > 280)   
			        WorkSegSet[6].uiSetDryTmp -= 10;
				else
					WorkSegSet[6].uiSetDryTmp = 880;
		    }
			else if(flgFlashShow ==0x1a)
			{
			    if(WorkSegSet[6].uiSetWetTmp > 280)   
			        WorkSegSet[6].uiSetWetTmp -=  5;
				else
					WorkSegSet[6].uiSetWetTmp = 880;
			}
			else if(flgFlashShow ==0x1b)
			{
			    if(WorkSegSet[6].ucSetStaTim > 1)    
			        WorkSegSet[6].ucSetStaTim--;
				else
					WorkSegSet[6].ucSetStaTim = 88;
			}

			/*�ڰ˶�*/
			else if(flgFlashShow ==0x1c)
			{
			    if(WorkSegSet[7].ucSetRisTim > 1)   
			        WorkSegSet[7].ucSetRisTim--;
				else
					WorkSegSet[7].ucSetRisTim = 88;
		    }
			else if(flgFlashShow ==0x1d)
			{
			    if(WorkSegSet[7].uiSetDryTmp > 280)   
			        WorkSegSet[7].uiSetDryTmp -= 10;
				else
					WorkSegSet[7].uiSetDryTmp = 880;
		    }
			else if(flgFlashShow ==0x1e)
			{
			    if(WorkSegSet[7].uiSetWetTmp > 280)   
			        WorkSegSet[7].uiSetWetTmp -=  5;
				else
					WorkSegSet[7].uiSetWetTmp = 880;
			}
			else if(flgFlashShow ==0x1f)
			{
			    if(WorkSegSet[7].ucSetStaTim > 1)    
			        WorkSegSet[7].ucSetStaTim--;
				else
					WorkSegSet[7].ucSetStaTim = 88;
			}

			/*�ھŶ�*/
			else if(flgFlashShow ==0x20)
			{
			    if(WorkSegSet[8].ucSetRisTim > 1)   
			        WorkSegSet[8].ucSetRisTim--;
				else
					WorkSegSet[8].ucSetRisTim = 88;
		    }
			else if(flgFlashShow ==0x21)
			{
			    if(WorkSegSet[8].uiSetDryTmp > 280)   
			        WorkSegSet[8].uiSetDryTmp -= 10;
				else
					WorkSegSet[8].uiSetDryTmp = 880;
		    }
			else if(flgFlashShow ==0x22)
			{
			    if(WorkSegSet[8].uiSetWetTmp > 280)   
			        WorkSegSet[8].uiSetWetTmp -=  5;
				else
					WorkSegSet[8].uiSetWetTmp = 880;
			}
			else if(flgFlashShow ==0x23)
			{
			    if(WorkSegSet[8].ucSetStaTim > 1)    
			        WorkSegSet[8].ucSetStaTim--;
				else
					WorkSegSet[8].ucSetStaTim = 88;
			}

			/*��ʮ��*/
			else if(flgFlashShow ==0x24)
			{
			    if(WorkSegSet[9].ucSetRisTim > 1)   
			        WorkSegSet[9].ucSetRisTim--;
				else
					WorkSegSet[9].ucSetRisTim = 88;
		    }
			else if(flgFlashShow ==0x25)
			{
			    if(WorkSegSet[9].uiSetDryTmp > 280)   
			        WorkSegSet[9].uiSetDryTmp -= 10;
				else
					WorkSegSet[9].uiSetDryTmp = 880;
		    }
			else if(flgFlashShow ==0x26)
			{
			    if(WorkSegSet[9].uiSetWetTmp > 280)   
			        WorkSegSet[9].uiSetWetTmp -=  5;
				else
					WorkSegSet[9].uiSetWetTmp = 880;
			}
			else if(flgFlashShow ==0x27)
			{
			    if(WorkSegSet[9].ucSetStaTim > 1)    
			        WorkSegSet[9].ucSetStaTim--;
				else
					WorkSegSet[9].ucSetStaTim = 88;
			}
		
		
		
		}//end of else if(ucSetupClassIndex == 0x02)

	}//end of if(enWorkMode == SET_MODE)
}

/**********************************************************
* Function Name  : vdFillDispBuff                         *
* Description    : ������ʾ���������                     *
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/
void vdFillDispBuff(void)
{
    unsigned char i;
	unsigned int Temp;

	if(flgTmpDispGist == flgMainSensor)      //MAINSENSOR     
	{
	    uiDispBuff[0] = uiDryTempVal;        //��ʾ��������
	    uiDispBuff[1] = uiWetTempVal;
	}
	else
    {
	    uiDispBuff[0] = uiAuxDryTempVal;     //��ʾ��������
	    uiDispBuff[1] = uiAuxWetTempVal;
	}

	uiDispBuff[2] = uiTargetDryTemp/10;        //Ŀ�����

	uiDispBuff[3] = uiTargetWetTemp/10;        //Ŀ��ʪ��;

	WetTempValCtrT= uiDispBuff[3];
	Temp = 0;
	for(i=0; i<ucCurvRunSegNum;i++)					  
	{
	  Temp = Temp + (WorkSeg[i].ucSetRisTim + WorkSeg[i].ucSetStaTim); 
	}
    /*�׶�ʣ��ʱ��,ÿ0.1Ϊ6����*/
	 Temp = Temp + ucSegCntTime;
    uiTotalRunHour = Temp;
	uiDispBuff[5] = uiTotalRunHour;              //������ʱ��
	if(enWorkMode == DEBUG_MODE)
	{
      if(uiAcCurVal_A<30)
	  {
       uiAcCurVal_A = 0;
	  }

	  if(uiAcCurVal_B<30)
	  {
       uiAcCurVal_B = 0;
	  }
	  if(uiAcCurVal_C<30)
	  {
       uiAcCurVal_C = 0;
	  }
 	    uiDispBuff[0] = uiAcCurVal_A*2.1;
	    uiDispBuff[1] = uiAcCurVal_B*2.1;
        uiDispBuff[2] = uiAcCurVal_C*2.1;
        uiDispBuff[5] = i16CtrAmount0Set;

	}
	if(enWorkMode == DEBUG_MasterAddressSet)
	{
	    uiDispBuff[0] = u16MasterAddressSet;
		uiDispBuff[1] = u16SlaveAddressSet*10;
	}
	if(enWorkMode == DEBUG_SlaveAddressSet)
	{
	    uiDispBuff[0] = u16MasterAddressSet;
		uiDispBuff[1] = u16SlaveAddressSet*10;
	}
    /*��ʾ������*/
	uiDispBuff[7] = ucKaofangNum;

    switch(enWorkMode)   
    {   
        case SET_MODE:     
			/*ʱ�䲿�����*/
			for(i=0; i<5;i++)                     
	            ucRTCDispBuff[i] = ucDateTimeSet[i];  
   			/*���߲��ּ�������*/
            ucCurveDispBuff[0] = 0x10;   //0x10:��ʾ���ߣ�0x01:��ʾ���0x11:���߼������ʾ��0x00:���߼���򶼲���ʾ
            ucCurveDispBuff[1] = 0x10;
	        ucCurveDispBuff[2] = 0x10;
	        ucCurveDispBuff[3] = 0x10;
            ucCurveDispBuff[ucCurveNumSet] = 0x11;  
			if(ucBakWorkMode == RUN_MODE)
			{
			    ucCurveDispBuff[0] = 0x00; 
                ucCurveDispBuff[1] = 0x00;
	            ucCurveDispBuff[2] = 0x00;
	            ucCurveDispBuff[3] = 0x00;
                ucCurveDispBuff[ucCurveNumSet] = 0x11;
			}
			/*��ʪ�ȡ�ʱ����򲿷����*/
            for(i=0; i<20; i++)
                ucSegTimRimDispBuff[i] = 0;			
            ucSegTimRimDispBuff[ucRealRunSegNum] = 0x01;  //��ʾ���

			for(i=0; i<10; i++)
			{
			    ucDryTmpRimDispBuff[i] = 0;
                ucWetTmpRimDispBuff[i] = 0;
            }

			/*�ϡ����ﲿ�ּ�������*/
            ucUpDownDispBuff[0] = 0x02;   //0x02:��ʾ"����"��0x01:��ʾ���0x03:��ʾ������ݣ�0x00:���Ｐ��򶼲���ʾ
            ucUpDownDispBuff[1] = 0x02;   //0x02:��ʾ"����"��0x01:��ʾ���0x03:��ʾ������ݣ�0x00:���Ｐ��򶼲���ʾ
            ucUpDownDispBuff[flgMainSensorSet] = 0x03;
			break;
        case RUN_MODE:
			/*ʱ�䲿�����*/
			for(i=0; i<5;i++)                     
	            ucRTCDispBuff[i] = ucDateTime[i];    
           
			/*���߲��ּ�������*/
            ucCurveDispBuff[0] = 0x00;   //0x10:��ʾ���ߣ�0x01:��ʾ���0x11:���߼������ʾ��0x00:���߼���򶼲���ʾ
            ucCurveDispBuff[1] = 0x00;
	        ucCurveDispBuff[2] = 0x00;
	        ucCurveDispBuff[3] = 0x00;
            ucCurveDispBuff[ucCurveNum] = 0x11;
            
			/*ʱ����򲿷����*/
            for(i=0; i<20; i++)
                ucSegTimRimDispBuff[i] = 0;			
            ucSegTimRimDispBuff[ucRealRunSegNum] = 0x01;  //��ʾ���
			
			/*��ʪ����򲿷����*/
            for(i=0; i<10; i++)
			{
			    ucDryTmpRimDispBuff[i] = 0;
                ucWetTmpRimDispBuff[i] = 0;
            }
            if(ucRealRunSegNum == 0x01)  //��Ӧ��ϵ��0,1(0)2,3(1)4,5(2)6,7(3)........10,11(5),18,19(9)
			    ucCurvRunSegNum = 0;
			else if(ucRealRunSegNum % 2 ==0)
			    ucCurvRunSegNum = ucRealRunSegNum / 2;
			
			else //����
			    ucCurvRunSegNum = (ucRealRunSegNum -1) / 2;
			  
			ucDryTmpRimDispBuff[ucCurvRunSegNum] = 0x01;  //��ʾ���;
            ucWetTmpRimDispBuff[ucCurvRunSegNum] = 0x01;  //��ʾ���;   


			/*�ϡ����ﲿ�ּ�������*/
            ucUpDownDispBuff[0] = 0x00;   //0x02:��ʾ"����"��0x01:��ʾ���0x03:��ʾ������ݣ�0x00:���Ｐ��򶼲���ʾ
            ucUpDownDispBuff[1] = 0x00;   //0x02:��ʾ"����"��0x01:��ʾ���0x03:��ʾ������ݣ�0x00:���Ｐ��򶼲���ʾ
            
			if(flgUpDownFlashShow) 
			    ucUpDownDispBuff[flgMainSensor^ 0x01] = 0x03;
            else
			    ucUpDownDispBuff[flgMainSensor] = 0x03;

			break;
        
		case STOP_MODE:
			/*ʱ�䲿�����*/
			for(i=0; i<5;i++)                     
	            ucRTCDispBuff[i] = ucDateTime[i];   
				
			/*���߲�����䣬�������*/
			ucCurveDispBuff[0] = 0x10;  
            ucCurveDispBuff[1] = 0x10;
	        ucCurveDispBuff[2] = 0x10;
	        ucCurveDispBuff[3] = 0x10;
            ucCurveDispBuff[ucCurveNum] = 0x11;

			/*��ʪ�ȡ�ʱ��������*/
            for(i=0; i<20; i++)
                ucSegTimRimDispBuff[i] = 0;			
            ucSegTimRimDispBuff[ucRealRunSegNum] = 0x01;

			for(i=0; i<10; i++)
			{
			    ucDryTmpRimDispBuff[i] = 0;
                ucWetTmpRimDispBuff[i] = 0;
            }

			/*�ϡ����ﲿ�ּ�������*/
            ucUpDownDispBuff[0] = 0x02;   //0x02:��ʾ"����"��0x01:��ʾ���0x03:��ʾ������ݣ�0x00:���Ｐ��򶼲���ʾ
            ucUpDownDispBuff[1] = 0x02;   //0x02:��ʾ"����"��0x01:��ʾ���0x03:��ʾ������ݣ�0x00:���Ｐ��򶼲���ʾ
            
			if(flgUpDownFlashShow) 
			    ucUpDownDispBuff[flgMainSensor^ 0x01] = 0x03;  //
            else
			    ucUpDownDispBuff[flgMainSensor] = 0x03;
			break;
        case QUERY_MODE: 

		    /*ʱ�䲿�����*/
			for(i=0; i<5;i++)                     
	            ucRTCDispBuff[i] = ucDateTimeSet[i];
			uiDispBuff[0] = uiQueryData[0];           //��ѯ����
            uiDispBuff[1] = uiQueryData[1];           //��ѯʪ��
			uiDispBuff[2] = uiQueryData[2]/10;           //��ѯĿ�����
            uiDispBuff[3] = uiQueryData[3]/10;           //��ѯĿ��ʪ��;
            uiDispBuff[5] = uiTotalQueryHour;         //��ѯʱ��
			uiDispBuff[7] = ucKaofangNumQuery;
            		    
            //asm("NOP");
            break;
        default:
		    break;

    }
	/*�����������*/
    if(enWorkMode == SET_MODE)
	{
	    for(i=0; i<10; i++)
	    {
	        WorkSegDispBuff[i].ucSetRisTim = WorkSegSet[i].ucSetRisTim;
	        WorkSegDispBuff[i].uiSetDryTmp = WorkSegSet[i].uiSetDryTmp;
            WorkSegDispBuff[i].uiSetWetTmp = WorkSegSet[i].uiSetWetTmp;
		    WorkSegDispBuff[i].ucSetStaTim = WorkSegSet[i].ucSetStaTim;
        }
	
	}
	else
	{
	    for(i=0; i<10; i++)
	    {
	        WorkSegDispBuff[i].ucSetRisTim = WorkSeg[i].ucSetRisTim;
	        WorkSegDispBuff[i].uiSetDryTmp = WorkSeg[i].uiSetDryTmp;
            WorkSegDispBuff[i].uiSetWetTmp = WorkSeg[i].uiSetWetTmp;
		    WorkSegDispBuff[i].ucSetStaTim = WorkSeg[i].ucSetStaTim;
        } 
	}
	if(ucRealRunSegNum % 2 ==0 )   //ż�������£�����������
	{
	    ucOtherDispBuff[SHENGWEN] = 0x01;
		ucOtherDispBuff[HENGWEN]  = 0x00; 
	}
	else
	{
	    ucOtherDispBuff[SHENGWEN] = 0x00;
		ucOtherDispBuff[HENGWEN]  = 0x01;     
    }
	ucOtherDispBuff[PIANWEN]     = flgPianWen;                      //
	ucOtherDispBuff[GUOZAI]      = flgOverCurProt;                  //����
	ucOtherDispBuff[QUEXIANG]    = flgFengjiLackPh | flgPwrLackPhase;
	ucOtherDispBuff[ZHURAN]      = flgFanOn;
	if(WetTempValCtrT>=WetTempValCtr)
    {
        ucOtherDispBuff[PAISHI] = 0x00;
	}
	else
	{
	    ucOtherDispBuff[PAISHI] = 0x01;
	}
	ucOtherDispBuff[XUNZIDONG]   = 0x00;                            //ѭ����� �Զ�������ʾ;
    ucOtherDispBuff[XUNGAO]      = (flgXunHuanGaoDi & 0x02) >> 1;        //����
	ucOtherDispBuff[XUNDI]       = flgXunHuanGaoDi & 0x01;               //����

	    
    /*ʪ�ȵ�dot 5 ��ʾ����*/
	for(i=0; i<10; i++)
   {
	    if((WorkSegDispBuff[i].uiSetWetTmp % 10) != 0)
		    ucDot5DispBuff[i]  = 0x01;
        else
	        ucDot5DispBuff[i]  = 0x00;
	}
    /*�׶�ʣ��ʱ��,ÿ0.1Ϊ6����*/
	if(ucRealRunSegNum % 2 ==0)   //���½׶�
	{ 
	    if(WorkSeg[ucCurvRunSegNum].ucSetRisTim > ucSegCntTime)
	        uiDispBuff[4] = WorkSeg[ucCurvRunSegNum].ucSetRisTim * 10 - ucSegCntTime *10 - ucSixMinCnt;
        else
		    uiDispBuff[4] = 0;
	}
	else                          //���½׶�
	{
	    if(ucSegCntTime<WorkSeg[ucCurvRunSegNum].ucSetRisTim)
        {     ucSegCntTime = WorkSeg[ucCurvRunSegNum].ucSetRisTim;
    		IICaddr = SEGTIME;
	    	IICdatt =((ucSegCntTime)&0xFF);
		    WordTo24C();
		    _delay_ms(4);
		}
	    if((WorkSeg[ucCurvRunSegNum].ucSetRisTim + WorkSeg[ucCurvRunSegNum].ucSetStaTim)> ucSegCntTime)
		    uiDispBuff[4] = (WorkSeg[ucCurvRunSegNum].ucSetRisTim + WorkSeg[ucCurvRunSegNum].ucSetStaTim) * 10 - ucSegCntTime *10 - ucSixMinCnt; 
		else
		    uiDispBuff[4] = 0; 
	}

    uiDispBuff[6] = uiAcVolVal;              //������ѹ
	uiDispBuff[6] =((uiDispBuff[6]/100) << 8) + ((uiDispBuff[6] %100 / 10) << 4)  + (( uiDispBuff[6] % 10) & 0x000F); 
  
    vdDeciToBcd();
}
/**********************************************************
* Function Name  : vdDeciToBcd                            *
* Description    : ʮ����ת��ΪBCD��                      *
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/
void vdDeciToBcd(void)
{
    unsigned char i;
    /*DEC TO BCD����Ϊ����ʾ��˸Ч������*/
    /*DEC TO BCD*/
	for(i=0;i<5;i++)
	    ucRTCDispBuff[i] =  (( ucRTCDispBuff[i] / 10) << 4)  + (( ucRTCDispBuff[i] % 10) & 0x0F); 
	
    /*DEC TO BCD����Ϊ����ʾ��˸Ч������*/
	for(i=0; i<10; i++)
	{

	    WorkSegDispBuff[i].ucSetRisTim =  ((WorkSegDispBuff[i].ucSetRisTim/100) << 8) + ((WorkSegDispBuff[i].ucSetRisTim %100 / 10) << 4)  + (( WorkSegDispBuff[i].ucSetRisTim % 10) & 0x000F); 
        WorkSegDispBuff[i].ucSetRisTim &= 0x00FF;

	    WorkSegDispBuff[i].uiSetDryTmp =  ((WorkSegDispBuff[i].uiSetDryTmp/100) << 8) + ((WorkSegDispBuff[i].uiSetDryTmp %100 / 10) << 4)  + (( WorkSegDispBuff[i].uiSetDryTmp % 10) & 0x000F); 
        WorkSegDispBuff[i].uiSetDryTmp &= 0x0FFF;

		WorkSegDispBuff[i].uiSetWetTmp =  ((WorkSegDispBuff[i].uiSetWetTmp/100) << 8) + ((WorkSegDispBuff[i].uiSetWetTmp %100 / 10) << 4)  + (( WorkSegDispBuff[i].uiSetWetTmp % 10) & 0x000F); 
        WorkSegDispBuff[i].uiSetWetTmp &= 0x0FFF;

		WorkSegDispBuff[i].ucSetStaTim =  ((WorkSegDispBuff[i].ucSetStaTim/100) << 8) + ((WorkSegDispBuff[i].ucSetStaTim %100 / 10) << 4)  + (( WorkSegDispBuff[i].ucSetStaTim % 10) & 0x000F); 
        WorkSegDispBuff[i].ucSetStaTim &= 0x00FF;
	}

	uiDispBuff[5]  =((uiDispBuff[5]/100) << 8) + ((uiDispBuff[5] %100 / 10) << 4)  + (( uiDispBuff[5] % 10) & 0x000F); 
	uiDispBuff[5] &= 0x0FFF;


	switch(enWorkMode)
	{
	    case SET_MODE:
		    
			if(ucCurveNumSet == SELFSET)
			{   
				for(i = ucSelfCurvRunSegMaxNumSet +1; i<10; i++)
			    {
			        WorkSegDispBuff[i].ucSetRisTim = 0x0aa;
			        WorkSegDispBuff[i].uiSetDryTmp = 0xaaa;
				    WorkSegDispBuff[i].uiSetWetTmp = 0xaaa;
				    ucDot5DispBuff[i]  = 0x00;  //�ر�.5��ʾ
				    WorkSegDispBuff[i].ucSetStaTim = 0x0aa;	
			    }
			}
			break;
        case STOP_MODE:
		    
			if(ucCurveNum == SELFSET)
			{ 
				for(i = ucSelfCurvRunSegMaxNum +1; i<10; i++)
				{
				    WorkSegDispBuff[i].ucSetRisTim = 0x0aa;
				    WorkSegDispBuff[i].uiSetDryTmp = 0xaaa;
					WorkSegDispBuff[i].uiSetWetTmp = 0xaaa;
					ucDot5DispBuff[i]  = 0x00;  //�ر�.5��ʾ
					WorkSegDispBuff[i].ucSetStaTim = 0x0aa;	
				}
			}
			break;
        case RUN_MODE:

		    if(ucCurveNum == SELFSET) 
			{
				for(i = ucCurvRunSegNum +1; i<10; i++)
				{
				    WorkSegDispBuff[i].ucSetRisTim = 0x0aa;
				    WorkSegDispBuff[i].uiSetDryTmp = 0xaaa;
					WorkSegDispBuff[i].uiSetWetTmp = 0xaaa;
					ucDot5DispBuff[i]  = 0x00;  //�ر� .5��ʾ
					WorkSegDispBuff[i].ucSetStaTim = 0x0aa;	
				}
			}
			break;
            
		default:
			    
		    break;
	}//end of switch(enWorkMode)
}

/**********************************************************
* Function Name  : vdReadDatFrEep                         *
* Description    : ���ڲ�eeprom�洢��������               *
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/
void vdReadDatFrEep(unsigned char quxianhao)
{
    unsigned char i;
   //ԭʼ����	
   unsigned int  addr; 
   //д�洢����������
   for(i=0; i<10; i++)
   {
	    // 0:����ģʽ��1���²�Ҷ��2���в�Ҷ��3���ϲ�Ҷ
	    addr =(quxianhao)*80+i*8;      
		
		IICaddr=addr+IEEP_DATA_ADDRESS;
  		WordFrom24C();          
		WorkSegSet[i].ucSetRisTim =IICdatt;
		_delay_us(5);
       
	   	IICaddr=addr+IEEP_DATA_ADDRESS+2;
  		WordFrom24C();          
		WorkSegSet[i].uiSetDryTmp =IICdatt;
		_delay_us(5);
		
		IICaddr=addr+IEEP_DATA_ADDRESS+4;
  		WordFrom24C();          
		WorkSegSet[i].uiSetWetTmp =IICdatt;
		_delay_us(5);
		
		IICaddr=addr+IEEP_DATA_ADDRESS+6;
  		WordFrom24C();          
		WorkSegSet[i].ucSetStaTim =IICdatt; 
		_delay_us(5);
	}
    for(i=0; i<10; i++)
	{
	    WorkSeg[i].ucSetRisTim = WorkSegSet[i].ucSetRisTim;
	    WorkSeg[i].uiSetDryTmp = WorkSegSet[i].uiSetDryTmp;
        WorkSeg[i].uiSetWetTmp = WorkSegSet[i].uiSetWetTmp;
	    WorkSeg[i].ucSetStaTim = WorkSegSet[i].ucSetStaTim;
    }
}

/**********************************************************
* Function Name  : vdSaveDatToEep                         *
* Description    : ���ڲ�eeprom�洢��������               *
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/
void vdSaveDatToEep(unsigned char quxianhao)
{
    unsigned char i;
	unsigned int  addr;
    
	/*д�洢����������*/
	for(i=0; i<10; i++)
	{
	    /* 0:����ģʽ��1���²�Ҷ��2���в�Ҷ��3���ϲ�Ҷ */
	    addr = (quxianhao)*80+i*8+IEEP_DATA_ADDRESS;     
		                   
		IICaddr=addr;
  		IICdatt=WorkSegSet[i].ucSetRisTim;
		WordTo24C();
		_delay_ms(5);          
       
		IICaddr=addr+2;
  		IICdatt=WorkSegSet[i].uiSetDryTmp;
		WordTo24C();
		_delay_ms(5);    
		
		IICaddr=addr+4;
  		IICdatt=WorkSegSet[i].uiSetWetTmp;
		WordTo24C();
		_delay_ms(5);  
		  
		IICaddr=addr+6;
  		IICdatt=WorkSegSet[i].ucSetStaTim;
		WordTo24C();
		_delay_ms(5);              
	}
}
/**********************************************************
* Function Name  : vdSaveDatToEep                         *
* Description    : ���ڲ�eeprom�洢��������               *
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/
void vdChangeDatToOriginal()
{
    unsigned char i;
	unsigned int  addr;

	ucRealRunSegNum = 0;
	RunStopState = 0;
	uiTotalRunHour = 0;
	IICaddr= RUNSTATE_SAVE_ADDRESS;//��ʼ��λ�洢�����š�����������ߺ�
  	IICdatt=((ucRealRunSegNum)&0x7f)+(RunStopState<<7);
	IICdatt = IICdatt << 8;
	IICdatt = IICdatt + (uiTotalRunHour&0xff);
	WordTo24C();
	_delay_ms(4);

	ucSegCntTime = 0;
	IICaddr= SEGTIME;
  	IICdatt=((ucSegCntTime)&0xFF);
	WordTo24C();
	_delay_ms(4);

	IICaddr= CtrAmount0;
  	IICdatt= 2;
	WordTo24C();
	_delay_ms(4);
	
	/*д�洢����������*/
	for(i=0; i<10; i++)
	{
	    /* 0:����ģʽ��1���²�Ҷ��2���в�Ҷ��3���ϲ�Ҷ */
	    addr = i*8+IEEP_DATA_ADDRESS;     
		                   
		IICaddr=addr;
  		IICdatt=CurveNumzishe[i][0];
		WordTo24C();
		_delay_ms(5);          

		IICaddr=addr+2;
  		IICdatt=CurveNumzishe[i][1];
		WordTo24C();
		_delay_ms(5);    
		
		IICaddr=addr+4;
  		IICdatt=CurveNumzishe[i][2];
		WordTo24C();
		_delay_ms(5);  
		  
		IICaddr=addr+6;
  		IICdatt=CurveNumzishe[i][3];
		WordTo24C();
		_delay_ms(5);              
	}
	for(i=0; i<10; i++)
	{
	    /* 0:����ģʽ��1���²�Ҷ��2���в�Ҷ��3���ϲ�Ҷ */
	    addr = (1)*80+i*8+IEEP_DATA_ADDRESS;     
		                   
		IICaddr=addr;
  		IICdatt=CurveNumxia[i][0];
		WordTo24C();
		_delay_ms(5);          
       
		IICaddr=addr+2;
  		IICdatt=CurveNumxia[i][1];
		WordTo24C();
		_delay_ms(5);    
		
		IICaddr=addr+4;
  		IICdatt=CurveNumxia[i][2];
		WordTo24C();
		_delay_ms(5);  
		  
		IICaddr=addr+6;
  		IICdatt=CurveNumxia[i][3];
		WordTo24C();
		_delay_ms(5);              
	}
	for(i=0; i<10; i++)
	{
	    /* 0:����ģʽ��1���²�Ҷ��2���в�Ҷ��3���ϲ�Ҷ */
	    addr = ( 2)*80+i*8+IEEP_DATA_ADDRESS;     
		                   
		IICaddr=addr;
  		IICdatt=CurveNumzhong[i][0];
		WordTo24C();
		_delay_ms(5);          
       
		IICaddr=addr+2;
  		IICdatt=CurveNumzhong[i][1];
		WordTo24C();
		_delay_ms(5);    
		
		IICaddr=addr+4;
  		IICdatt=CurveNumzhong[i][2];
		WordTo24C();
		_delay_ms(5);  
		  
		IICaddr=addr+6;
  		IICdatt=CurveNumzhong[i][3];
		WordTo24C();
		_delay_ms(5);              
	}
	for(i=0; i<10; i++)
	{
	    /* 0:����ģʽ��1���²�Ҷ��2���в�Ҷ��3���ϲ�Ҷ */
	    addr = (3)*80+i*8+IEEP_DATA_ADDRESS;     
		                   
		IICaddr=addr;
  		IICdatt=CurveNumshang[i][0];
		WordTo24C();
		_delay_ms(5);          
       
		IICaddr=addr+2;
  		IICdatt=CurveNumshang[i][1];
		WordTo24C();
		_delay_ms(5);    
		
		IICaddr=addr+4;
  		IICdatt=CurveNumshang[i][2];
		WordTo24C();
		_delay_ms(5);  
		  
		IICaddr=addr+6;
  		IICdatt=CurveNumshang[i][3];
		WordTo24C();
		_delay_ms(5);              
	}



	IICaddr = 0x0000;
	IICdatt=((ucKaofangNum)<<8)+0x0011;
	WordTo24C();
	_delay_ms(5);  
				
	//���Ź���
	IICaddr=0x0000;
	uiCnt1000ms=0;
	WordFrom24C();
	ucKaofangNum =(unsigned char)(IICdatt>>8);
	if(ucKaofangNum >= 10)
	    ucKaofangNum = 0;

	ucCurveNum =(unsigned char)(IICdatt&0x0f);//��Ϊ��������
	if(ucCurveNum > 3)  //�ܹ��������ߣ���0��ʼ
        ucCurveNum = 1;

	flgMainSensor = (IICdatt&0xf0)>>4;//��Ϊ��������
	if(flgMainSensor > 1)  //ֻ����·����������0��ʼ
	    flgMainSensor = DWSENSOR;
    flgTmpDispGist = flgMainSensor;

	i16CtrAmount0 = 2;

}

/**********************************************************
* Function Name  : vdSaveRunDatExEep                      *
* Description    : ����ʱÿСʱ���ⲿ�洢���������       *
*                  ÿ���޶�200Сʱ���洢��50ҳһ��        *
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/
void vdSaveRunDatExEep(unsigned char kaohao,unsigned char uchour)
{
    unsigned char i;
	unsigned int  addr;
	unsigned int  waitdat[8];
	
	/*1���洢���ڡ�ʱ��*/
	if( uiTotalRunHour <= 200 ) 
	{
	    waitdat[0] =  ucDateTime[1]<<8;   //����
		waitdat[0] =  waitdat[0]+ ucDateTime[0];

		waitdat[1] =  ucDateTime[3]<<8;   //����
		waitdat[1] =  waitdat[1]+ ucDateTime[2];

		waitdat[2] =  (unsigned char)ucDateTime[4];   //��

        waitdat[3] =  uiDryTempVal;
		waitdat[4] =  uiWetTempVal;

		waitdat[5] =  uiTargetDryTemp;
		waitdat[6] =  uiTargetWetTemp;

        waitdat[7] =  flgFengjiLackPh+flgOverCurProt<<1+flgOverVol<<2+flgUnderVol<<3;   //��4λ:���������ϣ���4λ��ȱ�ࣻ

		addr=IEEP_RESULT_ADDRESS+kaohao * KAONUM_PAGE_NUM * AT24CXXX_PAGE_SIZE + uchour*16;

		for(i=0;i<8;i++)
		{
			IICaddr=addr+i*2;
  			IICdatt=waitdat[i];
			WordTo24C();
			_delay_ms(5);
		}	
	}
}
/**********************************************************
* Function Name  : vdSaveRunDatExEep                      *
* Description    : ����ʱÿСʱ���ⲿ�洢���������       *
*                  ÿ���޶�200Сʱ���洢��50ҳһ��        *
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/
void vdReadRunDatFrExEep(unsigned char kaohao,unsigned char uchour)
{
    unsigned char i;
	unsigned int readdat[8];
    unsigned int  addr;

	addr=IEEP_RESULT_ADDRESS+ucKaofangNumQuery * KAONUM_PAGE_NUM * AT24CXXX_PAGE_SIZE + uchour*16;

	for(i=0;i<8;i++)
	{
		IICaddr=addr + 2*i;
  		WordFrom24C();          
		_delay_ms(5);
		readdat[i] =IICdatt;
	}
    ucDateTimeSet[0] = (unsigned char)(readdat[0]&0xff);   //����
	ucDateTimeSet[1] = (unsigned char)(readdat[0]>>8);   //Сʱ
	ucDateTimeSet[2] = (unsigned char)(readdat[1]&0x00ff);;   //��
	ucDateTimeSet[3] = (unsigned char)(readdat[1]>>8);   //Сʱ;   //��
	ucDateTimeSet[4] = (unsigned char)readdat[2];   //��
    uiQueryData[0]     = readdat[3];

	if((readdat[3]>40)&&(readdat[3]<800))
	   	 uiQueryData[0] = readdat[3];
    else
	     uiQueryData[0]	 = 0;

	if((readdat[4]>40)&&(readdat[4]<800))
	   	 uiQueryData[1] = readdat[4];
    else
	     uiQueryData[1]	 = 0;
    
	uiQueryData[2]     = readdat[5];
    uiQueryData[3]     = readdat[6];
}

/**********************************************************
* Function Name  : vdVolAlarm                             *
* Description    : ��ѹ����������                         *
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/
void vdVolProc(void)
{
   if((uiAcVolVal >= ProtectVoltageMax)||(uiAcVolVal < ProtectVoltageMin))    //170
   {
	  if(alarm_cnt < 60 )
	  {
	       alarm_cnt++;
	       flgOverVol  = 0x00;
	       flgUnderVol = 0x00; 
	   }
	  else                     
	  {
	     flgVolProt = 0x01;   //��ѹ��Ƿѹ�ﵽʱ��󱣻�����
	     if((uiAcVolVal >= ProtectVoltageMax)&&(!flgOverVol))
	     {
		      flgOverVol  = 0x01;            
			  VoicePlay(VOL_HIGH);
		 }
		 if((uiAcVolVal < ProtectVoltageMin)&&(!flgUnderVol))
		 {
		      flgUnderVol = 0x01;            
			  VoicePlay(VOL_LOW); 
         }
		 if(uiAcVolVal>660)
		 {
		   flgVolProt = 0x00;
		   uiAcVolVal = 0;
		 }
	  }
	  if((uiAcVolVal <(ProtectVoltageMin-10))&&(uiAcVolVal >=1))
	  {
	      alarm_cnt = 60;
	      flgUnderVol = 0x01;            //VoicePlay(UNDERVOL); 
	      flgVolProt = 0x01; 
	  }
	  if(uiAcVolVal <=1)
	  {
	       flgVolProt = 0x00;	  
	  }
	  if(uiAcVolVal >= (ProtectVoltageMax+10))
	  {
		  flgOverVol  = 0x01;            //VoicePlay(OVERVOL);
		  alarm_cnt = 60;
		  flgVolProt = 0x01; 
      }	  
   }
   else if((uiAcVolVal< ProtectVoltageMax)&&(uiAcVolVal >= ProtectVoltageMin))  //175,��ѹ������3���ӻָ���ȡ������
   {
    	if(alarm_cnt > 0)
		   alarm_cnt--;
        else
		{
		   flgVolProt = 0x00;
		   flgOverVol  = 0x00;
		   flgUnderVol = 0x00;
		}
	}
}
/**********************************************************
* Function Name  : vdCurProc                              *
* Description    : ��������������                         *
*                                                         *
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/
void vdCurProc(void)
{
    /* 4��������û�е������жϷ����û��������������*/
    if((uiAcCurVal_A < 30)&&(uiAcCurVal_C < 30)&&(uiAcCurVal_B<30))  
	{
	    flgFengjiNoCur = 0x01;
		FengJiRunTimeFlag = 1;

	}
	else
	{
	    flgFengjiNoCur = 0x00;
	}

	if(uiAcCurVal_A<30)
	{
       uiAcCurVal_A = 0;
	}

	if(uiAcCurVal_B<30)
	{
       uiAcCurVal_B = 0;
	}
	if(uiAcCurVal_C<30)
	{
       uiAcCurVal_C = 0;
	}

	if(flgXunHuanGaoDiPreDelay<3)
    {
	    if(((uiAcCurVal_A <11)&&(uiAcCurVal_C >=100))||((uiAcCurVal_A>=100)&&(uiAcCurVal_C <11))||((uiAcCurVal_A>=100)&&(uiAcCurVal_B <11))||((uiAcCurVal_B>=100)&&(uiAcCurVal_A <11))||(uiAcCurVal_A>(uiAcCurVal_C+ucProtCurValErr))||(uiAcCurVal_C>(uiAcCurVal_A+ucProtCurValErr))||(uiAcCurVal_A>(uiAcCurVal_B+ucProtCurValErr+25))||(uiAcCurVal_B>(uiAcCurVal_A+ucProtCurValErr+25))) 
		{
			 if((!flgFengjiLackPh)&&(1 == u16CurrUpdateFlag)&&(ucFengjiLackPhCnt<3))
		     {  
			        flgFengjiLackPh = 0x01;   //ȱ��ﵽʱ��󱣻�����
					ucFengjiLackPhCnt = 0x00;  //���������
					if(1 == FlagPowerStatus)
					VoicePlay(FEGNJI_LACK);
		     }
		}
		else
		{
			ucFengjiLackPhCnt = 0x00;  //���������
		}
	}
     	if((uiAcCurVal_A > ucProtCurVal)||(uiAcCurVal_B > (ucProtCurVal+50))||(uiAcCurVal_C > ucProtCurVal))
     	{
     		 if((uiAcCurVal_A>=650)||(uiAcCurVal_B>=650)||(uiAcCurVal_C>=650))
     		 {
     		  flgOverCurProt = 0;
     		  ucOverCurCnt = 0;
			  ucOverCurCnt1 = 0;
     		 }
     	     if( ucOverCurCnt1 < 15)
     			 ucOverCurCnt1++;
     		 else if((!flgOverCurProt)&&(!flgFengjiLackPh))
			 {
		     	 flgOverCurProt = 0x01; 	 //����(����)�ﵽʱ��󱣻�����
     			 ucAlarmTimCnt = 0;
     			 VoicePlay(FENGJI_OVER);
	 
			 }
     		 if( ucOverCurCnt < 1 )
     			 ucOverCurCnt++;
     		 else if((!flgOverCurProt)&&(!flgFengjiLackPh)&&(ucFengjiLackPhCnt<3))
     		 {
     			 flgFengjiLackPh = 0x01; 	 //����(����)�ﵽʱ��󱣻�����
     			 ucAlarmTimCnt = 0;
     			 VoicePlay(FEGNJI_LACK);
     		 }
     	}
     	else
        {
		   ucOverCurCnt = 0x00;
		   ucOverCurCnt1 = 0x00;
		}
	
}

/**********************************************************
* Function Name  : vdDryAlarm                             *
* Description    : ���±�������                           *
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/
void vdDryAlarm(void)
{
	if((uiDryTempVal > (uiTargetDryTemp /10 + 20)))  
	{
	    if( over_cnt < 150 )     //����150 * 4s = 600s/60 = 10min
		    over_cnt++;
		else  
		{   
		    flgDryHigh = 0x01;
			over_cnt = 0; 
	    }
	}
	else
	{
		over_cnt = 0;
		flgDryHigh = 0;
	}
	if((uiDryTempVal < (uiTargetDryTemp /10  - 20)))
	{
	    if( over_cnt1 < 150 )     //����150 * 4s = 600s/60 = 10min
		    over_cnt1++;
		else  
		{   
		    flgDryLow = 0x01;
			over_cnt1 = 0; 
	    }
	}
	else
	{
		    over_cnt1 = 0;
			flgDryLow = 0x00;
	}
}
/**********************************************************
* Function Name  : vdWetAlarm                             *
* Description    : ʪ�±�������                           *
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/

void vdWetAlarm(void)
{
	if((uiWetTempVal > (uiTargetWetTemp/10 + 10)))  
	{
	   if( over_cnt2 < 150 )     //����150 * 4s = 600s/60 = 10min
	     over_cnt2++;
	   else  
	   {   
		    flgWetHigh = 0x01;
			over_cnt2 = 0; 
	   }
	}
	else
	{
	    over_cnt2 = 0;
		flgWetHigh = 0;
	}

	if(uiWetTempVal < (uiTargetWetTemp /10  - 10))  
	{
	   if( over_cnt3 < 150 )     //����150 * 4s = 600s/60 = 10min
	        over_cnt3++;
	   else  
	   {   
		    flgWetLow = 0x01;
			over_cnt3 = 0; 
	   }
	}
	else
	{
	    over_cnt3 = 0;
		flgWetLow = 0;
	}
}
void vdGetXunHuanGaodiFlag(void)
{
	    if(flgXunHuanGaoDiCntPre>=4)
	    {
	     flgXunHuanGaoDi = 0x01;
		 if((flgXunHuanGaoDiPre!=flgXunHuanGaoDi)&&(!flgFengjiNoCur))
		 {
			 flgXunHuanGaoDiPreDelay = 30;
		 }
	    }
  	    else
	    {
	      flgXunHuanGaoDi = 0x02;
		  if((flgXunHuanGaoDiPre!=flgXunHuanGaoDi)&&(!flgFengjiNoCur))
		  {
		  	  flgXunHuanGaoDiPreDelay = 30;
		  }
	    }
	 if((flgFengjiNoCur==0x01)||(enWorkMode  == STOP_MODE))
	 {
	    flgXunHuanGaoDi=0x00;
	 }

     flgXunHuanGaoDiPre = flgXunHuanGaoDi; 

}
void vdGetFengmenCtrModFlag(void)
{
  	  uiFengmenCtrMod = uiGaugeACC(ACV_SAMP_FengmenCtrMod);
	  if(uiFengmenCtrMod >100)
		 FengmenCtrMod  =  1;
	  else
		 FengmenCtrMod  =  0;
      if((FengmenCtrMod!=FengmenCtrModPre)&&(1 == FlagPowerStatus)&&(1 == FengmenCtrMod)&&(flgWindRun == 0x00))
	  {
		  	  WIND_CLOS;
			  flgWindRun = 0x01; //��־���ŵ��������ת
			  uiWindCnt  = 650;//FengmenAngleN-300;  
			  uiFmNonCnt = 0;
			  FengmenAngleN =0;
	  }
 	  FengmenCtrModPre = FengmenCtrMod;
}
void vdSampCoalMotorOverload(void)
{

      unsigned int Temp;
	  Temp = i16CtrAmount3*10;
  	  u16CoalMotorOverLoad = uiGaugeACC(ACV_SAMP_CoalMotorOverload);
	  if(u16CoalMotorOverLoad > Temp)
		 u16CoalMotorOverLoadFlag  =  1;
	  else
		 u16CoalMotorOverLoadFlag  =  0;
}
void AutoAddCoalMotorControlTimeChange(void)  //�Զ���ú�������ϵͳ
{
    unsigned int uiBakTargetDryTemp;
	uiBakTargetDryTemp = uiTargetDryTemp/10;
    if(uiBakTargetDryTemp >= (uiDryTempVal + 20))
	{
		 i16DryTempErrScale = 3;                    //ֹͣ���
	}
    else if(uiBakTargetDryTemp >= (uiDryTempVal + 5))

    {
         i16DryTempErrScale = 2;     
	}
	else 
    {
         i16DryTempErrScale = 1;     
	}
	
    if(uiBakTargetDryTemp>=580)
    {
    	i16DryTempScale = 2;
    }
	else if(uiBakTargetDryTemp>=420)
    {
         i16DryTempScale = 1;
	}
	else
	{	
	     i16DryTempScale = 0;
	}
    i16GeneralAmount = u16CoalMotorCntrTable[i16DryTempScale][0];
	i16CompensateAmount = u16CoalMotorCntrTable[i16DryTempScale][i16DryTempErrScale];
}
