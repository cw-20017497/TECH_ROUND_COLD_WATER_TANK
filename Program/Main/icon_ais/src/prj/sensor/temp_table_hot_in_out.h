#ifndef __TEMP_TABLE_HOT_IN_OUT_H__
#define __TEMP_TABLE_HOT_IN_OUT_H__

#include "prj_type.h"

TEMP_T   ConvAdc2Temp_HotOut(U16 mu16Adc);  // 순간 온수 / 온수 히터 내부 
TEMP_T   ConvAdc2Temp_HotIn(U16 mu16Adc);   // 순간 입수 

#endif /* __TEMP_TABLE_HOT_IN_OUT_H__ */
