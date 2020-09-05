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
* Description    : 硬件接口层，读取8位键值
* Input          : None
* Output         : none
* Return         : 8位键值
74165操作方法：
sh/ld 为低时并口数据进入寄存器，当sh/ld为高时将并口数据锁存，并允许串口输出。

clk 上升沿输出数据
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
* Description    : 驱动层，只对由ucReadKeyPort(void)返回的8位键值进行处理
* Input          : None
* Output         : none
* Return         : none

****************************************************************************/
void KeyScan(void)                    //键扫描
{
    unsigned char tempkey;

	tempkey = ucReadKeyPort();

    if(tempkey != 0x00)       //有键按下
	{
	    ucCurRdKey = tempkey;
      if(ucCurRdKey == ucPreRdKey)
		{
		    ucKeyOld++;
       if((ucCurRdKey == KEY_RUN)||(ucCurRdKey == KEY_SWIMA)||(ucCurRdKey == KEY_SETUP)||(ucCurRdKey == KEY_MINUS))   //区分长按键
	  {
         if(ucKeyOld == KEYDELAY)              //连续键,不要则把此if屏蔽
		{
		    if(flgChangKey == 0x00)     //如果已发出一长按键后一直按住不松，则不会再发第二个长按键，键弹起时flgChangKey清零
			{
			    flgChangKey = 0x01;
		        flgKeyOk    = 0x01;
                    
			    if(tempkey == KEY_RUN)
		            ucKeyVal   = KEY_STOP;        //变换成长按键
	            else if(tempkey == KEY_SWIMA)
			        ucKeyVal   = KEY_QUERY;       //变换成长按键
				else if(tempkey == KEY_SETUP)
			        ucKeyVal   = KEY_RESET;       //变换成长按键
			    else if(tempkey == KEY_MINUS)
			    {
			       		ucKeyVal = KEY_DEBUG;       //变换成长按键
			    }
	         }
        }
			}
			else if((ucCurRdKey ==KEY_PLUS)||(ucCurRdKey ==KEY_SELL)||(ucCurRdKey ==KEY_SELR)) //区分加速键
			{ 
			    if(ucKeyOld == KEYJIADELAY)            //连续键,不要则把此if屏蔽
			    {
			        flgKeyOk   = 1;
					if(tempkey == KEY_PLUS)      //变换成连击键
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
		else                 //不相同，keyold清零，退出
		{
		    ucKeyOld = 0;
		    ucPreRdKey = ucCurRdKey;
		}
		if(tempkey == KEY_MasterAddressSet)
	    {
			ucKeyVal = KEY_MasterAddressSet;		 //变换成长按键
			flgKeyOk =TRUE;
	   }
	   if(tempkey == KEY_SlaveAddressSet)
	   {
		   ucKeyVal = KEY_SlaveAddressSet;		//变换成长按键
		   flgKeyOk =TRUE;
	   }
	}
    else                              //键抬起
	{
		if(flgChangKey == 0x00)   //发出长按键后若不抬起，则不会再发出短键
		{
			if((ucKeyOld < 240) && (ucKeyOld > 2))  //key_old不为0说明有键按下再放开,短按键不会超过 40 * 20ms(定时中断) = 800ms
			{
			    
				flgKeyOk = 1;
	            ucKeyVal = ucCurRdKey; 
			}  
		}
		else
		{
		    if( ucKeyOld > 2 )     //弹起至少2 *20 = 40ms后才清零
		        flgChangKey = 0x00;
        }
		ucKeyOld = 0;   
	
	}
}




