#ifndef _DS18B20_H_
#define _DS18B20_H_


	#define    _BV(bit) (1<<bit)
    #define    TEMP_SAMP_DIR   DDRE
    #define    TEMP_SAMP_OUT   PORTE
    #define    TEMP_SAMP_IN    PINE
    
	/*新的700套程序定义*/
	#define    TEMP_SAMP_CH0   PE4
    #define    TEMP_SAMP_CH1   PE3
    #define    TEMP_SAMP_CH2   PE2
	#define    TEMP_SAMP_CH3   PE5

    //DS18B20 command
    #define 	CONVERT			0x44
    #define 	SEARCH_ROM		0xF0
    #define 	READ_ROM		0x33
    #define 	MATCH_ROM		0x55
    #define 	SKIP_ROM		0xCC
    #define 	ALARM_SEARCH	0xEC

    #define 	READ_POWER		0xB4

    #define 	WRITE_TH		0x4E
    #define 	WRITE_BIT		0x1F  
    #define 	READ_RAM		0xBE
    #define 	SAVE_TH			0x48
    #define 	RECALL_TH		0xB8

    #define 	MOD09BIT		0x00
    #define 	MOD10BIT		0x20
    #define 	MOD11BIT		0x40
    #define 	MOD12BIT		0x60

    unsigned char  ucStartRdTmp(unsigned int *ptmp);

#endif

