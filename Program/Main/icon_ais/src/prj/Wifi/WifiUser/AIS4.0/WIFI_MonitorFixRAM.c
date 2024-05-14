
#include "WIFI_Common.h"
#include "WIFI_DefUser.h"
#include "WIFI_SetCommon.h"
#include "WIFI_UserInterface.h"
#include "WIFI_MonitorFixRAM.h"
#include "WIFI_UserInterface_IconAIS.h"

//#include "hw.h"
//#include "front.h"
//#include "rtc.h"
//#include "sound.h"
//#include "water_out.h"
//#include "relay.h"
//#include "eol.h"
//#include "fct.h"
//#include "eeprom.h"
//#include "sound.h"
#include "error.h"
#include "display_error.h"
#include "temp.h"
#include "cold_water.h"
#include "comp.h"
#include "comp_bldc.h"

#include "process_display.h"



/***** Directmap *****************************************************************/ 
#pragma address _RamColdTemp          = 0xFFC00
#pragma address _RamAmbiTemp          = 0xFFC02
#pragma address _RamColdRegion        = 0xFFC10
#pragma address _RamCompTargetRps     = 0xFFC12
#pragma address _RamCompFeedbackRps   = 0xFFC14
#pragma address _RamCompProtectTime   = 0xFFC18
#pragma address _RamCompOnTime        = 0xFFC1A
#pragma address _RamCompOffTime       = 0xFFC1C
#pragma address _RamError             = 0xFFC20
#pragma address _RamCold2Temp         = 0xFFC40
#pragma address _RamColdExtraTime     = 0xFFC80
#pragma address _RamColdStarting      = 0xFFCA0
#pragma address _RamColdTempMinus     = 0xFFCA2
#pragma address _RamCold2TempMinus    = 0xFFCA4
 
U16 __near _RamColdTemp;
U16 __near _RamAmbiTemp;
U8  __near _RamColdRegion;
U8  __near _RamCompTargetRps;
U8  __near _RamCompFeedbackRps;
U16 __near _RamCompProtectTime;
U16 __near _RamCompOnTime;
U16 __near _RamCompOffTime;
U8  __near _RamError;
U16 __near _RamCold2Temp;
U16 __near _RamColdExtraTime;
U16 __near _RamColdStarting;
U16 __near _RamCold2TempMinus;
U16 __near _RamColdTempMinus;



#define A_RAM_COLD_TEMP             0xFC00
#define A_RAM_AMBI_TEMP             0xFC02
#define A_RAM_COLD_REGION           0xFC10
#define A_RAM_COMP_TARGET_RPS       0xFC12
#define A_RAM_COMP_FEEDBACK_RPS     0xFC14
#define A_RAM_COMP_PROTECT_TIME     0xFC18
#define A_RAM_COMP_ON_TIME          0xFC1A
#define A_RAM_COMP_OFF_TIME         0xFC1C
#define A_RAM_ERROR                 0xFC20
#define A_RAM_COLD_2_TEMP           0xFC40
#define A_RAM_COLD_EXTRA_TIME       0xFC80
#define A_RAM_COLD_STARTING         0xFCA0
#define A_RAM_COLD_TEMP_MINUS       0xFCA2
#define A_RAM_COLD_2_TEMP_MINUS     0xFCA4
static U16 RamColdTemp;
static U16 RamAmbiTemp;
static U8  RamColdRegion;
static U8  RamCompTargetRps;
static U8  RamCompFeedbackRps;
static U16 RamCompProtectTime;
static U16 RamCompOnTime;
static U16 RamCompOffTime;
static U8  RamError;
static U16 RamCold2Temp;
static U16 RamColdExtraTime;
static U16 RamColdStarting;
static U16 RamColdTempMinus;
static U16 RamCold2TempMinus;

extern U8 A_NULL=0;

/******** typedef *********/
typedef U8 *(*WifiWriteFun_T)(U16); 
typedef struct 
{ 
    U16 AddressList;  
    U8 *TargetVariable;  
    U8 VarSize; 
    WifiWriteFun_T WriteFunc; 
} WifiFixRamList_T; 

static const WifiFixRamList_T WifiRamList[] =
{
    { A_RAM_COLD_TEMP,              (U8*)(&RamColdTemp),            TwoByte,        Read_Only },
    { A_RAM_AMBI_TEMP,              (U8*)(&RamAmbiTemp),            TwoByte,        Read_Only },
    { A_RAM_COLD_REGION,            (U8*)(&RamColdRegion),          OneByte,        Read_Only },
    { A_RAM_COMP_TARGET_RPS,        (U8*)(&RamCompTargetRps),       OneByte,        Read_Only },
    { A_RAM_COMP_FEEDBACK_RPS,      (U8*)(&RamCompFeedbackRps),     OneByte,        Read_Only },
    { A_RAM_COMP_PROTECT_TIME,      (U8*)(&RamCompProtectTime),     TwoByte,        Read_Only },
    { A_RAM_COMP_ON_TIME,           (U8*)(&RamCompOnTime),          TwoByte,        Read_Only },
    { A_RAM_COMP_OFF_TIME,          (U8*)(&RamCompOffTime),         TwoByte,        Read_Only },
    { A_RAM_ERROR,                  (U8*)(&RamError),               OneByte,        Read_Only },
    { A_RAM_COLD_2_TEMP,            (U8*)(&RamCold2Temp),           TwoByte,        Read_Only },
    { A_RAM_COLD_EXTRA_TIME,        (U8*)(&RamColdExtraTime),       TwoByte,        Read_Only },
    { A_RAM_COLD_STARTING,          (U8*)(&RamColdStarting),        OneByte,        Read_Only },
    { A_RAM_COLD_TEMP_MINUS,        (U8*)(&RamColdTempMinus),       TwoByte,        Read_Only },
    { A_RAM_COLD_2_TEMP_MINUS,      (U8*)(&RamCold2TempMinus),      TwoByte,        Read_Only },
};

#define SZ_RAM_ADR_LIST ( sizeof(WifiRamList) / sizeof(WifiFixRamList_T) )
/*--------------------------------------------------*/
/*	Parameter : write Ram Address (server -> wifi)  */
/*	return : U8* write value(start address) (wifi -> mcu)           */
/*--------------------------------------------------*/
#if 0
U8 *SetRamData (U16 mu16address)
{
    static U16 mu16Data = 0U;
    U8 *buf;
    switch ( mu16address )
    {    
         buf = (U8*)mu16address;     // fix ram value

    }
    return (U8*)(&mu16Data);
}
#endif

extern ColdWater_T Cold;
static void UpdateRamData(void)
{
    TEMP_T temp;


    RamColdTemp         = (U16)( GetTemp( TEMP_ID_COLD_WATER ) * 100.0f );
    RamAmbiTemp         = (U16)( GetTemp( TEMP_ID_AMBIENT ) * 100.0f );
    RamColdRegion       = Cold.RegionAmbi;
    RamCompTargetRps    = GetCompBldcTargetRps();
    RamCompFeedbackRps  = GetCompBldcCurrentRps();
    RamCompProtectTime  = GetCompProtectOffTime();
    RamCompOnTime       = GetCompOnTime();
    RamCompOffTime      = GetCompOffTime();
    RamError            = GetErrorDisplayId( GetErrorId() );
    RamCold2Temp        = (U16)( GetTemp( TEMP_ID_EVA_1 ) * 100.0f );
    RamColdExtraTime    = Cold.ExtraMakeTime;
    RamColdStarting     = Cold.StartingMode;

    temp = ( GetTemp( TEMP_ID_COLD_WATER ) * 100.0f );
    if( temp < 0.0f )
    {
        temp = -temp;
        RamColdTempMinus    = (U16)temp;
    }
    else
    {
        RamColdTempMinus    = 0;
    }

    temp = ( GetTemp( TEMP_ID_EVA_1 ) * 100.0f );
    if( temp < 0.0f )
    {
        temp = -temp;
        RamCold2TempMinus    = (U16)temp;
    }
    else
    {
        RamCold2TempMinus    = 0;
    }
}

void GetRamData (void)
{
    __near U8 *pFixRamAddress;
    U8 *pTempPtr ;
    U8 *pTempPtrReturn ;
    U16 RamCheckAddress =0;
    U8 i=0;
    WifiWriteFun_T pWriteFunc;

    UpdateRamData();

    for ( i = 0 ; i < SZ_RAM_ADR_LIST; i++ )
    {
        pFixRamAddress = ((__near U8*)WifiRamList[i].AddressList);
        pTempPtr = WifiRamList[i].TargetVariable;
        RamCheckAddress = (U16)(GetUserSystemRamWriteStatus((U16)(WifiRamList[i].AddressList)));
        if (pTempPtr == &A_NULL) 
        {
            RamCheckAddress = CLEAR_ADR;
            SetUserSystemRamWriteStatus((U16)RamCheckAddress); // clear
        }
        else 
        {
            if (RamCheckAddress != CLEAR_ADR) // write check
            {
                pWriteFunc =  WifiRamList[i].WriteFunc;
                if ( pWriteFunc != Read_Only ) // write check
                {
                    pTempPtrReturn = (U8*)pWriteFunc((U16)(RamCheckAddress));
                    *(pTempPtr) = *(pTempPtrReturn); // 1byte write

                    if (WifiRamList[i].VarSize == TwoByte ) // 2byte write
                    {
                        pTempPtr++;
                        RamCheckAddress++;
                        pTempPtrReturn = (U8*)RamCheckAddress;
                        *(pTempPtr) = *(pTempPtrReturn);
                    }
                    RamCheckAddress = CLEAR_ADR;
                    SetUserSystemRamWriteStatus((U16)RamCheckAddress); // clear
                    break;
                }
                else 
                {
                    RamCheckAddress = CLEAR_ADR;
                    SetUserSystemRamWriteStatus((U16)RamCheckAddress); // clear
                }
            }
            else  // read
            { //1byte read
                *pFixRamAddress = *(WifiRamList[i].TargetVariable);

                if (WifiRamList[i].VarSize == TwoByte ) 
                { //2byte variable
                    pTempPtr = WifiRamList[i].TargetVariable;
                    pFixRamAddress++;
                    pTempPtr++;
                    *pFixRamAddress = *pTempPtr;
                }
            }
        }

    }
}
