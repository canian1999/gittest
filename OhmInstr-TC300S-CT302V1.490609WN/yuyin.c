#include "stm32f10x_lib.h"
#include "system.h"

#include "yuyin.h"


/**********************************************************
* Function Name  : InitIODDR                              *
* Description    : 语音芯片aP89170有关控制函数            *                          
* Input          : None                                   *
* Output         : None                                   *
* Return         : None                                   *
***********************************************************/

void InitVoiceIO(void)
{
    CLR_YUCS;
    CLR_YUSCK;
    CLR_YUDA;
}

void VoiceSendByte(unsigned char byte)
{
    unsigned char i;

    for(i=0; i<8; i++)
	{
	    
		_delay_us(1);

        if((byte >> i) & 0x01)
	        SET_YUDA; 
        else
	        CLR_YUDA;
        _delay_us(2);
        
		SET_YUSCK;
		_delay_us(2);
	    CLR_YUSCK;
        _delay_us(2); 

	} 
   
}

void VoicePlayAll(void)
{
    
	unsigned char i;
    
	i = 0;
    for(i=20;i<51;i++)
	    VoicePlay(i);
    /*

	*/
}


void VoicePlay(unsigned char gaddr)
{
    
//	if((VOICEBUSY_IN & (~VOICEBUSY_PIN)) ==0)
//	{
	    
	    SET_YUCS;
		_delay_us(1);
        VoiceSendByte(CMD_PREFETCH);//CMD_PLAY
        VoiceSendByte(gaddr);   //播放语音地址
        CLR_YUCS;

        _delay_us(40);
        SET_YUCS;
        VoiceSendByte(CMD_PUP2);
        CLR_YUCS;/**/
//	}
    
	//while((VOICEBUSY_IN & _BV(VOICEBUSY_PIN)) ==0);
	_delay_us(10);
	
}
