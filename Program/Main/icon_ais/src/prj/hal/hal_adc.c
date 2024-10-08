#include "hw.h"
#include "hal_adc.h"
#include "Config_ADC.h"



/* TEMP ON/OFF DELAY TIME */
#define TURN_ON_DELAY       1000UL       // 1000ms @1ms

/* MACRO ADC */
#define ADC_SET_CHANNEL(x)      do{ ADS = (x); }while(0)
#define ADC_START_CONVERSION()  do{ ADCS = 1; }while(0)
#define ADC_STOP_CONVERSION()   do{ ADCS = 0; }while(0)
#define ADC_SAMPLING()             while(!ADIF)
#define ADC_GET_VALUE()         (ADCR >> 6)

typedef void (*Action_T)(void);

typedef struct _adc_config_
{
    U8   id;        // ADC ID
    U8   channel;   // ADC CHANNEL
    U8   resolution;     // ADC resolution

    U8   count;     // SUM COUNTER
    U16  min;       // FILTERING MIN VALUE
    U16  max;       // FILTERING MAX VALUE

    U8          OnOff;          // Channel on/off
    U16         TurnOnDelay;    // Adc Channel On delay
    Action_T    pfEnable;       // Adc Channel On Function
    Action_T    pfDisable;      // Adc Channel Off Function
} TAdcConfig_T;

static void EnableTempCold(void);
#if !CONFIG_TEST_TEMP_READ
static void DisableTempCold(void);
#endif
static void EnableTempRoom(void);
#if !CONFIG_TEST_TEMP_READ
static void DisableTempRoom(void);
#endif

static TAdcConfig_T adcConfList[] = 
{ 
    /* ADC CHANNEL,              CH SUM COUNTER, R_BIT, MAX ,    MIN      On/Off  On Delay        pfEnable            pfDislabe */
    { ANI_TEMP_HOT_OUT,          0,    12, 6U,   4096U,      0U,     ON,     0U,             NULL,               NULL},
    { ANI_TEMP_HEATER,           1,    12, 6U,   4096U,      0U,     ON,     0U,             NULL,               NULL},
                                               
    { ANI_TEMP_AMBIENT,          2,    12, 52U,  4096U,      0U,     ON,     0U,             NULL,               NULL},
#if CONFIG_TEST_TEMP_READ                      
    { ANI_TEMP_ROOM_WATER,       3,    12, 52U,  4096U,      0U,     ON,     TURN_ON_DELAY,  EnableTempRoom,     EnableTempRoom},
    { ANI_TEMP_COLD_WATER,       4,    12, 52U,  4096U,      0U,     ON,     TURN_ON_DELAY,  EnableTempCold,     EnableTempCold},
#else                                          
    { ANI_TEMP_ROOM_WATER,       3,    12, 52U,  4096U,      0U,     ON,     TURN_ON_DELAY,  EnableTempRoom,     EnableTempRoom},
    { ANI_TEMP_COLD_WATER,       4,    12, 52U,  4096U,      0U,     ON,     TURN_ON_DELAY,  EnableTempCold,     DisableTempCold},
#endif                                         
    { ANI_TEMP_EVA_1,            5,    12, 52U,  4096U,      0U,     ON,     0U,             NULL,               NULL},

    { ANI_SENSOR_LEAK,           8,   10, 52U,  1024U,      0U,     ON,     0U,             NULL,               NULL},
    { ANI_SENSOR_INFRARED,       9,   10, 52U,  1024U,      0U,     ON,     0U,             NULL,               NULL},
    { ANI_DC_FAN,               10,   10, 52U,  1024U,      0U,     ON,     0U,             NULL,               NULL},
                                              
    { ANI_TEMP_HOT_IN,          11,   12, 6U,   4096U,      0U,     ON,     0U,             NULL,               NULL},
    { ANI_24V_FB,               12,   10, 52U,  1024U,      0U,     ON,     0U,             NULL,               NULL},
    { ANI_12V_FB,               13,   10, 52U,  1024U,      0U,     ON,     0U,             NULL,               NULL},
                                              
    { ANI_UV_ICE_TANK,          14,   10, 52U,  1024U,      0U,     ON,     0U,             NULL,               NULL},
    { ANI_HEATER_FB,            18,   10, 12U,  1024U,      0U,     ON,     0U,             NULL,               NULL},
    { ANI_UV_ICE_TRAY,          19,   10, 52U,  1024U,      0U,     ON,     0U,             NULL,               NULL},
    { ANI_PUMP_DRAIN_FB,        20,   10, 52U,  1024U,      0U,     ON,     0U,             NULL,               NULL}
}; 
#define SZ_ADC_CONF_LIST    (sizeof(adcConfList)/sizeof(TAdcConfig_T))

static void EnableTempCold(void)
{
    TURN_ON_ADC_COLD();
}

#if !CONFIG_TEST_TEMP_READ
static void DisableTempCold(void)
{
    TURN_OFF_ADC_COLD();
}
#endif


static void EnableTempRoom(void)
{
    TURN_ON_ADC_ROOM();
}

#if !CONFIG_TEST_TEMP_READ
static void DisableTempRoom(void)
{
    TURN_OFF_ADC_ROOM();
}
#endif

// RETURN CONFIG TABLE LIST INDEX
static U8 GetIndexConfList(U8 mu8Id, U8 *pIndex)
{
    U8 i;

    for( i = 0; i < SZ_ADC_CONF_LIST ; i++ )
    {
        if( adcConfList[ i ].id == mu8Id )
        {
            *pIndex = i;
            return TRUE;
        }
    }

    return FALSE;
}

void HAL_TurnOnSensingAdc(U8 mu8Id )
{
    U8 mu8Index;

    if( GetIndexConfList( mu8Id, &mu8Index) == TRUE )
    {
        adcConfList[ mu8Index ].OnOff = ON;
    }

}

void HAL_TurnOffSensingAdc(U8 mu8Id )
{
    U8 mu8Index;

    if( GetIndexConfList( mu8Id, &mu8Index) == TRUE )
    {
        adcConfList[ mu8Index ].OnOff = OFF;
    }
}

typedef struct _sensor_
{
    U32  sum;        
    U16  sum_count; 

    U32  average;  

    U16  min;        
    U16  max;       
} AdcValue_T;
AdcValue_T        adcValue[ ANI_MAX_NUM ];


static U16 SampleAdc( U8 channel, U8 resolution );
static void Sum( void );
static void Average( void );


void HAL_InitAdc(void)
{
    U8 i;
    U8 ch;
    U8 res;

    // Setup inital value...
    for( i = 0; i < SZ_ADC_CONF_LIST ; i++ )
    {
        if( adcConfList[ i ].pfEnable != NULL )
        {
            adcConfList[ i ].pfEnable();
        }

        ch = adcConfList[ i ].channel;
        res = adcConfList[ i ].resolution;
        adcValue[ i ].average = SampleAdc( ch, res );
    }
}

U16 HAL_GetAdcValue( U8 mu8Id )
{
    if( mu8Id >= ANI_MAX_NUM )
    {
        return 0;
    }

    return adcValue[ mu8Id ].average;
}

void HAL_SetAdcValue(U8 mu8Id, U16 mu16Adc )
{
    if( mu8Id < ANI_MAX_NUM )
    {
        adcValue[ mu8Id ].average = mu16Adc;
    }
}


static U16 SampleAdc( U8 channel, U8 resolution )
{
    U16     raw = 0;


    /* Stop */
    ADIF = 0;
    ADC_STOP_CONVERSION();

    /* Set channel */
    R_Config_ADC_Set_ADChannel( (e_ad_channel_t)channel );  //ADC_SET_CHANNEL( channel );

    /* Start adc */
    R_Config_ADC_Set_OperationOn();
    R_Config_ADC_Start();   // ADC_START_CONVERSION();

    /* Drop garbage value */
    ADC_SAMPLING();
    R_Config_ADC_Get_Result_12bit( &raw );

    /* Get real value */
    ADC_SAMPLING();
    R_Config_ADC_Get_Result_12bit( &raw );

    /* Stop */
    R_Config_ADC_Stop();        //ADC_STOP_CONVERSION();
    R_Config_ADC_Set_OperationOff();

    if( resolution == 10 )
    {
        raw = raw >> 2; // divide by 4
    }

    return raw;
}

static void InitAdcValue(U8 mu8ValueId)
{
    U8 mu8Index;


    if( GetIndexConfList( mu8ValueId, &mu8Index) == TRUE )
    {
        adcValue[ mu8ValueId ].sum         = 0;
        adcValue[ mu8ValueId ].sum_count   = 0;
        adcValue[ mu8ValueId ].min         = adcConfList[ mu8Index ].min;
        adcValue[ mu8ValueId ].max         = adcConfList[ mu8Index ].max;
    }
}

static void Sum( void )
{
    U8  mu8Index;
    U8  mu8ValueId;
    U8  mu8Ch;
    U8  mu8Res;
    U8  mu8Count;
    U16  mu16Val;

    for( mu8Index = 0; mu8Index < SZ_ADC_CONF_LIST ; mu8Index++ )
    {
        mu8ValueId = adcConfList[ mu8Index ].id;
#if !CONFIG_TEST_TEMP_READ
        if( adcConfList[ mu8Index ].OnOff == OFF )
        {
            // 초기화
            if( adcConfList[ mu8Index ].pfDisable != NULL )
            {
                // PORT를 DISABLE하고 나서는 지연 시간을 초기화 한다.
                adcConfList[ mu8Index ].pfDisable();
                adcConfList[ mu8Index ].TurnOnDelay = TURN_ON_DELAY;
            }

            InitAdcValue( mu8ValueId );
        }
        else
#endif
        {
            // 센싱 
            if( adcConfList[ mu8Index ].pfEnable != NULL )
            {
                adcConfList[ mu8Index ].pfEnable();

#if !CONFIG_TEST_TEMP_READ
                // PORT를 ENABLE하고, 센서 값이 안정화 될 때가지 지연시간을 둔다.
                if( adcConfList[ mu8Index ].TurnOnDelay != 0 )
                {
                    adcConfList[ mu8Index ].TurnOnDelay--;
                    continue ;
                }
#endif
            }

            mu8Count = adcValue[ mu8ValueId ].sum_count;
            if( mu8Count < adcConfList [ mu8Index ].count )
            {
                mu8Ch   = adcConfList[ mu8Index ].channel;
                mu8Res  = adcConfList[ mu8Index ].resolution;
                mu16Val = SampleAdc( mu8Ch, mu8Res ); 

                adcValue[ mu8ValueId ].sum += mu16Val;
                adcValue[ mu8ValueId ].sum_count++;

                // Sampling min/max value
                if( mu16Val < adcValue[ mu8ValueId ].min )
                {
                    adcValue[ mu8ValueId ].min = mu16Val;
                }

                if( mu16Val > adcValue[ mu8ValueId ].max )
                {
                    adcValue[ mu8ValueId ].max = mu16Val;
                }
            }
        }
    }
}


static void Average( void )
{
    U8   mu8Index;
    U8   mu8ValueId;
    U8   count;
    AdcValue_T  *pValue = NULL;


    for( mu8Index = 0; mu8Index < SZ_ADC_CONF_LIST ; mu8Index++ )
    {
        mu8ValueId  = adcConfList[ mu8Index ].id;
        pValue      = &adcValue[ mu8ValueId ];

        count       = pValue->sum_count;
        if(  adcConfList[ mu8Index ].count == count )
        {

            pValue->sum -=  pValue->min + pValue->max;
            pValue->average =  pValue->sum / (count - 2);
            pValue->average =  pValue->sum / (count - 2);

            InitAdcValue( mu8ValueId );
        }
    }
}

void HAL_ProcessAdc(void)
{
    Sum();
    Average();
}
