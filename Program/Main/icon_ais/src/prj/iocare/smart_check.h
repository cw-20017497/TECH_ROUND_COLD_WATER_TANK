#ifndef __SMART_CHECK_H__
#define __SMART_CHECK_H__

#include "prj_type.h"
#include "WIFI_SetPartData.h"
#include "WIFI_SetFunctionData.h"


typedef enum _sc_status_
{
    SC_STATUS_NORMAL,
    SC_STATUS_START,
    SC_STATUS_GOING,
    SC_STATUS_COMPLETE,
    SC_STATUS_STOP,
    SC_STATUS_INVALID,
} SC_STATUS_T;


void InitSmartCheck(void);

// return false or true
U8 IsValidStartSmartCheck(void);

void StartSmartCheck(void);
void StopSmartCheck(void);
U8 GetSmartCheck(void);

void SetSmartCheckStatus(SC_STATUS_T mu8Status);
U8 GetSmartCheckStatus(void);

U16 GetSmartCheckStatusId( E_API_A1010_T id );

void ControlSmartCheck(void);
void ControlContinueSmartCheck(void);

#define SMART_CHECK_PROGRESS_LEVEL_0    0
#define SMART_CHECK_PROGRESS_LEVEL_1    1
#define SMART_CHECK_PROGRESS_LEVEL_2    2
#define SMART_CHECK_PROGRESS_LEVEL_3    3
#define SMART_CHECK_PROGRESS_LEVEL_4    4
#define SMART_CHECK_PROGRESS_LEVEL_5    5
void GetSmartCheckProgress(void);
void SetSmartCheckDataId( E_API_A1080_T id, U16 mu16Data );


U8 IsValidPartID( E_API_A1080_T id );
U16 GetSmartCheckDataId( E_API_A1080_T id );

#endif /* __SMART_CHECK_H__ */
