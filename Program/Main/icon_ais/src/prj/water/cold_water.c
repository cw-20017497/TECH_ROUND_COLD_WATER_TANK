#include "cold_water.h"
#include "level.h"
#include "temp.h"
#include "valve.h"
#include "error.h"
#include "power_saving.h"
#include "comp.h"
#include "gas_switch_valve.h"
#include "eeprom.h"
#include "front.h"
#include "tank_clean.h"
#include "ice_maker.h"
#include "water_out.h"
#include "filter.h"
#include "drain_water.h"
#include "flush_water_out.h"
#include "flow_meter.h"
#include "drain_pump.h"
#include "hot_water_out.h"
#include "cody_kit.h"


/* TARGET OFF TEMP - PROTECTION SUB-COOL */
#define PROTECT_COLD_TEMP   2.0f

/* TARGET TEMP - Power Saving */
static const TEMP_T OnTempPowerSaving    = 10.0f;


// Starting Mode
#define MODE_INIT_STARTING      0
#define MODE_RESTARTING         1
#define MODE_NUM                2

#define TEMP_INIT_STARTING      23.0f   // Cold water temp, for init starting....
#define ON_DELAY_TIME           10      // 10sec @1sec

#define INIT_STARTING_AMOUNT_TIME   180U // 3min

// Target Rps by Cold Temp Region...
#define REGION_COLD_LOWER       0
#define REGION_COLD_UPPER       1


#define PROTECTION_MIN_EXTRA_TIME     180U // 3min

typedef enum
{
    REGION_AMBIENT_0,
    REGION_AMBIENT_1, 
    REGION_AMBIENT_2, 
    REGION_AMBIENT_3, 
    REGION_AMBIENT_4, 
    REGION_AMBIENT_5, 
    REGION_AMBIENT_6, 
    REGION_AMBIENT_7, 
    REGION_AMBIENT_8, 
    REGION_AMBIENT_9, 
    REGION_AMBIENT_10, 
    REGION_NUM,       
} ColdRegion_T;


typedef struct _cold_water_make_table_
{
    ColdRegion_T RegionAmbi;

    TEMP_T  TempOn;           // 
    TEMP_T  TempPrtOn;        // 
    TEMP_T  TempOff;

    TEMP_T  TempCompRps;      // "Comp Rps" reference temp
    RPS_T   RpsLower;         // Comp RPS when cold temp < ref temp 
    RPS_T   RpsUpper;         // Comp RPS when cold temp > ref temp

    U16     ExtraMakeTime;      // @1sec
} ColdMakeTable_T;


#define T_UNUSED    -10.0f
static const ColdMakeTable_T     ColdMakeTableList[ MODE_NUM ][ REGION_NUM ] = 
{
    /* COLD WATER >= TEMP_INIT_STARTING */
    {                       
        /*  Region              cold on, prt on,   cold off,   comp_t   tC_L, tC_H,   extra time */
        {   REGION_AMBIENT_0,   3.5f,    T_UNUSED,  2.0f,       0.0f,    45,   45,     90 },
        {   REGION_AMBIENT_1,   4.0f,    T_UNUSED,  2.5f,       0.0f,    47,   47,     150},
        {   REGION_AMBIENT_2,   4.5f,    T_UNUSED,  3.0f,       0.0f,    47,   47,     210}, 
        {   REGION_AMBIENT_3,   5.0f,    T_UNUSED,  3.5f,       0.0f,    47,   47,     240},
        {   REGION_AMBIENT_4,   6.0f,    T_UNUSED,  4.0f,       0.0f,    47,   47,     330},
        {   REGION_AMBIENT_5,   7.0f,    T_UNUSED,  5.0f,       0.0f,    47,   47,     480},
        {   REGION_AMBIENT_6,   7.5f,    T_UNUSED,  5.0f,       0.0f,    47,   47,     540},
        {   REGION_AMBIENT_7,   8.0f,    T_UNUSED,  5.0f,       0.0f,    47,   47,     660},
        {   REGION_AMBIENT_8,   10.0f,   T_UNUSED,  5.0f,       0.0f,    47,   47,     780},
        {   REGION_AMBIENT_9,   12.5f,   T_UNUSED,  5.5f,       5.0f,    60,   45,     990},
        {   REGION_AMBIENT_10,  14.0f,   T_UNUSED,  5.5f,       5.0f,    60,   45,     1050}
    },

    /* COLD WATER < TEMP_INIT_STARTING */
    {
        /*  Region              cold on, prt on,    cold off,   comp_t  tC_L, tC_H,   extra time */
        {   REGION_AMBIENT_0,   3.6f,    3.5f,      2.0f,       0.0f,    45,   45,    0  },
        {   REGION_AMBIENT_1,   4.2f,    3.8f,      2.5f,       0.0f,    47,   47,    90 },
        {   REGION_AMBIENT_2,   5.0f,    4.0f,      3.0f,       0.0f,    47,   47,    120}, 
        {   REGION_AMBIENT_3,   5.5f,    4.1f,      3.5f,       0.0f,    47,   47,    180},
        {   REGION_AMBIENT_4,   6.5f,    4.3f,      4.0f,       0.0f,    47,   47,    210},
        {   REGION_AMBIENT_5,   7.5f,    4.6f,      5.0f,       0.0f,    47,   47,    240},
        {   REGION_AMBIENT_6,   8.0f,    4.8f,      5.0f,       0.0f,    47,   47,    270},
        {   REGION_AMBIENT_7,   8.5f,    5.0f,      5.0f,       0.0f,    47,   47,    330},
        {   REGION_AMBIENT_8,   10.5f,   5.5f,      5.0f,       0.0f,    47,   47,    360},
        {   REGION_AMBIENT_9,   13.0f,   6.3f,      5.5f,       5.5f,    60,   45,    390},
        {   REGION_AMBIENT_10,  14.5f,   6.5f,      5.5f,       5.5f,    60,   45,    420}
    }
};


const ColdMakeTable_T *pColdMake = NULL;
ColdWater_T Cold;



static U8 GetRegionByTempAmbi(void);
static void SetColdMakeTable( U8 StartingMode );
static TEMP_T GetTargetOnTemp(void);
static TEMP_T GetTargetOnPrtTemp(void);
static TEMP_T GetTargetOffTemp(void);

void  InitColdWater(void)
{
    Cold.InitFull       = FALSE;
    Cold.Level          = LEVEL_HIGH;
    Cold.ConfigMake     = TRUE;
    Cold.Make           = FALSE;
    Cold.Mode           = COLD_MODE_INIT;

    // Make
    Cold.StartingModeTime   = 0U;

    SetColdMakeTable( MODE_RESTARTING );
    Cold.TempTargetOn       = GetTargetOnTemp();
    Cold.TempPrtTargetOn    = GetTargetOnPrtTemp();
    Cold.TempTargetOff      = GetTargetOffTemp();
    Cold.TempCurrent        = GetTemp( TEMP_ID_COLD_WATER );
    Cold.TempPrtCurrent     = GetTemp( TEMP_ID_PRT_COLD_WATER );
    Cold.TempStatus         = COLD_STATUS_GOOD;

    Cold.RegionCold         = REGION_COLD_UPPER;
    Cold.TargetRps          = 48;

    Cold.ExtraMake          = FALSE;
    Cold.ExtraMakeTime      = 0;

    ResetColdAmount();
}


void  GetColdWaterData(ColdWater_T *pData)
{
    MEMCPY( (void __FAR *)pData, (const void __FAR *)&Cold, sizeof( ColdWater_T ));
}

void  SetColdWaterInitFull(U8 mu8Val )
{
    Cold.InitFull = mu8Val;
}

U8    GetColdWaterInitFull(void)
{
    return Cold.InitFull;
}


void  SetColdWaterLevel(U8 mu8Val )
{
    Cold.Level = mu8Val;
}

U8    GetColdWaterLevel(void)
{
    return Cold.Level;
}

void  SetColdWaterConfigMake(U8 mu8Val )
{
    if( mu8Val != TRUE && mu8Val != FALSE )
    {
        return ;
    }

    Cold.ConfigMake = mu8Val;

    SaveEepromId( EEP_ID_CONF_MAKE_COLD );
}

U8    GetColdWaterConfigMake(void)
{
    return Cold.ConfigMake;
}


void  SetColdWaterMake(U8 mu8Val )
{
    Cold.Make = mu8Val;
}

U8    GetColdWaterMake(void)
{
    return Cold.Make;
}

TEMP_T GetColdTargetOffTemp(void)
{
    return Cold.TempTargetOff;
}

TEMP_T GetColdTargetOnTemp(void)
{
    return Cold.TempTargetOn;
}


U8 GetColdWaterTempStatus(void)
{
    return Cold.TempStatus;
}

void CheckColdWaterTempStatus(void)
{
    // 냉수 OFF시, 온도등 OFF
    if( GetColdWaterConfigMake() == FALSE )
    {
    //    Cold.TempStatus = COLD_STATUS_GOOD;
        return ;
    }
    
    // 온도에 따른 Region 계산 
    if( Cold.TempStatus == COLD_STATUS_GOOD )
    {
        if( GetTemp( TEMP_ID_COLD_WATER ) >= GetColdTargetOnTemp() )
        {
            Cold.TempStatus = COLD_STATUS_BAD;
        }
    }
    else
    {
        if( GetTemp( TEMP_ID_COLD_WATER ) <= GetColdTargetOffTemp() )
        {
            Cold.TempStatus = COLD_STATUS_GOOD;
        }
    }
}


void SetColdWaterMakeMode(U8 mu8Mode )
{
    Cold.Mode |= mu8Mode;
}

void ClearColdWaterMakeMode(U8 mu8Mode )
{
    Cold.Mode &= ~mu8Mode;
}

U8 IsSetColdWaterMakeMode(U8 mu8Mode )
{
    if( (Cold.Mode & mu8Mode ) == mu8Mode )
    {
        return TRUE;
    }

    return FALSE;
}

U8 GetColdWaterMakeMode(void )
{
    return Cold.Mode;
}

RPS_T   GetColdWaterMakeCompRps(void)
{
    return Cold.TargetRps;
}


/*
 * 냉수 탱크 수위 제어
 */
void  ControlColdWaterLevel(void)
{
    U8 IsFilterOpen = FALSE;


    /* FILTER COVER OPEN 검사
     * 단, 코디 검사 중에는 Skip 
     */
    if( GetCodyKitStatus() == FALSE )
    {
        if(  IsOpenFilter( ID_FILTER) == TRUE 
                || IsOpenFilter( ID_FILTER_COVER) == TRUE )
        {
            IsFilterOpen = TRUE;
        }
    }

    /* ERROR -  LEAK */
    if( IsError( ERR_LEAK ) == TRUE 
            || IsError( ERR_LEAK_FLOW ) == TRUE 
            || IsFilterOpen == TRUE )
    {
        CloseValve( VALVE_NOS );
    }
    else
    {
        OpenValve( VALVE_NOS );
    }
}


static U8 IsValidMake(void)
{
    /* Turn Off make, dont want to make */
    if( Cold.ConfigMake == FALSE )
    {
        return FALSE;
    }

    if( IsDoneFlushStatus() == FALSE )
    {
        return FALSE;
    }

#if 0
    /* 냉수 탱크가 최초 만수위를 하지 못한 경우 */
    if( Cold.InitFull == FALSE )
    {
        return FALSE;
    }
#endif

    /* 슬립 중에는 정지 */
#if 0
    if( GetSavingStatus() == TRUE )
    {
        return FALSE;
    }
#endif

#if 0
    /* 압축기 구속 시간 */
    if( IsExpiredCompProtectTime() == FALSE )
    {
        return FALSE;
    }
#endif

    /* 압축기 보호 시간 초과 */
    if( IsOverCompProtectOnTime() == TRUE )
    {
        return FALSE;
    }

    /* 압축기 BLDC 보호동작 */
    if( IsCompBldcProtectMode() == TRUE )
    {
        return FALSE;
    }
    
    /* Turn Off make, Error */
    if( IsErrorType( ERR_TYPE_COLD_MAKE ) == TRUE )
    {
        return FALSE;
    }

    /* 강제 배수 */
    if( IsGoingOnDrainWaterFull() == TRUE )
    {
        return FALSE;
    }

    // 냉수 청소 모드
#if 0
    if( GetTankCleanOffLoad() == TRUE )
    {
        return FALSE;
    }

    if( GetCodyService() == TRUE )
    {
        return FALSE;
    }
#endif

    return TRUE;  // Yes, valid
}


static TEMP_T   GetTargetOnTemp(void)
{
    // 절전
    if( GetSavingStatus() == STATUS_SLEEP )
    {
        return OnTempPowerSaving;
    }

    return pColdMake->TempOn;
}

static TEMP_T   GetTargetOffTemp(void)
{
    return pColdMake->TempOff;
}

static TEMP_T   GetTargetOnPrtTemp(void)
{
    return pColdMake->TempPrtOn;
}


static void ClearExtraMake(void)
{
    Cold.ExtraMake      = FALSE;
    Cold.ExtraMakeTime  = 0;
    Cold.ExtraMakeRetry = FALSE;
}

static void ClearPrtExtraMake(void)
{
    Cold.PrtExtraMake       = FALSE;
    Cold.PrtExtraMakeTime   = 0;
}

// 냉각 추가 타이머 decounter
static void UpdateExtraMakeTime(void)
{
    if( GetCompOnOff() == OFF )
    {
        return ;
    }

    if( GetGasSwitchStatus() != GAS_SWITCH_COLD )
    {
        return ;
    }

    if( Cold.ExtraMakeTime != 0 )
    {
        Cold.ExtraMakeTime--;
    }
}


void ResetColdAmount(void)
{
    Cold.Amount     = AMOUNT_240;
    Cold.AmountTime = INIT_STARTING_AMOUNT_TIME;    // 3min
}

void UpdateColdAmount(U32 mu32Amount)
{
    if( Cold.Amount < mu32Amount )
    {
        Cold.Amount = 0;
    }
    else
    {
        Cold.Amount -= mu32Amount;
    }
}

static void CheckColdOutAmount(void)
{
    if( Cold.Amount < AMOUNT_240 )
    {
        if( Cold.AmountTime == 0 )
        {
            if( Cold.Amount != 0 )
            {
                ResetColdAmount();
            }
        }
        else
        {
            Cold.AmountTime--;
        }
    }
}

static U8 IsInitStartingModeByColdOut(void)
{
    if( GetWaterOut() == TRUE 
            && GetWaterOutSelect() == SEL_WATER_COLD )
    {
        Cold.StartingModeTime = ON_DELAY_TIME;  
    }

    // 모드 결정 시간이 만료되면 냉각 여부 및 
    if( Cold.StartingModeTime != 0 )
    {
        Cold.StartingModeTime--;
    }
    else
    {
        // 추출량이 기준치에 도달하면 재기동 모드로 동작
        if( Cold.Amount == 0 )
        {
            return TRUE;
        }
    }

    return FALSE;
}




// 외기 온도 기준 결정
static U8 GetRegionByTempAmbi(void)
{
    U8 mu8Region;
    TEMP_T  mTempAmbi;

    mTempAmbi = GetTemp( TEMP_ID_AMBIENT );
    if( mTempAmbi <= 4.0f )         { mu8Region = REGION_AMBIENT_0; }
    else if( mTempAmbi <= 8.0f )    { mu8Region = REGION_AMBIENT_1; }
    else if( mTempAmbi <= 12.0f )   { mu8Region = REGION_AMBIENT_2; }
    else if( mTempAmbi <= 16.0f )   { mu8Region = REGION_AMBIENT_3; }
    else if( mTempAmbi <= 20.0f )   { mu8Region = REGION_AMBIENT_4; }
    else if( mTempAmbi <= 24.0f )   { mu8Region = REGION_AMBIENT_5; }
    else if( mTempAmbi <= 28.0f )   { mu8Region = REGION_AMBIENT_6; }
    else if( mTempAmbi <= 32.0f )   { mu8Region = REGION_AMBIENT_7; }
    else if( mTempAmbi <= 36.0f )   { mu8Region = REGION_AMBIENT_8; }
    else if( mTempAmbi <= 40.0f )   { mu8Region = REGION_AMBIENT_9; }
    else { mu8Region = REGION_AMBIENT_10; }

    return mu8Region;
}

static void RefreshMakeTable(void)
{
    Cold.RegionAmbi     = GetRegionByTempAmbi();
    pColdMake = &ColdMakeTableList[ Cold.StartingMode ][ Cold.RegionAmbi ];

    // Get Temperture
    Cold.TempTargetOn       = GetTargetOnTemp();
    Cold.TempPrtTargetOn    = GetTargetOnPrtTemp();
    Cold.TempTargetOff      = GetTargetOffTemp();
}

static void SetColdMakeTable( U8 StartingMode )
{
    Cold.StartingMode   = StartingMode;

    RefreshMakeTable();
}



static U8 CheckStartMake(void)
{
    U8 mu8Make = FALSE;


    if( GetTemp( TEMP_ID_COLD_WATER ) >= TEMP_INIT_STARTING 
            || IsInitStartingModeByColdOut() == TRUE )
    {
        SetColdMakeTable( MODE_INIT_STARTING );

        if( Cold.TempCurrent >= Cold.TempTargetOn )
        {
            ClearPrtExtraMake();

            mu8Make = TRUE;
        }
        else
        {
            mu8Make = FALSE;
        }
    }
    else if( Cold.TempPrtCurrent > PROTECT_COLD_TEMP )
    {
        SetColdMakeTable( MODE_RESTARTING );

        if( Cold.TempCurrent >= Cold.TempTargetOn 
                && Cold.TempPrtCurrent >= Cold.TempPrtTargetOn )
        {
            ClearPrtExtraMake();

            mu8Make = TRUE;
        }
        else if( Cold.PrtExtraMake == TRUE 
                && IsExpiredCompProtectTime() == TRUE )
        {
            Cold.ExtraMakeRetry = TRUE; // set flag..

            Cold.ExtraMake      = TRUE;
            Cold.ExtraMakeTime = Cold.PrtExtraMakeTime;

            ClearPrtExtraMake();

            mu8Make = FALSE;
        }
        else
        {
            mu8Make = FALSE;
        }
    }

    return mu8Make;
}


static U8  CheckStopMake(void)
{
    U8 mu8Make = TRUE;      // 냉각


    // 1. 운전 중에 
    // 냉수 온도가 매우 높거나, 
    // 냉수 추출이 일정 부분 있었으면 
    // 초기 기동 모드로 전환.
    if( Cold.TempCurrent >= TEMP_INIT_STARTING 
            || IsInitStartingModeByColdOut() == TRUE )
    {
        SetColdMakeTable( MODE_INIT_STARTING );
    }

    // 2. 냉각 정지 온도에 도달하면 추가 냉각 운전 시간을 설정하고 종료.
    if( Cold.TempTargetOff >= Cold.TempCurrent )
    {
        Cold.ExtraMake      = TRUE;
        Cold.ExtraMakeTime  = pColdMake->ExtraMakeTime;
        if( Cold.ExtraMakeTime == 0 )
        {
            // 추가 냉각 시간이 설정되어 있지 않으면 OFF
            Cold.ExtraMake  = FALSE;    
            mu8Make         = FALSE;    
        }
    }

    return mu8Make;
}

static U8  CheckExtraMake(void)
{
    U8 mu8Make = TRUE;

    // Extra Make Cold 
    if( Cold.ExtraMakeTime != 0 )
    {
        mu8Make = TRUE;
    }
    else
    {
        ClearExtraMake();

        // 추가 냉각이 종료되고 다시 냉각 재운전 조건이면 동작하도록...
        mu8Make = CheckStartMake();
    }

    return mu8Make;
}

static U8  CheckProtectionMake(U8 mu8Make)
{
    // 보호 운전 조건( 재기동, 보호 온도 2도 이하 )
    //if( Cold.StartingMode == MODE_RESTARTING && Cold.TempPrtCurrent <= PROTECT_COLD_TEMP )
    if( Cold.TempPrtCurrent <= PROTECT_COLD_TEMP )
    {
        if( Cold.RegionAmbi >= REGION_AMBIENT_8 )
        {
            // 환경 온도 32도 이상, 일반 운전 중이면 즉시 중지
            // 환경 온도 32도 이상, 추가 냉각 중이면 
            if( Cold.ExtraMake == TRUE )
            {
                if( Cold.ExtraMakeRetry == FALSE )
                {
                    if( Cold.ExtraMakeTime >= PROTECTION_MIN_EXTRA_TIME)
                    {
                        Cold.PrtExtraMake       = TRUE;
                        Cold.PrtExtraMakeTime   = Cold.ExtraMakeTime;
                    }

                    ClearExtraMake();
                    mu8Make = FALSE;    
                }
            }
            else if( mu8Make == TRUE )
            {
                ClearExtraMake();
                ClearPrtExtraMake();
                mu8Make = FALSE;    
            }
        }
        else
        {
            ClearExtraMake();
            ClearPrtExtraMake();
            mu8Make = FALSE;    
        }
        
    }

    return mu8Make;
}




// 냉수 온도에 따라 목표 RPS를 결정
static RPS_T CheckCompRps(void)
{
    RPS_T   mTargetRps;
    TEMP_T  mTempLower;             // Lower(A)
    TEMP_T  mTempUpper;             // Upper(B)


    // Set Cold Temp Region
    mTempLower = pColdMake->TempCompRps;            // Region A
    mTempUpper = pColdMake->TempCompRps + 0.5f;     // Region B
    if( Cold.RegionCold == REGION_COLD_LOWER )
    {
        if(  mTempUpper <= Cold.TempCurrent )
        {
            Cold.RegionCold = REGION_COLD_UPPER;
        }
    }
    else
    {
        if(  Cold.TempCurrent < mTempLower )
        {
            Cold.RegionCold = REGION_COLD_LOWER;
        }

    }

    // Set Target RPS
    if( Cold.RegionCold == REGION_COLD_LOWER )
    {
        mTargetRps = pColdMake->RpsLower;
    }
    else
    {
        mTargetRps = pColdMake->RpsUpper;
    }

    return mTargetRps;
}




void  MakeColdWater(void)
{
    U8 mu8Make = FALSE;


    Cold.TempCurrent    = GetTemp( TEMP_ID_COLD_WATER );
    Cold.TempPrtCurrent = GetTemp( TEMP_ID_PRT_COLD_WATER );
    RefreshMakeTable();

    UpdateExtraMakeTime();

    CheckColdOutAmount();

    // Get Current Make Status
    mu8Make = GetColdWaterMake();


    // 추각 냉각 조건이면, 일반 운전은 운전 조건으로 검사.
    if( Cold.ExtraMake == TRUE )
    {
        mu8Make = CheckExtraMake();
    }
    else 
    {
        if( mu8Make == TRUE )
        {
            mu8Make = CheckStopMake();
        }
        else
        {
            mu8Make = CheckStartMake();
        }
    }

    // Protection
    mu8Make = CheckProtectionMake( mu8Make );


    // Comp rps
    if( mu8Make == TRUE )
    {
        Cold.TargetRps = CheckCompRps();
    }

    /* 압축기 구속 시간 */
    if( IsExpiredCompProtectTime() == FALSE )
    {
        mu8Make = FALSE;
    }

    // Config Off
    if( IsValidMake() == FALSE )
    {
        mu8Make = FALSE;

        ClearExtraMake();
        ClearPrtExtraMake();
    }

    // Set new make status
    SetColdWaterMake( mu8Make );
}

