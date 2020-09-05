#ifndef _IO_H_
#define _IO_H_

#define ALARMON GPIOA->BSRR |= 0x0002
#define ALARMOFF GPIOA->BRR |= 0x0002

#define KEY1 0x01
#define KEY2 0x02
#define KEY3 0x04
#define KEY4 0x08
#define KEY5 0x10

#define LCDWRDAT(dat) GPIOE->ODR = GPIOE->ODR & 0xFF | ((dat) << 8)

#define LCDA1ON GPIOD->BSRR |= 0x2000   // A1 = 1
#define LCDA0ON GPIOD->BSRR |= 0x1000   // A0 = 1
#define LCDCSON GPIOD->BRR |= 0x0800    // CS = 0
#define LCDWRON GPIOD->BRR |= 0x0400    // WR = 0
#define LCDRDON GPIOD->BRR |= 0x0200    // RD = 0

#define LCDA1OFF GPIOD->BRR |= 0x2000   // A1 = 0
#define LCDA0OFF GPIOD->BRR |= 0x1000   // A0 = 0
#define LCDCSOFF GPIOD->BSRR |= 0x0800  // CS = 1
#define LCDWROFF GPIOD->BSRR |= 0x0400  // WR = 1
#define LCDRDOFF GPIOD->BSRR |= 0x0200  // RD = 1

#define M0ON GPIOD->BSRR |= 0x0020  // M0 = 1
#define M1ON GPIOD->BSRR |= 0x0004  // M1 = 1
#define M2ON GPIOD->BSRR |= 0x0008  // M2 = 1
#define M3ON GPIOD->BSRR |= 0x0010  // M3 = 1

#define M0OFF GPIOD->BRR |= 0x0020  // M0 = 0
#define M1OFF GPIOD->BRR |= 0x0004  // M1 = 0
#define M2OFF GPIOD->BRR |= 0x0008  // M2 = 0
#define M3OFF GPIOD->BRR |= 0x0010  // M3 = 0

#define PWMON GPIOD->BSRR |= 0x0002 // SD = 1
#define PWMOFF GPIOD->BRR |= 0x0002 // SD = 0

#define IMODE1ON GPIOE->BSRR |= 0x0010  // IMODE1 = 1
#define IMODE2ON GPIOE->BSRR |= 0x0040  // IMODE2 = 1
#define IMODE3ON GPIOD->BSRR |= 0x0040  // IMODE3 = 1

#define IMODE1OFF GPIOE->BRR |= 0x0010  // IMODE1 = 0
#define IMODE2OFF GPIOE->BRR |= 0x0040  // IMODE2 = 0
#define IMODE3OFF GPIOD->BRR |= 0x0040  // IMODE3 = 0

#define VMODE1ON GPIOE->BSRR |= 0x0008  // VMODE1 = 1
#define VMODE2ON GPIOE->BSRR |= 0x0020  // VMODE2 = 1
#define VMODE3ON GPIOD->BSRR |= 0x0080  // VMODE3 = 1

#define VMODE1OFF GPIOE->BRR |= 0x0008  // VMODE1 = 0
#define VMODE2OFF GPIOE->BRR |= 0x0020  // VMODE2 = 0
#define VMODE3OFF GPIOD->BRR |= 0x0080  // VMODE3 = 0

#define VDIV_ON GPIOE->BSRR |= 0x0080   // VDIV_ON = 1
#define VDIV_OFF GPIOE->BRR |= 0x0080   // VDIV_ON = 0

#define VMAX_3V     VMODE1OFF;VMODE2OFF;VMODE3OFF;
#define VMAX_6V     VMODE1ON;VMODE2OFF;VMODE3OFF;
#define VMAX_12V    VMODE1OFF;VMODE2ON;VMODE3OFF;
#define VMAX_24V    VMODE1OFF;VMODE2OFF;VMODE3ON;

#define IMAX_1mA    IMODE1ON;IMODE2OFF;IMODE3ON;M1OFF;M2OFF;M3ON;
#define IMAX_100mA  IMODE1ON;IMODE2OFF;IMODE3OFF;M1OFF;M2OFF;M3ON;
#define IMAX_1A     IMODE1OFF;IMODE2OFF;IMODE3ON;M1OFF;M2ON;M3OFF;
#define IMAX_3A     IMODE1OFF;IMODE2ON;IMODE3OFF;M1ON;M2OFF;M3OFF;
#define IMAX_10A    IMODE1OFF;IMODE2OFF;IMODE3ON;M1ON;M2OFF;M3OFF;

#define OUTON   M0ON
#define OUTOFF  M0OFF

extern void KeyScan(void);
extern u8 KeyIn;

#endif