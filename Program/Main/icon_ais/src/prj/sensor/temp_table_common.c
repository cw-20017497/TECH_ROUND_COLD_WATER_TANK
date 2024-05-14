#include "temp_table_common.h"



U16 GetTempTableIndex(U8 resolution, U16 rawAdc, U16 min, U16 max )
{
    U16 mu16Index;


    if( resolution == RES_ADC_TABLE_10BIT )
    {
        rawAdc = rawAdc >> 2;
    }

    if( rawAdc < min )
    {
        rawAdc = min;
    }
    else if( rawAdc > max )
    {
        rawAdc = max;
    }

    mu16Index = rawAdc - min;
    return mu16Index;
}

// 10 -> 1.0'c
// 123 -> 12.3'c

// 1 deciam place
TEMP_T ConvInt2Temp(int temp)
{
    TEMP_T  retTemp = 0.0f;

    retTemp = (TEMP_T)(temp/10);
    retTemp += (TEMP_T)(temp%10) * 0.1f;

    return retTemp;
}

// 2 decimal place
TEMP_T ConvInt2Temp2dp(int temp)
{
    TEMP_T  retTemp = 0.0f;

    retTemp = (TEMP_T)(temp/100);
    retTemp += (TEMP_T)(temp%100) * 0.01f;

    return retTemp;
}

