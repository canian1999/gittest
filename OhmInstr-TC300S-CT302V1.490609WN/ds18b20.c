#include "ds18b20.h"
#include "system.h"

u8 ucStartSuccFlg;

u8 u8DS18B20lowFlag = 0;
u8 u8DS18B20lowErrCnt_0 = 0;
u8 u8DS18B20lowErrCnt_1 = 0;
u8 u8DS18B20lowErrCnt_2 = 0;
u8 u8DS18B20lowErrCnt_3 = 0;

void DS18B20_IO_IN(u16 DS18B20_Pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;        //??GPIO??????
	GPIO_InitStructure.GPIO_Pin = DS18B20_Pin; 		
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //???????; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void DS18B20_IO_OUT(u16 DS18B20_Pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;        //??GPIO??????
	GPIO_InitStructure.GPIO_Pin = DS18B20_Pin; 		
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //???????; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
//ds18b20初始化
u8 ucInitB20(u8 ch_num)  //ch_num value: 0,1,2,3
{ 
	u8 ch,i,j,k;
	u16 ch_bit;

	ch_bit = ((u16)(0x01))<<(ch_num);

	i=10;
	k = 0;
	while(i-->0)
   {	
 		DS18B20_IO_OUT(ch_bit);
    	GPIO_ResetBits(GPIOB,ch_bit);		//置低			  
        _delay_us(520);						//延时520us
    	
    	GPIO_SetBits(GPIOB,ch_bit);			//置高
		_delay_us(60);						//延时32
		
		DS18B20_IO_IN(ch_bit);
		j=255;
    	while(j-->0)	// 255us的超时计数
    	{
        		_delay_us(1);	// 每us查询一次
        		ch = GPIO_ReadInputDataBit(GPIOB,ch_bit);//等待传感器响应
        		if(ch == 0)//正常
        		{
        	    while((GPIO_ReadInputDataBit(GPIOB,ch_bit)==0)&&(k<254))//等待传感器响应
                {
        	         _delay_us(1);//等待传感器响应
        	         k++;
        	    }
        		_delay_us(280);						//再延时280us
                if(k>=254)
					u8DS18B20lowFlag = 1;
				else
					u8DS18B20lowFlag = 0;
				return (0xFF);
    		}
    	}
    }	
	return 0x00;
}

//功能：单总线写 
void vdWrB20(u8 ch_num, u8 ucData) 
{ 
    u8 i; 
	u16 ch_bit;

	ch_bit = ((u16)(0x01))<<(ch_num);
	
	DS18B20_IO_OUT(ch_bit);
      for(i=0; i<8; i++) 
    {
    	GPIO_ResetBits(GPIOB,ch_bit);		//置低
    	_delay_us(8);						//延时5us  
    	
    	if(ucData & 0x01)
    		GPIO_SetBits(GPIOB,ch_bit);		//置高
    	else
    		GPIO_ResetBits(GPIOB,ch_bit);
    	
    	_delay_us(90);						//延时65us，误差8%
    	GPIO_SetBits(GPIOB,ch_bit);			//置高
    	_delay_us(35);						//延时2us
    	ucData >>= 1; 						//移位继续传输
    } 
}


//功能：单总线读 
u8 ucRdB20(u8 ch_num) 
{ 
    u8 i; 
    u8 temp; 
	u16 ch_bit;

	u8 j,ch,k;

	temp =0;
	ch_bit = ((u16)(0x01))<<(ch_num);

    for(i=0; i<8; i++) 
    {
		DS18B20_IO_OUT(ch_bit);
		temp >>= 1; 
		GPIO_ResetBits(GPIOB,ch_bit);
		

		//电平置位0
		_delay_us(3); 								// Delay 1us
		GPIO_SetBits(GPIOB,ch_bit);		//置高


		//电平置位1
	    
		_delay_us(3);                               // Delay 5us
		DS18B20_IO_IN(ch_bit);
		j=7;k=0;
		while(j-->0)
		{
		

			ch = GPIO_ReadInputDataBit(GPIOB,ch_bit);
			//电平置位0
        	if(ch == 1)
			{
				k++;
                if(k >= 2)
				{
					_delay_us(5);
					break;
				}
			}

			_delay_us(1);
			//置位高
		}
	   								 
		if(ch)
	    	temp |= 0x80;                  
		else
	    	temp &= 0x7F; 		

		_delay_us(80); 				// Delay50us，误差8% 
		
//		GPIO_SetBits(GPIOB,ch_bit);		//置高
    } 
    return(temp);
}


//CRC check
u8 ucCRC8(u8 *pAddr,u8 ucByteLen)
{
    u8 i,j,k;
    u8 ucChkVal,flag;
	
    ucChkVal = 0;
    for(i=ucByteLen +1; i>0; i--)
    {
		k = pAddr[ 9 - i ];
		for(j=8; j>0; j--)
		{
	    	flag = (ucChkVal ^ k) & 0x01;
	    	ucChkVal >>= 1;
	    	if(flag)			    // such as:  X^8 + X^5 + X^4 + 1
	    	{
				ucChkVal ^= 0x0C; 
				ucChkVal |= 0x80;
	    	}
	    	k >>= 1;
		}
    }
    return(ucChkVal);
}

//启动温度转换
u8 ucStartConvert(u8 ch_num)
{
    u8 ucRtnVal = 0;

    //_DINT();
    if(ucInitB20(ch_num)) 			//首先初始化总线
    { 
		vdWrB20(ch_num, SkipROM); 		//skip rom COMMAND
		vdWrB20(ch_num, StartConvert); 		
		ucRtnVal = 0xFF;
    }
    //_EINT();
    return(ucRtnVal);
}

//读取温度转换值
u16 uiRdTemp(u8 ch_num)
{
  	u8 ucRAM[9],i;
  	u16 a;
  	
	//_DINT();
	if(ucInitB20(ch_num)) 		      // Initialize One-Wire-Bus first
	{ 
	    vdWrB20(ch_num, SkipROM);
	    vdWrB20(ch_num, ReadScratchpad);

	    for(i=0; i<9; i++)
            ucRAM[i] = ucRdB20(ch_num);         // Read all 9 ram buffer
                  
	}
	a   = ucRAM[1]&0x07;
	a <<= 8;
	a  |= ucRAM[0];				// (<0x07D1) || (>0xFC89)有效
	
	if((a & 0xF000) == 0)				// 正温度		
		a = (a >> 1) + (a >> 3);
	else if((a & 0xF000) == 0xF000)		// 负温度
	{
		a = ~a + 1;	
		a = (a >> 1) + (a >> 3);
		a = ~a + 1;	
	}	
	//_EINT();
	return(a);
}

/***************************************************************************
* Function Name  : udStartRdTmp
* Description    : 读取传感器值，启动传感器温度转换
* Input          : 输出数据缓冲区首地址
* Output         : None
* Return         : None
* Date           : 14/01/2009
****************************************************************************/
u8 ucStartRdTmp(unsigned int *ptmp)
{
    u16 temp , i; //
    static u8 StartFailCnt;
	static u16 tbuf0[4]={0},tbuf1[4]={0};
	static s16 val;

	if((ucStartSuccFlg & 0x01) ==0x01)
	{ 
	    temp = uiRdTemp(TEMP_SAMP_CH0);   //长线传感器，长线一般安装在烤房上棚
        ucStartSuccFlg &= ~0x01;

		val = temp - *ptmp/2;
		if(temp < 850  &&  temp > 50 && (val > 20 || val < -20))
		{
		 	tbuf0[0]=tbuf1[0];
			tbuf1[0]=temp;
		}
		val = tbuf0[0]-tbuf1[0];
		if(val < 50 && val > -50)  *ptmp = tbuf1[0];
		if(1 == u8DS18B20lowFlag)
		{
			u8DS18B20lowErrCnt_0++;
		}
		else
		{
			u8DS18B20lowErrCnt_0 = 0;
		}
		u8DS18B20lowFlag = 0;
		if(u8DS18B20lowErrCnt_0 >= 5)
		{    
		     *ptmp = 0;
		     u8DS18B20lowErrCnt_0 = 5;
		}
    }
    
	if((ucStartSuccFlg & 0x02) ==0x02)
	{
	    temp = uiRdTemp(TEMP_SAMP_CH1);  
        ucStartSuccFlg &= ~0x02;
        
		val = temp - *(ptmp+1)/2;
		if(temp < 850  &&  temp > 50 && (val > 20 || val < -20))
		{
		 	tbuf0[1]=tbuf1[1];
			tbuf1[1]=temp;
		}
		val = tbuf0[1]-tbuf1[1];
		 if(val < 50 && val > -50)  *(ptmp+1) = tbuf1[1];
		 if(1 == u8DS18B20lowFlag)
		 {
			 u8DS18B20lowErrCnt_1++;
		 }
		 else
		 {
			 u8DS18B20lowErrCnt_1 = 0;
		 }
		 u8DS18B20lowFlag = 0;
		 if(u8DS18B20lowErrCnt_1 >= 5)
		 {	  
			  *(ptmp+1) = 0;
			  u8DS18B20lowErrCnt_1 = 5;
		 }
	}

	if((ucStartSuccFlg & 0x04) ==0x04)    //短线传感器，长线一般安装在烤房上棚
	{
	    temp = uiRdTemp(TEMP_SAMP_CH2);  
	    ucStartSuccFlg &= ~0x04;

		val = temp - *(ptmp+2)/2;
		if(temp < 850  &&  temp > 50 && (val > 20 || val < -20))
		{
		 	tbuf0[2]=tbuf1[2];
			tbuf1[2]=temp;
		}
		val = tbuf0[2]-tbuf1[2];
		 if(val < 50 && val > -50) *(ptmp+2) = tbuf1[2];
		 
		 if(1 == u8DS18B20lowFlag)
		 {
			 u8DS18B20lowErrCnt_2++;
		 }
		 else
		 {
			 u8DS18B20lowErrCnt_2 = 0;
		 }
		 u8DS18B20lowFlag = 0;
		 if(u8DS18B20lowErrCnt_2 >= 5)
		 {	  
			  *(ptmp+2) = 0;
			  u8DS18B20lowErrCnt_2 = 5;
		 }
	}
    
	if((ucStartSuccFlg & 0x08) ==0x08)
	{
	    temp = uiRdTemp(TEMP_SAMP_CH3);   
        ucStartSuccFlg &= ~0x08;

		val = temp - *(ptmp+3)/2;
		if(temp < 850  &&  temp > 50 && (val > 20 || val < -20))
		{
		 	tbuf0[3]=tbuf1[3];
			tbuf1[3]=temp;
		}
		val = tbuf0[3]-tbuf1[3];
		 if(val < 50 && val > -50) *(ptmp+3) = tbuf1[3];
		 if(1 == u8DS18B20lowFlag)
		 {
			 u8DS18B20lowErrCnt_3++;
		 }
		 else
		 {
			 u8DS18B20lowErrCnt_3 = 0;
		 }
		 u8DS18B20lowFlag = 0;
		 if(u8DS18B20lowErrCnt_3 >= 5)
		 {	  
			  *(ptmp+3) = 0;
			  u8DS18B20lowErrCnt_3 = 5;
		 }
	}
	
	if(ucStartConvert(TEMP_SAMP_CH0) ==0xff)
	{
	    ucStartSuccFlg  = 0x01;
		StartFailCnt    = 0;
    }
	else
	    StartFailCnt++;

	if(ucStartConvert(TEMP_SAMP_CH1) ==0xff)
	    ucStartSuccFlg |= 0x02;

	if(ucStartConvert(TEMP_SAMP_CH2) ==0xff)
	    ucStartSuccFlg |= 0x04;   

    if(ucStartConvert(TEMP_SAMP_CH3) ==0xff)
	    ucStartSuccFlg |= 0x08; 

    if(StartFailCnt < 5)
	    return(1);
    else
	{
	    for(i= 0; i< 4; i++)
		    *(ptmp+i) = 0x00;
		return(0);
    }
}
