   
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
struct SEGMENT                   //½á¹¹Ìå
	{
	    unsigned int  ucSetRisTim;    //ÉýÎÂÊ±¼ä
	    unsigned int  uiSetDryTmp;    //ÉèÖÃ¸ÉÎÂ
		unsigned int  uiSetWetTmp;    //ÉèÖÃÊªÎÂ
      	unsigned int  ucSetStaTim;    //ÎÈÎÂÊ±¼ä
		
    };
struct SEGMENT   WorkSeg[10];
struct SEGMENT   WorkSegSet[10];
struct SEGMENT   WorkSegDispBuff[10];
extern unsigned int    uiDispBuff[9]; 
extern unsigned char   ucUpDownDispBuff[2];     
extern unsigned char   ucRTCDispBuff[5]; 
extern unsigned char   ucCurveDispBuff[4];
extern unsigned char   ucSegTimRimDispBuff[20];  //0-19,rim,Íâ¿ò
extern unsigned char   ucDryTmpRimDispBuff[10];
extern unsigned char   ucWetTmpRimDispBuff[10];
extern unsigned char   ucDot5DispBuff[10];



extern unsigned int    uiWorkSegDispBuff[40];
extern unsigned char   flgFlashShow;
extern unsigned char   flgUpDownFlashShow;
extern unsigned char   flgMainSensor;

extern unsigned char   ucSetupClassIndex;
extern unsigned char   ucOtherDispBuff[10];    //0:ÉýÎÂ;1:ºãÎÂ;2:Æ«ÎÂ;3:È±Ïà;4:¹ýÔØ;5:ÖúÈ¼;6:ÅÅÊª;7:(Ñ­»··çËÙ)×Ô¶¯;8:(Ñ­»··çËÙ)¸ß;9:(Ñ­»··çËÙ)µÍ;

                                        //0     1    2    3    4   5    6     7    8    9   Ãð   A   b    C
const unsigned char  bank01Abuff[11]  = {0x5F,0x50,0x6B,0x79,0x74,0x3D,0x3F,0x58,0x7F,0x7D,0x00};  //IC1:CS1µÄ1¡¢2¡¢3¡¢4¡¢5¡¢6¡¢7¡¢8¡¢9Î»
const unsigned char  bank01Bbuff[11]  = {0xAF,0xA0,0x6D,0xE9,0xE2,0xCB,0xCF,0xA1,0xEF,0xEB,0x00};  //IC1:CS1µÄ10¡¢11¡¢12¡¢13Î»


const unsigned char  bank02Abuff[11]  = {0x5F,0x06,0x6B,0x2F,0x36,0x3D,0x7D,0x07,0x7F,0x3F,0x00};  //IC1:CS1µÄ1¡¢2¡¢3¡¢4¡¢5¡¢6¡¢7¡¢8¡¢9Î»
const unsigned char  bank02Bbuff[11]  = {0xAF,0x06,0x6D,0x4F,0xC6,0xCB,0xEB,0x0E,0xEF,0xCF,0x00};  //IC1:CS1µÄ10¡¢11¡¢12¡¢13Î»


const unsigned char  bank03Abuff[11]  = {0x5F,0x06,0x6B,0x2F,0x36,0x3D,0x7D,0x07,0x7F,0x3F,0x00};  //IC3:CS5µÄ23¡¢24¡¢25¡¢26¡¢27¡¢28
const unsigned char  bank03Bbuff[11]  = {0x5F,0x50,0x6B,0x79,0x74,0x3D,0x3F,0x58,0x7F,0x7D,0x00};  //IC3:CS5µÄ29¡¢30¡¢31¡¢32¡¢33¡¢34¡¢35¡¢36

const unsigned char  bank04Abuff[11]  = {0xAF,0xA0,0x6D,0xE9,0xE2,0xCB,0xCF,0xA1,0xEF,0xEB,0x00};  //IC4:CS3µÄ37¡¢38¡¢39¡¢40¡¢41¡¢42;47¡¢48¡¢49¡¢50¡¢51¡¢52
const unsigned char  bank04Bbuff[11]  = {0x5F,0x50,0x6B,0x79,0x74,0x3D,0x3F,0x58,0x7F,0x7D,0x00};  //IC4:CS3µÄ43¡¢44¡¢45¡¢46

const unsigned char  bank05Abuff[11]  = {0xAF,0xA0,0x6D,0xE9,0xE2,0xCB,0xCF,0xA1,0xEF,0xEB,0x00};  //IC4:CS3µÄ37¡¢38¡¢39¡¢40¡¢41¡¢42;47¡¢48¡¢49¡¢50¡¢51¡¢52
const unsigned char  bank05Bbuff[11]  = {0x5F,0x50,0x6B,0x79,0x74,0x3D,0x3F,0x58,0x7F,0x7D,0x00};  //IC4:CS3µÄ43¡¢44¡¢45¡¢46

const unsigned char  bank06Abuff[11]  = {0xAF,0xA0,0x6D,0xE9,0xE2,0xCB,0xCF,0xA1,0xEF,0xEB,0x00};  //IC4:CS3µÄ37¡¢38¡¢39¡¢40¡¢41¡¢42;47¡¢48¡¢49¡¢50¡¢51¡¢52

const unsigned char  bank07Abuff[11]  = {0xAF,0xA0,0x6D,0xE9,0xE2,0xCB,0xCF,0xA1,0xEF,0xEB,0x00};  //IC4:CS3µÄ37¡¢38¡¢39¡¢40¡¢41¡¢42;47¡¢48¡¢49¡¢50¡¢51¡¢52


//unsigned char  bank10buff[11]  = {0xAF,0xA0,0xCB,0xE9,0xE4,0x6D,0x6F,0xA8,0xEF,0xED,0x00};  //0x10const  PROGMEM
//unsigned char  bank08buff[12]  = {0xF5,0x60,0xB6,0xF2,0x63,0xD3,0xD7,0x70,0xF7,0xF3,0x00,0x75};  //0x08  ;0x75: ÏÔÊ¾ n
//unsigned char  bank01buff[11]  = {0xFA,0x60,0xBC,0xF4,0x66,0xD6,0xDE,0x70,0xFE,0xF6,0x00};  //0x01

unsigned char i16Test;
unsigned char i16Test1;
unsigned char i16Test2;
unsigned char test10 = 1;
unsigned char test12 = 0;

extern volatile unsigned char enWorkMode;

/***************************************************************************
* Function Name  : vdDrv164
* Description    : 74LS164Êä³ö,·¢ËÍ8¸öÆ¬Ñ¡ÐÅºÅ
* Input          : ÒªÊä³öµÄ×Ö½ÚÊý¾Ý
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
Ð´Èë±ÈÌØÎ»
²ÎÊýn: 0»ò1
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
//-----***-·¢ËÍÇ°µ¼Âë£º±íÊ¾²Ù×÷ÀàÐÍ-***-----//
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

//-----***-·¢ËÍµØÖ·-***-----//
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
ËÍÊý¾Ý£º
ÏÈËÍÈýÎ»ÃüÁîÄ£Ê½Âë101£¬ÔÙËÍ10¸ö
±ÈÌØÎ»£¨ËÍ6Î»µØÖ·+4Î»Êý¾Ý£©¡£
cs: Æ¬Ñ¡£¬address:6Î»µØÖ·Î»£»data1£º4Î»Êý¾Ý
********************************************/
void lcdwd(unsigned char address,unsigned char dat)  
{
    unsigned char i;
    
    //ËÍÃüÁîÄ£Ê½ÀàÐÍÂë£º101£»0x05;
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

//-----***-·¢ËÍÒ»¸ö×Ö½Ú-***-----//
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

//-----***-·¢ËÍ°ë¸ö×Ö½Ú-***-----//
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
ËÍÃüÁî×Ö£¬9Î»£¬×îµÍÎ»Îª0»ò1£»
ÏÈËÍÈýÎ»ÃüÁîÄ£Ê½Âë100£¬ÔÙËÍ9Î»
±ÈÌØ×é³ÉµÄÃüÁî×Ö£¬×îºóÒ»Î»Îª0¡£
cs: Æ¬Ñ¡£¬cmdcode:ÃüÁî×Ö
*******************************/
void lcdwc(unsigned char cs, unsigned char cmdcode)  //ËÍ¿ØÖÆ×Ö
{  
    unsigned char i;
   
    vdDrv164(cs);  //write cs signal
	//ËÍÃüÁîÄ£Ê½ÀàÐÍÂë£º100b:0x04;
	LcdSendId(0x04);
	for(i=0;i<8;i++)
    {
	    if((cmdcode&0x80)==0x80)
            lcdwbit(1);
        else
            lcdwbit(0);
        cmdcode<<=1;
    }
    lcdwbit(0);    //ËÍ×îºóÒ»Î»£º0»ò1ÎÞ¹ØÎ»     
    SET_LCDDA;
	vdDrv164(CS_NO);

}

/**********************************
ÉÏµçÊ±¸´Î»£¬¶¨Ê±¸´Î»
***********************************/
void lcdreset(void)                    //³õÊ¼»¯
{  
    
	//¸´Î»Æ¬1
	lcdwc(CS_1,CMDOFF);                  //µôµç
    lcdwc(CS_1,CMDLCDOFF);               //¹Ø±Õ

	//¸´Î»Æ¬2
	lcdwc(CS_2,CMDOFF);                  //µôµç
    lcdwc(CS_2,CMDLCDOFF);               //¹Ø±Õ

	//¸´Î»Æ¬3
	lcdwc(CS_3,CMDOFF);                  //µôµç
    lcdwc(CS_3,CMDLCDOFF);               //¹Ø±Õ

	//¸´Î»Æ¬4
	lcdwc(CS_4,CMDOFF);                  //µôµç
    lcdwc(CS_4,CMDLCDOFF);               //¹Ø±Õ

	//¸´Î»Æ¬5
	lcdwc(CS_5,CMDOFF);                  //µôµç
    lcdwc(CS_5,CMDLCDOFF);               //¹Ø±Õ

	//¸´Î»Æ¬6
	lcdwc(CS_6,CMDOFF);                  //µôµç
    lcdwc(CS_6,CMDLCDOFF);               //¹Ø±Õ

	//¸´Î»Æ¬7
	lcdwc(CS_7,CMDOFF);                  //µôµç
    lcdwc(CS_7,CMDLCDOFF);               //¹Ø±Õ

        
	_delay_ms(100);

    lcdwc(CS_1,CMDON);                   //ÉÏµç
    lcdwc(CS_1,CMDLCDON);                //ÏÔÊ¾
    lcdwc(CS_1,CMDB3C4);                 //Ä£Ê½ÉèÖÃ,1/3Æ«Ñ¹£¬4¸ö¹«¹²¶Ë¿Ú

	lcdwc(CS_2,CMDON);                   //ÉÏµç
    lcdwc(CS_2,CMDLCDON);                //ÏÔÊ¾
    lcdwc(CS_2,CMDB3C4);                 //Ä£Ê½ÉèÖÃ,1/3Æ«Ñ¹£¬4¸ö¹«¹²¶Ë¿Ú

	lcdwc(CS_3,CMDON);                   //ÉÏµç
    lcdwc(CS_3,CMDLCDON);                //ÏÔÊ¾
    lcdwc(CS_3,CMDB3C4);                 //Ä£Ê½ÉèÖÃ,1/3Æ«Ñ¹£¬4¸ö¹«¹²¶Ë¿Ú

	lcdwc(CS_4,CMDON);                   //ÉÏµç
    lcdwc(CS_4,CMDLCDON);                //ÏÔÊ¾
    lcdwc(CS_4,CMDB3C4);                 //Ä£Ê½ÉèÖÃ,1/3Æ«Ñ¹£¬4¸ö¹«¹²¶Ë¿Ú

	lcdwc(CS_5,CMDON);                   //ÉÏµç
    lcdwc(CS_5,CMDLCDON);                //ÏÔÊ¾
    lcdwc(CS_5,CMDB3C4);                 //Ä£Ê½ÉèÖÃ,1/3Æ«Ñ¹£¬4¸ö¹«¹²¶Ë¿Ú

	lcdwc(CS_6,CMDON);                   //ÉÏµç
    lcdwc(CS_6,CMDLCDON);                //ÏÔÊ¾
    lcdwc(CS_6,CMDB3C4);                 //Ä£Ê½ÉèÖÃ,1/3Æ«Ñ¹£¬4¸ö¹«¹²¶Ë¿Ú

	lcdwc(CS_7,CMDON);                   //ÉÏµç
    lcdwc(CS_7,CMDLCDON);                //ÏÔÊ¾
    lcdwc(CS_7,CMDB3C4);                 //Ä£Ê½ÉèÖÃ,1/3Æ«Ñ¹£¬4¸ö¹«¹²¶Ë¿Ú

}

/**********************************
³õÊ¼»¯ÓÐ¹ØI/O£¬¸´Î»£¬²¿·ÖÏÔÊ¾´¦Àí£»
***********************************/
void  vdInitLCD(void)
{
   

    CLR_LCDLED;  //close backlight
    vdDrv164(CS_NO);//CS singal OUTput is high
	lcdreset();   //¸´Î»  
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
    static unsigned char   ucFlashShowCnt =0;  //ÉÁË¸¼ÆÊý
    static unsigned char   flgFlash = 0;    

    ucFlashShowCnt++;                 //µ÷ÓÃÒ»´Î£¬¼Ó1£»100msµ÷ÓÃÒ»´Î
	if(ucFlashShowCnt >=  2)
	{
	    ucFlashShowCnt = 0;
		flgFlash ^= 0x01;
    }
    if(!flgFlash)
	{    
	    if(flgUpDownFlashShow ==0x01)           //"ÉÏÅï" ÉÁË¸
		{
		    if(flgMainSensor == DWSENSOR)   //ÆøÁ÷ÉÏÉýÊ½¿¾·¿
			{
	            ucUpDownDispBuff[UPSENSOR] = 0x00;

	        }
			else if(flgMainSensor == UPSENSOR)   //ÆøÁ÷ÏÂ½µÊ½¿¾·¿
			{
			    ucUpDownDispBuff[DWSENSOR] = 0x00;
			}
     	}
	    if(ucSetupClassIndex == 0x04) 
		{
		    if(flgFlashShow == 0x01)
			    uiDispBuff[5]  = 0x0aaa;
		}
	    else if(ucSetupClassIndex == DORT_INDEX)   //Ê±¼ä²¿·ÖÉÁË¸,0x00
		{
		    
			switch(flgFlashShow)   
	        {   
	           
			    case 0x05:        //·ÖÖÓÉÁË¸
				    ucRTCDispBuff[0] = 0xaa;
			        break;
	            case 0x04:        //Ð¡Ê±ÉÁË¸
				    ucRTCDispBuff[1] = 0xaa;
			        break;
	            case 0x03:        //ÈÕÉÁË¸
				    ucRTCDispBuff[2] = 0xaa;
			        break;
	            case 0x02:        //ÔÂÉÁË¸
				    ucRTCDispBuff[3] = 0xaa;
			        break;
	            case 0x01:        //ÄêÉÁË¸
				    ucRTCDispBuff[4] =  0xaa;
			        break;
	            
			    default:
			        break;
		    }//end of switch(flgFlashShow) 
			
		}
		else if(ucSetupClassIndex == UPDW_INDEX)  //ÉÏ¡¢ÏÂÅïÉÁË¸0x03
		{
		    switch(flgFlashShow)   
	        {   
	          
			    case 0x01:        //"ÉÏÅï"ÉÁË¸
					ucUpDownDispBuff[0] = 0x00;
			        break;
	            case 0x02:        //"ÏÂÅï"ÉÁË¸
			    {
				    ucUpDownDispBuff[1] = 0x00;
			        break;
	            }
			    
			    default:
			        break;
		    }//end of switch(flgFlashShow) 
		
		
		}
		else if(ucSetupClassIndex == CURV_INDEX)  //ÇúÏß²¿·ÖÉÁË¸0x01
		{
		    switch(flgFlashShow)   
	        {   
	          
			    case 0x01:        //×ÔÉèÉÁË¸
		    	
					ucCurveDispBuff[0] = 0x00;
			        break;
	            case 0x02:        //ÏÂ²¿Ò¶ÉÁË¸
			    {
				    ucCurveDispBuff[1] = 0x00;
			        break;
	            }
			    case 0x03:        //ÖÐ²¿Ò¶ÉÁË¸
			    {
				    ucCurveDispBuff[2]  = 0x00;
			        break;
	            }
			    case 0x04:        //ÉÏ²¿Ò¶ÉÁË¸
			    {
				    ucCurveDispBuff[3]  = 0x00;
			        break;
	            }
			    default:
			        break;
		    }//end of switch(flgFlashShow) 
		}
		else if(ucSetupClassIndex == PARA_INDEX)   //ÎÂÊª¶È²ÎÊýÉÁË¸Ð§¹û0x02
		{
		    switch(flgFlashShow)   
	        {   
				/*½×¶Î1*/
				case 0x01:        //½×¶Î1¸ÉÎÂÉèÖÃÉÁË¸
					WorkSegDispBuff[0].uiSetDryTmp = 0xaaa;
				    break;
		        case 0x02:        //½×¶Î1ÊªÎÂÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[0].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x03:        //½×¶Î1ÎÈÎÂÊ±¼äÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[0].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*½×¶Î2*/
				case 0x04:        //½×¶Î2ÉýÎÂÊ±¼äÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[1].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x05:        //½×¶Î2¸ÉÎÂÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[1].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x06:        //½×¶Î2ÊªÎÂÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[1].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x07:        //½×¶Î2ÎÈÎÂÊ±¼äÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[1].ucSetStaTim = 0x0aa;
				    break;
		        }
	            /*½×¶Î3*/
				case 0x08:        //½×¶Î3ÉýÎÂÊ±¼äÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[2].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x09:        //½×¶Î3¸ÉÎÂÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[2].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x0a:        //½×¶Î3ÊªÎÂÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[2].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x0b:        //½×¶Î3ÎÈÎÂÊ±¼äÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[2].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*½×¶Î4*/
				case 0x0c:        //½×¶Î4ÉýÎÂÊ±¼äÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[3].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x0d:        //½×¶Î4¸ÉÎÂÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[3].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x0e:        //½×¶Î4ÊªÎÂÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[3].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x0f:        //½×¶Î4ÎÈÎÂÊ±¼äÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[3].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*½×¶Î5*/
				case 0x10:        //½×¶Î5ÉýÎÂÊ±¼äÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[4].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x11:        //½×¶Î5¸ÉÎÂÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[4].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x12:        //½×¶Î5ÊªÎÂÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[4].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x13:        //½×¶Î5ÎÈÎÂÊ±¼äÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[4].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*½×¶Î6*/
				case 0x14:        //½×¶Î6ÉýÎÂÊ±¼äÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[5].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x15:        //½×¶Î6¸ÉÎÂÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[5].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x16:        //½×¶Î6ÊªÎÂÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[5].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x17:        //½×¶Î6ÎÈÎÂÊ±¼äÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[5].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*½×¶Î7*/
				case 0x18:        //½×¶Î7ÉýÎÂÊ±¼äÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[6].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x19:        //½×¶Î7¸ÉÎÂÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[6].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x1a:        //½×¶Î7ÊªÎÂÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[6].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x1b:        //½×¶Î7ÎÈÎÂÊ±¼äÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[6].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*½×¶Î8*/
				case 0x1c:        //½×¶Î8ÉýÎÂÊ±¼äÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[7].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x1d:        //½×¶Î8¸ÉÎÂÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[7].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x1e:        //½×¶Î8ÊªÎÂÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[7].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x1f:        //½×¶Î8ÎÈÎÂÊ±¼äÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[7].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*½×¶Î9*/
				case 0x20:        //½×¶Î9ÉýÎÂÊ±¼äÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[8].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x21:        //½×¶Î9¸ÉÎÂÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[8].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x22:        //½×¶Î9ÊªÎÂÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[8].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x23:        //½×¶Î9ÎÈÎÂÊ±¼äÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[8].ucSetStaTim = 0x0aa;
				    break;
		        }
				/*½×¶Î10*/
				case 0x24:        //½×¶Î10ÉýÎÂÊ±¼äÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[9].ucSetRisTim = 0x0aa;
				    break;
		        }
				case 0x25:        //½×¶Î10¸ÉÎÂÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[9].uiSetDryTmp = 0xaaa;
				    break;
		        }
				case 0x26:        //½×¶Î10ÊªÎÂÉèÖÃÉÁË¸
				{
				    WorkSegDispBuff[9].uiSetWetTmp = 0xaaa;
				    break;
		        }
				case 0x27:        //½×¶Î10ÎÈÎÂÊ±¼äÉèÖÃÉÁË¸
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
	LcdSendId(0x05);           //Ð´Êý¾ÝÒýµ¼Âë         
	LcdSendAddress(0x00);      //´ÓµØÖ·0¿ªÊ¼


    //¸ÉÇòÉè¶¨1£º¸ß¡¢µÍ, ÕûÊý£¬È¡¸ßÁ½Î»
    LcdSendByte(0x80*ucDryTmpRimDispBuff[0]|bank01Abuff[WorkSegDispBuff[0].uiSetDryTmp >> 0x08]);                         //µØÖ·19¡¢20£¬0x01£ºÉýÎÂ£»
	LcdSendByte(bank01Abuff[(WorkSegDispBuff[0].uiSetDryTmp >> 0x04)& 0x0f]);             //µØÖ·21¡¢22£¬0x01£º¸ÉÇòÉè¶¨1Íâ¿ò£»

	//¸ÉÇòÉè¶¨2£º¸ß¡¢µÍ
	LcdSendByte(0x80*ucDryTmpRimDispBuff[1]|bank01Abuff[WorkSegDispBuff[1].uiSetDryTmp >> 0x08]);                         //µØÖ·23¡¢24£¬0x10£ººãÎÂ£»
	LcdSendByte(bank01Abuff[(WorkSegDispBuff[1].uiSetDryTmp >> 0x04)& 0x0f]);            //µØÖ·25¡¢26£¬0x10£º¸ÉÇòÉè¶¨2Íâ¿ò£»

	//ÊªÇòÉè¶¨3£º¸ß¡¢µÍ
	LcdSendByte(0x80*ucDot5DispBuff[2]|bank01Abuff[(WorkSegDispBuff[2].uiSetWetTmp >> 0x04)& 0x0f]);		   //µØÖ·Îª18,19,0x80:ÊªÇòÉè¶¨3µÄ.5;
	LcdSendByte(0x80*ucWetTmpRimDispBuff[2]|bank01Abuff[WorkSegDispBuff[2].uiSetWetTmp >> 0x08]);		 //µØÖ·Îª16,17,0x80:ÊªÇòÉè¶¨3µÄÍâ¿ò£»

	 //ÊªÇòÉè¶¨2£º¸ß¡¢µÍ
	 LcdSendByte(0x80*ucDot5DispBuff[1]|bank01Abuff[(WorkSegDispBuff[1].uiSetWetTmp >> 0x04)& 0x0f]);			//µØÖ·Îª14,15,0x80:ÊªÇòÉè¶¨2µÄ.5;
	 LcdSendByte(0x80*ucWetTmpRimDispBuff[1]|bank01Abuff[WorkSegDispBuff[1].uiSetWetTmp >> 0x08]);		 //µØÖ·Îª12,13,0x80:ÊªÇòÉè¶¨2µÄÍâ¿ò£»

    //ÊªÇòÉè¶¨1£º¸ß¡¢µÍ
     LcdSendByte(0x80*ucDot5DispBuff[0]|bank01Abuff[(WorkSegDispBuff[0].uiSetWetTmp >> 0x04)& 0x0f]);           //µØÖ·Îª10,11,0x80:ÊªÇòÉè¶¨1µÄ.5£»
	 LcdSendByte(0x80*ucWetTmpRimDispBuff[0]|bank01Abuff[WorkSegDispBuff[0].uiSetWetTmp >> 0x08]);       //µØÖ·Îª8,9,0x80:ÊªÇòÉè¶¨1µÄÍâ¿ò£»
 
	//ÎÈÎÂÊ±¼äÉè¶¨1£º¸ß¡¢µÍ£¬µÚÒ»¶ÎÎÈÎÂucSegTimRimDispBuff[i]
	LcdSendByte(0x10*ucOtherDispBuff[5]|bank01Bbuff[WorkSegDispBuff[0].ucSetStaTim >> 0x04]);        //µØÖ·Îª0,1,0x80:ÅÅÊª£»
	LcdSendByte(0x10*ucSegTimRimDispBuff[1]|bank01Bbuff[WorkSegDispBuff[0].ucSetStaTim & 0x0f]); //µØÖ·Îª2,3,0x80:Ê±¼äÉè¶¨1µÄÍâ¿ò£»

	//ÉýÎÂÊ±¼äÉè¶¨2£º¸ß¡¢µÍ£¬µÚ¶þ¶ÎÉýÎÂ
	LcdSendByte(0x10*ucOtherDispBuff[6]|bank01Bbuff[WorkSegDispBuff[1].ucSetRisTim >> 0x04]);        //µØÖ·Îª4,5,0x80:ÖúÈ¼£»
	LcdSendByte(0x10*ucSegTimRimDispBuff[2]|bank01Bbuff[WorkSegDispBuff[1].ucSetRisTim & 0x0f]); //µØÖ·Îª6,7,0x80:Ê±¼äÉè¶¨2µÄÍâ¿ò£»

    //Ê±¼äÉè¶¨3£º¸ß¡¢µÍ£¬µÚ¶þ¶ÎÎÈÎÂ
	LcdSendByte(bank01Bbuff[WorkSegDispBuff[1].ucSetStaTim >> 0x04]);        //µØÖ·Îª8,9,0x80:Ê±¼äÉè¶¨3µÄÍâ¿ò£»
	LcdSendByte(0x10*ucSegTimRimDispBuff[3]|bank01Bbuff[WorkSegDispBuff[1].ucSetStaTim & 0x0f]);            //µØÖ·Îª10,11£»



	vdDrv164(CS_2);
	LcdSendId(0x05);           //Ð´Êý¾ÝÒýµ¼Âë         
	LcdSendAddress(0x00);      //´ÓµØÖ·0¿ªÊ¼

	i16Test2 = ((ucUpDownDispBuff[0]&0x01)*0x80);
	LcdSendByte(i16Test2|bank02Abuff[uiDispBuff[0]/100]);          //addr 7,8;0x08: È±Ïà£
	i16Test2 = ((ucUpDownDispBuff[0]&0x02)*0x40);
	LcdSendByte(i16Test2|bank02Abuff[uiDispBuff[0]%100/10]);       //addr 9,10£»0x08:È±Ïà¿ò
	if((enWorkMode == DEBUG_MasterAddressSet)||(enWorkMode == DEBUG_SlaveAddressSet))
	    LcdSendByte(bank02Abuff[uiDispBuff[0]%10]);           //addr 11,12;0x08:ÖÐ¼äÎ»Ð¡Êýµã
	else
	    LcdSendByte(0x80|bank02Abuff[uiDispBuff[0]%10]);           //addr 11,12;0x08:ÖÐ¼äÎ»Ð¡Êýµã
	i16Test2 =((ucUpDownDispBuff[1]&0x01)*0x80);
	//ÊªÇòÎÂ¶È£º¸ß¡¢ÖÐ¡¢µÍ
	LcdSendByte(i16Test2|bank02Abuff[uiDispBuff[1]/100]);          //addr 7,8;0x08: È±Ïà£»
	i16Test2 =((ucUpDownDispBuff[1]&0x02)*0x40);
	LcdSendByte(i16Test2|bank02Abuff[uiDispBuff[1]%100/10]);       //addr 9,10£»0x08:È±Ïà¿ò
	LcdSendByte(0x80|bank02Abuff[uiDispBuff[1]%10]);           //addr 11,12;0x08:ÖÐ¼äÎ»Ð¡Êýµã

	LcdSendByte(bank02Abuff[uiDispBuff[4]/100]);                //µØÖ·Îª0,1;
	LcdSendByte(bank02Abuff[uiDispBuff[4]%100/10]);             //µØÖ·Îª2,3;
	LcdSendByte(0x80|bank02Abuff[uiDispBuff[4]%10]);            //µØÖ·Îª4,5£»0x08£º½×¶ÎÊ±¼äµÄÐ¡Êýµã;

    //×ÜÊ±¼ä/**/	
	LcdSendByte(bank02Abuff[uiDispBuff[5] >> 0x08]);              //µØÖ·Îª6,7;
	LcdSendByte(bank02Abuff[(uiDispBuff[5] >> 4)&0x0f]);             //µØÖ·Îª8,9;
	LcdSendByte(bank02Abuff[uiDispBuff[5]&0x000f]);                 //µØÖ·Îª10,11;

	LcdSendByte(bank02Bbuff[uiDispBuff[3]/100]);          //addr 13,14£»0x08:¹ýÔØ
	//Ä¿±ê¸ÉÎÂ£º¸ß¡¢ÖÐ¡¢µÍ
	
	LcdSendByte(0x10|bank02Bbuff[uiDispBuff[2]%10]);           //addr 17,18;0x08:ÖÐ¼äÎ»Ð¡Êýµã

	LcdSendByte(((ucOtherDispBuff[1]&0x01)<<4)|bank02Bbuff[uiDispBuff[2]%100/10]);       //addr 15,16£»0x08:¹ýÔØ¿ò
  LcdSendByte(((ucOtherDispBuff[0]&0x01)<<4)|bank02Bbuff[uiDispBuff[2]/100]);          //addr 13,14£»0x08:¹ýÔØ

	vdDrv164(CS_3);
	LcdSendId(0x05);           //Ð´Êý¾ÝÒýµ¼Âë         
	LcdSendAddress(0x00);      //´ÓµØÖ·0¿ªÊ¼
	
	LcdSendByte(0x80|bank03Abuff[uiDispBuff[3]%10]);			//µØÖ·Îª16,17;0x08£ºÄ¿±êÊªÎÂµÄÐ¡Êýµã;

	LcdSendByte(bank03Abuff[uiDispBuff[3]%100/10]);             //µØÖ·Îª14,15;

  //¸ÉÇòÉè¶¨4£º¸ß¡¢µÍ
	LcdSendByte(0x80*ucDryTmpRimDispBuff[3]|bank03Bbuff[WorkSegDispBuff[3].uiSetDryTmp >> 0x08]);       //µØÖ·Îª4,5,0x10:¸ÉÇòÉè¶¨4µÄÍâ¿ò£»
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[3].uiSetDryTmp >> 0x04)& 0x0f]);                //µØÖ·Îª6,7;

	//¸ÉÇòÉè¶¨5£º¸ß¡¢µÍ
	LcdSendByte(0x80*ucDryTmpRimDispBuff[4]|bank03Bbuff[WorkSegDispBuff[4].uiSetDryTmp >> 0x08]);       //µØÖ·Îª0,1,0x80:¸ÉÇòÉè¶¨5µÄÍâ¿ò£»
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[4].uiSetDryTmp >> 0x04)& 0x0f]);                //µØÖ·Îª2,3£»
	
    //¸ÉÇòÉè¶¨6£º¸ß¡¢µÍ
	LcdSendByte(0x80*ucDryTmpRimDispBuff[5]|bank03Bbuff[WorkSegDispBuff[5].uiSetDryTmp >> 0x08]);       //µØÖ·Îª4,5,0x80:¸ÉÇòÉè¶¨6µÄÍâ¿ò£»
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[5].uiSetDryTmp >> 0x04)& 0x0f]);                //µØÖ·Îª6,7;

    //¸ÉÇòÉè¶¨7£º¸ß¡¢µÍ
	LcdSendByte(0x80*ucDryTmpRimDispBuff[6]|bank03Bbuff[WorkSegDispBuff[6].uiSetDryTmp >> 0x08]);        //µØÖ·Îª8,9,0x80:¸ÉÇòÉè¶¨7µÄÍâ¿ò£»
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[6].uiSetDryTmp >> 0x04)& 0x0f]);                 //µØÖ·Îª10,11£»

	//¸ÉÇòÉè¶¨8£º¸ß¡¢µÍ
	LcdSendByte(0x80*ucDryTmpRimDispBuff[7]|bank03Bbuff[WorkSegDispBuff[7].uiSetDryTmp >> 0x08]);       //µØÖ·Îª0,1,0x80:¸ÉÇòÉè¶¨8µÄÍâ¿ò£»
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[7].uiSetDryTmp >> 0x04)& 0x0f]);                //µØÖ·Îª2,3£»
	
    //¸ÉÇòÉè¶¨9£º¸ß¡¢µÍ
	LcdSendByte(0x80*ucDryTmpRimDispBuff[8]|bank03Bbuff[WorkSegDispBuff[8].uiSetDryTmp >> 0x08]);       //µØÖ·Îª4,5,0x80:¸ÉÇòÉè¶¨9µÄÍâ¿ò£»
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[8].uiSetDryTmp >> 0x04)& 0x0f]);                //µØÖ·Îª6,7;

    //¸ÉÇòÉè¶¨10£º¸ß¡¢µÍ
	LcdSendByte(0x80*ucDryTmpRimDispBuff[9]|bank03Bbuff[WorkSegDispBuff[9].uiSetDryTmp >> 0x08]);        //µØÖ·Îª8,9,0x80:¸ÉÇòÉè¶¨10µÄÍâ¿ò£»
	LcdSendByte(bank03Bbuff[(WorkSegDispBuff[9].uiSetDryTmp >> 0x04)& 0x0f]);                 //µØÖ·Îª10,11£»

	vdDrv164(CS_4);
	LcdSendId(0x05);           //Ð´Êý¾ÝÒýµ¼Âë         
	LcdSendAddress(0x00);      //´ÓµØÖ·0¿ªÊ¼

	//ÊªÇòÉè¶¨10£º¸ß¡¢µÍ
	LcdSendByte(bank04Abuff[(WorkSegDispBuff[9].uiSetWetTmp >> 0x04)& 0x0f]);                 //µØÖ·Îª18,19£»
	LcdSendByte(0x10*ucWetTmpRimDispBuff[9]|bank04Abuff[WorkSegDispBuff[9].uiSetWetTmp >> 0x08]);        //µØÖ·Îª16,17,0x10:ÊªÇòÉè¶¨10µÄÍâ¿ò£»

	//ÊªÇòÉè¶¨9£º¸ß¡¢µÍ
	LcdSendByte(0x10*ucDot5DispBuff[8]|bank04Abuff[(WorkSegDispBuff[8].uiSetWetTmp >> 0x04)& 0x0f]);            //µØÖ·Îª14,15,0x10:ÊªÇòÉè¶¨9µÄ.5£»
	LcdSendByte(0x10*ucWetTmpRimDispBuff[8]|bank04Abuff[WorkSegDispBuff[8].uiSetWetTmp >> 0x08]);        //µØÖ·Îª12,13,0x10:ÊªÇòÉè¶¨9µÄÍâ¿ò£»

	//ÊªÇòÉè¶¨8£º¸ß¡¢µÍ
	
	LcdSendByte(0x10*ucDot5DispBuff[7]|bank04Abuff[(WorkSegDispBuff[7].uiSetWetTmp >> 0x04)& 0x0f]);            //µØÖ·Îª26,27,0x10:ÊªÇòÉè¶¨8µÄ.5£»
	LcdSendByte(0x10*ucWetTmpRimDispBuff[7]|bank04Abuff[WorkSegDispBuff[7].uiSetWetTmp >> 0x08]);        //µØÖ·Îª24,25,0x10:ÊªÇòÉè¶¨8µÄÍâ¿ò£»

	//ÊªÇòÉè¶¨7£º¸ß¡¢µÍ
	LcdSendByte(0x10*ucDot5DispBuff[6]|bank04Abuff[(WorkSegDispBuff[6].uiSetWetTmp >> 0x04)& 0x0f]);            //µØÖ·Îª22,23,0x10:ÊªÇòÉè¶¨7µÄ.5£»
	LcdSendByte(0x10*ucWetTmpRimDispBuff[6]|bank04Abuff[WorkSegDispBuff[6].uiSetWetTmp >> 0x08]);        //µØÖ·Îª20,21,0x10:ÊªÇòÉè¶¨7µÄÍâ¿ò£»

	LcdSendByte(0x10*ucDot5DispBuff[5]|bank04Abuff[(WorkSegDispBuff[5].uiSetWetTmp >> 0x04)& 0x0f]);            //µØÖ·Îª18,19,0x10:ÊªÇòÉè¶¨6µÄ.5£»
	LcdSendByte(0x10*ucWetTmpRimDispBuff[5]|bank04Abuff[WorkSegDispBuff[5].uiSetWetTmp >> 0x08]);        //µØÖ·Îª16,17,0x10:ÊªÇòÉè¶¨6µÄÍâ¿ò£»

	//Ê±¼äÉè¶¨19£º¸ß¡¢µÍ£¬µÚÊ®¶ÎÎÈÎÂ
	LcdSendByte(0x80*ucSegTimRimDispBuff[19]|bank04Bbuff[WorkSegDispBuff[9].ucSetStaTim & 0x0f]);                 //µØÖ·Îª30,31£»
	LcdSendByte(bank04Bbuff[WorkSegDispBuff[9].ucSetStaTim >> 0x04]);        //µØÖ·Îª28,29,0x80:Ê±¼äÉè¶¨19µÄÍâ¿ò£»

	//Ê±¼äÉè¶¨18£º¸ß¡¢µÍ£¬µÚÊ®¶ÎÉýÎÂ
	LcdSendByte(0x80*ucSegTimRimDispBuff[18]|bank04Bbuff[WorkSegDispBuff[9].ucSetRisTim & 0x0f]);                 //µØÖ·Îª26,27£»
	LcdSendByte(bank04Bbuff[WorkSegDispBuff[9].ucSetRisTim >> 0x04]);        //µØÖ·Îª24,25,0x80:Ê±¼äÉè¶¨18µÄÍâ¿ò£»

	//Ê±¼äÉè¶¨17£º¸ß¡¢µÍ£¬µÚ¾Å¶ÎÎÈÎÂ£¬È¡µÍÁ½Î»
	LcdSendByte(0x80*ucSegTimRimDispBuff[17]|bank04Bbuff[WorkSegDispBuff[8].ucSetStaTim & 0x0f]);                 //µØÖ·Îª22,23£»
	LcdSendByte(bank04Bbuff[WorkSegDispBuff[8].ucSetStaTim >> 0x04]);        //µØÖ·Îª20,21,0x80:Ê±¼äÉè¶¨17µÄÍâ¿ò£»




	



	vdDrv164(CS_5);
	LcdSendId(0x05);           //Ð´Êý¾ÝÒýµ¼Âë         
	LcdSendAddress(0x00);      //´ÓµØÖ·0¿ªÊ¼

	//Ê±¼äÉè¶¨14£º¸ß¡¢µÍ£¬µÚ¾Å¶ÎÉýÎÂ
	LcdSendByte(bank05Abuff[WorkSegDispBuff[7].ucSetRisTim >> 0x04]);        //µØÖ·Îª4,5,0x80:Ê±¼äÉè¶¨16µÄÍâ¿ò£»
	LcdSendByte(0x10*ucSegTimRimDispBuff[14]|bank05Abuff[WorkSegDispBuff[7].ucSetRisTim & 0x0f]);                 //µØÖ·Îª6,7£»

    //Ê±¼äÉè¶¨15£º¸ß¡¢µÍ£¬µÚ°Ë¶ÎÎÈÎÂ
	LcdSendByte(bank05Abuff[WorkSegDispBuff[7].ucSetStaTim >> 0x04]);        //µØÖ·Îª0,1,0x80:Ê±¼äÉè¶¨15µÄÍâ¿ò£»
	LcdSendByte(0x10*ucSegTimRimDispBuff[15]|bank05Abuff[WorkSegDispBuff[7].ucSetStaTim & 0x0f]);                 //µØÖ·Îª2,3£»

	//Ê±¼äÉè¶¨16£º¸ß¡¢µÍ£¬µÚ¾Å¶ÎÉýÎÂ
	LcdSendByte(bank05Abuff[WorkSegDispBuff[8].ucSetRisTim >> 0x04]);        //µØÖ·Îª4,5,0x80:Ê±¼äÉè¶¨16µÄÍâ¿ò£»
	LcdSendByte(0x10*ucSegTimRimDispBuff[16]|bank05Abuff[WorkSegDispBuff[8].ucSetRisTim & 0x0f]);                 //µØÖ·Îª6,7£»

	//Ê±¼ä·Ö£º¸ß¡¢µÍ
	LcdSendByte(((ucCurveDispBuff[3]&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[0] & 0x0f]);         //µØÖ·Îª28,29,0x10:ÉÏ²¿Ò¶µÄÍâ¿ò£»
	LcdSendByte((((ucCurveDispBuff[3]>>4)&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[0]>> 0x04]);         //µØÖ·Îª26,27,0x10:ÉÏ²¿Ò¶£»

	//Ê±¼äÊ±£º¸ß¡¢µÍ
	LcdSendByte(((ucCurveDispBuff[2]&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[1] & 0x0f]);         //µØÖ·Îª24,25,0x10:ÖÐ²¿Ò¶µÄÍâ¿ò£»
	LcdSendByte((((ucCurveDispBuff[2]>>4)&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[1]>> 0x04]);         //µØÖ·Îª22,23,0x10:ÖÐ²¿Ò¶£»

    //date£º¸ß¡¢µÍ
	LcdSendByte(((ucCurveDispBuff[1]&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[2] & 0x0f]);         //µØÖ·Îª20,21
	LcdSendByte((((ucCurveDispBuff[1]>>4)&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[2]>> 0x04]);         //µØÖ·Îª18,19

	//ÔÂ·Ý£º¸ß¡¢µÍ
	LcdSendByte(bank05Bbuff[ucRTCDispBuff[3] & 0x0f]);         //µØÖ·Îª16,17,0x10:ÏÂ²¿Ò¶µÄÍâ¿ò£»
	LcdSendByte(bank05Bbuff[ucRTCDispBuff[3]>> 0x04]);         //µØÖ·Îª14,15,0x10:ÏÂ²¿Ò¶£»

	//ÄêºÅ£º¸ß¡¢µÍ£¬ucDateATime[4]£¬BCDÂë
	LcdSendByte(((ucCurveDispBuff[0]&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[4] & 0x0f]);         //µØÖ·Îª12,13,0x10:×ÔÉèµÄÍâ¿ò£»
	LcdSendByte((((ucCurveDispBuff[0]>>4)&0x01)*0x80)|bank05Bbuff[ucRTCDispBuff[4] >> 0x04]);        //µØÖ·Îª10,11,0x10:×ÔÉè£»

	vdDrv164(CS_6);
	LcdSendId(0x05);           //Ð´Êý¾ÝÒýµ¼Âë         
	LcdSendAddress(0x00);      //´ÓµØÖ·0¿ªÊ¼

	//Ê±¼äÉè¶¨7£º¸ß¡¢µÍ£¬µÚËÄ¶ÎÎÈÎÂ
	LcdSendByte(bank07Abuff[WorkSegDispBuff[3].ucSetStaTim >> 0x04]);        //µØÖ·Îª24,25,0x80:Ê±¼äÉè¶¨7µÄÍâ¿ò£»
	LcdSendByte(0x10*ucSegTimRimDispBuff[7]|bank07Abuff[WorkSegDispBuff[3].ucSetStaTim & 0x0f]);            //µØÖ·Îª26,27£»

	LcdSendByte(bank07Abuff[WorkSegDispBuff[4].ucSetRisTim >> 0x04]);        //µØÖ·Îª28,29,0x80:Ê±¼äÉè¶¨8µÄÍâ¿ò£»
	LcdSendByte(0x10*ucSegTimRimDispBuff[8]|bank07Abuff[WorkSegDispBuff[4].ucSetRisTim & 0x0f]);            //µØÖ·Îª30,31£»

     //Ê±¼äÉè¶¨9£º¸ß¡¢µÍ£¬µÚÎå¶ÎÎÈÎÂ
	LcdSendByte(0x10|bank06Abuff[WorkSegDispBuff[4].ucSetStaTim >> 0x04]);        //µØÖ·Îª0,1,0x80:Ê±¼äÉè¶¨9µÄÍâ¿ò£»
	LcdSendByte(0x10*ucSegTimRimDispBuff[9]|bank06Abuff[WorkSegDispBuff[4].ucSetStaTim & 0x0f]);                 //µØÖ·Îª2,3£»


/*	//Ê±¼äÉè¶¨10£º¸ß¡¢µÍ£¬µÚÁù¶ÎÉýÎÂ
	LcdSendByte(0x10*ucOtherDispBuff[7]|bank06Abuff[WorkSegDispBuff[5].ucSetRisTim >> 0x04]);        //µØÖ·Îª4,5,0x80:Ê±¼äÉè¶¨10µÄÍâ¿ò£»
	LcdSendByte(0x10*ucSegTimRimDispBuff[10]|bank06Abuff[WorkSegDispBuff[5].ucSetRisTim & 0x0f]);                 //µØÖ·Îª6,7£»

	//Ê±¼äÉè¶¨11£º¸ß¡¢µÍ£¬µÚÁù¶ÎÎÈÎÂ
	LcdSendByte(bank06Abuff[WorkSegDispBuff[5].ucSetStaTim >> 0x04]);        //µØÖ·Îª8,9,0x80:Ê±¼äÉè¶¨11µÄÍâ¿ò£»
	LcdSendByte(0x10*ucSegTimRimDispBuff[11]|bank06Abuff[WorkSegDispBuff[5].ucSetStaTim & 0x0f]);                 //µØÖ·Îª10,11£»

	//Ê±¼äÉè¶¨13£º¸ß¡¢µÍ£¬µÚÆß¶ÎÎÈÎÂ
	LcdSendByte(0x10*ucOtherDispBuff[9]|bank06Abuff[WorkSegDispBuff[6].ucSetStaTim >> 0x04]);        //µØÖ·Îª16,17,0x80:Ê±¼äÉè¶¨13µÄÍâ¿ò£»
	LcdSendByte(0x10*ucSegTimRimDispBuff[13]|bank06Abuff[WorkSegDispBuff[6].ucSetStaTim & 0x0f]);                 //µØÖ·Îª18,19£»


    //Ê±¼äÉè¶¨12£º¸ß¡¢µÍ£¬µÚÆß¶ÎÉýÎÂ
	LcdSendByte(0x10*ucOtherDispBuff[8]|bank06Abuff[WorkSegDispBuff[6].ucSetRisTim >> 0x04]);        //µØÖ·Îª12,13,0x80:Ê±¼äÉè¶¨12µÄÍâ¿ò£»
	LcdSendByte(0x10*ucSegTimRimDispBuff[12]|bank06Abuff[WorkSegDispBuff[6].ucSetRisTim & 0x0f]);                 //µØÖ·Îª14,15£»
*/

	i16Test1 = 0x10*ucOtherDispBuff[7];

	//Ê±¼äÉè¶¨10£º¸ß¡¢µÍ£¬µÚÁù¶ÎÉýÎÂ
	LcdSendByte(i16Test1|bank06Abuff[WorkSegDispBuff[5].ucSetRisTim >> 0x04]);        //µØÖ·Îª4,5,0x80:Ê±¼äÉè¶¨10µÄÍâ¿ò£»
	LcdSendByte(0x10*ucSegTimRimDispBuff[10]|bank06Abuff[WorkSegDispBuff[5].ucSetRisTim & 0x0f]);                 //µØÖ·Îª6,7£»

	//Ê±¼äÉè¶¨11£º¸ß¡¢µÍ£¬µÚÁù¶ÎÎÈÎÂ
	LcdSendByte(0x00|bank06Abuff[WorkSegDispBuff[5].ucSetStaTim >> 0x04]);        //µØÖ·Îª8,9,0x80:Ê±¼äÉè¶¨11µÄÍâ¿ò£»
	LcdSendByte(0x10*ucSegTimRimDispBuff[11]|bank06Abuff[WorkSegDispBuff[5].ucSetStaTim & 0x0f]);                 //µØÖ·Îª10,11£»

	i16Test1 = 0x10*ucOtherDispBuff[8];

    //Ê±¼äÉè¶¨12£º¸ß¡¢µÍ£¬µÚÆß¶ÎÉýÎÂ
	LcdSendByte(i16Test1|bank06Abuff[WorkSegDispBuff[6].ucSetRisTim >> 0x04]);        //µØÖ·Îª12,13,0x80:Ê±¼äÉè¶¨12µÄÍâ¿ò£»
	LcdSendByte(0x10*ucSegTimRimDispBuff[12]|bank06Abuff[WorkSegDispBuff[6].ucSetRisTim & 0x0f]);                 //µØÖ·Îª14,15£»


	i16Test1 = 0x10*ucOtherDispBuff[9];

	//Ê±¼äÉè¶¨13£º¸ß¡¢µÍ£¬µÚÆß¶ÎÎÈÎÂ
	LcdSendByte(i16Test1|bank06Abuff[WorkSegDispBuff[6].ucSetStaTim >> 0x04]);        //µØÖ·Îª16,17,0x80:Ê±¼äÉè¶¨13µÄÍâ¿ò£»
	LcdSendByte(0x10*ucSegTimRimDispBuff[13]|bank06Abuff[WorkSegDispBuff[6].ucSetStaTim & 0x0f]);                 //µØÖ·Îª18,19£»



	LcdSendByte(bank06Abuff[uiDispBuff[7]%10]);                 //µØÖ·Îª8,9£»
	LcdSendHalfByte(0x00);

	vdDrv164(CS_7);
	LcdSendId(0x05);           //Ð´Êý¾ÝÒýµ¼Âë         
	LcdSendAddress(0x00);      //´ÓµØÖ·0¿ªÊ¼

	//¸ÉÇòÉè¶¨2£º¸ß¡¢µÍ
	LcdSendByte(0x10*ucDryTmpRimDispBuff[2]|bank07Abuff[WorkSegDispBuff[2].uiSetDryTmp >> 0x08]);                         //µØÖ·23¡¢24£¬0x10£ººãÎÂ£»
	LcdSendByte(bank07Abuff[(WorkSegDispBuff[2].uiSetDryTmp >> 0x04)& 0x0f]);            //µØÖ·25¡¢26£¬0x10£º¸ÉÇòÉè¶¨2Íâ¿ò£»

	//ÊªÇòÉè¶¨5£º¸ß¡¢
	LcdSendByte(0x10*ucDot5DispBuff[4]|bank07Abuff[(WorkSegDispBuff[4].uiSetWetTmp >> 0x04)& 0x0f]);            //µØÖ·Îª14,15,0x10:ÊªÇòÉè¶¨5µÄ.5£»
	LcdSendByte(0x10*ucWetTmpRimDispBuff[4]|bank07Abuff[WorkSegDispBuff[4].uiSetWetTmp >> 0x08]);        //µØÖ·Îª12,13,0x10:ÊªÇòÉè¶¨5µÄÍâ¿ò£»

	//ÊªÇòÉè¶¨4£º¸ß¡¢µ
	LcdSendByte(0x10*ucDot5DispBuff[3]|bank07Abuff[(WorkSegDispBuff[3].uiSetWetTmp >> 0x04)& 0x0f]);            //µØÖ·Îª14,15,0x10:ÊªÇòÉè¶¨5µÄ.5£»
	LcdSendByte(0x10*ucWetTmpRimDispBuff[3]|bank07Abuff[WorkSegDispBuff[3].uiSetWetTmp >> 0x08]);        //µØÖ·Îª12,13,0x10:ÊªÇòÉè¶¨5µÄÍâ¿ò£»


	//Ê±¼äÉè¶¨4£º¸ß¡¢µÍ£¬µÚÈý¶ÎÉýÎÂ
	LcdSendByte(bank07Abuff[WorkSegDispBuff[2].ucSetRisTim >> 0x04]);        //µØÖ·Îª12,13,0x80:Ê±¼äÉè¶¨4µÄÍâ¿ò£»
	LcdSendByte(0x10*ucSegTimRimDispBuff[4]|bank07Abuff[WorkSegDispBuff[2].ucSetRisTim & 0x0f]);            //µØÖ·Îª14,15£»

	//Ê±¼äÉè¶¨5£º¸ß¡¢µÍ£¬µÚÈý¶ÎÎÈÎÂ
	LcdSendByte(bank07Abuff[WorkSegDispBuff[2].ucSetStaTim >> 0x04]);        //µØÖ·Îª16,17,0x80:Ê±¼äÉè¶¨5µÄÍâ¿ò£»
	LcdSendByte(0x10*ucSegTimRimDispBuff[5]|bank07Abuff[WorkSegDispBuff[2].ucSetStaTim & 0x0f]);            //µØÖ·Îª18,19£»

	//Ê±¼äÉè¶¨6£º¸ß¡¢µÍ£¬µÚËÄ¶ÎÉýÎÂ
	LcdSendByte(bank07Abuff[WorkSegDispBuff[3].ucSetRisTim >> 0x04]);        //µØÖ·Îª20,21,0x80:Ê±¼äÉè¶¨6µÄÍâ¿ò£»
	LcdSendByte(0x10*ucSegTimRimDispBuff[6]|bank07Abuff[WorkSegDispBuff[3].ucSetRisTim & 0x0f]);            //µØÖ·Îª22,23£»

	//²âÁ¿µçÑ¹£º¸ß¡¢ÖÐ¡¢µÍ
	LcdSendByte(bank07Abuff[uiDispBuff[6] >> 8]);           //µØÖ·Îª24,25,0x10: Ñ­»··çËÙµÄµÍ£»
	LcdSendByte(bank07Abuff[(uiDispBuff[6] >> 4)&0x0f]);        //µØÖ·Îª26,27,0x10£ºÑ­»··çËÙµÄ¸ß£»
	LcdSendByte(bank07Abuff[uiDispBuff[6]&0x000f]);            //µØÖ·Îª28,29,0x10£ºÑ­»··çËÙµÄ×Ô¶¯£»

	i16Test = 0x00;
	i16Test  = ucOtherDispBuff[3]*0x02+ucOtherDispBuff[3]*0x20;
	i16Test += ucOtherDispBuff[4]*0x04+ucOtherDispBuff[4]*0x40;
	i16Test += ucOtherDispBuff[2]*0x08+ucOtherDispBuff[2]*0x80;
	LcdSendByte(i16Test);            //µØÖ·Îª28,29,0x10£ºÑ­»··çËÙµÄ×Ô¶¯£»
	vdDrv164(CS_NO); 

}
