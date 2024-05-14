#include "temp_table_room.h"
#include "temp_table_cold.h"

TEMP_T   ConvAdc2Temp_RoomWater(U16 mu16Adc)
{
    return ConvAdc2Temp_ColdWater( mu16Adc );
}

