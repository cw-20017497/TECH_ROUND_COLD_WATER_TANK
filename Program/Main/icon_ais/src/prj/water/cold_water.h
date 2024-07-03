#ifndef __COLD_WATER_H__
#define __COLD_WATER_H__

#include "prj_type.h"
#include "level.h"
#include "comp_bldc.h"


/* COLD IN EXTRA FEED TIME WHEN HIGH LEVEL */
#define EXTRA_FEED_TIME    0U /* 0sec @100ms */



typedef struct _cold_water_
{
    U8 InitFull;
    U8 Level;

    /* MAKE */
    U8 ConfigMake;
    U8 Make;

    /* MODE */
    U8 Mode;

    /* TEMP */
    U8      StartingMode;
    U16     StartingModeTime;

    U8      RegionAmbi;         

    TEMP_T  TempTargetOn;
    TEMP_T  TempTargetOff;
    TEMP_T  TempCurrent;
    U8      TempStatus;

    U8      RegionCold;
    RPS_T   TargetRps;

    U8 IsExtraMake;     // 추가 냉각 제한 ( FASLE : STOP, TRUE : RUN )

    U8 ExtraMake;       // 추가 냉각 여부
    U16 ExtraMakeTime;  // 추가 냉각 시간

    U16 AmountTime;     
    U32 Amount;
} ColdWater_T;

// 초기화
void  InitColdWater(void);
void  GetColdWaterData(ColdWater_T *pData);

// 냉수 최초 만수위
void  SetColdWaterInitFull(U8 mu8Val );
U8    GetColdWaterInitFull(void);

// 냉수 수위 
void  SetColdWaterLevel(U8 mu8Val );
U8    GetColdWaterLevel(void);

// 냉수 생성 설정
void  SetColdWaterConfigMake(U8 mu8Val );
U8    GetColdWaterConfigMake(void);

// 냉수 생성 명령
void  SetColdWaterMake(U8 mu8Val );
U8    GetColdWaterMake(void);

// 냉수 Target OFF 온도
TEMP_T GetColdTargetOffTemp(void);
TEMP_T GetColdTargetOnTemp(void);

// 냉수 재기동 조건 초기화
void ResetColdAmount(void);

// 냉수 온도 상태 
#define COLD_STATUS_GOOD     0
#define COLD_STATUS_BAD      1
U8 GetColdWaterTempStatus(void);
void CheckColdWaterTempStatus(void);


#if 0
// 냉수 입수 지연 시간
void  SetColdWaterFeedTime(U16 mu16Time );
U16   GetColdWaterFeedTime(void);
#endif

// 냉수 제어 상세 모드
#define  COLD_MODE_NONE          0x00
#define  COLD_MODE_INIT          0x01
#define  COLD_MODE_COOLING       0x02
#define  COLD_MODE_DONE          0x08
#define  COLD_MODE_ALL           0xFF
void SetColdWaterMakeMode(U8 mu8Mode );
void ClearColdWaterMakeMode(U8 mu8Mode );
U8 IsSetColdWaterMakeMode(U8 mu8Mode );
U8 GetColdWaterMakeMode(void );

RPS_T   GetColdWaterMakeCompRps(void);

// 냉수 탱크 수위 제어
void StartFeedColdWater(void);
void StopFeedColdWater(void);
void  ControlColdWaterLevel(void);

// 냉수 생성 조건 제어
void  MakeColdWater(void);

#endif /* __COLD_WATER_H__ */
