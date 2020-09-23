#ifndef _SYSTEM_H_
#define _SYSTEM_H_

extern void RCC_Configuration(void);
extern void GPIO_Configuration(void);
extern void NVIC_Configuration(void);
extern void _delay_us(u16 time);
extern void _delay_us1(u16 time);
extern void _delay_ms(u16 time);
extern void vdGetFengmenCtrModFlag(void);
void _delay_500us(void);
void _delay_90us(void);
void SPI_Configuration(void);

#endif
