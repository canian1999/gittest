#ifndef _LOWFILTER_H
#define _LOWFILTER_H

typedef struct
{
	s16  in;		// input
	u16 coeff;
	s32  preout;
	s16  out;		// output
}LOWFILTER;

typedef struct
{
	u32  in;		// input, Q24
	u32 coeff;  // Q8
	u32  preout;// Q32
	u32  out;		// output, Q24
}LOWFILTER32;

#define LF_DISP_DEFAULT {0,8192,0,0}
#define LF_AD_DEFAULT {0,128,0,0}

extern LOWFILTER PwrVltFilter;
extern LOWFILTER32 VltFilter;
extern LOWFILTER32 CurFilter;

extern void LowFilterCalc(LOWFILTER *);
extern void LowFilter32Calc(LOWFILTER32 *ptr);

#endif

//===========================================================================
// No more.
//===========================================================================

