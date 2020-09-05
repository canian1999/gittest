#ifndef _IIC_H
#define _IIC_H


#define Set_SDA()  GPIOC->BSRR |= GPIO_Pin_12
#define Clr_SDA()  GPIOC->BRR  |= GPIO_Pin_12

#define Set_SCL()  GPIOC->BSRR |= GPIO_Pin_11
#define Clr_SCL()  GPIOC->BRR  |= GPIO_Pin_11

#define SDAHi (GPIOC->IDR&0x1000)==0x1000
#define SDALo (GPIOC->IDR&0x1000)==0x00

void WordFrom24C(void);
void WordTo24C(void); 

#endif
