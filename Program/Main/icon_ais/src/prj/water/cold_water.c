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
#define PROTECT_COLD_TEMP   0.0f

/* TARGET TEMP - Power Saving */
static const TEMP_T OnTempPowerSaving    = 10.0f;


// Starting Mode
#define MODE_INIT_STARTING      0
#define MODE_RESTARTING         1
#define MODE_SUSTAIN            2
#define MODE_NUM                3

#define TEMP_INIT_STARTING      20.0f   // Cold water temp, for init starting....
#define ON_DELAY_TIME           10      // 10sec @1sec

/* RESTARTING COUNT LIMIT - NO EXTRA MAKE TIME */
#define RESTARTING_COUNT        2

// Target Rps by Cold Temp Region...
#define REGION_COLD_LOWER       0
#define REGION_COLD_UPPER       1

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

#define INIT_STARTING_AMOUNT_TIME   180U // 3min

// EXTRA MAKE TIME @1sec
//#define EXTRA_MAKE_TIME_0MIN    0U         // 0sec
//#define EXTRA_MAKE_TIME_5MIN    300U       // 5min = 5 x 60sec 
//#define EXTRA_MAKE_TIME_10MIN   600U       // 10min = 10 x 60sec   
//#define EXTRA_MAKE_TIME_15MIN   900U       // 15min = 15 x 60sec
//#define EXTRA_MAKE_TIME_20MIN   1200U       // 20min = 20 x 60sec


// EXTRA MAKE COLD TEMP
//#define TEMP_ON                 7.5f
//#define TEMP_OFF_5D8            5.8f
//#define TEMP_OFF_6D0            6.0f


typedef struct _cold_water_make_table_
{
    ColdRegion_T RegionAmbi;

    TEMP_T  TempOn;
    TEMP_T  TempOff;

    TEMP_T  TempCompRps;      // cold temp 
    RPS_T   RpsLower;         // cold temp lower region comp rps
    RPS_T   RpsUpper;         // cold temp upper region comp rps

    U16     ExtraMakeTime;      // @1sec
} ColdMakeTable_T;


#if 0
static const ColdMakeTable_T     ColdMakeTableList[ MODE_NUM ][ REGION_NUM ] = 
{
    /* COLD WATER >= TEMP_INIT_STARTING */
    {
        {   REGION_AMBIENT_A, TEMP_ON,   TEMP_OFF_6D0,   0.0f,   45,   45,    EXTRA_MAKE_TIME_0MIN  },
        {   REGION_AMBIENT_B, TEMP_ON,   TEMP_OFF_5D8,   0.0f,   45,   45,    EXTRA_MAKE_TIME_0MIN  },
        {   REGION_AMBIENT_C, TEMP_ON,   TEMP_OFF_5D8,   8.0f,   60,   45,    EXTRA_MAKE_TIME_0MIN  },
        {   REGION_AMBIENT_D, TEMP_ON,   TEMP_OFF_5D8,   10.0f,  60,   45,    EXTRA_MAKE_TIME_5MIN  }
    },

    /* COLD WATER < TEMP_INIT_STARTING */
    {
        {   REGION_AMBIENT_A, TEMP_ON,   TEMP_OFF_6D0,   0.0f,   40, 40,    EXTRA_MAKE_TIME_0MIN  },
        {   REGION_AMBIENT_B, TEMP_ON,   TEMP_OFF_5D8,   6.5f,   40, 45,    EXTRA_MAKE_TIME_0MIN  },
        {   REGION_AMBIENT_C, TEMP_ON,   TEMP_OFF_5D8,   10.0f,  60, 45,    EXTRA_MAKE_TIME_0MIN  },
        {   REGION_AMBIENT_D, TEMP_ON,   TEMP_OFF_5D8,   10.0f,  60, 45,    EXTRA_MAKE_TIME_5MIN  }
    }
};
#else
static const ColdMakeTable_T     ColdMakeTableList[ MODE_NUM ][ REGION_NUM ] = 
{
    /* COLD WATER >= TEMP_INIT_STARTING */
    {                       
        /*  Region              cold on, cold off,   comp_t   tC_L, tC_H,   extra time */
        {   REGION_AMBIENT_0,   3.5f,    2.0f,       0.0f,    45,   45,      90     },
        {   REGION_AMBIENT_1,   4.0f,    2.5f,       0.0f,    48,   48,      120    },
        {   REGION_AMBIENT_2,   4.5f,    3.0f,       0.0f,    48,   48,      180    }, 
        {   REGION_AMBIENT_3,   5.0f,    3.5f,       0.0f,    48,   48,      210    },
        {   REGION_AMBIENT_4,   6.0f,    4.0f,       0.0f,    48,   48,      240    },
        {   REGION_AMBIENT_5,   7.0f,    5.0f,       0.0f,    48,   48,      480    },
        {   REGION_AMBIENT_6,   8.0f,    5.0f,       0.0f,    51,   51,      600    },
        {   REGION_AMBIENT_7,   9.5f,    5.0f,       0.0f,    51,   51,      690    },
        {   REGION_AMBIENT_8,   11.0f,   5.0f,       0.0f,    51,   51,      750    },
        {   REGION_AMBIENT_9,   13.0f,   5.0f,       5.0f,    53,   45,      840    },
        {   REGION_AMBIENT_10,  14.0f,   5.0f,       5.0f,    53,   45,      930    }
    },

    /* COLD WATER < TEMP_INIT_STARTING */
    {
        /*  Region              cold on, cold off,   comp_t  tC_L, tC_H,   extra time */
        {   REGION_AMBIENT_0,   3.5f,    2.0f,       0.0f,    45,   45,     0   },
        {   REGION_AMBIENT_1,   4.0f,    2.5f,       0.0f,    45,   45,     60  },
        {   REGION_AMBIENT_2,   5.0f,    3.0f,       0.0f,    45,   45,     90  }, 
        {   REGION_AMBIENT_3,   8.0f,    3.5f,       0.0f,    45,   45,     120 },
        {   REGION_AMBIENT_4,   9.0f,    4.0f,       0.0f,    45,   45,     150 },
        {   REGION_AMBIENT_5,   10.0f,   5.0f,       0.0f,    45,   45,     300 },
        {   REGION_AMBIENT_6,   11.5f,   5.0f,       5.0f,    45,   41,     390 },
        {   REGION_AMBIENT_7,   13.0f,   5.0f,       5.0f,    47,   41,     450 },
        {   REGION_AMBIENT_8,   14.5f,   5.0f,       5.0f,    49,   43,     480 },
        {   REGION_AMBIENT_9,   16.0f,   5.0f,       5.0f,    51,   45,     540 },
        {   REGION_AMBIENT_10,  17.0f,   5.0f,       5.0f,    51,   45,     600 }
    },

    /* RESTARTING over 3 times */
    {
        /*  Region              cold on, cold off,   comp_t  tC_L, tC_H,   extra time */
        {   REGION_AMBIENT_0,   3.5f,    2.0f,       0.0f,    45,   45,     0   },
        {   REGION_AMBIENT_1,   4.0f,    2.5f,       0.0f,    45,   45,     60  },
        {   REGION_AMBIENT_2,   5.0f,    3.0f,       0.0f,    45,   45,     90  }, 
        {   REGION_AMBIENT_3,   8.0f,    3.5f,       0.0f,    45,   45,     120 },
        {   REGION_AMBIENT_4,   9.0f,    4.0f,       0.0f,    45,   45,     150 },
        {   REGION_AMBIENT_5,   10.0f,   5.0f,       0.0f,    45,   45,     300 },
        {   REGION_AMBIENT_6,   11.5f,   5.0f,       5.0f,    45,   41,     390 },
        {   REGION_AMBIENT_7,   13.0f,   5.0f,       5.0f,    47,   41,     450 },
        {   REGION_AMBIENT_8,   14.5f,   5.0f,       5.0f,    49,   43,     480 },
        {   REGION_AMBIENT_9,   16.0f,   5.0f,       5.0f,    51,   45,     540 },
        {   REGION_AMBIENT_10,  17.0f,   5.0f,       5.0f,    51,   45,     600 }
    }
};
#endif

const ColdMakeTable_T *pColdMake = NULL;
ColdWater_T Cold;



static void UpdateColdMakeData(void);
static TEMP_T GetTargetOnTemp(void);
static TEMP_T GetTargetOffTemp(void);

void  InitColdWater(void)
{
    Cold.InitFull       = FALSE;
    Cold.Level          = LEVEL_HIGH;
    Cold.ConfigMake     = TRUE;
    Cold.Make           = FALSE;
    Cold.Mode           = COLD_MODE_INIT;

    // Make
    Cold.StartingMode       = MODE_INIT_STARTING;
    Cold.StartingModeTime   = 0U;

    UpdateColdMakeData();
    Cold.TempTargetOn       = GetTargetOnTemp();
    Cold.TempTargetOff      = GetTargetOffTemp();
    Cold.TempCurrent        = GetTemp( TEMP_ID_COLD_WATER );
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

void InitColdWaterRestartingCount(void)
{
    Cold.RestartingCount = RESTARTING_COUNT;
}

void CountColdWaterRestartingCount(void)
{
    if( Cold.StartingMode == MODE_INIT_STARTING )
    {
        Cold.RestartingCount = RESTARTING_COUNT;
    }
    else
    {
        if( Cold.RestartingCount != 0 )
        {
            Cold.RestartingCount--;
        }
    }
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
        return FALSE ;
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

    /* 압축기 구속 시간 */
    if( IsExpiredCompProtectTime() == FALSE )
    {
        return FALSE;
    }

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

#if 0
// 추가 냉각 여부 확인
// TRUE 반환이면 추가 냉각
// FALSE 이면 추가 냉각 없음.
static U8 IsExtraMake(void)
{
    // 외기 온도 센서 에러 추가 냉각 없음
    if( IsError( ERR_TEMP_AMBIENT ) == TRUE )
    {
        return FALSE;   // NO EXTRA MAKE
    }

    // 외기 온도 기준치 이하 추가 냉각 없음
    if( pColdMake->ExtraMakeTime == EXTRA_MAKE_TIME_0MIN )
    {
        return FALSE; // NO EXTRA MAKE;
    }

    return TRUE;   // EXTRA MAKE
}
#endif

void ResetColdAmount(void)
{
    Cold.Amount     = AMOUNT_250;
    Cold.AmountTime = INIT_STARTING_AMOUNT_TIME;
}

static void CheckInitStartingModeByColdAmount(void)
{
    if( Cold.Amount < AMOUNT_250 )
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

// 냉수2번 온도에 따른 RESTARTING 재기동 대기 여부
// TRUE : protect mode 
// FALSE : normal
static U8 IsProtectRestartingByFreezing(void)
{
    TEMP_T  mCurrentTemp;
    TEMP_T  mTargetTemp;


    mCurrentTemp = GetTemp( TEMP_ID_EVA_1 );   // actually cold water-2 
    switch( Cold.RegionAmbi )
    {
        case REGION_AMBIENT_0: mTargetTemp = 0.5f; break;
        case REGION_AMBIENT_1: mTargetTemp = 0.6f; break;
        case REGION_AMBIENT_2: mTargetTemp = 0.7f; break;
        case REGION_AMBIENT_3: mTargetTemp = 0.9f; break;
        case REGION_AMBIENT_4: mTargetTemp = 1.1f; break;
        case REGION_AMBIENT_5: mTargetTemp = 1.2f; break;
        case REGION_AMBIENT_6: mTargetTemp = 1.3f; break;
        case REGION_AMBIENT_7: mTargetTemp = 1.4f; break;
        case REGION_AMBIENT_8: mTargetTemp = 1.5f; break;
        case REGION_AMBIENT_9: mTargetTemp = 1.6f; break;
        case REGION_AMBIENT_10: mTargetTemp = 1.6f; break;
        default: mTargetTemp = 1.6f; break;
    }

    if( mTargetTemp <= mCurrentTemp )
    {
        return FALSE;   // normal ( comp on )
    }

    return TRUE; // protect( comp off )
}

static U16 ProtectFreezingExtraTime( U16 mExtraMakeTime )
{
    U16 mProtectMakeTime;
    TEMP_T  mCurrentTemp;


    switch( Cold.RegionAmbi )
    {
        case REGION_AMBIENT_0: mProtectMakeTime = 0; break;
        case REGION_AMBIENT_1: mProtectMakeTime = 60; break;
        case REGION_AMBIENT_2: mProtectMakeTime = 90; break;
        case REGION_AMBIENT_3: mProtectMakeTime = 90; break;
        case REGION_AMBIENT_4: mProtectMakeTime = 120; break;
        case REGION_AMBIENT_5: mProtectMakeTime = 240; break;
        case REGION_AMBIENT_6: mProtectMakeTime = 300; break;
        case REGION_AMBIENT_7: mProtectMakeTime = 330; break;
        case REGION_AMBIENT_8: mProtectMakeTime = 360; break;
        case REGION_AMBIENT_9: mProtectMakeTime = 420; break;
        case REGION_AMBIENT_10: mProtectMakeTime = 480; break;
        default: mProtectMakeTime = 480; break;
    }

    mCurrentTemp = GetTemp( TEMP_ID_EVA_1 );   // actually cold water-2 
    if( mCurrentTemp <= 0.0f )
    {
        if( mExtraMakeTime > mProtectMakeTime )
        {
            mExtraMakeTime = mProtectMakeTime;
        }
    }

    return mExtraMakeTime;
}

// Starting Mode 결정
// 냉각 여부 결정
static U8 CheckStartMake(void)
{
    U8 mu8Make = FALSE;

    // 냉수 온도가 기준치 이상  조건에서는 즉시 냉각
    if( Cold.TempCurrent >= TEMP_INIT_STARTING )
    {
        Cold.StartingMode = MODE_INIT_STARTING;
        mu8Make = TRUE;
    }
    else 
    {
        // 냉수 온도 기준 이하 조건에서는 추출 중인 경우에는
        // 10초 지연 후 냉각 여부 결정.
        if( Cold.TempCurrent >= Cold.TempTargetOn )
        {
            // 냉수 추출 중에는 시간 재설정
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
                    Cold.StartingMode = MODE_INIT_STARTING;
                    mu8Make = TRUE;
                }
                else
                {
                    // 재기동 모드를 결정
                    if( Cold.RestartingCount == 0 )
                    {
                        Cold.StartingMode = MODE_SUSTAIN;
                    }
                    else
                    {
                        Cold.StartingMode = MODE_RESTARTING;
                    }

                    // 냉수 센서 보호 동작 확인 후 재기동 여부 판단
                    if( IsProtectRestartingByFreezing() == TRUE )
                    {
                        mu8Make = FALSE;
                    }
                    else
                    {
                        mu8Make = TRUE;
                    }
                }

            }
        }
        else
        {
            Cold.StartingModeTime = ON_DELAY_TIME;
        }
    }

    return mu8Make;
}

static U8  CheckStopMake(void)
{
    U8 mu8Make = TRUE;      // 냉각


    if( Cold.TempTargetOff >= Cold.TempCurrent 
            ||  0.0f >= GetTemp( TEMP_ID_EVA_1 ) )
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



// 외기 온도 기준 결정
static void UpdateColdMakeData(void)
{
    TEMP_T  mTempAmbi;


    mTempAmbi = GetTemp( TEMP_ID_AMBIENT );
    if( mTempAmbi <= 4.0f )         { Cold.RegionAmbi = REGION_AMBIENT_0; }
    else if( mTempAmbi <= 8.0f )    { Cold.RegionAmbi = REGION_AMBIENT_1; }
    else if( mTempAmbi <= 12.0f )   { Cold.RegionAmbi = REGION_AMBIENT_2; }
    else if( mTempAmbi <= 16.0f )   { Cold.RegionAmbi = REGION_AMBIENT_3; }
    else if( mTempAmbi <= 20.0f )   { Cold.RegionAmbi = REGION_AMBIENT_4; }
    else if( mTempAmbi <= 24.0f )   { Cold.RegionAmbi = REGION_AMBIENT_5; }
    else if( mTempAmbi <= 28.0f )   { Cold.RegionAmbi = REGION_AMBIENT_6; }
    else if( mTempAmbi <= 32.0f )   { Cold.RegionAmbi = REGION_AMBIENT_7; }
    else if( mTempAmbi <= 36.0f )   { Cold.RegionAmbi = REGION_AMBIENT_8; }
    else if( mTempAmbi <= 40.0f )   { Cold.RegionAmbi = REGION_AMBIENT_9; }
    else { Cold.RegionAmbi = REGION_AMBIENT_10; }

    pColdMake = &ColdMakeTableList[ Cold.StartingMode ][ Cold.RegionAmbi ];
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


void  MakeColdWater(void)
{
    U8 mu8Make = FALSE;


    UpdateExtraMakeTime();

    UpdateColdMakeData();

    CheckInitStartingModeByColdAmount();

    // Get Current Make Status
    mu8Make = GetColdWaterMake();

    // Get Temperture
    Cold.TempTargetOn  = GetTargetOnTemp();
    Cold.TempTargetOff = GetTargetOffTemp();
    Cold.TempCurrent   = GetTemp( TEMP_ID_COLD_WATER );


    if( Cold.ExtraMake == TRUE )
    {
        // 추가 냉각 중이면, 일반 냉각 동작은 OFF로 변경
        mu8Make = FALSE;
    }

    if( mu8Make == TRUE )
    {
        mu8Make = CheckStopMake();
    }
    else
    {
        mu8Make = CheckStartMake();
    }

    // Extra Make Cold 
    if( Cold.ExtraMake == TRUE )
    {
        if( Cold.StartingMode != MODE_INIT_STARTING )
        {
            Cold.ExtraMakeTime = ProtectFreezingExtraTime( Cold.ExtraMakeTime );
        }

        if( Cold.ExtraMakeTime != 0 )
        {
            mu8Make = TRUE;
        }
        else
        {
            Cold.ExtraMake = FALSE;
        }
    }

    // Comp rps
    if( mu8Make == TRUE )
    {
        Cold.TargetRps = CheckCompRps();
    }

    // Protectino Off
    //if( Cold.TempCurrent <= PROTECT_COLD_TEMP )
    //{
    //    mu8Make = FALSE;
    //    Cold.ExtraMakeTime = 0;
    //}

    // Config Off
    if( IsValidMake() == FALSE )
    {
        mu8Make = FALSE;
        Cold.ExtraMakeTime = 0;
    }

    // Set new make status
    SetColdWaterMake( mu8Make );
}
