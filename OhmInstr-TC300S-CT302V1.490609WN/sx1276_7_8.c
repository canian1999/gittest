#include "stm32f10x_lib.h"
#include "stm32f10x_gpio.h"
#include "spi.h"
#include "sx1276_7_8.h"
/************************Description************************
							 ---------------
	   key2 ----    |PD4         PD3|	 ----key1
 GREEN_LED----    |PD5         PD2|	 ----NIRQ
   RED_LED----    |PD6         PD1|	 ----
	    	  ----    |NRST        PC7|   ----SDO
	        ----    |PA1         PC6|   ----SDI
 	        ----    |PA2         PC5|	 ----SCK
	    	  ----    |Vss         PC4|   ----
		      ----    |Vcap        PC3|   ----SDN
		      ----    |Vdd         PB4|	 ----
		  NSEL----    |PA3         PB5|	 ----
									---------------
										stm8s103f3p
************************************************/
/************************************************
//  RF module:           sx1276_7_8
//  FSK:
//  Carry Frequency:     434MHz
//  Bit Rate:            1.2Kbps/2.4Kbps/4.8Kbps/9.6Kbps
//  Tx Power Output:     20dbm/17dbm/14dbm/11dbm
//  Frequency Deviation: +/-35KHz
//  Receive Bandwidth:   83KHz
//  Coding:              NRZ
//  Packet Format:       0x5555555555+0xAA2DD4+"HR_WT Lora sx1276_7_8" (total: 29 bytes)
//  LoRa:
//  Carry Frequency:     434MHz
//  Spreading Factor:    6/7/8/9/10/11/12
//  Tx Power Output:     20dbm/17dbm/14dbm/11dbm
//  Receive Bandwidth:   7.8KHz/10.4KHz/15.6KHz/20.8KHz/31.2KHz/41.7KHz/62.5KHz/125KHz/250KHz/500KHz
//  Coding:              NRZ
//  Packet Format:       "HR_WT Lora sx1276_7_8" (total: 21 bytes)
//  Tx Current:          about 120mA  (RFOP=+20dBm,typ.)
//  Rx Current:          about 11.5mA  (typ.)       
**********************************************************/

/**********************************************************
**Parameter table define
**********************************************************/
u8 sx1276_7_8FreqTbl[1][3] = 
{ 
  {0x6C, 0x80, 0x00}, //434MHz
};

const u8 sx1276_7_8PowerTbl[4] =
{ 
  0xFF,                   //20dbm  
  0xFC,                   //17dbm
  0xF9,                   //14dbm
  0xF6,                   //11dbm 
};

const u8 sx1276_7_8SpreadFactorTbl[7] =
{
  6,7,8,9,10,11,12
};

const u8 sx1276_7_8LoRaBwTbl[10] =
{
//7.8KHz,10.4KHz,15.6KHz,20.8KHz,31.2KHz,41.7KHz,62.5KHz,125KHz,250KHz,500KHz
  0,1,2,3,4,5,6,7,8,9
};

u8 RxData[64];
u8 SysTime_MicroSec;

extern unsigned int u16MasterAddress;
extern unsigned int u16SlaveAddress;

/**********************************************************
**Variable define
**********************************************************/

void sx1276_7_8_Config(void);

/**********************************************************
**Name:     sx1276_7_8_Standby
**Function: Entry standby mode
**Input:    None
**Output:   None
**********************************************************/
void sx1276_7_8_Standby(void)
{
  SPIWrite(LR_RegOpMode,0x09);                              		//Standby//Low Frequency Mode
	//SPIWrite(LR_RegOpMode,0x01);                              	 //Standby//High Frequency Mode
}

/**********************************************************
**Name:     sx1276_7_8_Sleep
**Function: Entry sleep mode
**Input:    None
**Output:   None
**********************************************************/
void sx1276_7_8_Sleep(void)
{
  SPIWrite(LR_RegOpMode,0x08);                              		//Sleep//Low Frequency Mode
	//SPIWrite(LR_RegOpMode,0x00);                            		 //Sleep//High Frequency Mode
}

/*********************************************************/
//LoRa mode
/*********************************************************/
/**********************************************************
**Name:     sx1276_7_8_EntryLoRa
**Function: Set RFM69 entry LoRa(LongRange) mode
**Input:    None
**Output:   None
**********************************************************/
void sx1276_7_8_EntryLoRa(void)
{
  SPIWrite(LR_RegOpMode,0x88);//Low Frequency Mode
	//SPIWrite(LR_RegOpMode,0x80);//High Frequency Mode
}

/**********************************************************
**Name:     sx1276_7_8_LoRaClearIrq
**Function: Clear all irq
**Input:    None
**Output:   None
**********************************************************/
void sx1276_7_8_LoRaClearIrq(void)
{
  SPIWrite(LR_RegIrqFlags,0xFF);
}

/**********************************************************
**Name:     sx1276_7_8_LoRaEntryRx
**Function: Entry Rx mode
**Input:    None
**Output:   None
**********************************************************/
void sx1276_7_8_LoRaEntryRx(void)
{
  u8 addr; 
  
  SPIWrite(REG_LR_PADAC,0x84);                              //Normal and Rx
  SPIWrite(LR_RegHopPeriod,0xFF);                          //RegHopPeriod NO FHSS
  SPIWrite(REG_LR_DIOMAPPING1,0x01);                       //DIO0=00, DIO1=00, DIO2=00, DIO3=01
      
  SPIWrite(LR_RegIrqFlagsMask,0x3F);                       //Open RxDone interrupt & Timeout
  sx1276_7_8_LoRaClearIrq();   
  
    
  addr = SPIRead(LR_RegFifoRxBaseAddr);           				//Read RxBaseAddr
  SPIWrite(LR_RegFifoAddrPtr,addr);                        //RxBaseAddr -> FiFoAddrPtr　 
  SPIWrite(LR_RegOpMode,0x8d);                        		//Continuous Rx Mode//Low Frequency Mode
	//SPIWrite(LR_RegOpMode,0x05);                        		//Continuous Rx Mode//High Frequency Mode
	SysTime_MicroSec = 0;
	while(1)
	{
		if((SPIRead(LR_RegModemStat)&0x04)==0x04)   //Rx-on going RegModemStat
			break;
		if(SysTime_MicroSec>=4)	
			break;                                              //over time for error
	}
}
/**********************************************************
**Name:     sx1276_7_8_LoRaReadRSSI
**Function: Read the RSSI value
**Input:    none
**Output:   temp, RSSI value
**********************************************************/
u8 sx1276_7_8_LoRaReadRSSI(void)
{
  u16 temp=10;
  temp=SPIRead(LR_RegRssiValue);                  //Read RegRssiValue，Rssi value
  temp=temp+127-137;                                       //127:Max RSSI, 137:RSSI offset
  return (u8)temp;
}

/**********************************************************
**Name:     sx1276_7_8_LoRaRxPacket
**Function: Receive data in LoRa mode
**Input:    None
**Output:   1- Success
            0- Fail
**********************************************************/
u8 sx1276_7_8_LoRaRxPacket(void)
{
  u8 i; 
  u8 addr;
  u8 packet_size;
 
  if(Get_NIRQ())
  {
    for(i=0;i<32;i++) 
      RxData[i] = 0x00;
    addr = SPIRead(LR_RegFifoRxCurrentaddr);      //last packet addr
    SPIWrite(LR_RegFifoAddrPtr,addr);                      //RxBaseAddr -> FiFoAddrPtr    
    packet_size = SPIRead(LR_RegRxNbBytes);     //Number for received bytes    
    SPIBurstRead(0x00, RxData, packet_size);
    
    sx1276_7_8_LoRaClearIrq();
    if(i>=3)                                              //Rx success
      return(1);
    else
      return(0);
  }
  else
    return(0);
}

/**********************************************************
**Name:     sx1276_7_8_LoRaEntryTx
**Function: Entry Tx mode
**Input:    None
**Output:   None
**********************************************************/
void sx1276_7_8_LoRaEntryTx(u8 len)
{
  u8 addr;//,temp;
	
  SPIWrite(REG_LR_PADAC,0x87);                                   //Tx for 20dBm
  SPIWrite(LR_RegHopPeriod,0x00);                               //RegHopPeriod NO FHSS
  SPIWrite(REG_LR_DIOMAPPING1,0x41);                       //DIO0=01, DIO1=00, DIO2=00, DIO3=01
  
  sx1276_7_8_LoRaClearIrq();
  SPIWrite(LR_RegIrqFlagsMask,0xF7);                       //Open TxDone interrupt
  SPIWrite(LR_RegPayloadLength,len);                       //RegPayloadLength  21byte
  
  addr = SPIRead(LR_RegFifoTxBaseAddr);           //RegFiFoTxBaseAddr
  SPIWrite(LR_RegFifoAddrPtr,addr);                        //RegFifoAddrPtr

}
/**********************************************************
**Name:     sx1276_7_8_LoRaTxPacket
**Function: Send data in LoRa mode
**Input:    None
**Output:   1- Send over
**********************************************************/
void sx1276_7_8_LoRaTxPacket(u8* pBuf,u8 len)
{ 
	BurstWrite(0x00, (u8 *)pBuf, len);
	SPIWrite(LR_RegOpMode,0x8b);                    //Tx Mode           
	while(1)
	{
		if(Get_NIRQ())                      //Packet send over
		{      
			sx1276_7_8_LoRaClearIrq();                                //Clear irq
			break;
		}
	} 
}

/**********************************************************
**Name:     sx1276_7_8_ReadRSSI
**Function: Read the RSSI value
**Input:    none
**Output:   temp, RSSI value
**********************************************************/
u8 sx1276_7_8_ReadRSSI(void)
{
  u8 temp=0xff;
	
  temp=SPIRead(0x11);
  temp>>=1;
  temp=127-temp;                                           //127:Max RSSI
  return temp;
}
/**********************************************************
**Name:     sx1276_7_8_Config
**Function: sx1276_7_8 base config
**Input:    mode
**Output:   None
**********************************************************/
void sx1276_7_8_Config(void)
{
  //u8 i; 
    
  sx1276_7_8_Sleep();                                      //Change modem mode Must in Sleep mode 
 // for(i=250;i!=0;i--)                                      //Delay
 //   nop();
	_delay_ms(20);

  //lora mode
	sx1276_7_8_EntryLoRa();  
	//SPIWrite(0x5904);   //?? Change digital regulator form 1.6V to 1.47V: see errata note
    
	BurstWrite(LR_RegFrMsb,sx1276_7_8FreqTbl[Freq_Sel],3);  //setting frequency parameter

	//setting base parameter 
	SPIWrite(LR_RegPaConfig,sx1276_7_8PowerTbl[Power_Sel]);             //Setting output power parameter  
    
	SPIWrite(LR_RegOcp,0x0B);                              //RegOcp,Close Ocp
	SPIWrite(LR_RegLna,0x23);                              //RegLNA,High & LNA Enable
    
	if(sx1276_7_8SpreadFactorTbl[Lora_Rate_Sel]==6)           //SFactor=6
	{
		u8 tmp;
		SPIWrite(LR_RegModemConfig1,((sx1276_7_8LoRaBwTbl[BandWide_Sel]<<4)+(CR<<1)+0x01));//Implicit Enable CRC Enable(0x02) & Error Coding rate 4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)
		SPIWrite(LR_RegModemConfig2,((sx1276_7_8SpreadFactorTbl[Lora_Rate_Sel]<<4)+(CRC_1<<2)+0x03));
      
		tmp = SPIRead(0x31);
		tmp &= 0xF8;
		tmp |= 0x05;
		SPIWrite(0x31,tmp);
		SPIWrite(0x37,0x0C);
	} 
	else
	{
		SPIWrite(LR_RegModemConfig1,((sx1276_7_8LoRaBwTbl[BandWide_Sel]<<4)+(CR<<1)+0x00));//Explicit Enable CRC Enable(0x02) & Error Coding rate 4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)
		SPIWrite(LR_RegModemConfig2,((sx1276_7_8SpreadFactorTbl[Lora_Rate_Sel]<<4)+(CRC_1<<2)+0x03));  //SFactor &  LNA gain set by the internal AGC loop 
	}
	SPIWrite(LR_RegSymbTimeoutLsb,0xFF);                   //RegSymbTimeoutLsb Timeout = 0x3FF(Max) 
    
	SPIWrite(LR_RegPreambleMsb,0x00);                       //RegPreambleMsb 
	SPIWrite(LR_RegPreambleLsb,12);                      //RegPreambleLsb 8+4=12byte Preamble
    
	SPIWrite(REG_LR_DIOMAPPING2,0x01);                     //RegDioMapping2 DIO5=00, DIO4=01
	
  sx1276_7_8_Standby();                                         //Entry standby mode
}

u8 SFBW[4][2] = {{4,9},{5,9},{4,8},{5,8}};
void Init1278(u8 Master)
{
	
	mode = 0x01;//lora mode
	Freq_Sel = 0x00;//433M
	Power_Sel = 0x03;//
	Lora_Rate_Sel = SFBW[Master%4][0];//扩频因子 //04 10
	BandWide_Sel = SFBW[Master%4][1]; //带宽?? 
	Fsk_Rate_Sel = 0x00;
	//复位
	SDN_L();
	_delay_ms(15);
	SDN_H();
	_delay_ms(15);
	
	sx1276_7_8_Config();
	
}
void SendRF1278(u8* pBuf,u8 len)
{
	sx1276_7_8_LoRaEntryTx(len);//enter transmition
	sx1276_7_8_LoRaTxPacket((u8 *)pBuf,len);//??????????
	sx1276_7_8_LoRaEntryRx();	
}
u8 rBuf[32] = {0};	
u8 GetRFFrame_1278(void)
{
  u8 addr,i;
  u8 temp;  
  u8 length;
    if(Get_NIRQ())  //GPIO_ReadInputPin(GPIOB, GPIO_PIN_4)!=RESET
    {
       	addr = SPIRead(LR_RegFifoRxCurrentaddr);         //last packet addr
        SPIWrite(LR_RegFifoAddrPtr,addr);                //RxBaseAddr -> FiFoAddrPtr    
        length = SPIRead(LR_RegRxNbBytes);       //Number for received bytes  

       	if(length >= 4)
       	{
       		SPIBurstRead(0x00,rBuf, length);
			    if((u16MasterAddress != rBuf[0])||(u16SlaveAddress !=rBuf[2]))
       			return 0;
       		for(i = 0;i<(length-1);i++)
       			temp += rBuf[i];
       		temp = (u8)(~(temp&0xff)+1);
       		if(temp != rBuf[length-1])
       			return 0;	
       		sx1276_7_8_LoRaClearIrq();
			      return 1;
       	}
  	}
	return 0;
}
