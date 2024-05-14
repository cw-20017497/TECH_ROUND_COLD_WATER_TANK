#include "hw.h"
#include "flush_water_out.h"
#include "valve.h"
#include "flow_meter.h"
#include "user.h"
#include "level.h"
#include "eeprom.h"
#include "front.h"
#include "water_out_common.h"
#include "hot_water.h"
#include "hot_water_out.h"
#include "hot_water_heater.h"
#include "heater_out.h"
#include "flow_valve.h"
#include "WIFI_Control.h"
#include "sound.h"
#include "drain_pump.h"
#include "cold_water.h"
#include "error.h"
#include "filter.h"


// 저유량 조건 상관 없음.
// 플러싱은 3분
//


FlushOut_T   FlushOut;


static void InitOutEventList(void);
static void UpdateFlushCurrentAmount(void);
static U32 GetFlushFindOutTargetTime( WaterOutType_T mType);


#if 0   
// 필터 플러싱 목표 시간... @10ms..
#define FLUSH_AMOUNT_FILTER       18000UL // ( 3min * 60ses * 100 )  // 필터 플러싱 시간 + 온수 채움 시간
#define FLUSH_AMOUNT_HEATER        3000UL // ( 30sec * 100 )         // 온수 탱크 채움 시간 
#define FLUSH_AMOUNT_COOLER        6000UL // ( 1min * 60sec * 100 )  // 냉수 탱크 채움 시간
#endif

#define VALVE_DELAY_TIME            10U

// 필터 플러싱 최대 동작 시간
#define MAX_FILTER_TIME_OUT         ( 5U * 60U * 100U ) // 5min * 60sec * 100ms 

// 냉수 탱크 채우기 최대 동작 시간
#define MAX_COLD_TIME_OUT           ( 3U * 60U * 100U ) // 3min * 60sec * 100ms 

#define MAX_FLUSH_COLD_TIME_OUT     ( 5U * 60U * 100U ) // 5min * 60sec * 100ms 

// 잠깐 완료 대기 
#define MAX_DONE_WAIT_TIME          ( 1U * 100U )      // 1sec * 100ms 


const static WaterOutTable_T FlushTableList[] = 
{
    { OUT_AMOUNT_FLUSH_FILTER,          AMOUNT_3000  },
    //{ OUT_AMOUNT_FLUSH_HEATER,          AMOUNT_250   },
    { OUT_AMOUNT_FLUSH_HEATER,          AMOUNT_500   },
    { OUT_AMOUNT_FLUSH_COOLER,          AMOUNT_1000  },
    { OUT_AMOUNT_FLUSH_COOLER_DRAIN,    AMOUNT_5000 },
    { OUT_AMOUNT_FLUSH_CHECK_HEATER,    AMOUNT_500   },
    { OUT_AMOUNT_CHECK_HEATER,          AMOUNT_120   }
};

#define SZ_FLUSH_TABLE   (sizeof(FlushTableList)/sizeof(WaterOutTable_T))
#define DEFAULT_AMOUNT  0UL

// 용량별 추출 시간을 반환한다.
static U32 GetFlushFindOutTargetTime( WaterOutType_T mType)
{
    U8 i;

    for( i = 0; i < SZ_FLUSH_TABLE ; i++ )
    {
        if( FlushTableList[ i ].Type == mType )
        {
            return FlushTableList[ i ].Amount;
        }
    }

    return DEFAULT_AMOUNT;
}


void InitFlushOut(void)
{
    //FlushOut.SetupInit = FLUSH_STATUS_INIT;     // 초기 설치
    //FlushOut.Status    = FLUSH_STATUS_INIT;     // 필터 교체 
    //FlushOut.PowerOn   = FLUSH_STATUS_INIT;     // 전원드레인 탱크 에러 발생을 위해서는  리셋
    //FlushOut.SetupInit = FLUSH_STATUS_DONE;     // 초기 설치
    //FlushOut.Status    = FLUSH_STATUS_DONE;     // 필터 교체 
    //FlushOut.PowerOn   = FLUSH_STATUS_DONE;     // 전원드레인 탱크 에러 발생을 위해서는  리셋
    FlushOut.SetupInit = FLUSH_STATUS_DONE;     // 초기 설치
    FlushOut.Status    = FLUSH_STATUS_DONE;     // 필터 교체 
    FlushOut.PowerOn   = FLUSH_STATUS_INIT;     // 전원드레인 탱크 에러 발생을 위해서는  리셋
    FlushOut.Out       = FALSE;
    FlushOut.PrevOut   = FALSE;

    FlushOut.TargetAmount       = 0UL;
    FlushOut.FilterTargetAmount = 0UL;
    FlushOut.FlushColdTargetAmount = 0UL;
    FlushOut.CurrentAmount      = 0UL;

    FlushOut.Mode = 0U;
}


// FILTER SETUP FLUSHING STATUS
void SetFlushSetupInit(FlushStatus_T mu8Status)
{
    FlushOut.SetupInit = mu8Status;
}

FlushStatus_T GetFlushSetupInit(void)
{
    return FlushOut.SetupInit;
}


// FILTER FLUSHING STATUS
void SetFlushStatus(FlushStatus_T mu8Status)
{
    FlushOut.Status = mu8Status;
}

FlushStatus_T GetFlushStatus(void)
{
    return FlushOut.Status;
}


// POWER ON FLUSHING STATUS
void SetFlushPowerOn(FlushStatus_T mu8Status)
{
    FlushOut.PowerOn = mu8Status;
}

FlushStatus_T GetFlushPowerOn(void)
{
    return FlushOut.PowerOn;
}


U32 GetFlushCurrentAmount(void)
{
    return FlushOut.CurrentAmount;
}

void SetFlushCurrentAmount(U32 mu32Amount)
{
    FlushOut.CurrentAmount = mu32Amount;
}

// 초기 설치 및 필터 플러싱
U8 IsDoneFlushStatus(void)
{
    if( GetFlushStatus() == FLUSH_STATUS_DONE
            && GetFlushSetupInit() == FLUSH_STATUS_DONE 
      )
    {
        return TRUE;
    }

    return FALSE;
}


// 초기 설치 & 필터 플러싱 & 초기화 배수
U8 IsDoneFlushInitSetup(void)
{
    if( IsDoneFlushStatus() == TRUE 
            && GetFlushPowerOn() == FLUSH_STATUS_DONE )
    {
        return TRUE;
    }

    return FALSE;
}

void ResetFlushWaterOut(void)
{
    FlushOut.Status = FLUSH_STATUS_INIT;
    FlushOut.CurrentAmount = 0UL;

    SaveEepromId( EEP_ID_FLUSH_STATUS );
}

// 상세 제어 모드 
void SetFlushOutMode( U16 mu16Mode )
{
    FlushOut.Mode |= mu16Mode;
}

void ClearFlushOutMode( U16 mu16Mode )
{
    FlushOut.Mode &= ~mu16Mode;
}

U8 IsSetFlushOutMode(U16 mu8Mode )
{
    if( (FlushOut.Mode & mu8Mode ) == mu8Mode )
    {
        return TRUE;
    }

    return FALSE;
}


U16 GetFlushOutMode(void )
{
    return FlushOut.Mode;
}


#if 0
void TurnOnFlushOut(WaterOutType_T mType)
{
    (void)mType;

    FlushOut.Out          = TRUE;

    // 필터 플러싱 목표량(유량 센서) 계산
    // 필터 플러싱 + 온수 히터 채우기 + 냉수 탱크 채우기
    FlushOut.TargetAmount = GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_FILTER );
    FlushOut.TargetAmount += GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_HEATER );

    // 최초 설치시에만 냉수 채우기 동작
    if( FlushOut.SetupInit == FLUSH_STATUS_INIT )
    {
        FlushOut.TargetAmount += GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_COOLER );
    }

    // 필터 플러싱 + 온수 히터 채우기 시간 계산
    FlushOut.FilterTargetAmount = GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_FILTER );
    FlushOut.FilterTargetAmount += GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_HEATER );
}
#endif
void TurnOnFlushOut(WaterOutType_T mType)
{
    (void)mType;

    FlushOut.Out          = TRUE;

    if( FlushOut.SetupInit == FLUSH_STATUS_INIT )
    {
        // 필터 플러싱 + 온수 히터 채우기 + 냉수 탱크 채우기
        FlushOut.TargetAmount = GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_FILTER );
        FlushOut.TargetAmount += GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_HEATER );
        FlushOut.TargetAmount += GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_COOLER );
        FlushOut.TargetAmount += GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_COOLER_DRAIN );

        // 필터 플러싱 + 온수 히터 채우기 시간 계산
        FlushOut.FilterTargetAmount = GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_FILTER );
        FlushOut.FilterTargetAmount += GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_HEATER );

        // 필터 플러싱 + 온수 히터 + 냉수 채우기 시간 계산
        FlushOut.FlushColdTargetAmount = GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_FILTER );
        FlushOut.FlushColdTargetAmount += GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_HEATER );
        FlushOut.FlushColdTargetAmount += GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_COOLER );
    }
    else if( FlushOut.Status == FLUSH_STATUS_INIT )
    {
        // 필터 플러싱 + 온수 히터 채우기 + 냉수 탱크 채우기
        FlushOut.TargetAmount = GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_FILTER );
        FlushOut.TargetAmount += GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_HEATER );
        FlushOut.TargetAmount += GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_COOLER );

        // 필터 플러싱 + 온수 히터 채우기 시간 계산
        FlushOut.FilterTargetAmount = GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_FILTER );
        FlushOut.FilterTargetAmount += GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_HEATER );
    }
    else 
    {
        if( GetSystem() == SYSTEM_CHP )
        {
            FlushOut.TargetAmount = GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_HEATER );
            FlushOut.TargetAmount += GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_COOLER );

            FlushOut.FilterTargetAmount = GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_HEATER );
        }
        else
        {
            FlushOut.TargetAmount = GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_COOLER );
        }
    }
}


U8 IsFlushOut(void)
{
    return FlushOut.Out;
}


void TurnOffFlushOut(void)
{
    FlushOut.Out = FALSE;
}


static U8 IsChangedOutStatus(void)
{
    if( FlushOut.Out != FlushOut.PrevOut )
    {
        FlushOut.PrevOut = FlushOut.Out;

        return TRUE;
    }

    return FALSE;
}

static void StartFlushOut(void)
{
    if( FlushOut.SetupInit == FLUSH_STATUS_DONE 
            && FlushOut.Status == FLUSH_STATUS_DONE )
    {
        // POWER ON
        if( GetSystem() == SYSTEM_CHP )
        {
            SetFlushOutMode( POWER_ON_CHP );
        }
        else
        {
            SetFlushOutMode( POWER_ON_CP );
        }
    }
    else
    {
        // FLUSH SETUP or FILTER
        if( GetSystem() == SYSTEM_CHP )
        {
            SetFlushOutMode( FLUSH_OUT_CHP );
        }
        else
        {
            SetFlushOutMode( FLUSH_OUT_CP );
        }

        // 필터 교체인 경우 wifi paring, 냉수 탱크 플러싱 삭제
        if( FlushOut.SetupInit == FLUSH_STATUS_DONE )
        {
            ClearFlushOutMode( FLUSH_OUT_EXCEPT_SETUP_INIT );
            SetFlushOutMode( FLUSH_OUT_COLD );
        }
    }
}


static void StopFlushOut(void)
{
    ClearFlushOutMode( FLUSH_OUT_CANCEL );
    SetFlushOutMode( FLUSH_OUT_CLOSE_VALVE );
}



typedef U8 (*DoAction_T)(U8 *pStep);
typedef struct _flush_out_event_
{
    U16 Mode;               // 출수 동작 상세 제어 모드
    U8  Step;               // 이벤트 헨들러 스텝 변수
    DoAction_T DoAction;    // 제어 모드에 대한 이벤트 핸들러
} FlushOutEvent_T;

static U8 DoParingWifi(U8 *pStep);
static U8 DoFilterFlushing_CHP(U8 *pStep);
static U8 DoFilterFlushing_CP(U8 *pStep);
static U8 DoFillHotWater(U8 *pStep);
static U8 DoFillColdWater(U8 *pStep);
static U8 DoFillColdFlushWater(U8 *pStep);
static U8 DoCloseValves(U8 *pStep);
static U8 DoDone(U8 *pStep);
static FlushOutEvent_T OutEventList[] = 
{
    { FLUSH_OUT_PARING_WIFI,    0U, DoParingWifi         },
    { FLUSH_OUT_FILTER,         0U, DoFilterFlushing_CHP },     // FILTER + HEATER ( HOT DRAIN )
    { FLUSH_OUT_FILTER_CP,      0U, DoFilterFlushing_CP  },     // FILTER + HEATER ( ROOM OUT )
    { FLUSH_OUT_HEATER,         0U, DoFillHotWater       },
    { FLUSH_OUT_COLD,           0U, DoFillColdWater      },     // FILLING COLD
    { FLUSH_OUT_FLUSH_COLD,     0U, DoFillColdFlushWater },     // FILLING COLD & FLUSH COLD
    { FLUSH_OUT_CLOSE_VALVE,    0U, DoCloseValves        },
    { FLUSH_OUT_DONE,           0U, DoDone               }
};
#define SZ_FLUSH_OUT_EVENT_LIST  (sizeof(OutEventList)/sizeof(FlushOutEvent_T))

static void InitOutEventList(void)
{
    U8 i;

    for( i = 0; i < SZ_FLUSH_OUT_EVENT_LIST ; i++ )
    {
        OutEventList[i].Step = 0U;
    }
}

static void Outflow(void)
{
    U8 i;
    U8 mu8Ret = FALSE;
    FlushOutEvent_T *pList = NULL;

    for( i = 0; i < SZ_FLUSH_OUT_EVENT_LIST ; i++ )
    {
        pList = &OutEventList[ i ];

        if( IsSetFlushOutMode( pList->Mode ) == TRUE )
        {
            if( pList->DoAction != NULL )
            {
                mu8Ret = pList->DoAction( &pList->Step );
                if( mu8Ret == TRUE )
                {
                    ClearFlushOutMode( pList->Mode );
                }
                break;
            }
        }
    }
}

static void SoundFlushingDone(void)
{
    if( GetFlushSetupInit() == FALSE )
    {
        // 설치 플러싱 종료음
        Sound( SOUND_SETUP_FLUSHED );
    }
    else if( GetFlushStatus() == FALSE )
    {
        // 필터 플러싱 종료음
        Sound( SOUND_FILTER_FLUSHED );
    }
    else
    {
        // 온수 채우기 종료음
        if( GetFotaReboot() == TRUE )
        {
            SetFotaReboot( FALSE );
        }
        else
        {
            Sound( SOUND_EFFLUENT_END );
        }
    }
}

// 페어링 시작
static U8 DoParingWifi(U8 *pStep)
{
    // WIFI 페이렁 모드 시작
    WifiKey(WIFI_KEY_BLE);

    // 초기 플러그인 출력
    Sound( SOUND_SETUP_FLUSHING );

    return TRUE;
}

enum
{
    FLUSH_OPEN_VALVE,
    FLUSH_CHECK_OUTAGE,
    FLUSH_FILLING,
    FLUSH_DONE
};

// 필터 플러싱 + 온수 히터 채우기 3분...
#define CHECK_HEATER_WATT_TIME   300U    // 3sec
static U8 DoFilterFlushing_CHP(U8 *pStep)
{
    static U16 mu16Time = 0U; // @10ms
    U32  mAmount;


    switch( *pStep )
    {
        case FLUSH_OPEN_VALVE:
            // open valve... hot water drain 
            OpenValve( VALVE_HOT_IN );
            OpenValve( VALVE_HOT_DRAIN );
            CloseValve( VALVE_HOT_OUT );

            mu16Time = VALVE_DELAY_TIME;
            (*pStep)++;
            break;

        case FLUSH_CHECK_OUTAGE:
            // Check outage..
            // 0.1초 대기...
            if( mu16Time > 0 )
            {
                mu16Time--;
            }
            else
            {
                FlushOut.FilterTimeOut  = MAX_FILTER_TIME_OUT;

                RegisterFlowMeterId( FLOW_ID_WATER, UpdateFlushCurrentAmount );
                StartCheckOutage( OUTAGE_FLUSHING_DELAY_TIME, OUTAGE_CHECK_TIME_FLUSHING, TYPE_HOT_WATER  );

                FlowValveInit();

                mu16Time = CHECK_HEATER_WATT_TIME;
                (*pStep)++;
            }
            break;


        case FLUSH_FILLING:
            if( FlushOut.FilterTimeOut != 0 )
            {
                FlushOut.FilterTimeOut--;
            }

            // 일정 유량이 흐르면, 히터 전력 및 필요 유량 계산
            mAmount = GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_CHECK_HEATER );
            if( FlushOut.CurrentAmount >= mAmount )
            {
                if( mu16Time != 0 )
                {
                    mu16Time--;
                    SetHeaterOutput( 100 );
                }
                else
                {
                    SetHeaterOutput( 0 );
                }

                SetHotWaterInitFull( TRUE );
            }
            CheckHeaterWatt( GetHeaterOutput() );

            // done filter flush and fill heater 
            if( FlushOut.FilterTimeOut == 0 )
            {
                // 1. 최대 동작 시간 타임 아웃
                FlushOut.CurrentAmount = GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_FILTER );
                FlushOut.CurrentAmount += GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_HEATER );

                SetHeaterOutput( 0 );
                (*pStep)++;
            }
            else if( FlushOut.CurrentAmount >= FlushOut.FilterTargetAmount )
            {
                // 2. 2. 목표 유량 도달 
                SetHeaterOutput( 0 );
                (*pStep)++;
            }

            break;

        case FLUSH_DONE:
            SetHeaterOutput( 0 );

            UpdateRegionTempIn();
            StopCheckOutage();

            ResetFilterUsage();     // 필터 사용 시간 초기화
            FlowValveTarget( DEFAULT_FLOW_TARGET );
            CloseValve( VALVE_HOT_IN );
            (*pStep) = 0;
            return TRUE;
            

        default:
            (*pStep) = 0;

            break;
    }

    return FALSE;
}

static U8 DoFilterFlushing_CP(U8 *pStep)
{
    static U16 mu16Time = 0U; // @10ms


    switch( *pStep )
    {
        case FLUSH_OPEN_VALVE:
            OpenValve( VALVE_COLD_FLUSH );

            mu16Time = VALVE_DELAY_TIME;
            (*pStep)++;
            break;

        case FLUSH_CHECK_OUTAGE:
            // Check outage..
            // 0.1초 대기...
            if( mu16Time > 0 )
            {
                mu16Time--;
            }
            else
            {
                FlushOut.FilterTimeOut  = MAX_FILTER_TIME_OUT;

                RegisterFlowMeterId( FLOW_ID_WATER, UpdateFlushCurrentAmount );
                StartCheckOutage( OUTAGE_FLUSHING_DELAY_TIME, OUTAGE_CHECK_TIME_FLUSHING, TYPE_WATER );

                (*pStep)++;
            }
            break;


        case FLUSH_FILLING:
            if( FlushOut.FilterTimeOut != 0 )
            {
                FlushOut.FilterTimeOut--;
            }

            // done filter flush and fill heater 
            if( FlushOut.FilterTimeOut == 0 )
            {
                // 1. 최대 동작 시간 타임 아웃
                FlushOut.CurrentAmount = GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_FILTER );
                FlushOut.CurrentAmount += GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_HEATER );

                (*pStep)++;
            }
            else if( FlushOut.CurrentAmount >= FlushOut.FilterTargetAmount )
            {
                // 2. 2. 목표 유량 도달 
                (*pStep)++;
            }

            break;

        case FLUSH_DONE:
            CloseValve( VALVE_COLD_FLUSH );
            StopCheckOutage();
            ResetFilterUsage();     // 필터 사용 시간 초기화
            (*pStep) = 0;
            return TRUE;
            

        default:
            (*pStep) = 0;

            break;
    }

    return FALSE;
}

enum
{
    HOT_OPEN_VALVE,
    HOT_CHECK_OUTAGE,
    HOT_FILLING,
    HOT_DONE
};

static U8 DoFillHotWater(U8 *pStep)
{
    static U16 mu16Time = 0U; // @10ms
    U32  mAmount;


    switch( *pStep )
    {
        case HOT_OPEN_VALVE:
            // open valve... hot water drain 
            OpenValve( VALVE_HOT_IN );
            OpenValve( VALVE_HOT_DRAIN );
            CloseValve( VALVE_HOT_OUT );

            InitCheckHeaterWatt();

            mu16Time = VALVE_DELAY_TIME;
            (*pStep)++;
            break;

        case HOT_CHECK_OUTAGE:
            // Check outage..
            // 0.1초 대기...
            if( mu16Time > 0 )
            {
                mu16Time--;
            }
            else
            {
                FlushOut.FilterTimeOut  = MAX_FILTER_TIME_OUT;

                RegisterFlowMeterId( FLOW_ID_WATER, UpdateFlushCurrentAmount );
                StartCheckOutage( OUTAGE_FLUSHING_DELAY_TIME, OUTAGE_CHECK_TIME_10SEC, TYPE_HOT_WATER  );

                FlowValveInit();
                mu16Time = CHECK_HEATER_WATT_TIME;

                (*pStep)++;
            }
            break;


        case HOT_FILLING:
            if( FlushOut.FilterTimeOut != 0 )
            {
                FlushOut.FilterTimeOut--;
            }

            // 일정 유량이 흐르면, 히터 전력 및 필요 유량 계산
            mAmount = GetFlushFindOutTargetTime( OUT_AMOUNT_CHECK_HEATER );
            if( FlushOut.CurrentAmount >= mAmount )
            {
                if( mu16Time != 0 )
                {
                    mu16Time--;
                    SetHeaterOutput( 100 );
                }
                else
                {
                    SetHeaterOutput( 0 );
                }

                SetHotWaterInitFull( TRUE );
            }
            CheckHeaterWatt( GetHeaterOutput() );

            // done filter flush and fill heater 
            if( FlushOut.FilterTimeOut == 0 )
            {
                // 1. 최대 동작 시간 타임 아웃
                FlushOut.CurrentAmount = GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_HEATER );

                SetHeaterOutput( 0 );
                (*pStep)++;
            }
            else if( FlushOut.CurrentAmount >= FlushOut.FilterTargetAmount )
            {
                // 2. 2. 목표 유량 도달 
                SetHeaterOutput( 0 );
                (*pStep)++;
            }

            break;

        case HOT_DONE:
            StopCheckOutage();

            SetHeaterOutput( 0 );
            UpdateRegionTempIn();

            FlowValveTarget( DEFAULT_FLOW_TARGET );
            CloseValve( VALVE_HOT_IN );
            (*pStep) = 0;
            return TRUE;
            

        default:
            (*pStep) = 0;

            break;
    }

    return FALSE;
}



enum
{
    N_COLD_CHECK_WAIT,
    N_COLD_CHECK_LEVEL,
    N_COLD_WAIT_DRAIN_LEVEL,
    N_COLD_OPEN_VALVE,
    N_COLD_CHECK_OUTAGE,
    N_COLD_FILLING,
    N_COLD_LAZY_DONE,
    N_COLD_DONE
};


static void CloseFillColdWater(void)
{
    // INIT VALVES...
    StopCheckOutage();

    SetFlushOutMode( FLUSH_OUT_CLOSE_VALVE );

    UpdateHotOutLPM( GetFlowMeterLPM() );
    SetColdWaterInitFull( TRUE );

    CloseValve( VALVE_COLD_IN );
}

static U8 DoFillColdWater(U8 *pStep)
{
    static U16 mu16Time = 0U; // @10ms


    switch( *pStep )
    {
        case N_COLD_CHECK_WAIT:
            // 수위 센서 감지 지연 대기 시간
            mu16Time = 400U;    // 4sec..

            (*pStep)++;
            break;

        case N_COLD_CHECK_LEVEL:
            if( mu16Time != 0 )
            {
                mu16Time--;
                break;
            }

            // 배수 수위 검사 ( 배수 동작 ) 
            if( GetTankLevel( LEVEL_ID_DRAIN ) != LEVEL_LOW )
            {
                TurnOnDrainPump();
            }
            else 
            {
                CloseValve( VALVE_COLD_OUT );
                OpenValve( VALVE_COLD_AIR );
            }

            (*pStep)++;
            break;

        case N_COLD_WAIT_DRAIN_LEVEL:
            // 배수 탱크 수위가 low가 될때까지 대기
            if( GetDrainPumpStatus() == OFF 
                    || GetTankLevel( LEVEL_ID_DRAIN ) == LEVEL_LOW )
            {
                (*pStep)++;
            }
            break;

        case N_COLD_OPEN_VALVE:
            // fill cold water..
            OpenValve( VALVE_COLD_IN );
            CloseValve( VALVE_COLD_DRAIN );

            mu16Time = 10U;    // 100ms
            (*pStep)++;
            break;

        case N_COLD_CHECK_OUTAGE:
            if( mu16Time > 0 )
            {
                mu16Time--;
            }
            else
            {
                FlushOut.ColdTimeOut    = MAX_COLD_TIME_OUT;

                RegisterFlowMeterId( FLOW_ID_WATER, UpdateFlushCurrentAmount );
                StartCheckOutage( OUTAGE_FLUSHING_DELAY_TIME, OUTAGE_CHECK_TIME_10SEC, TYPE_WATER );

                (*pStep)++;
            }
            break;

        case N_COLD_FILLING:
            if( FlushOut.ColdTimeOut != 0 )
            {
                FlushOut.ColdTimeOut--;
            }

            // 냉수 탱크 만수위이면 재정수 종료
            if( GetTankLevel(LEVEL_ID_DRAIN) != LEVEL_LOW 
                    || FlushOut.ColdTimeOut == 0 )
            {
                CloseFillColdWater();
                (*pStep)++;
            }
            else if( FlushOut.TargetAmount <= FlushOut.CurrentAmount )
            {
                (*pStep) = N_COLD_DONE;
            }

            break;


        case N_COLD_LAZY_DONE:
             // 즉시 종료하지 않고,CurrentAmount를 100%로 채우고 종료함.

            FlushOut.CurrentAmount += 10;
            if( FlushOut.TargetAmount <= FlushOut.CurrentAmount )
            {
                (*pStep)++;
            }
            break;

        case N_COLD_DONE:
            CloseFillColdWater();

            (*pStep) = 0;
            return TRUE;

            

        default:
            (*pStep) = 0;

            break;
    }

    return FALSE;
}

enum
{
    // 냉수 탱크 채우기
    N_FLUSH_COLD_CHECK_WAIT,      
    N_FLUSH_COLD_CHECK_LEVEL,     
    N_FLUSH_COLD_WAIT_DRAIN_LEVEL,
    N_FLUSH_COLD_OPEN_VALVE,
    N_FLUSH_COLD_CHECK_OUTAGE,
    N_FLUSH_COLD_FILLING,

    // 냉수 탱크 플러싱
    N_FLUSH_COLD_READY,      // 배수 준비
    N_FLUSH_COLD,
    N_FLUSH_COLD_COLD,
    N_FLUSH_COLD_CLOSE_DRAIN,
    N_FLUSH_COLD_LAZY_DONE,
    N_FLUSH_COLD_DONE,
};

// 냉수 채우기 & 냉수 탱크 플러싱
static U8 DoFillColdFlushWater(U8 *pStep)
{
    /*살균 배수동작 Step*/
    static U16 mu16Time = 0;
    U32 mAmount = 0;


    switch( *pStep )
    {
        case N_FLUSH_COLD_CHECK_WAIT:
            // 수위 센서 감지 지연 대기 시간
            mu16Time = 400U;    // 4sec..

            (*pStep)++;
            break;

        case N_FLUSH_COLD_CHECK_LEVEL:
            if( mu16Time != 0 )
            {
                mu16Time--;
                break;
            }

            // 배수 수위 검사 ( 배수 동작 ) 
            if( GetTankLevel( LEVEL_ID_DRAIN ) != LEVEL_LOW )
            {
                TurnOnDrainPump();
            }
            else 
            {
                CloseValve( VALVE_COLD_OUT );
                OpenValve( VALVE_COLD_AIR );
            }

            (*pStep)++;
            break;

        case N_FLUSH_COLD_WAIT_DRAIN_LEVEL:
            // 배수 탱크 수위가 low가 될때까지 대기
            if( GetDrainPumpStatus() == OFF 
                    || GetTankLevel( LEVEL_ID_DRAIN ) == LEVEL_LOW )
            {
                (*pStep)++;
            }
            break;

        case N_FLUSH_COLD_OPEN_VALVE:
            // fill cold water..
            OpenValve( VALVE_COLD_IN );
            CloseValve( VALVE_COLD_DRAIN );

            mu16Time = 10U;    // 100ms
            (*pStep)++;
            break;

        case N_FLUSH_COLD_CHECK_OUTAGE:
            if( mu16Time > 0 )
            {
                mu16Time--;
            }
            else
            {
                FlushOut.ColdTimeOut    = MAX_COLD_TIME_OUT;

                RegisterFlowMeterId( FLOW_ID_WATER, UpdateFlushCurrentAmount );
                StartCheckOutage( OUTAGE_FLUSHING_DELAY_TIME, OUTAGE_CHECK_TIME_10SEC, TYPE_WATER );

                (*pStep)++;
            }
            break;

        case N_FLUSH_COLD_FILLING:
            if( FlushOut.ColdTimeOut != 0 )
            {
                FlushOut.ColdTimeOut--;
            }

            mAmount = GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_FILTER );
            mAmount += GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_HEATER );
            mAmount += GetFlushFindOutTargetTime( OUT_AMOUNT_FLUSH_COOLER );

            // 냉수 탱크 만수위이면 재정수 종료
            if( GetTankLevel(LEVEL_ID_DRAIN) != LEVEL_LOW 
                    || FlushOut.ColdTimeOut == 0  )
            {
                // 냉수 입수 타임 아웃..
                FlushOut.CurrentAmount = mAmount;       
                (*pStep)++;
            }
            else if(  FlushOut.CurrentAmount >= mAmount )
            {
                (*pStep)++;
            }
            break;

            ///////////// FLUSH COLD WATER 
        case N_FLUSH_COLD_READY:
            // open drain valve
            OpenValve( VALVE_COLD_DRAIN );

            // 단수 지연 적용 ( 유로 변경 )
            FlushOut.ColdTimeOut = MAX_FLUSH_COLD_TIME_OUT;

            mu16Time = WAIT_200MS;
            (*pStep)++;
            break;

        case N_FLUSH_COLD:
            // close air valve
            if( mu16Time != 0 )
            {
                mu16Time--;
            }
            else
            {
                StartCheckOutage( OUTAGE_FLUSHING_DELAY_TIME, OUTAGE_CHECK_TIME_60SEC, TYPE_WATER );

                CloseValve( VALVE_COLD_AIR );
                OpenValve( VALVE_COLD_IN );
                (*pStep)++;
            }
            break;

        case N_FLUSH_COLD_COLD:
            if( FlushOut.ColdTimeOut != 0 )
            {
                FlushOut.ColdTimeOut--;
            }

            // 냉수 탱크 비우기
            CloseValve( VALVE_COLD_AIR );
            CloseValve( VALVE_COLD_OUT );

            OpenValve( VALVE_COLD_DRAIN );
            OpenValve( VALVE_COLD_IN );

            // 냉수 탱크 만수위이면 재정수 종료
            if( FlushOut.ColdTimeOut == 0 )
            {
                // 에어 밸브 먼저 OPEN하고 지연 시간 적용 후, COLD DRAIN 밸브를 CLOSE
                OpenValve( VALVE_COLD_AIR );
                mu16Time = 200; // 200ms...

                (*pStep)++;
            }
            else if( FlushOut.TargetAmount <= FlushOut.CurrentAmount )
            {
                OpenValve( VALVE_COLD_AIR );
                mu16Time = 200; // 200ms...

                (*pStep)++;
            }

            break;
            
        case  N_FLUSH_COLD_CLOSE_DRAIN:
            if( mu16Time != 0 )
            {
                mu16Time--;
                break;
            }

            if( FlushOut.ColdTimeOut == 0 )
            {
                CloseValve( VALVE_COLD_DRAIN );
                CloseFillColdWater();
                (*pStep)++;
            }
            else
            {
                CloseValve( VALVE_COLD_DRAIN );
                (*pStep) = N_FLUSH_COLD_DONE;
            }

            break;

        case N_FLUSH_COLD_LAZY_DONE:
             // 즉시 종료하지 않고,CurrentAmount를 100%로 채우고 종료함.

            FlushOut.CurrentAmount += 50;
            if( FlushOut.TargetAmount <= FlushOut.CurrentAmount )
            {
                (*pStep)++;
            }
            break;

        case N_FLUSH_COLD_DONE:
            CloseFillColdWater();

            (*pStep) = 0;
            return TRUE;

        default:
            (*pStep) = 0;
            break;
    }


    return FALSE;
}

enum
{
    CLOSE_INIT,
    CLOSE_VALVE,
    CLOSE_WAIT
};

static U8 DoCloseValves(U8 *pStep)
{
    switch( *pStep )
    {
        case CLOSE_INIT:
            FlushOut.DoneWaitTime = MAX_DONE_WAIT_TIME;

            (*pStep)++;
            break;

        case CLOSE_VALVE:
            // Close All Valvaes..
            CloseValve( VALVE_HOT_IN );
            OpenValve( VALVE_HOT_DRAIN );

            OpenValve( VALVE_COLD_AIR );
            CloseValve( VALVE_COLD_IN );

            CloseValve( VALVE_COLD_FLUSH );   // for CP model...

            CloseValve( VALVE_COLD_DRAIN );

            SetHeaterOutput( 0 );
            // Unreigster flow meter..
            RegisterFlowMeterId( FLOW_ID_WATER, NULL );


            (*pStep)++;
            break;

        case CLOSE_WAIT:
            if( FlushOut.DoneWaitTime != 0 )
            {
                FlushOut.DoneWaitTime--;
            }
            else
            {
                (*pStep) = 0;

                // FLUSING DONE... ( 변수 초기화 )
                if( FlushOut.TargetAmount <= FlushOut.CurrentAmount )
                {
                    SetFlushOutMode( FLUSH_OUT_DONE );

                }
                return TRUE;
            }


            break;

        default:
            (*pStep) = 0;
            break;
    }

    return FALSE;
}



static U8 DoDone(U8 *pStep)
{
    SoundFlushingDone();

    // 필터 플러싱 완료 체크
    if( GetFlushSetupInit() == FLUSH_STATUS_DONE 
            && GetFlushStatus() == FLUSH_STATUS_INIT )
    {
        SetFilterChangedFlushStatus( FILTER_CHANGED_FLUSH );
    }

    // 최초 셋업 완료
    SetFlushSetupInit( FLUSH_STATUS_DONE );
    SaveEepromId( EEP_ID_FLUSH_SETUP_INIT );

    // 플러싱 완료
    SetFlushStatus( FLUSH_STATUS_DONE );
    SaveEepromId( EEP_ID_FLUSH_STATUS );

    // 채우기 완료
    SetFlushPowerOn( FLUSH_STATUS_DONE );

    // Select water room
    SetWaterOutSelect( SEL_WATER_ROOM );
    return TRUE;
}


// 추출량 반환 ( percent )
U8 GetFlushOutPercent(void)
{
    return CalcPercent( FlushOut.TargetAmount, FlushOut.CurrentAmount );
}

// 추출량 ml 단위 반환
U16 GetFlushOutAmount(void)
{
    return CalcLiter( FlushOut.CurrentAmount );
}


static void UpdateFlushCurrentAmount(void)
{
    FlushOut.CurrentAmount++;     
}

static U8 IsDoneFlushOut(void)
{
    // 단수 조건이면 종료한다.
    if( IsErrorType( ERR_TYPE_ROOM_WATER_OUT ) == TRUE )
    {
        return TRUE;
    }

    if( GetSystem() == SYSTEM_CHP )
    {
        if( IsError( ERR_OUTAGE_HOT_WATER ) == TRUE )
        {
            return TRUE;
        }
    }

    return FALSE;
}

// 추출 제어
// RETURN 0x01 : 추출 완료
// 0x00 추출 중...
U8 ControlWaterFlushOut(void)
{
    // 추출 상태 변경
    if( IsChangedOutStatus() == TRUE )
    {
        // 정지 상태이면, 추출 시작
        if( FlushOut.Out == TRUE )
        {
            // 추출 시작
            StartFlushOut();
            InitOutEventList();
        }
        else
        {
            // 추출 중이였으면, 추출 중지
            StopFlushOut();

            StopCheckOutage();
        }
    }


    // 목표 추출량 만큼만 추출하고 종료한다.
    if( FlushOut.Out == TRUE )
    {
        if( IsDoneFlushOut() == TRUE )
        {
            SetFlushCurrentAmount( 0UL );
            TurnOffFlushOut();
            return WATER_OUT_DONE;
        }
        else if( IsSetFlushOutMode( FLUSH_OUT_DONE ) == TRUE )
        {
            TurnOffFlushOut();
            return WATER_OUT_DONE;
        }
    }

    Outflow();
    return WATER_OUT_GOING;
}
