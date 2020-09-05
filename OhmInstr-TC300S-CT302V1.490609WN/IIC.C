#include "IIC.h"
#include "stm32f10x_lib.h"
#include "stm32f10x_gpio.h"
extern unsigned int IICaddr,IICdatt,IICk;
unsigned int IICl,IICj,IICi;
void SDA_Do()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC,&GPIO_InitStructure);
}

void SDA_Di()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOC,&GPIO_InitStructure);
}

void IICdelay()
{
  for(IICl=0;IICl<80;IICl++);
}

void SETBSDA()
{
  Set_SDA();
  IICdelay();
}  
  
void SETBSCL()
{
  Set_SCL();
  IICdelay();
}  

void CLRBSDA()
{
  Clr_SDA();
  IICdelay();
}  
  
void CLRBSCL()
{
  Clr_SCL();
  IICdelay();
}  

void WR24C_ST()
{
   SDA_Do();
   Set_SDA();
   Set_SCL();
   IICdelay();
   CLRBSDA();
   CLRBSCL();
}

void WR24C_SP()
{
   SDA_Do();
   CLRBSDA();
   SETBSCL();
   SETBSDA();
}

void WR24C_Byte(int datt)
{
  IICj=datt;
  SDA_Do();
  CLRBSCL();
  for(IICi=0;IICi<8;IICi++){
    if((IICj & 0x80)==0x80)SETBSDA();
    else CLRBSDA();
    SETBSCL();
    IICj=IICj+IICj;
    CLRBSCL(); 
    }
  SDA_Di();
  SETBSCL();
  CLRBSCL(); 
}

int RD24C_ByteAck()
{
  IICj=0;
  SDA_Di();
  CLRBSCL();
  for(IICi=0;IICi<8;IICi++){
    SETBSCL();
    IICj=IICj+IICj;
    if(SDAHi)IICj++;
    CLRBSCL(); 
    }
  SDA_Do();
  CLRBSDA();
  SETBSCL();
  CLRBSCL(); 
  return(IICj);
}

int RD24C_ByteNAck()
{
  IICj=0;
  SDA_Di();
  CLRBSCL();
  for(IICi=0;IICi<8;IICi++){
    IICj=IICj+IICj;
    SETBSCL();
    if(SDAHi)IICj++;
    CLRBSCL(); 
    }
  SDA_Do();
  Set_SDA();
  SETBSCL();
  CLRBSCL(); 
  return(IICj);
}

void WR24C_Word(int datt)
{
  IICk=datt >> 8;
  IICk=IICk & 0xff;
  WR24C_Byte(IICk);
  IICk=datt & 0xff;
  WR24C_Byte(IICk);
}

int RD24C_Word()
{
 int i;
  IICk=RD24C_ByteAck();
  IICk=IICk*256;
  i=RD24C_ByteNAck();
  IICk=IICk+i;
  return(IICk);
}

void WordTo24C() 
{
  WR24C_ST();
  WR24C_Byte(0xa0);
  WR24C_Word(IICaddr);
  WR24C_Word(IICdatt);
  WR24C_SP();
} 

void WordFrom24C() 
{
  WR24C_ST();
  WR24C_Byte(0xa0);
  WR24C_Word(IICaddr);
  WR24C_ST();
  WR24C_Byte(0xa1);
  SDA_Di();
  IICdatt=RD24C_Word();
  WR24C_SP();
} 
