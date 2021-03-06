   
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
struct SEGMENT                   //结构体
	{
	    unsigned int  ucSetRisTim;    //升温时间
	    unsigned int  uiSetDryTmp;    //设置干温
		unsigned int  uiSetWetTmp;    //设置湿温
      	unsigned int  ucSetStaTim;    //稳温时间
		
    };
struct SEGMENT   WorkSeg[10];
struct SEGMENT   WorkSegSet[10];
struct SEGMENT   WorkSegDispBuff[10];
extern unsigned int    uiDispBuff[9]; 
extern unsigned char   ucUpDownDispBuff[2];     
extern unsigned char   ucRTCDispBuff[5]; 
extern unsigned char   ucCurveDispBuff[4];
extern unsigned char   ucSegTimRimDispBuff[20];  //0-19,rim,外框
extern unsigned char   ucDryTmpRimDispBuff[10];
extern unsigned char   ucWetTmpRimDispBuff[10];
extern unsigned char   ucDot5DispBuff[10];



extern unsigned int    uiWorkSegDispBuff[40];
extern unsigned char   flgFlashShow;
extern unsigned char   flgUpDownFlashShow;
extern unsigned char   flgMainSensor;

extern unsigned char   ucSetupClassIndex;
extern unsigned char   ucOtherDispBuff[10];    //0:升温;1:恒温;2:偏温;3:缺相;4:过载;5:助燃;6:排湿;7:(循环风速)自动;8:(循环风速)高;9:(循环风速)低;

                                        //0     1    2    3    4   5    6     7    8    9   灭   A   b    C
const unsigned char  bank01Abuff[11]  = {0x5F,0x50,0x6B,0x79,0x74,0x3D,0x3F,0x58,0x7F,0x7D,0x00};  //IC1:CS1的1、2、3、4、5、6、7、8、9位
const unsigned char  bank01Bbuff[11]  = {0xAF,0xA0,0x6D,0xE9,0xE2,0xCB,0xCF,0xA1,0xEF,0xEB,0x00};  //IC1:CS1的10、11、12、13位


const unsigned char  bank02Abuff[11]  = {0x5F,0x06,0x6B,0x2F,0x36,0x3D,0x7D,0x07,0x7F,0x3F,0x00};  //IC1:CS1的1、2、3、4、5、6、7、8、9位
const unsigned char  bank02Bbuff[11]  = {0xAF,0x06,0x6D,0x4F,0xC6,0xCB,0xEB,0x0E,0xEF,0xCF,0x00};  //IC1:CS1的10、11、12、13位


const unsigned char  bank03Abuff[11]  = {0x5F,0x06,0x6B,0x2F,0x36,0x3D,0x7D,0x07,0x7F,0x3F,0x00};  //IC3:CS5的23、24、25、26、27、28
const unsigned char  bank03Bbuff[11]  = {0x5F,0x50,0x6B,0x79,0x74,0x3D,0x3F,0x58,0x7F,0x7D,0x00};  //IC3:CS5的29、30、31、32、33、34、35、36

const unsigned char  bank04Abuff[11]  = {0xAF,0xA0,0x6D,0xE9,0xE2,0xCB,0xCF,0xA1,0xEF,0xEB,0x00};  //IC4:CS3的37、38、39、40、41、42;47、48、49、50、51、52
const unsigned char  bank04Bbuff[11]  = {0x5F,0x50,0x6B,0x79,0x74,0x3D,0x3F,0x58,0x7F,0x7D,0x00};  //IC4:CS3的43、44、45、46

const unsigned char  bank05Abuff[11]  = {0xAF,0xA0,0x6D,0xE9,0xE2,0xCB,0xCF,0xA1,0xEF,0xEB,0x00};  //IC4:CS3的37、38、39、40、41、42;47、48、49、50、51、52
const unsigned char  bank05Bbuff[11]  = {0x5F,0x50,0x6B,0x79,0x74,0x3D,0x3F,0x58,0x7F,0x7D,0x00};  //IC4:CS3的43、44、45、46

const unsigned char  bank06Abuff[11]  = {0xAF,0xA0,0x6D,0xE9,0xE2,0xCB,0xCF,0xA1,0xEF,0xEB,0x00};  //IC4:CS3的37、38、39、40、41、42;47、48、49、50、51、52

const unsigned char  bank07Abuff[11]  = {0xAF,0xA0,0x6D,0xE9,0xE2,0xCB,0xCF,0xA1,0xEF,0xEB,0x00};  //IC4:CS3的37、38、39、40、41、42;47、48、49、50、51、52


//unsigned char  bank10buff[11]  = {0xAF,0xA0,0xCB,0xE9,0xE4,0x6D,0x6F,0xA8,0xEF,0xED,0x00};  //0x10const  PROGMEM
//unsigned char  bank08buff[12]  = {0xF5,0x60,0xB6,0xF2,0x63,0xD3,0xD7,0x70,0xF7,0xF3,0x00,0x75};  //0x08  ;0x75: 显示 n
//unsigned char  bank01buff[11]  = {0xFA,0x60,0xBC,0xF4,0x66,0xD6,0xDE,0x70,0xFE,0xF6,0x00};  //0x01

unsigned char i16Test;
unsigned char i16Test1;
unsigned char i16Test2;
unsigned char test10 = 1;
unsigned char test12 = 0;

extern volatile unsigned char enWorkMode;

/***************************************************************************
* Function Name  : vdDrv164
* Description    : 74LS164输出,发送8个片选信号
* Input          : 要输出的字节数据
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
写入比特位
参数n: 0或1
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
//-----***-发送前导码：表示操作类型-***-----//
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

//-----***-发送地址-***-----//
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
送数据：
先送三位命令模式码101，再送10个
比特位（送6位地址+4位数据）。
cs: 片选，address:6位地址位；data1：4位数据
********************************************/
void lcdwd(unsigned char address,unsigned char dat)  
{
    unsigned char i;
    
    //送命令模式类型码：101；0x05;
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

//-----***-发送一个字节-***-----//
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

//-----***-发送半个字节-***-----//
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
送命令字，9位，最低位为0或1；
先送三位命令模式码100，再送9位
比特组成的命令字，最后一位为0。
cs: 片选，cmdcode:命令字
*******************************/
void lcdwc(unsigned char cs, unsigned char cmdcode)  //送控制字
{  
    unsigned char i;
   
    vdDrv164(cs);  //write cs signal
	//送命令模式类型码：100b:0x04;
	LcdSendId(0x04);
	for(i=0;i<8;i++)
    {
	    if((cmdcode&0x80)==0x80)
            lcdwbit(1);
        else
            lcdwbit(0);
        cmdcode<<=1;
    }
    lcdwbit(0);    //送最后一位：0或1无关位     
    SET_LCDDA;
	vdDrv164(CS_NO);

}

/**********************************
上电时复位，定时复位
***********************************/
void lcdreset(void)                    //初始化
{  
    
	//复位片1
	lcdwc(CS_1,CMDOFF);                  //掉电
    lcdwc(CS_1,CMDLCDOFF);               //关闭

	//复位片2
	lcdwc(CS_2,CMDOFF);                  //掉电
    lcdwc(CS_2,CMDLCDOFF);               //关闭

	//复位片3
	lcdwc(CS_3,CMDOFF);                  //掉电
    lcdwc(CS_3,CMDLCDOFF);               //关闭

	//复位片4
	lcdwc(CS_4,CMDOFF);                  //掉电
    lcdwc(CS_4,CMDLCDOFF);               //关闭

	//复位片5
	lcdwc(CS_5,CMDOFF);                  //掉电
    lcdwc(CS_5,CMDLCDOFF);               //关闭

	//复位片6
	lcdwc(CS_6,CMDOFF);                  //掉电
    lcdwc(CS_6,CMDLCDOFF);               //关闭

	//复位片7
	lcdwc(CS_7,CMDOFF);                  //掉电
    lcdwc(CS_7,CMDLCDOFF);               //关闭

        
	_delay_ms(100);

    lcdwc(CS_1,CMDON);                   //上电
    lcdwc(CS_1,CMDLCDON);                //显示
    lcdwc(CS_1,CMDB3C4);                 //模式设置,1/3偏压，4个公共端口

	lcdwc(CS_2,CMDON);                   //上电
    lcdwc(CS_2,CMDLCDON);                //显示
    lcdwc(CS_2,CMDB3C4);                 //模式设置,1/3偏压，4个公共端口

	lcdwc(CS_3,CMDON);                   //上电
    lcdwc(CS_3,CMDLCDON);                //显示
    lcdwc(CS_3,CMDB3C4);                 //模式设置,1/3偏压，4个公共端口

	lcdwc(CS_4,CMDON);                   //上电
    lcdwc(CS_4,CMDLCDON);                //显示
    lcdwc(CS_4,CMDB3C4);                 //模式设置,1/3偏压，4个公共端口

	lcdwc(CS_5,CMDON);                   //上电
    lcdwc(CS_5,CMDLCDON);                //显示
    lcdwc(CS_5,CMDB3C4);                 //模式设置,1/3偏压，4个公共端口

	lcdwc(CS_6,CMDON);                   //上电
    lcdwc(CS_6,CMDLCDON);                //显示
    lcdwc(CS_6,CMDB3C4);                 //模式设置,1/3偏压，4个公共端口

	lcdwc(CS_7,CMDON);                   //上电
    lcdwc(CS_7,CMDLCDON);                //显示
    lcdwc(CS_7,CMDB3C4);                 //模式设置,1/3偏压，4个公共端口

}

/**********************************
初始化有关I/O，复位，部分显示处理；
***********************************/
void  vdInitLCD(void)
{
   

    CLR_LCDLED;  //close backlight
    vdDrv164(CS_NO);//CS singal OUTput is high
	lcdreset();   //复位  
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
    static unsigned char   ucFlashShowCnt =0;  //闪烁计数
    static unsigned char   flgFlash = 0;    

    ucFlashShowCnt++;                 //调用一次，加1；100ms调用一次
	if(ucFlashShowCnt >=  2)
	{
	    ucFlashShowCnt = 0;
		flgFlash ^= 0x01;
    }
    if(!flgFlash)
	{    
	    if(flgUpDownFlashShow ==0x01)           //"上棚" 闪烁
		{
		    if(flgMainSensor == DWSENSOR)   //气流上升式烤房
			{
	            ucUpDownDispBuff[UPSENSOR] = 0x00;

	        }
			else if(flgMainSensor == UPSENSOR)   //气流下降式烤房
			{
			    ucUpDownDispBuff[DWSENSOR] = 0x00;
			}
     	}
	    if(ucSetupClassIndex == 0x04) 
		{
		    if(flgFlashShow == 0x01)
			    uiDispBuff[5]  = 0x0aaa;
		}
	    else if(ucSetupClassIndex == DORT_INDEX)   //时间部分闪烁,0x00
		{
		    
			switch(flgFlashShow)   
	        {   
	           
			    case 0x05:        //分钟闪烁
				    ucRTCDispBuff[0] = 0xaa;
			        break;
	            case 0x04:        //小时闪烁
				    ucRTCDispBuff[1] = 0xaa;
			        break;
	            case 0x03:        //日闪烁
				    ucRTCDispBuff[2] = 0xaa;
			        break;
	            case 0x02:        //月闪烁
				    ucRTCDispBuff[3] = 0xaa;
			        break;
	            case 0x01:        //年闪烁
				    ucRTCDispBuff[4] =  0xaa;
			        break;
	            
			    default:
			        break;
		    }//end of switch(flgFlashShow) 
			
		}
		else if(ucSetupClassIndex == UPDW_INDEX)  //上、下棚闪烁0x03
		{
		    switch(flgFlashShow)   
	        {   
	          
			    case 0x01:        //"上棚"闪烁
					ucUpDownDispBuff[0] = 0x00;
			        break;
	            case 0x02:        //"下棚"闪烁
			    {
				    ucUpDownDispBuff[1] = 0x00;
			        break;
	            }
			    
			    default:
			        break;
		    }//end of switch(flgFlashShow) 
		
		
		}
		else if(ucSetupClassIndex == CURV_INDEX)  //曲线部分闪烁0x01
		{
		    switch(flgFlashShow)   
	        {   
	          
			    case 0x01:        //自设闪烁
		    	
					ucCurveDispBuff[0] = 0x00;
			        break;
	            case 0x02:        //下部叶闪烁
			    {
				    ucCurveDispBuff[1] = 0x00;
			        break;
	            }
			    case 0x03:        //中部叶闪烁
			    {
				    ucCurveDispBuff[2]  = 0x00;
			        break;
	            }
			    case 0x04:        //上部叶闪烁
			    {
				    ucCurveDispBuff[3]  = 0x00;
			        break;
	            }
			    default:
			        break;
		    }//end of switch(flgFlashShow) 
		}
		else if(ucSetupClassIndex == PARA_INDEX)   //温湿度参数闪烁效果0x02
		{
		    switch(flgFlashShow)   
	        {   
				/*阶段1*/
				case 0x01:        //阶段1干温设置闪烁
					WorkSegDispBuff[0].uiSetDryTmp = 0xaaa;
				    break;
		        case 0x02:        //阶段1湿温设置闪烁
				{
				    WorkSegDispBuff[0].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x03:        //阶段1稳温时间设置闪烁
				{
				    WorkSegDispBuff[0].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*阶段2*/
				case 0x04:        //阶段2升温时间设置闪烁
				{
				    WorkSegDispBuff[1].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x05:        //阶段2干温设置闪烁
				{
				    WorkSegDispBuff[1].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x06:        //阶段2湿温设置闪烁
				{
				    WorkSegDispBuff[1].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x07:        //阶段2稳温时间设置闪烁
				{
				    WorkSegDispBuff[1].ucSetStaTim = 0x0aa;
				    break;
		        }
	            /*阶段3*/
				case 0x08:        //阶段3升温时间设置闪烁
				{
				    WorkSegDispBuff[2].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x09:        //阶段3干温设置闪烁
				{
				    WorkSegDispBuff[2].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x0a:        //阶段3湿温设置闪烁
				{
				    WorkSegDispBuff[2].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x0b:        //阶段3稳温时间设置闪烁
				{
				    WorkSegDispBuff[2].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*阶段4*/
				case 0x0c:        //阶段4升温时间设置闪烁
				{
				    WorkSegDispBuff[3].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x0d:        //阶段4干温设置闪烁
				{
				    WorkSegDispBuff[3].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x0e:        //阶段4湿温设置闪烁
				{
				    WorkSegDispBuff[3].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x0f:        //阶段4稳温时间设置闪烁
				{
				    WorkSegDispBuff[3].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*阶段5*/
				case 0x10:        //阶段5升温时间设置闪烁
				{
				    WorkSegDispBuff[4].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x11:        //阶段5干温设置闪烁
				{
				    WorkSegDispBuff[4].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x12:        //阶段5湿温设置闪烁
				{
				    WorkSegDispBuff[4].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x13:        //阶段5稳温时间设置闪烁
				{
				    WorkSegDispBuff[4].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*阶段6*/
				case 0x14:        //阶段6升温时间设置闪烁
				{
				    WorkSegDispBuff[5].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x15:        //阶段6干温设置闪烁
				{
				    WorkSegDispBuff[5].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x16:        //阶段6湿温设置闪烁
				{
				    WorkSegDispBuff[5].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x17:        //阶段6稳温时间设置闪烁
				{
				    WorkSegDispBuff[5].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*阶段7*/
				case 0x18:        //阶段7升温时间设置闪烁
				{
				    WorkSegDispBuff[6].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x19:        //阶段7干温设置闪烁
				{
				    WorkSegDispBuff[6].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x1a:        //阶段7湿温设置闪烁
				{
				    WorkSegDispBuff[6].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x1b:        //阶段7稳温时间设置闪烁
				{
				    WorkSegDispBuff[6].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*阶段8*/
				case 0x1c:        //阶段8升温时间设置闪烁
				{
				    WorkSegDispBuff[7].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x1d:        //阶段8干温设置闪烁
				{
				    WorkSegDispBuff[7].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x1e:        //阶段8湿温设置闪烁
				{
				    WorkSegDispBuff[7].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x1f:        //阶段8稳温时间设置闪烁
				{
				    WorkSegDispBuff[7].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*阶段9*/
				case 0x20:        //阶段9升温时间设置闪烁
				{
				    WorkSegDispBuff[8].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x21:        //阶段9干温设置闪烁
				{
				    WorkSegDispBuff[8].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x22:        //阶段9湿温设置闪烁
				{
				    WorkSegDispBuff[8].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x23:        //阶段9稳温时间设置闪烁
				{
				    WorkSegDispBuff[8].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*阶段10*/
				case 0x24:        //阶段10升温时间设置闪烁
				{
				    WorkSegDispBuff[9].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x25:        //阶段10干温设置闪烁
				{
				    WorkSegDispBuff[9].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x26:        //阶段10湿温设置闪烁
				{
				    WorkSegDispBuff[9].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x27:        //阶段10稳温时间设置闪烁
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
	LcdSendId(0x05);           //写数据引导码         
	LcdSendAddress(0x00);      //从地址0开始


    //干球设定1：高、低, 整数，取高两位
    LcdSendByte(0x80*ucDryTmpRimDispBuff[0]|bank01Abuff[WorkSegDispBuff[0].uiSetDryTmp >> 0x08]);                         //地址19、20，0x01：升温；
	LcdSendByte(bank01Abuff[(WorkSegDispBuff[0].uiSetDryTmp >> 0x04)& 0x0f]);             //地址21、22，0x01：干球设定1外框；

	//干球设定2：高、低
	LcdSendByte(0x80*ucDryTmpRimDispBuff[1]|bank01Abuff[WorkSegDispBuff[1].uiSetDryTmp >> 0x08]);                         //地址23、24，0x10：恒温；
	LcdSendByte(bank01Abuff[(WorkSegDispBuff[1].uiSetDryTmp >> 0x04)& 0x0f]);            //地址25、26，0x10：干球设定2外框；

	//湿球设定3：高、低
	LcdSendByte(0x80*ucDot5DispBuff[2]|bank01Abuff[(WorkSegDispBuff[2].uiSetWetTmp >> 0x04)& 0x0f]);		   //地址为18,19,0x80:湿球设定3的.5;
	LcdSendByte(0x80*ucWetTmpRimDispBuff[2]|bank01Abuff[WorkSegDispBuff[2].uiSetWetTmp >> 0x08]);		 //地址为16,17,0x80:湿球设定3的外框；

	 //湿球设定2：高、低
	 LcdSendByte(0x80*ucDot5DispBuff[1]|bank01Abuff[(WorkSegDispBuff[1].uiSetWetTmp >> 0x04)& 0x0f]);			//地址为14,15,0x80:湿球设定2的.5;
	 LcdSendByte(0x80*ucWetTmpRimDispBuff[1]|bank01Abuff[WorkSegDispBuff[1].uiSetWetTmp >> 0x08]);		 //地址为12,13,0x80:湿球设定2的外框；

    //湿球设定1：高、低
     LcdSendByte(0x80*ucDot5DispBuff[0]|bank01Abuff[(WorkSegDispBuff[0].uiSetWetTmp >> 0x04)& 0x0f]);           //地址为10,11,0x80:湿球设定1的.5；
	 LcdSendByte(0x80*ucWetTmpRimDispBuff[0]|bank01Abuff[WorkSegDispBuff[0].uiSetWetTmp >> 0x08]);       //地址为8,9,0x80:湿球设定1的外框；
 
	//稳温时间设定1：高、低，第一段稳温ucSegTimRimDispBuff[i]
	LcdSendByte(0x10*ucOtherDispBuff[5]|bank01Bbuff[WorkSegDispBuff[0].ucSetStaTim >> 0x04]);        //地址为0,1,0x80:排湿；
	LcdSendByte(0x10*ucSegTimRimDispBuff[1]|bank01Bbuff[WorkSegDispBuff[0].ucSetStaTim & 0x0f]); //地址为2,3,0x80:时间设定1的外框；

	//升温时间设定2：高、低，第二段升温
	LcdSendByte(0x10*ucOtherDispBuff[6]|bank01Bbuff[WorkSegDispBuff[1].ucSetRisTim >> 0x04]);        //地址为4,5,0x80:助燃；
	LcdSendByte(0x10*ucSegTimRimDispBuff[2]|bank01Bbuff[WorkSegDispBuff[1].ucSetRisTim & 0x0f]); //地址为6,7,0x80:时间设定2的外框；

    //时间设定3：高、低，第二段稳温
	LcdSendByte(bank01Bbuff[WorkSegDispBuff[1].ucSetStaTim >> 0x04]);        //地址为8,9,0x80:时间设定3的外框；
	LcdSendByte(0x10*ucSegTimRimDispBuff[3]|bank01Bbuff[WorkSegDispBuff[1].ucSetStaTim & 0x0f]);            //地址为10,11；



	vdDrv164(CS_2);
	LcdSendId(0x05);           //写数据引导码         
	LcdSendAddress(0x00);      //从地址0开始

	i16Test2 = ((ucUpDownDispBuff[0]&0x01)*0x80);
	LcdSendByte(i16Test2|bank02Abuff[uiDispBuff[0]/100]);          //addr 7,8;0x08: 缺相�
	i16Test2 = ((ucUpDownDispBuff[0]&0x02)*0x40);
	LcdSendByte(i16Test2|bank02Abuff[uiDispBuff[0]%100/10]);       //addr 9,10；0x08:缺相框
	if((enWorkMode == DEBUG_MasterAddressSet)||(enWorkMode == DEBUG_SlaveAddressSet))
	    LcdSendByte(bank02Abuff[uiDispBuff[0]%10]);           //addr 11,12;0x08:中间位小数点
	else
	    LcdSendByte(0x80|bank02Abuff[uiDispBuff[0]%10]);           //addr 11,12;0x08:中间位小数点
	i16Test2 =((ucUpDownDispBuff[1]&0x01)*0x80);
	//湿球温度：高、中、低
	LcdSendByte(i16Test2|bank02Abuff[uiDispBuff[1]/100]);          //addr 7,8;0x08: 缺相；
	i16Test2 =((ucUpDownDispBuff[1]&0x02)*0x40);
	LcdSendByte(i16Test2|bank02Abuff[uiDispBuff[1]%100/10]);       //addr 9,10；0x08:缺相框
	LcdSendByte(0x80|bank02Abuff[uiDispBuff[1]%10]);           //addr 11,12;0x08:中间位小数点

	LcdSendByte(bank02Abuff[uiDispBuff[4]/100]);                //地址为0,1;
	LcdSendByte(bank02Abuff[uiDispBuff[4]%100/10]);             //地址为2,3;
	LcdSendByte(0x80|bank02Abuff[uiDispBuff[4]%10]);            //地址为4,5；0x08：阶段时间的小数点;

    //总时间/**/	
	LcdSendByte(bank02Abuff[uiDispBuff[5] >> 0x08]);              //地址为6,7;
	LcdSendByte(bank02Abuff[(uiDispBuff[5] >> 4)&0x0f]);             //地址为8,9;
	LcdSendByte(bank02Abuff[uiDispBuff[5]&0x000f]);                 //地址为10,11;

	LcdSendByte(bank02Bbuff[uiDispBuff[3]/100]);          //addr 13,14；0x08:过载
	//目标干温：高、中、低
	
	LcdSendByte(0x10|bank02Bbuff[uiDispBuff[2]%10]);           //addr 17,18;0x08:中间位小数点

	LcdSendByte(((ucOtherDispBuff[1]&0x01)<<4)|bank02Bbuff[uiDispBuff[2]%100/10]);       //addr 15,16；0x08:过载框
  LcdSendByte(((ucOtherDispBuff[0]&0x01)<<4)|bank02Bbuff[uiDispBuff[2]/100]);          //addr 13,14；0x08:过载

	vdDrv164(CS_3);
	LcdSendId(0x05);           //写数据引导码         
	LcdSendAddress(0x00);      //从地址0开始
	
	LcdSendByte(0x80|bank03Abuff[uiDispBuff[3]%10]);			//地址为16,17;0x08：目标湿温的小数点;

	LcdSendByte(bank03Abuff[uiDispBuff[3]%100/10]);             //地址为14,15;

  //干球设定4：高、低
	LcdSendByte(0x80*ucDryTmpRimDispBuff[3]|bank03Bbuff[WorkSegDispBuff[3].uiSetDryTmp >> 0x08]);       //地址为4,5,0x10:干球设定4的外框；
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[3].uiSetDryTmp >> 0x04)& 0x0f]);                //地址为6,7;

	//干球设定5：高、低
	LcdSendByte(0x80*ucDryTmpRimDispBuff[4]|bank03Bbuff[WorkSegDispBuff[4].uiSetDryTmp >> 0x08]);       //地址为0,1,0x80:干球设定5的外框；
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[4].uiSetDryTmp >> 0x04)& 0x0f]);                //地址为2,3；
	
    //干球设定6：高、低
	LcdSendByte(0x80*ucDryTmpRimDispBuff[5]|bank03Bbuff[WorkSegDispBuff[5].uiSetDryTmp >> 0x08]);       //地址为4,5,0x80:干球设定6的外框；
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[5].uiSetDryTmp >> 0x04)& 0x0f]);                //地址为6,7;

    //干球设定7：高、低
	LcdSendByte(0x80*ucDryTmpRimDispBuff[6]|bank03Bbuff[WorkSegDispBuff[6].uiSetDryTmp >> 0x08]);        //地址为8,9,0x80:干球设定7的外框；
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[6].uiSetDryTmp >> 0x04)& 0x0f]);                 //地址为10,11；

	//干球设定8：高、低
	LcdSendByte(0x80*ucDryTmpRimDispBuff[7]|bank03Bbuff[WorkSegDispBuff[7].uiSetDryTmp >> 0x08]);       //地址为0,1,0x80:干球设定8的外框；
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[7].uiSetDryTmp >> 0x04)& 0x0f]);                //地址为2,3；
	
    //干球设定9：高、低
	LcdSendByte(0x80*ucDryTmpRimDispBuff[8]|bank03Bbuff[WorkSegDispBuff[8].uiSetDryTmp >> 0x08]);       //地址为4,5,0x80:干球设定9的外框；
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[8].uiSetDryTmp >> 0x04)& 0x0f]);                //地址为6,7;

    //干球设定10：高、低
	LcdSendByte(0x80*ucDryTmpRimDispBuff[9]|bank03Bbuff[WorkSegDispBuff[9].uiSetDryTmp >> 0x08]);        //地址为8,9,0x80:干球设定10的外框；
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[9].uiSetDryTmp >> 0x04)& 0x0f]);                 //地址为10,11；

	vdDrv164(CS_4);
	LcdSendId(0x05);           //写数据引导码         
	LcdSendAddress(0x00);      //从地址0开始

	//湿球设定10：高、低
	LcdSendByte(bank04Abuff[(WorkSegDispBuff[9].uiSetWetTmp >> 0x04)& 0x0f]);                 //地址为18,19；
	LcdSendByte(0x10*ucWetTmpRimDispBuff[9]|bank04Abuff[WorkSegDispBuff[9].uiSetWetTmp >> 0x08]);        //地址为16,17,0x10:湿球设定10的外框；

	//湿球设定9：高、低
	LcdSendByte(0x10*ucDot5DispBuff[8]|bank04Abuff[(WorkSegDispBuff[8].uiSetWetTmp >> 0x04)& 0x0f]);            //地址为14,15,0x10:湿球设定9的.5；
	LcdSendByte(0x10*ucWetTmpRimDispBuff[8]|bank04Abuff[WorkSegDispBuff[8].uiSetWetTmp >> 0x08]);        //地址为12,13,0x10:湿球设定9的外框；

	//湿球设定8：高、低
	
	LcdSendByte(0x10*ucDot5DispBuff[7]|bank04Abuff[(WorkSegDispBuff[7].uiSetWetTmp >> 0x04)& 0x0f]);            //地址为26,27,0x10:湿球设定8的.5；
	LcdSendByte(0x10*ucWetTmpRimDispBuff[7]|bank04Abuff[WorkSegDispBuff[7].uiSetWetTmp >> 0x08]);        //地址为24,25,0x10:湿球设定8的外框；

	//湿球设定7：高、低
	LcdSendByte(0x10*ucDot5DispBuff[6]|bank04Abuff[(WorkSegDispBuff[6].uiSetWetTmp >> 0x04)& 0x0f]);            //地址为22,23,0x10:湿球设定7的.5；
	LcdSendByte(0x10*ucWetTmpRimDispBuff[6]|bank04Abuff[WorkSegDispBuff[6].uiSetWetTmp >> 0x08]);        //地址为20,21,0x10:湿球设定7的外框；

	LcdSendByte(0x10*ucDot5DispBuff[5]|bank04Abuff[(WorkSegDispBuff[5].uiSetWetTmp >> 0x04)& 0x0f]);            //地址为18,19,0x10:湿球设定6的.5；
	LcdSendByte(0x10*ucWetTmpRimDispBuff[5]|bank04Abuff[WorkSegDispBuff[5].uiSetWetTmp >> 0x08]);        //地址为16,17,0x10:湿球设定6的外框；

	//时间设定19：高、低，第十段稳温
	LcdSendByte(0x80*ucSegTimRimDispBuff[19]|bank04Bbuff[WorkSegDispBuff[9].ucSetStaTim & 0x0f]);                 //地址为30,31；
	LcdSendByte(bank04Bbuff[WorkSegDispBuff[9].ucSetStaTim >> 0x04]);        //地址为28,29,0x80:时间设定19的外框；

	//时间设定18：高、低，第十段升温
	LcdSendByte(0x80*ucSegTimRimDispBuff[18]|bank04Bbuff[WorkSegDispBuff[9].ucSetRisTim & 0x0f]);                 //地址为26,27；
	LcdSendByte(bank04Bbuff[WorkSegDispBuff[9].ucSetRisTim >> 0x04]);        //地址为24,25,0x80:时间设定18的外框；

	//时间设定17：高、低，第九段稳温，取低两位
	LcdSendByte(0x80*ucSegTimRimDispBuff[17]|bank04Bbuff[WorkSegDispBuff[8].ucSetStaTim & 0x0f]);                 //地址为22,23；
	LcdSendByte(bank04Bbuff[WorkSegDispBuff[8].ucSetStaTim >> 0x04]);        //地址为20,21,0x80:时间设定17的外框；




	



	vdDrv164(CS_5);
	LcdSendId(0x05);           //写数据引导码         
	LcdSendAddress(0x00);      //从地址0开始

	//时间设定14：高、低，第九段升温
	LcdSendByte(bank05Abuff[WorkSegDispBuff[7].ucSetRisTim >> 0x04]);        //地址为4,5,0x80:时间设定16的外框；
	LcdSendByte(0x10*ucSegTimRimDispBuff[14]|bank05Abuff[WorkSegDispBuff[7].ucSetRisTim & 0x0f]);                 //地址为6,7；

    //时间设定15：高、低，第八段稳温
	LcdSendByte(bank05Abuff[WorkSegDispBuff[7].ucSetStaTim >> 0x04]);        //地址为0,1,0x80:时间设定15的外框；
	LcdSendByte(0x10*ucSegTimRimDispBuff[15]|bank05Abuff[WorkSegDispBuff[7].ucSetStaTim & 0x0f]);                 //地址为2,3；

	//时间设定16：高、低，第九段升温
	LcdSendByte(bank05Abuff[WorkSegDispBuff[8].ucSetRisTim >> 0x04]);        //地址为4,5,0x80:时间设定16的外框；
	LcdSendByte(0x10*ucSegTimRimDispBuff[16]|bank05Abuff[WorkSegDispBuff[8].ucSetRisTim & 0x0f]);                 //地址为6,7；

	//时间分：高、低
	LcdSendByte(((ucCurveDispBuff[3]&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[0] & 0x0f]);         //地址为28,29,0x10:上部叶的外框；
	LcdSendByte((((ucCurveDispBuff[3]>>4)&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[0]>> 0x04]);         //地址为26,27,0x10:上部叶；

	//时间时：高、低
	LcdSendByte(((ucCurveDispBuff[2]&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[1] & 0x0f]);         //地址为24,25,0x10:中部叶的外框；
	LcdSendByte((((ucCurveDispBuff[2]>>4)&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[1]>> 0x04]);         //地址为22,23,0x10:中部叶；

    //date：高、低
	LcdSendByte(((ucCurveDispBuff[1]&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[2] & 0x0f]);         //地址为20,21
	LcdSendByte((((ucCurveDispBuff[1]>>4)&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[2]>> 0x04]);         //地址为18,19

	//月份：高、低
	LcdSendByte(bank05Bbuff[ucRTCDispBuff[3] & 0x0f]);         //地址为16,17,0x10:下部叶的外框；
	LcdSendByte(bank05Bbuff[ucRTCDispBuff[3]>> 0x04]);         //地址为14,15,0x10:下部叶；

	//年号：高、低，ucDateATime[4]，BCD码
	LcdSendByte(((ucCurveDispBuff[0]&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[4] & 0x0f]);         //地址为12,13,0x10:自设的外框；
	LcdSendByte((((ucCurveDispBuff[0]>>4)&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[4] >> 0x04]);        //地址为10,11,0x10:自设；

	vdDrv164(CS_6);
	LcdSendId(0x05);           //写数据引导码         
	LcdSendAddress(0x00);      //从地址0开始

	//时间设定7：高、低，第四段稳温
	LcdSendByte(bank07Abuff[WorkSegDispBuff[3].ucSetStaTim >> 0x04]);        //地址为24,25,0x80:时间设定7的外框；
	LcdSendByte(0x10*ucSegTimRimDispBuff[7]|bank07Abuff[WorkSegDispBuff[3].ucSetStaTim & 0x0f]);            //地址为26,27；

	LcdSendByte(bank07Abuff[WorkSegDispBuff[4].ucSetRisTim >> 0x04]);        //地址为28,29,0x80:时间设定8的外框；
	LcdSendByte(0x10*ucSegTimRimDispBuff[8]|bank07Abuff[WorkSegDispBuff[4].ucSetRisTim & 0x0f]);            //地址为30,31；

     //时间设定9：高、低，第五段稳温
	LcdSendByte(0x10|bank06Abuff[WorkSegDispBuff[4].ucSetStaTim >> 0x04]);        //地址为0,1,0x80:时间设定9的外框；
	LcdSendByte(0x10*ucSegTimRimDispBuff[9]|bank06Abuff[WorkSegDispBuff[4].ucSetStaTim & 0x0f]);                 //地址为2,3；


/*	//时间设定10：高、低，第六段升温
	LcdSendByte(0x10*ucOtherDispBuff[7]|bank06Abuff[WorkSegDispBuff[5].ucSetRisTim >> 0x04]);        //地址为4,5,0x80:时间设定10的外框；
	LcdSendByte(0x10*ucSegTimRimDispBuff[10]|bank06Abuff[WorkSegDispBuff[5].ucSetRisTim & 0x0f]);                 //地址为6,7；

	//时间设定11：高、低，第六段稳温
	LcdSendByte(bank06Abuff[WorkSegDispBuff[5].ucSetStaTim >> 0x04]);        //地址为8,9,0x80:时间设定11的外框；
	LcdSendByte(0x10*ucSegTimRimDispBuff[11]|bank06Abuff[WorkSegDispBuff[5].ucSetStaTim & 0x0f]);                 //地址为10,11；

	//时间设定13：高、低，第七段稳温
	LcdSendByte(0x10*ucOtherDispBuff[9]|bank06Abuff[WorkSegDispBuff[6].ucSetStaTim >> 0x04]);        //地址为16,17,0x80:时间设定13的外框；
	LcdSendByte(0x10*ucSegTimRimDispBuff[13]|bank06Abuff[WorkSegDispBuff[6].ucSetStaTim & 0x0f]);                 //地址为18,19；


    //时间设定12：高、低，第七段升温
	LcdSendByte(0x10*ucOtherDispBuff[8]|bank06Abuff[WorkSegDispBuff[6].ucSetRisTim >> 0x04]);        //地址为12,13,0x80:时间设定12的外框；
	LcdSendByte(0x10*ucSegTimRimDispBuff[12]|bank06Abuff[WorkSegDispBuff[6].ucSetRisTim & 0x0f]);                 //地址为14,15；
*/

	i16Test1 = 0x10*ucOtherDispBuff[7];

	//时间设定10：高、低，第六段升温
	LcdSendByte(i16Test1|bank06Abuff[WorkSegDispBuff[5].ucSetRisTim >> 0x04]);        //地址为4,5,0x80:时间设定10的外框；
	LcdSendByte(0x10*ucSegTimRimDispBuff[10]|bank06Abuff[WorkSegDispBuff[5].ucSetRisTim & 0x0f]);                 //地址为6,7；

	//时间设定11：高、低，第六段稳温
	LcdSendByte(0x00|bank06Abuff[WorkSegDispBuff[5].ucSetStaTim >> 0x04]);        //地址为8,9,0x80:时间设定11的外框；
	LcdSendByte(0x10*ucSegTimRimDispBuff[11]|bank06Abuff[WorkSegDispBuff[5].ucSetStaTim & 0x0f]);                 //地址为10,11；

	i16Test1 = 0x10*ucOtherDispBuff[8];

    //时间设定12：高、低，第七段升温
	LcdSendByte(i16Test1|bank06Abuff[WorkSegDispBuff[6].ucSetRisTim >> 0x04]);        //地址为12,13,0x80:时间设定12的外框；
	LcdSendByte(0x10*ucSegTimRimDispBuff[12]|bank06Abuff[WorkSegDispBuff[6].ucSetRisTim & 0x0f]);                 //地址为14,15；


	i16Test1 = 0x10*ucOtherDispBuff[9];

	//时间设定13：高、低，第七段稳温
	LcdSendByte(i16Test1|bank06Abuff[WorkSegDispBuff[6].ucSetStaTim >> 0x04]);        //地址为16,17,0x80:时间设定13的外框；
	LcdSendByte(0x10*ucSegTimRimDispBuff[13]|bank06Abuff[WorkSegDispBuff[6].ucSetStaTim & 0x0f]);                 //地址为18,19；



	LcdSendByte(bank06Abuff[uiDispBuff[7]%10]);                 //地址为8,9；
	LcdSendHalfByte(0x00);

	vdDrv164(CS_7);
	LcdSendId(0x05);           //写数据引导码         
	LcdSendAddress(0x00);      //从地址0开始

	//干球设定2：高、低
	LcdSendByte(0x10*ucDryTmpRimDispBuff[2]|bank07Abuff[WorkSegDispBuff[2].uiSetDryTmp >> 0x08]);                         //地址23、24，0x10：恒温；
	LcdSendByte(bank07Abuff[(WorkSegDispBuff[2].uiSetDryTmp >> 0x04)& 0x0f]);            //地址25、26，0x10：干球设定2外框；

	//湿球设定5：高、
	LcdSendByte(0x10*ucDot5DispBuff[4]|bank07Abuff[(WorkSegDispBuff[4].uiSetWetTmp >> 0x04)& 0x0f]);            //地址为14,15,0x10:湿球设定5的.5；
	LcdSendByte(0x10*ucWetTmpRimDispBuff[4]|bank07Abuff[WorkSegDispBuff[4].uiSetWetTmp >> 0x08]);        //地址为12,13,0x10:湿球设定5的外框；

	//湿球设定4：高、�
	LcdSendByte(0x10*ucDot5DispBuff[3]|bank07Abuff[(WorkSegDispBuff[3].uiSetWetTmp >> 0x04)& 0x0f]);            //地址为14,15,0x10:湿球设定5的.5；
	LcdSendByte(0x10*ucWetTmpRimDispBuff[3]|bank07Abuff[WorkSegDispBuff[3].uiSetWetTmp >> 0x08]);        //地址为12,13,0x10:湿球设定5的外框；


	//时间设定4：高、低，第三段升温
	LcdSendByte(bank07Abuff[WorkSegDispBuff[2].ucSetRisTim >> 0x04]);        //地址为12,13,0x80:时间设定4的外框；
	LcdSendByte(0x10*ucSegTimRimDispBuff[4]|bank07Abuff[WorkSegDispBuff[2].ucSetRisTim & 0x0f]);            //地址为14,15；

	//时间设定5：高、低，第三段稳温
	LcdSendByte(bank07Abuff[WorkSegDispBuff[2].ucSetStaTim >> 0x04]);        //地址为16,17,0x80:时间设定5的外框；
	LcdSendByte(0x10*ucSegTimRimDispBuff[5]|bank07Abuff[WorkSegDispBuff[2].ucSetStaTim & 0x0f]);            //地址为18,19；

	//时间设定6：高、低，第四段升温
	LcdSendByte(bank07Abuff[WorkSegDispBuff[3].ucSetRisTim >> 0x04]);        //地址为20,21,0x80:时间设定6的外框；
	LcdSendByte(0x10*ucSegTimRimDispBuff[6]|bank07Abuff[WorkSegDispBuff[3].ucSetRisTim & 0x0f]);            //地址为22,23；

	//测量电压：高、中、低
	LcdSendByte(bank07Abuff[uiDispBuff[6] >> 8]);           //地址为24,25,0x10: 循环风速的低；
	LcdSendByte(bank07Abuff[(uiDispBuff[6] >> 4)&0x0f]);        //地址为26,27,0x10：循环风速的高；
	LcdSendByte(bank07Abuff[uiDispBuff[6]&0x000f]);            //地址为28,29,0x10：循环风速的自动；

	i16Test = 0x00;
	i16Test  = ucOtherDispBuff[3]*0x02+ucOtherDispBuff[3]*0x20;
	i16Test += ucOtherDispBuff[4]*0x04+ucOtherDispBuff[4]*0x40;
	i16Test += ucOtherDispBuff[2]*0x08+ucOtherDispBuff[2]*0x80;
	LcdSendByte(i16Test);            //地址为28,29,0x10：循环风速的自动；
	vdDrv164(CS_NO); 

}
