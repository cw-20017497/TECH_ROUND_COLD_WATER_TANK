#ifndef __TEMP_TABLE_COMMON_H__
#define __TEMP_TABLE_COMMON_H__

#include "prj_type.h"


#define RES_ADC_TABLE_10BIT     0
#define RES_ADC_TABLE_12BIT     1
U16 GetTempTableIndex(U8 resolution, U16 rawAdc, U16 min, U16 max );

// 1 deciam place
// 10 -> 1.0'c
// 123 -> 12.3'c
TEMP_T ConvInt2Temp(int temp);


// 2 decimal place
// 10 -> 1.0'c
// 123 -> 1.23'c
TEMP_T ConvInt2Temp2dp(int temp);

#endif /* __TEMP_TABLE_COMMON_H__ */
