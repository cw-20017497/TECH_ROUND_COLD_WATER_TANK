#ifndef __TEMP_TABLE_COLD_H__
#define __TEMP_TABLE_COLD_H__

#include "prj_type.h"


// 1 decimal place( 10bit resoultion )
// 75.5'C ~ -3.5'C
TEMP_T   ConvAdc2Temp_ColdWater(U16 mu16Adc);


// 2 decimal place ( 12bit resolution )
// 8.50'C ~ -0.50'C
TEMP_T   ConvAdc2Temp_ColdWater_2dp(U16 mu16Adc);

#endif /* __TEMP_TABLE_COLD_H__ */
