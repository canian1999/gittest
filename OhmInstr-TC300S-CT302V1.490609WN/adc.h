#ifndef _ADC_H_
#define _ADC_H_

#define CMD_RD_CFG 0x0B
#define CMD_WR_CFG 0x03
#define CMD_RD_OFFSET 0x09
#define CMD_WR_OFFSET 0x01
#define CMD_RD_GAIN 0x0A
#define CMD_WR_GAIN 0x02
#define CMD_RD_SETUP 0x0D
#define CMD_WR_SETUP 0x05
#define CMD_START_CONV 0x80
#define CMD_SelfOffsetCali 0x81
#define CMD_SelfGainCali 0x82
#define CMD_SysOffsetCali 0x85
#define CMD_SysGainCali 0x86
#define CMD_SYNC1 0xFF
#define CMD_SYNC0 0xFE
#define CMD_NULL 0x00


#define K_BATVLT (3.278 * 4.89) // 参考电压、分压电阻系数

extern unsigned int uiGaugeACV(unsigned char ch);
extern unsigned int uiGaugeACC(unsigned char ch);
extern void ADCInit(void);
extern void CS5532_Sample(void);
extern void CS5532_Init(void);

extern u16 BatVolt;
extern u32 ADRslt1,ADRslt2;

#endif
