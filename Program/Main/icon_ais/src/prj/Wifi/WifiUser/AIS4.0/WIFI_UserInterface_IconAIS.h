
#ifndef _WIFI_UserInterface_ICON_AIS_H_
#define _WIFI_UserInterface_ICON_AIS_H_


typedef struct _wifi_status_
{
    U8 mUonOff;
    U8 mUfirstParing;
    U8 mUfotaModule;
    U8 mUfotaMain;
} SWifiStatus_T;

typedef enum _status_key_event_id_
{
    STATUS_KEY_EVENT_ID_CONF_COLD = 0U,     // 0 :
    STATUS_KEY_EVENT_ID_LOCK_HOT,           // 1 :
    STATUS_KEY_EVENT_ID_LOCK_ALL,           // 2 :
    STATUS_KEY_EVENT_ID_SOUND,              // 3 :
    STATUS_KEY_EVENT_ID_WATER_SEL,          // 4 :
    STATUS_KEY_EVENT_ID_CONF_AUTO_DRAIN,    // 5 :
    STATUS_KEY_EVENT_ID_CONF_HOT,           // 6 :
    STATUS_KEY_EVENT_ID_HIGH_MODE,          // 7 :
    STATUS_KEY_EVENT_ID_DEFAULT_AMOUNT,     // 8 :
    STATUS_KEY_EVENT_ID_UNIT_AMOUNT,        // 9 :
    STATUS_KEY_EVENT_ID_ANTI_CLEAN,         // 10 :
    STATUS_KEY_EVENT_ID_STER_SETTING_HOUR,  // 11 :
    STATUS_KEY_EVENT_ID_STER_SETTING_MIN,   // 12 :
    STATUS_KEY_EVENT_ID_WATER_OUTQUANTITY,  // 13 :
    STATUS_KEY_EVENT_ID_VOICE_LEVEL,        // 14 :
    STATUS_KEY_EVENT_ID_TEMP_TARGET,        // 15 :
    STATUS_KEY_EVENT_ID_FILTER_SEQ,         // 16 :
} EStatusKeyEventId_T;

void UserWriteEep ( U16 mu8Addr, U8 mu8Val );
U8 UserReadEep ( U16 mu8Addr );
void UserBuzzSound ( U8 mu8Buzz );

U8 GetUserSystemStatus ( U8 mu8Value );

U16 GetUserSystemError ( U16 mu16Err );
U16 GetUserSystemFunction ( U16 mu16Func );
U32 GetUserSystemRamMonitor ( U16 mu16Func, U16 mLen);
I16 GetUserSystemSensor ( U16 mu16Sen );
U16 GetUserSystemPart ( U16 mu16Part );
U32 GetUserSystemAccumulate ( U16 mu16Acc );

void SetUserSystemFunction ( U16 mu16Func, U16 mData );
void SetUserSystemRamMonitor ( U16 mu16Func, I8 *mData );

void ClearUserSystemData ( void );
void SetUserSystemDisplay ( U8 mu8Display );
void SetUserSystemApplyData ( U16 mId );
void SetUserSystemTime ( U16 mYear, U16 mMonth, U16 mDate, U16 mHour, U16 mMin, U16 mSec, U16 mDay );

void SetUserSystemRamWriteStatus ( U16 mu16Address );
U16 GetUserSystemRamWriteStatus ( U16 mu16Address );
void SetKeySetting(  EStatusKeyEventId_T xEid, U8 xUset);
/* Status Progress */
void ProcessChangedStatus(void);
void ProcessKeyChangedStatus(void);

#endif

