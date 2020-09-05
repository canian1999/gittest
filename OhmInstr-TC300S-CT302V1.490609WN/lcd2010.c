   
/******************** (C) COPYRIGHT 2009 zke ***********************************
* File Name          : lcd2010.c
* Author             : yangxg
* Version            : v4.0
* Date               : 04/12/2009
* Description        : lcd operation
********************************************************************************/

#include "lcd2010.h"
#include "macro.h"
#include "system.h"

//extern unsigned char   flgTmpDispGist;
struct SEGMENT                   //�ṹ��
	{
	    unsigned int  ucSetRisTim;    //����ʱ��
	    unsigned int  uiSetDryTmp;    //���ø���
		unsigned int  uiSetWetTmp;    //����ʪ��
      	unsigned int  ucSetStaTim;    //����ʱ��
		
    };
struct SEGMENT   WorkSeg[10];
struct SEGMENT   WorkSegSet[10];
struct SEGMENT   WorkSegDispBuff[10];
extern unsigned int    uiDispBuff[9]; 
extern unsigned char   ucUpDownDispBuff[2];     
extern unsigned char   ucRTCDispBuff[5]; 
extern unsigned char   ucCurveDispBuff[4];
extern unsigned char   ucSegTimRimDispBuff[20];  //0-19,rim,���
extern unsigned char   ucDryTmpRimDispBuff[10];
extern unsigned char   ucWetTmpRimDispBuff[10];
extern unsigned char   ucDot5DispBuff[10];



extern unsigned int    uiWorkSegDispBuff[40];
extern unsigned char   flgFlashShow;
extern unsigned char   flgUpDownFlashShow;
extern unsigned char   flgMainSensor;

extern unsigned char   ucSetupClassIndex;
extern unsigned char   ucOtherDispBuff[10];    //0:����;1:����;2:ƫ��;3:ȱ��;4:����;5:��ȼ;6:��ʪ;7:(ѭ������)�Զ�;8:(ѭ������)��;9:(ѭ������)��;

                                        //0     1    2    3    4   5    6     7    8    9   ��   A   b    C
const unsigned char  bank01Abuff[11]  = {0x5F,0x50,0x6B,0x79,0x74,0x3D,0x3F,0x58,0x7F,0x7D,0x00};  //IC1:CS1��1��2��3��4��5��6��7��8��9λ
const unsigned char  bank01Bbuff[11]  = {0xAF,0xA0,0x6D,0xE9,0xE2,0xCB,0xCF,0xA1,0xEF,0xEB,0x00};  //IC1:CS1��10��11��12��13λ


const unsigned char  bank02Abuff[11]  = {0x5F,0x06,0x6B,0x2F,0x36,0x3D,0x7D,0x07,0x7F,0x3F,0x00};  //IC1:CS1��1��2��3��4��5��6��7��8��9λ
const unsigned char  bank02Bbuff[11]  = {0xAF,0x06,0x6D,0x4F,0xC6,0xCB,0xEB,0x0E,0xEF,0xCF,0x00};  //IC1:CS1��10��11��12��13λ


const unsigned char  bank03Abuff[11]  = {0x5F,0x06,0x6B,0x2F,0x36,0x3D,0x7D,0x07,0x7F,0x3F,0x00};  //IC3:CS5��23��24��25��26��27��28
const unsigned char  bank03Bbuff[11]  = {0x5F,0x50,0x6B,0x79,0x74,0x3D,0x3F,0x58,0x7F,0x7D,0x00};  //IC3:CS5��29��30��31��32��33��34��35��36

const unsigned char  bank04Abuff[11]  = {0xAF,0xA0,0x6D,0xE9,0xE2,0xCB,0xCF,0xA1,0xEF,0xEB,0x00};  //IC4:CS3��37��38��39��40��41��42;47��48��49��50��51��52
const unsigned char  bank04Bbuff[11]  = {0x5F,0x50,0x6B,0x79,0x74,0x3D,0x3F,0x58,0x7F,0x7D,0x00};  //IC4:CS3��43��44��45��46

const unsigned char  bank05Abuff[11]  = {0xAF,0xA0,0x6D,0xE9,0xE2,0xCB,0xCF,0xA1,0xEF,0xEB,0x00};  //IC4:CS3��37��38��39��40��41��42;47��48��49��50��51��52
const unsigned char  bank05Bbuff[11]  = {0x5F,0x50,0x6B,0x79,0x74,0x3D,0x3F,0x58,0x7F,0x7D,0x00};  //IC4:CS3��43��44��45��46

const unsigned char  bank06Abuff[11]  = {0xAF,0xA0,0x6D,0xE9,0xE2,0xCB,0xCF,0xA1,0xEF,0xEB,0x00};  //IC4:CS3��37��38��39��40��41��42;47��48��49��50��51��52

const unsigned char  bank07Abuff[11]  = {0xAF,0xA0,0x6D,0xE9,0xE2,0xCB,0xCF,0xA1,0xEF,0xEB,0x00};  //IC4:CS3��37��38��39��40��41��42;47��48��49��50��51��52


//unsigned char  bank10buff[11]  = {0xAF,0xA0,0xCB,0xE9,0xE4,0x6D,0x6F,0xA8,0xEF,0xED,0x00};  //0x10const  PROGMEM
//unsigned char  bank08buff[12]  = {0xF5,0x60,0xB6,0xF2,0x63,0xD3,0xD7,0x70,0xF7,0xF3,0x00,0x75};  //0x08  ;0x75: ��ʾ n
//unsigned char  bank01buff[11]  = {0xFA,0x60,0xBC,0xF4,0x66,0xD6,0xDE,0x70,0xFE,0xF6,0x00};  //0x01

unsigned char i16Test;
unsigned char i16Test1;
unsigned char i16Test2;
unsigned char test10 = 1;
unsigned char test12 = 0;

extern volatile unsigned char enWorkMode;

/***************************************************************************
* Function Name  : vdDrv164
* Description    : 74LS164���,����8��Ƭѡ�ź�
* Input          : Ҫ������ֽ�����
* Output         : None
* Return         : None
****************************************************************************/
void vdDrv164(unsigned char datt)
{
    unsigned char i;
    	
	datt = datt ^ 0xff;
	for(i=0; i<8; i++)
	{
	    CLR_LCDCSCK;
		if((datt << i) & 0x80)
	        SET_LCDCSDA; 
        else
	        CLR_LCDCSDA;
        _delay_us(5);
		SET_LCDCSCK;
	   _delay_us(5);
	} 
}

/**********************
д�����λ
����n: 0��1
**********************/

void lcdwbit(unsigned char n)
{  

	CLR_LCDCK;
	if(n & 0x01)
	    SET_LCDDA;
	else
	    CLR_LCDDA;
    
	_delay_us(10);

	SET_LCDCK;
	_delay_us(10);    
	
}
//-----***-����ǰ���룺��ʾ��������-***-----//
void LcdSendId(unsigned char ID)
{
    unsigned char i;

    ID<<=5;
    for(i =0; i<3; i++)
    {
        
		if((ID&0x80)==0x80)
            lcdwbit(1);
        else
            lcdwbit(0);
		
		ID<<=1;
    }
}

//-----***-���͵�ַ-***-----//
void LcdSendAddress(unsigned char address)
{
    unsigned char i;
    
	address<<=2;
    for(i=0;i<6;i++)
    {
	    if((address&0x80)==0x80)
            lcdwbit(1);
        else
            lcdwbit(0);
        address<<=1;
    }
}


/********************************************
�����ݣ�
������λ����ģʽ��101������10��
����λ����6λ��ַ+4λ���ݣ���
cs: Ƭѡ��address:6λ��ַλ��data1��4λ����
********************************************/
void lcdwd(unsigned char address,unsigned char dat)  
{
    unsigned char i;
    
    //������ģʽ�����룺101��0x05;
	LcdSendId(0x05);

    address<<=2;
    for(i=0;i<6;i++)
    {
	    if((address&0x80)==0x80)
            lcdwbit(1);
        else
            lcdwbit(0);
        address<<=1;
    }
    for(i=0;i<8;i++)
    {
	    if((dat&0x01)==0x01)
            lcdwbit(1);
        else
            lcdwbit(0);
        dat>>=1;
    }
    SET_LCDDA;
}

//-----***-����һ���ֽ�-***-----//
void LcdSendByte(unsigned char dat)
{
    unsigned char i;
   
    for(i=0; i<8; i++)
    {
        if((dat&0x01)==0x01)
            lcdwbit(1);
        else
            lcdwbit(0);
        dat>>=1;
    }
	SET_LCDDA;
}

//-----***-���Ͱ���ֽ�-***-----//
void LcdSendHalfByte(unsigned char dat)
{
    unsigned char i;
   
    for(i=0; i<4; i++)
    {
        if((dat&0x01)==0x01)
            lcdwbit(1);
        else
            lcdwbit(0);
        dat>>=1;
    }
	SET_LCDDA;
}

/******************************
�������֣�9λ�����λΪ0��1��
������λ����ģʽ��100������9λ
������ɵ������֣����һλΪ0��
cs: Ƭѡ��cmdcode:������
*******************************/
void lcdwc(unsigned char cs, unsigned char cmdcode)  //�Ϳ�����
{  
    unsigned char i;
   
    vdDrv164(cs);  //write cs signal
	//������ģʽ�����룺100b:0x04;
	LcdSendId(0x04);
	for(i=0;i<8;i++)
    {
	    if((cmdcode&0x80)==0x80)
            lcdwbit(1);
        else
            lcdwbit(0);
        cmdcode<<=1;
    }
    lcdwbit(0);    //�����һλ��0��1�޹�λ     
    SET_LCDDA;
	vdDrv164(CS_NO);

}

/**********************************
�ϵ�ʱ��λ����ʱ��λ
***********************************/
void lcdreset(void)                    //��ʼ��
{  
    
	//��λƬ1
	lcdwc(CS_1,CMDOFF);                  //����
    lcdwc(CS_1,CMDLCDOFF);               //�ر�

	//��λƬ2
	lcdwc(CS_2,CMDOFF);                  //����
    lcdwc(CS_2,CMDLCDOFF);               //�ر�

	//��λƬ3
	lcdwc(CS_3,CMDOFF);                  //����
    lcdwc(CS_3,CMDLCDOFF);               //�ر�

	//��λƬ4
	lcdwc(CS_4,CMDOFF);                  //����
    lcdwc(CS_4,CMDLCDOFF);               //�ر�

	//��λƬ5
	lcdwc(CS_5,CMDOFF);                  //����
    lcdwc(CS_5,CMDLCDOFF);               //�ر�

	//��λƬ6
	lcdwc(CS_6,CMDOFF);                  //����
    lcdwc(CS_6,CMDLCDOFF);               //�ر�

	//��λƬ7
	lcdwc(CS_7,CMDOFF);                  //����
    lcdwc(CS_7,CMDLCDOFF);               //�ر�

        
	_delay_ms(100);

    lcdwc(CS_1,CMDON);                   //�ϵ�
    lcdwc(CS_1,CMDLCDON);                //��ʾ
    lcdwc(CS_1,CMDB3C4);                 //ģʽ����,1/3ƫѹ��4�������˿�

	lcdwc(CS_2,CMDON);                   //�ϵ�
    lcdwc(CS_2,CMDLCDON);                //��ʾ
    lcdwc(CS_2,CMDB3C4);                 //ģʽ����,1/3ƫѹ��4�������˿�

	lcdwc(CS_3,CMDON);                   //�ϵ�
    lcdwc(CS_3,CMDLCDON);                //��ʾ
    lcdwc(CS_3,CMDB3C4);                 //ģʽ����,1/3ƫѹ��4�������˿�

	lcdwc(CS_4,CMDON);                   //�ϵ�
    lcdwc(CS_4,CMDLCDON);                //��ʾ
    lcdwc(CS_4,CMDB3C4);                 //ģʽ����,1/3ƫѹ��4�������˿�

	lcdwc(CS_5,CMDON);                   //�ϵ�
    lcdwc(CS_5,CMDLCDON);                //��ʾ
    lcdwc(CS_5,CMDB3C4);                 //ģʽ����,1/3ƫѹ��4�������˿�

	lcdwc(CS_6,CMDON);                   //�ϵ�
    lcdwc(CS_6,CMDLCDON);                //��ʾ
    lcdwc(CS_6,CMDB3C4);                 //ģʽ����,1/3ƫѹ��4�������˿�

	lcdwc(CS_7,CMDON);                   //�ϵ�
    lcdwc(CS_7,CMDLCDON);                //��ʾ
    lcdwc(CS_7,CMDB3C4);                 //ģʽ����,1/3ƫѹ��4�������˿�

}

/**********************************
��ʼ���й�I/O����λ��������ʾ����
***********************************/
void  vdInitLCD(void)
{
   

    CLR_LCDLED;  //close backlight
    vdDrv164(CS_NO);//CS singal OUTput is high
	lcdreset();   //��λ  
}

/***************************************************************************
* Function Name  : vdDispBrush
* Description    : brush lcd
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
/***************************************************************************
* Function Name  : vdDispBrush
* Description    : brush lcd
* Input          : None
* Output         : None
* Return         : None
****************************************************************************/
void vdDispBrush(void)
{
//    unsigned char i;
    static unsigned char   ucFlashShowCnt =0;  //��˸����
    static unsigned char   flgFlash = 0;    

    ucFlashShowCnt++;                 //����һ�Σ���1��100ms����һ��
	if(ucFlashShowCnt >=  2)
	{
	    ucFlashShowCnt = 0;
		flgFlash ^= 0x01;
    }
    if(!flgFlash)
	{    
	    if(flgUpDownFlashShow ==0x01)           //"����" ��˸
		{
		    if(flgMainSensor == DWSENSOR)   //��������ʽ����
			{
	            ucUpDownDispBuff[UPSENSOR] = 0x00;

	        }
			else if(flgMainSensor == UPSENSOR)   //�����½�ʽ����
			{
			    ucUpDownDispBuff[DWSENSOR] = 0x00;
			}
     	}
	    if(ucSetupClassIndex == 0x04) 
		{
		    if(flgFlashShow == 0x01)
			    uiDispBuff[5]  = 0x0aaa;
		}
	    else if(ucSetupClassIndex == DORT_INDEX)   //ʱ�䲿����˸,0x00
		{
		    
			switch(flgFlashShow)   
	        {   
	           
			    case 0x05:        //������˸
				    ucRTCDispBuff[0] = 0xaa;
			        break;
	            case 0x04:        //Сʱ��˸
				    ucRTCDispBuff[1] = 0xaa;
			        break;
	            case 0x03:        //����˸
				    ucRTCDispBuff[2] = 0xaa;
			        break;
	            case 0x02:        //����˸
				    ucRTCDispBuff[3] = 0xaa;
			        break;
	            case 0x01:        //����˸
				    ucRTCDispBuff[4] =  0xaa;
			        break;
	            
			    default:
			        break;
		    }//end of switch(flgFlashShow) 
			
		}
		else if(ucSetupClassIndex == UPDW_INDEX)  //�ϡ�������˸0x03
		{
		    switch(flgFlashShow)   
	        {   
	          
			    case 0x01:        //"����"��˸
					ucUpDownDispBuff[0] = 0x00;
			        break;
	            case 0x02:        //"����"��˸
			    {
				    ucUpDownDispBuff[1] = 0x00;
			        break;
	            }
			    
			    default:
			        break;
		    }//end of switch(flgFlashShow) 
		
		
		}
		else if(ucSetupClassIndex == CURV_INDEX)  //���߲�����˸0x01
		{
		    switch(flgFlashShow)   
	        {   
	          
			    case 0x01:        //������˸
		    	
					ucCurveDispBuff[0] = 0x00;
			        break;
	            case 0x02:        //�²�Ҷ��˸
			    {
				    ucCurveDispBuff[1] = 0x00;
			        break;
	            }
			    case 0x03:        //�в�Ҷ��˸
			    {
				    ucCurveDispBuff[2]  = 0x00;
			        break;
	            }
			    case 0x04:        //�ϲ�Ҷ��˸
			    {
				    ucCurveDispBuff[3]  = 0x00;
			        break;
	            }
			    default:
			        break;
		    }//end of switch(flgFlashShow) 
		}
		else if(ucSetupClassIndex == PARA_INDEX)   //��ʪ�Ȳ�����˸Ч��0x02
		{
		    switch(flgFlashShow)   
	        {   
				/*�׶�1*/
				case 0x01:        //�׶�1����������˸
					WorkSegDispBuff[0].uiSetDryTmp = 0xaaa;
				    break;
		        case 0x02:        //�׶�1ʪ��������˸
				{
				    WorkSegDispBuff[0].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x03:        //�׶�1����ʱ��������˸
				{
				    WorkSegDispBuff[0].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*�׶�2*/
				case 0x04:        //�׶�2����ʱ��������˸
				{
				    WorkSegDispBuff[1].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x05:        //�׶�2����������˸
				{
				    WorkSegDispBuff[1].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x06:        //�׶�2ʪ��������˸
				{
				    WorkSegDispBuff[1].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x07:        //�׶�2����ʱ��������˸
				{
				    WorkSegDispBuff[1].ucSetStaTim = 0x0aa;
				    break;
		        }
	            /*�׶�3*/
				case 0x08:        //�׶�3����ʱ��������˸
				{
				    WorkSegDispBuff[2].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x09:        //�׶�3����������˸
				{
				    WorkSegDispBuff[2].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x0a:        //�׶�3ʪ��������˸
				{
				    WorkSegDispBuff[2].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x0b:        //�׶�3����ʱ��������˸
				{
				    WorkSegDispBuff[2].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*�׶�4*/
				case 0x0c:        //�׶�4����ʱ��������˸
				{
				    WorkSegDispBuff[3].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x0d:        //�׶�4����������˸
				{
				    WorkSegDispBuff[3].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x0e:        //�׶�4ʪ��������˸
				{
				    WorkSegDispBuff[3].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x0f:        //�׶�4����ʱ��������˸
				{
				    WorkSegDispBuff[3].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*�׶�5*/
				case 0x10:        //�׶�5����ʱ��������˸
				{
				    WorkSegDispBuff[4].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x11:        //�׶�5����������˸
				{
				    WorkSegDispBuff[4].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x12:        //�׶�5ʪ��������˸
				{
				    WorkSegDispBuff[4].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x13:        //�׶�5����ʱ��������˸
				{
				    WorkSegDispBuff[4].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*�׶�6*/
				case 0x14:        //�׶�6����ʱ��������˸
				{
				    WorkSegDispBuff[5].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x15:        //�׶�6����������˸
				{
				    WorkSegDispBuff[5].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x16:        //�׶�6ʪ��������˸
				{
				    WorkSegDispBuff[5].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x17:        //�׶�6����ʱ��������˸
				{
				    WorkSegDispBuff[5].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*�׶�7*/
				case 0x18:        //�׶�7����ʱ��������˸
				{
				    WorkSegDispBuff[6].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x19:        //�׶�7����������˸
				{
				    WorkSegDispBuff[6].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x1a:        //�׶�7ʪ��������˸
				{
				    WorkSegDispBuff[6].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x1b:        //�׶�7����ʱ��������˸
				{
				    WorkSegDispBuff[6].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*�׶�8*/
				case 0x1c:        //�׶�8����ʱ��������˸
				{
				    WorkSegDispBuff[7].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x1d:        //�׶�8����������˸
				{
				    WorkSegDispBuff[7].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x1e:        //�׶�8ʪ��������˸
				{
				    WorkSegDispBuff[7].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x1f:        //�׶�8����ʱ��������˸
				{
				    WorkSegDispBuff[7].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*�׶�9*/
				case 0x20:        //�׶�9����ʱ��������˸
				{
				    WorkSegDispBuff[8].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x21:        //�׶�9����������˸
				{
				    WorkSegDispBuff[8].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x22:        //�׶�9ʪ��������˸
				{
				    WorkSegDispBuff[8].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x23:        //�׶�9����ʱ��������˸
				{
				    WorkSegDispBuff[8].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*�׶�10*/
				case 0x24:        //�׶�10����ʱ��������˸
				{
				    WorkSegDispBuff[9].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x25:        //�׶�10����������˸
				{
				    WorkSegDispBuff[9].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x26:        //�׶�10ʪ��������˸
				{
				    WorkSegDispBuff[9].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x27:        //�׶�10����ʱ��������˸
				{
				    WorkSegDispBuff[9].ucSetStaTim = 0x0aa;
				    break;
		        }
				default:
				    break;
		
            }//end of switch(flgFlashShow) 
	
		}
    }//end of if(!flgFlash)

//IC1:CS1
	vdDrv164(CS_1);
	LcdSendId(0x05);           //д����������         
	LcdSendAddress(0x00);      //�ӵ�ַ0��ʼ


    //�����趨1���ߡ���, ������ȡ����λ
    LcdSendByte(0x80*ucDryTmpRimDispBuff[0]|bank01Abuff[WorkSegDispBuff[0].uiSetDryTmp >> 0x08]);                         //��ַ19��20��0x01�����£�
	LcdSendByte(bank01Abuff[(WorkSegDispBuff[0].uiSetDryTmp >> 0x04)& 0x0f]);             //��ַ21��22��0x01�������趨1���

	//�����趨2���ߡ���
	LcdSendByte(0x80*ucDryTmpRimDispBuff[1]|bank01Abuff[WorkSegDispBuff[1].uiSetDryTmp >> 0x08]);                         //��ַ23��24��0x10�����£�
	LcdSendByte(bank01Abuff[(WorkSegDispBuff[1].uiSetDryTmp >> 0x04)& 0x0f]);            //��ַ25��26��0x10�������趨2���

	//ʪ���趨3���ߡ���
	LcdSendByte(0x80*ucDot5DispBuff[2]|bank01Abuff[(WorkSegDispBuff[2].uiSetWetTmp >> 0x04)& 0x0f]);		   //��ַΪ18,19,0x80:ʪ���趨3��.5;
	LcdSendByte(0x80*ucWetTmpRimDispBuff[2]|bank01Abuff[WorkSegDispBuff[2].uiSetWetTmp >> 0x08]);		 //��ַΪ16,17,0x80:ʪ���趨3�����

	 //ʪ���趨2���ߡ���
	 LcdSendByte(0x80*ucDot5DispBuff[1]|bank01Abuff[(WorkSegDispBuff[1].uiSetWetTmp >> 0x04)& 0x0f]);			//��ַΪ14,15,0x80:ʪ���趨2��.5;
	 LcdSendByte(0x80*ucWetTmpRimDispBuff[1]|bank01Abuff[WorkSegDispBuff[1].uiSetWetTmp >> 0x08]);		 //��ַΪ12,13,0x80:ʪ���趨2�����

    //ʪ���趨1���ߡ���
     LcdSendByte(0x80*ucDot5DispBuff[0]|bank01Abuff[(WorkSegDispBuff[0].uiSetWetTmp >> 0x04)& 0x0f]);           //��ַΪ10,11,0x80:ʪ���趨1��.5��
	 LcdSendByte(0x80*ucWetTmpRimDispBuff[0]|bank01Abuff[WorkSegDispBuff[0].uiSetWetTmp >> 0x08]);       //��ַΪ8,9,0x80:ʪ���趨1�����
 
	//����ʱ���趨1���ߡ��ͣ���һ������ucSegTimRimDispBuff[i]
	LcdSendByte(0x10*ucOtherDispBuff[5]|bank01Bbuff[WorkSegDispBuff[0].ucSetStaTim >> 0x04]);        //��ַΪ0,1,0x80:��ʪ��
	LcdSendByte(0x10*ucSegTimRimDispBuff[1]|bank01Bbuff[WorkSegDispBuff[0].ucSetStaTim & 0x0f]); //��ַΪ2,3,0x80:ʱ���趨1�����

	//����ʱ���趨2���ߡ��ͣ��ڶ�������
	LcdSendByte(0x10*ucOtherDispBuff[6]|bank01Bbuff[WorkSegDispBuff[1].ucSetRisTim >> 0x04]);        //��ַΪ4,5,0x80:��ȼ��
	LcdSendByte(0x10*ucSegTimRimDispBuff[2]|bank01Bbuff[WorkSegDispBuff[1].ucSetRisTim & 0x0f]); //��ַΪ6,7,0x80:ʱ���趨2�����

    //ʱ���趨3���ߡ��ͣ��ڶ�������
	LcdSendByte(bank01Bbuff[WorkSegDispBuff[1].ucSetStaTim >> 0x04]);        //��ַΪ8,9,0x80:ʱ���趨3�����
	LcdSendByte(0x10*ucSegTimRimDispBuff[3]|bank01Bbuff[WorkSegDispBuff[1].ucSetStaTim & 0x0f]);            //��ַΪ10,11��



	vdDrv164(CS_2);
	LcdSendId(0x05);           //д����������         
	LcdSendAddress(0x00);      //�ӵ�ַ0��ʼ

	i16Test2 = ((ucUpDownDispBuff[0]&0x01)*0x80);
	LcdSendByte(i16Test2|bank02Abuff[uiDispBuff[0]/100]);          //addr 7,8;0x08: ȱ��
	i16Test2 = ((ucUpDownDispBuff[0]&0x02)*0x40);
	LcdSendByte(i16Test2|bank02Abuff[uiDispBuff[0]%100/10]);       //addr 9,10��0x08:ȱ���
	if((enWorkMode == DEBUG_MasterAddressSet)||(enWorkMode == DEBUG_SlaveAddressSet))
	    LcdSendByte(bank02Abuff[uiDispBuff[0]%10]);           //addr 11,12;0x08:�м�λС����
	else
	    LcdSendByte(0x80|bank02Abuff[uiDispBuff[0]%10]);           //addr 11,12;0x08:�м�λС����
	i16Test2 =((ucUpDownDispBuff[1]&0x01)*0x80);
	//ʪ���¶ȣ��ߡ��С���
	LcdSendByte(i16Test2|bank02Abuff[uiDispBuff[1]/100]);          //addr 7,8;0x08: ȱ�ࣻ
	i16Test2 =((ucUpDownDispBuff[1]&0x02)*0x40);
	LcdSendByte(i16Test2|bank02Abuff[uiDispBuff[1]%100/10]);       //addr 9,10��0x08:ȱ���
	LcdSendByte(0x80|bank02Abuff[uiDispBuff[1]%10]);           //addr 11,12;0x08:�м�λС����

	LcdSendByte(bank02Abuff[uiDispBuff[4]/100]);                //��ַΪ0,1;
	LcdSendByte(bank02Abuff[uiDispBuff[4]%100/10]);             //��ַΪ2,3;
	LcdSendByte(0x80|bank02Abuff[uiDispBuff[4]%10]);            //��ַΪ4,5��0x08���׶�ʱ���С����;

    //��ʱ��/**/	
	LcdSendByte(bank02Abuff[uiDispBuff[5] >> 0x08]);              //��ַΪ6,7;
	LcdSendByte(bank02Abuff[(uiDispBuff[5] >> 4)&0x0f]);             //��ַΪ8,9;
	LcdSendByte(bank02Abuff[uiDispBuff[5]&0x000f]);                 //��ַΪ10,11;

	LcdSendByte(bank02Bbuff[uiDispBuff[3]/100]);          //addr 13,14��0x08:����
	//Ŀ����£��ߡ��С���
	
	LcdSendByte(0x10|bank02Bbuff[uiDispBuff[2]%10]);           //addr 17,18;0x08:�м�λС����

	LcdSendByte(((ucOtherDispBuff[1]&0x01)<<4)|bank02Bbuff[uiDispBuff[2]%100/10]);       //addr 15,16��0x08:���ؿ�
  LcdSendByte(((ucOtherDispBuff[0]&0x01)<<4)|bank02Bbuff[uiDispBuff[2]/100]);          //addr 13,14��0x08:����

	vdDrv164(CS_3);
	LcdSendId(0x05);           //д����������         
	LcdSendAddress(0x00);      //�ӵ�ַ0��ʼ
	
	LcdSendByte(0x80|bank03Abuff[uiDispBuff[3]%10]);			//��ַΪ16,17;0x08��Ŀ��ʪ�µ�С����;

	LcdSendByte(bank03Abuff[uiDispBuff[3]%100/10]);             //��ַΪ14,15;

  //�����趨4���ߡ���
	LcdSendByte(0x80*ucDryTmpRimDispBuff[3]|bank03Bbuff[WorkSegDispBuff[3].uiSetDryTmp >> 0x08]);       //��ַΪ4,5,0x10:�����趨4�����
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[3].uiSetDryTmp >> 0x04)& 0x0f]);                //��ַΪ6,7;

	//�����趨5���ߡ���
	LcdSendByte(0x80*ucDryTmpRimDispBuff[4]|bank03Bbuff[WorkSegDispBuff[4].uiSetDryTmp >> 0x08]);       //��ַΪ0,1,0x80:�����趨5�����
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[4].uiSetDryTmp >> 0x04)& 0x0f]);                //��ַΪ2,3��
	
    //�����趨6���ߡ���
	LcdSendByte(0x80*ucDryTmpRimDispBuff[5]|bank03Bbuff[WorkSegDispBuff[5].uiSetDryTmp >> 0x08]);       //��ַΪ4,5,0x80:�����趨6�����
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[5].uiSetDryTmp >> 0x04)& 0x0f]);                //��ַΪ6,7;

    //�����趨7���ߡ���
	LcdSendByte(0x80*ucDryTmpRimDispBuff[6]|bank03Bbuff[WorkSegDispBuff[6].uiSetDryTmp >> 0x08]);        //��ַΪ8,9,0x80:�����趨7�����
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[6].uiSetDryTmp >> 0x04)& 0x0f]);                 //��ַΪ10,11��

	//�����趨8���ߡ���
	LcdSendByte(0x80*ucDryTmpRimDispBuff[7]|bank03Bbuff[WorkSegDispBuff[7].uiSetDryTmp >> 0x08]);       //��ַΪ0,1,0x80:�����趨8�����
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[7].uiSetDryTmp >> 0x04)& 0x0f]);                //��ַΪ2,3��
	
    //�����趨9���ߡ���
	LcdSendByte(0x80*ucDryTmpRimDispBuff[8]|bank03Bbuff[WorkSegDispBuff[8].uiSetDryTmp >> 0x08]);       //��ַΪ4,5,0x80:�����趨9�����
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[8].uiSetDryTmp >> 0x04)& 0x0f]);                //��ַΪ6,7;

    //�����趨10���ߡ���
	LcdSendByte(0x80*ucDryTmpRimDispBuff[9]|bank03Bbuff[WorkSegDispBuff[9].uiSetDryTmp >> 0x08]);        //��ַΪ8,9,0x80:�����趨10�����
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[9].uiSetDryTmp >> 0x04)& 0x0f]);                 //��ַΪ10,11��

	vdDrv164(CS_4);
	LcdSendId(0x05);           //д����������         
	LcdSendAddress(0x00);      //�ӵ�ַ0��ʼ

	//ʪ���趨10���ߡ���
	LcdSendByte(bank04Abuff[(WorkSegDispBuff[9].uiSetWetTmp >> 0x04)& 0x0f]);                 //��ַΪ18,19��
	LcdSendByte(0x10*ucWetTmpRimDispBuff[9]|bank04Abuff[WorkSegDispBuff[9].uiSetWetTmp >> 0x08]);        //��ַΪ16,17,0x10:ʪ���趨10�����

	//ʪ���趨9���ߡ���
	LcdSendByte(0x10*ucDot5DispBuff[8]|bank04Abuff[(WorkSegDispBuff[8].uiSetWetTmp >> 0x04)& 0x0f]);            //��ַΪ14,15,0x10:ʪ���趨9��.5��
	LcdSendByte(0x10*ucWetTmpRimDispBuff[8]|bank04Abuff[WorkSegDispBuff[8].uiSetWetTmp >> 0x08]);        //��ַΪ12,13,0x10:ʪ���趨9�����

	//ʪ���趨8���ߡ���
	
	LcdSendByte(0x10*ucDot5DispBuff[7]|bank04Abuff[(WorkSegDispBuff[7].uiSetWetTmp >> 0x04)& 0x0f]);            //��ַΪ26,27,0x10:ʪ���趨8��.5��
	LcdSendByte(0x10*ucWetTmpRimDispBuff[7]|bank04Abuff[WorkSegDispBuff[7].uiSetWetTmp >> 0x08]);        //��ַΪ24,25,0x10:ʪ���趨8�����

	//ʪ���趨7���ߡ���
	LcdSendByte(0x10*ucDot5DispBuff[6]|bank04Abuff[(WorkSegDispBuff[6].uiSetWetTmp >> 0x04)& 0x0f]);            //��ַΪ22,23,0x10:ʪ���趨7��.5��
	LcdSendByte(0x10*ucWetTmpRimDispBuff[6]|bank04Abuff[WorkSegDispBuff[6].uiSetWetTmp >> 0x08]);        //��ַΪ20,21,0x10:ʪ���趨7�����

	LcdSendByte(0x10*ucDot5DispBuff[5]|bank04Abuff[(WorkSegDispBuff[5].uiSetWetTmp >> 0x04)& 0x0f]);            //��ַΪ18,19,0x10:ʪ���趨6��.5��
	LcdSendByte(0x10*ucWetTmpRimDispBuff[5]|bank04Abuff[WorkSegDispBuff[5].uiSetWetTmp >> 0x08]);        //��ַΪ16,17,0x10:ʪ���趨6�����

	//ʱ���趨19���ߡ��ͣ���ʮ������
	LcdSendByte(0x80*ucSegTimRimDispBuff[19]|bank04Bbuff[WorkSegDispBuff[9].ucSetStaTim & 0x0f]);                 //��ַΪ30,31��
	LcdSendByte(bank04Bbuff[WorkSegDispBuff[9].ucSetStaTim >> 0x04]);        //��ַΪ28,29,0x80:ʱ���趨19�����

	//ʱ���趨18���ߡ��ͣ���ʮ������
	LcdSendByte(0x80*ucSegTimRimDispBuff[18]|bank04Bbuff[WorkSegDispBuff[9].ucSetRisTim & 0x0f]);                 //��ַΪ26,27��
	LcdSendByte(bank04Bbuff[WorkSegDispBuff[9].ucSetRisTim >> 0x04]);        //��ַΪ24,25,0x80:ʱ���趨18�����

	//ʱ���趨17���ߡ��ͣ��ھŶ����£�ȡ����λ
	LcdSendByte(0x80*ucSegTimRimDispBuff[17]|bank04Bbuff[WorkSegDispBuff[8].ucSetStaTim & 0x0f]);                 //��ַΪ22,23��
	LcdSendByte(bank04Bbuff[WorkSegDispBuff[8].ucSetStaTim >> 0x04]);        //��ַΪ20,21,0x80:ʱ���趨17�����




	



	vdDrv164(CS_5);
	LcdSendId(0x05);           //д����������         
	LcdSendAddress(0x00);      //�ӵ�ַ0��ʼ

	//ʱ���趨14���ߡ��ͣ��ھŶ�����
	LcdSendByte(bank05Abuff[WorkSegDispBuff[7].ucSetRisTim >> 0x04]);        //��ַΪ4,5,0x80:ʱ���趨16�����
	LcdSendByte(0x10*ucSegTimRimDispBuff[14]|bank05Abuff[WorkSegDispBuff[7].ucSetRisTim & 0x0f]);                 //��ַΪ6,7��

    //ʱ���趨15���ߡ��ͣ��ڰ˶�����
	LcdSendByte(bank05Abuff[WorkSegDispBuff[7].ucSetStaTim >> 0x04]);        //��ַΪ0,1,0x80:ʱ���趨15�����
	LcdSendByte(0x10*ucSegTimRimDispBuff[15]|bank05Abuff[WorkSegDispBuff[7].ucSetStaTim & 0x0f]);                 //��ַΪ2,3��

	//ʱ���趨16���ߡ��ͣ��ھŶ�����
	LcdSendByte(bank05Abuff[WorkSegDispBuff[8].ucSetRisTim >> 0x04]);        //��ַΪ4,5,0x80:ʱ���趨16�����
	LcdSendByte(0x10*ucSegTimRimDispBuff[16]|bank05Abuff[WorkSegDispBuff[8].ucSetRisTim & 0x0f]);                 //��ַΪ6,7��

	//ʱ��֣��ߡ���
	LcdSendByte(((ucCurveDispBuff[3]&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[0] & 0x0f]);         //��ַΪ28,29,0x10:�ϲ�Ҷ�����
	LcdSendByte((((ucCurveDispBuff[3]>>4)&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[0]>> 0x04]);         //��ַΪ26,27,0x10:�ϲ�Ҷ��

	//ʱ��ʱ���ߡ���
	LcdSendByte(((ucCurveDispBuff[2]&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[1] & 0x0f]);         //��ַΪ24,25,0x10:�в�Ҷ�����
	LcdSendByte((((ucCurveDispBuff[2]>>4)&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[1]>> 0x04]);         //��ַΪ22,23,0x10:�в�Ҷ��

    //date���ߡ���
	LcdSendByte(((ucCurveDispBuff[1]&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[2] & 0x0f]);         //��ַΪ20,21
	LcdSendByte((((ucCurveDispBuff[1]>>4)&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[2]>> 0x04]);         //��ַΪ18,19

	//�·ݣ��ߡ���
	LcdSendByte(bank05Bbuff[ucRTCDispBuff[3] & 0x0f]);         //��ַΪ16,17,0x10:�²�Ҷ�����
	LcdSendByte(bank05Bbuff[ucRTCDispBuff[3]>> 0x04]);         //��ַΪ14,15,0x10:�²�Ҷ��

	//��ţ��ߡ��ͣ�ucDateATime[4]��BCD��
	LcdSendByte(((ucCurveDispBuff[0]&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[4] & 0x0f]);         //��ַΪ12,13,0x10:��������
	LcdSendByte((((ucCurveDispBuff[0]>>4)&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[4] >> 0x04]);        //��ַΪ10,11,0x10:���裻

	vdDrv164(CS_6);
	LcdSendId(0x05);           //д����������         
	LcdSendAddress(0x00);      //�ӵ�ַ0��ʼ

	//ʱ���趨7���ߡ��ͣ����Ķ�����
	LcdSendByte(bank07Abuff[WorkSegDispBuff[3].ucSetStaTim >> 0x04]);        //��ַΪ24,25,0x80:ʱ���趨7�����
	LcdSendByte(0x10*ucSegTimRimDispBuff[7]|bank07Abuff[WorkSegDispBuff[3].ucSetStaTim & 0x0f]);            //��ַΪ26,27��

	LcdSendByte(bank07Abuff[WorkSegDispBuff[4].ucSetRisTim >> 0x04]);        //��ַΪ28,29,0x80:ʱ���趨8�����
	LcdSendByte(0x10*ucSegTimRimDispBuff[8]|bank07Abuff[WorkSegDispBuff[4].ucSetRisTim & 0x0f]);            //��ַΪ30,31��

     //ʱ���趨9���ߡ��ͣ����������
	LcdSendByte(0x10|bank06Abuff[WorkSegDispBuff[4].ucSetStaTim >> 0x04]);        //��ַΪ0,1,0x80:ʱ���趨9�����
	LcdSendByte(0x10*ucSegTimRimDispBuff[9]|bank06Abuff[WorkSegDispBuff[4].ucSetStaTim & 0x0f]);                 //��ַΪ2,3��


/*	//ʱ���趨10���ߡ��ͣ�����������
	LcdSendByte(0x10*ucOtherDispBuff[7]|bank06Abuff[WorkSegDispBuff[5].ucSetRisTim >> 0x04]);        //��ַΪ4,5,0x80:ʱ���趨10�����
	LcdSendByte(0x10*ucSegTimRimDispBuff[10]|bank06Abuff[WorkSegDispBuff[5].ucSetRisTim & 0x0f]);                 //��ַΪ6,7��

	//ʱ���趨11���ߡ��ͣ�����������
	LcdSendByte(bank06Abuff[WorkSegDispBuff[5].ucSetStaTim >> 0x04]);        //��ַΪ8,9,0x80:ʱ���趨11�����
	LcdSendByte(0x10*ucSegTimRimDispBuff[11]|bank06Abuff[WorkSegDispBuff[5].ucSetStaTim & 0x0f]);                 //��ַΪ10,11��

	//ʱ���趨13���ߡ��ͣ����߶�����
	LcdSendByte(0x10*ucOtherDispBuff[9]|bank06Abuff[WorkSegDispBuff[6].ucSetStaTim >> 0x04]);        //��ַΪ16,17,0x80:ʱ���趨13�����
	LcdSendByte(0x10*ucSegTimRimDispBuff[13]|bank06Abuff[WorkSegDispBuff[6].ucSetStaTim & 0x0f]);                 //��ַΪ18,19��


    //ʱ���趨12���ߡ��ͣ����߶�����
	LcdSendByte(0x10*ucOtherDispBuff[8]|bank06Abuff[WorkSegDispBuff[6].ucSetRisTim >> 0x04]);        //��ַΪ12,13,0x80:ʱ���趨12�����
	LcdSendByte(0x10*ucSegTimRimDispBuff[12]|bank06Abuff[WorkSegDispBuff[6].ucSetRisTim & 0x0f]);                 //��ַΪ14,15��
*/

	i16Test1 = 0x10*ucOtherDispBuff[7];

	//ʱ���趨10���ߡ��ͣ�����������
	LcdSendByte(i16Test1|bank06Abuff[WorkSegDispBuff[5].ucSetRisTim >> 0x04]);        //��ַΪ4,5,0x80:ʱ���趨10�����
	LcdSendByte(0x10*ucSegTimRimDispBuff[10]|bank06Abuff[WorkSegDispBuff[5].ucSetRisTim & 0x0f]);                 //��ַΪ6,7��

	//ʱ���趨11���ߡ��ͣ�����������
	LcdSendByte(0x00|bank06Abuff[WorkSegDispBuff[5].ucSetStaTim >> 0x04]);        //��ַΪ8,9,0x80:ʱ���趨11�����
	LcdSendByte(0x10*ucSegTimRimDispBuff[11]|bank06Abuff[WorkSegDispBuff[5].ucSetStaTim & 0x0f]);                 //��ַΪ10,11��

	i16Test1 = 0x10*ucOtherDispBuff[8];

    //ʱ���趨12���ߡ��ͣ����߶�����
	LcdSendByte(i16Test1|bank06Abuff[WorkSegDispBuff[6].ucSetRisTim >> 0x04]);        //��ַΪ12,13,0x80:ʱ���趨12�����
	LcdSendByte(0x10*ucSegTimRimDispBuff[12]|bank06Abuff[WorkSegDispBuff[6].ucSetRisTim & 0x0f]);                 //��ַΪ14,15��


	i16Test1 = 0x10*ucOtherDispBuff[9];

	//ʱ���趨13���ߡ��ͣ����߶�����
	LcdSendByte(i16Test1|bank06Abuff[WorkSegDispBuff[6].ucSetStaTim >> 0x04]);        //��ַΪ16,17,0x80:ʱ���趨13�����
	LcdSendByte(0x10*ucSegTimRimDispBuff[13]|bank06Abuff[WorkSegDispBuff[6].ucSetStaTim & 0x0f]);                 //��ַΪ18,19��



	LcdSendByte(bank06Abuff[uiDispBuff[7]%10]);                 //��ַΪ8,9��
	LcdSendHalfByte(0x00);

	vdDrv164(CS_7);
	LcdSendId(0x05);           //д����������         
	LcdSendAddress(0x00);      //�ӵ�ַ0��ʼ

	//�����趨2���ߡ���
	LcdSendByte(0x10*ucDryTmpRimDispBuff[2]|bank07Abuff[WorkSegDispBuff[2].uiSetDryTmp >> 0x08]);                         //��ַ23��24��0x10�����£�
	LcdSendByte(bank07Abuff[(WorkSegDispBuff[2].uiSetDryTmp >> 0x04)& 0x0f]);            //��ַ25��26��0x10�������趨2���

	//ʪ���趨5���ߡ�
	LcdSendByte(0x10*ucDot5DispBuff[4]|bank07Abuff[(WorkSegDispBuff[4].uiSetWetTmp >> 0x04)& 0x0f]);            //��ַΪ14,15,0x10:ʪ���趨5��.5��
	LcdSendByte(0x10*ucWetTmpRimDispBuff[4]|bank07Abuff[WorkSegDispBuff[4].uiSetWetTmp >> 0x08]);        //��ַΪ12,13,0x10:ʪ���趨5�����

	//ʪ���趨4���ߡ��
	LcdSendByte(0x10*ucDot5DispBuff[3]|bank07Abuff[(WorkSegDispBuff[3].uiSetWetTmp >> 0x04)& 0x0f]);            //��ַΪ14,15,0x10:ʪ���趨5��.5��
	LcdSendByte(0x10*ucWetTmpRimDispBuff[3]|bank07Abuff[WorkSegDispBuff[3].uiSetWetTmp >> 0x08]);        //��ַΪ12,13,0x10:ʪ���趨5�����


	//ʱ���趨4���ߡ��ͣ�����������
	LcdSendByte(bank07Abuff[WorkSegDispBuff[2].ucSetRisTim >> 0x04]);        //��ַΪ12,13,0x80:ʱ���趨4�����
	LcdSendByte(0x10*ucSegTimRimDispBuff[4]|bank07Abuff[WorkSegDispBuff[2].ucSetRisTim & 0x0f]);            //��ַΪ14,15��

	//ʱ���趨5���ߡ��ͣ�����������
	LcdSendByte(bank07Abuff[WorkSegDispBuff[2].ucSetStaTim >> 0x04]);        //��ַΪ16,17,0x80:ʱ���趨5�����
	LcdSendByte(0x10*ucSegTimRimDispBuff[5]|bank07Abuff[WorkSegDispBuff[2].ucSetStaTim & 0x0f]);            //��ַΪ18,19��

	//ʱ���趨6���ߡ��ͣ����Ķ�����
	LcdSendByte(bank07Abuff[WorkSegDispBuff[3].ucSetRisTim >> 0x04]);        //��ַΪ20,21,0x80:ʱ���趨6�����
	LcdSendByte(0x10*ucSegTimRimDispBuff[6]|bank07Abuff[WorkSegDispBuff[3].ucSetRisTim & 0x0f]);            //��ַΪ22,23��

	//������ѹ���ߡ��С���
	LcdSendByte(bank07Abuff[uiDispBuff[6] >> 8]);           //��ַΪ24,25,0x10: ѭ�����ٵĵͣ�
	LcdSendByte(bank07Abuff[(uiDispBuff[6] >> 4)&0x0f]);        //��ַΪ26,27,0x10��ѭ�����ٵĸߣ�
	LcdSendByte(bank07Abuff[uiDispBuff[6]&0x000f]);            //��ַΪ28,29,0x10��ѭ�����ٵ��Զ���

	i16Test = 0x00;
	i16Test  = ucOtherDispBuff[3]*0x02+ucOtherDispBuff[3]*0x20;
	i16Test += ucOtherDispBuff[4]*0x04+ucOtherDispBuff[4]*0x40;
	i16Test += ucOtherDispBuff[2]*0x08+ucOtherDispBuff[2]*0x80;
	LcdSendByte(i16Test);            //��ַΪ28,29,0x10��ѭ�����ٵ��Զ���
	vdDrv164(CS_NO); 

}
