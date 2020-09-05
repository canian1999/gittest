/******************** (C) COPYRIGHT 2010 zke ***********************************
* File Name          : key2010.c
* Author             : yangxg
* Version            : V1.0
* Date               : 11/12/2010
* Description        : key scan,key driver,key process, and so on
********************************************************************************/
#include "key2010.h"
#include "system.h"
#include "iic.h"
#include "macro.h"

unsigned char ucPreRdKey;
unsigned char ucCurRdKey;
unsigned char ucKeyOld;

unsigned char flgChangKey = 0;

extern unsigned char ucKeyVal;
extern unsigned char flgKeyOk;
extern unsigned int IICaddr,IICdatt,IICk;
extern volatile unsigned char enWorkMode;
/***************************************************************************
* Function Name  : ucReadKey
* Description    : Ӳ���ӿڲ㣬��ȡ8λ��ֵ
* Input          : None
* Output         : none
* Return         : 8λ��ֵ
74165����������
sh/ld Ϊ��ʱ�������ݽ���Ĵ�������sh/ldΪ��ʱ�������������棬�������������

clk �������������
****************************************************************************/
unsigned char ucReadKeyPort(void)
{
    unsigned char addr,i;
	addr = 0;

	CLR_KEYPL;
	_delay_us(5);
    SET_KEYPL;
	_delay_us(5); 
    for(i = 0; i < 8; i++)
    {
        
		addr <<=1;
		if(((~GPIOC->IDR >> 5) & 0x01)==0x01)
            addr |= 0x01;

		CLR_KEYCK;
		_delay_us(5);
        SET_KEYCK;
		_delay_us(5);
    }
    return(addr); 
}/**/

/***************************************************************************
* Function Name  : KeyScan
* Description    : �����㣬ֻ����ucReadKeyPort(void)���ص�8λ��ֵ���д���
* Input          : None
* Output         : none
* Return         : none

****************************************************************************/
void KeyScan(void)                    //��ɨ��
{
    unsigned char tempkey;

	tempkey = ucReadKeyPort();

    if(tempkey != 0x00)       //�м�����
	{
	    ucCurRdKey = tempkey;
      if(ucCurRdKey == ucPreRdKey)
		{
		    ucKeyOld++;
       if((ucCurRdKey == KEY_RUN)||(ucCurRdKey == KEY_SWIMA)||(ucCurRdKey == KEY_SETUP)||(ucCurRdKey == KEY_MINUS))   //���ֳ�����
	  {
         if(ucKeyOld == KEYDELAY)              //������,��Ҫ��Ѵ�if����
		{
		    if(flgChangKey == 0x00)     //����ѷ���һ��������һֱ��ס���ɣ��򲻻��ٷ��ڶ�����������������ʱflgChangKey����
			{
			    flgChangKey = 0x01;
		        flgKeyOk    = 0x01;
                    
			    if(tempkey == KEY_RUN)
		            ucKeyVal   = KEY_STOP;        //�任�ɳ�����
	            else if(tempkey == KEY_SWIMA)
			        ucKeyVal   = KEY_QUERY;       //�任�ɳ�����
				else if(tempkey == KEY_SETUP)
			        ucKeyVal   = KEY_RESET;       //�任�ɳ�����
			    else if(tempkey == KEY_MINUS)
			    {
			       		ucKeyVal = KEY_DEBUG;       //�任�ɳ�����
			    }
	         }
        }
			}
			else if((ucCurRdKey ==KEY_PLUS)||(ucCurRdKey ==KEY_SELL)||(ucCurRdKey ==KEY_SELR)) //���ּ��ټ�
			{ 
			    if(ucKeyOld == KEYJIADELAY)            //������,��Ҫ��Ѵ�if����
			    {
			        flgKeyOk   = 1;
					if(tempkey == KEY_PLUS)      //�任��������
				    {
				        ucKeyVal   = KEY_CPLUS;
                       
            }
            else if(tempkey == KEY_SELL)
				    {
				        ucKeyVal   = KEY_CSELL;
            }
				    else if(tempkey == KEY_SELR)
				    {
				        ucKeyVal   = KEY_CSELR;
					  
            }
				    ucKeyOld  = KEYJIADELAY-KEYLOOP;
				}
		
			}
		}
		else                 //����ͬ��keyold���㣬�˳�
		{
		    ucKeyOld = 0;
		    ucPreRdKey = ucCurRdKey;
		}
		if(tempkey == KEY_MasterAddressSet)
	    {
			ucKeyVal = KEY_MasterAddressSet;		 //�任�ɳ�����
			flgKeyOk =TRUE;
	   }
	   if(tempkey == KEY_SlaveAddressSet)
	   {
		   ucKeyVal = KEY_SlaveAddressSet;		//�任�ɳ�����
		   flgKeyOk =TRUE;
	   }
	}
    else                              //��̧��
	{
		if(flgChangKey == 0x00)   //����������������̧���򲻻��ٷ����̼�
		{
			if((ucKeyOld < 240) && (ucKeyOld > 2))  //key_old��Ϊ0˵���м������ٷſ�,�̰������ᳬ�� 40 * 20ms(��ʱ�ж�) = 800ms
			{
			    
				flgKeyOk = 1;
	            ucKeyVal = ucCurRdKey; 
			}  
		}
		else
		{
		    if( ucKeyOld > 2 )     //��������2 *20 = 40ms�������
		        flgChangKey = 0x00;
        }
		ucKeyOld = 0;   
	
	}
}




