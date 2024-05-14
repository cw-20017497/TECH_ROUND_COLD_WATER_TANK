#include "mcu.h"
#include "r_smc_entry.h"

#include "hw.h"
#include "timer.h"
#include "util.h"
#include "eeprom.h"
#include "rtc.h"
#include "front.h"
#include "sound.h"
#include "parser.h"
#include "eol.h"
#include "water_out.h"
#include "flush_water_out.h"
#include "power_saving.h"
#include "process_display.h"
#include "process_sys_event.h"
#include "WIFI_Common.h"
#include "WIFI_UserInterface_IconAIS.h"
#include "WIFI_Control.h"

/* Init Timer */
static void InitStartTimers( void )
{
    InitTimer();

    StartTimer( TIMER_ID_1MS,   1);
    StartTimer( TIMER_ID_10MS,  10);
    StartTimer( TIMER_ID_100MS, 100);
    StartTimer( TIMER_ID_1SEC,  TIMER_SEC(1));
    StartTimer( TIMER_ID_10SEC, TIMER_SEC(1));

    // TIMER_ID_40SEC???�도 ?�서 ON/OFF ?�용 ?�?�머?�다.
    // ?�원 Reset ??10분이 지????40�?간격?�로 ?�도 ?�서�??�어?�다.
    StartTimer( TIMER_ID_40SEC, TIMER_SEC(600));
    StartTimer( TIMER_ID_1MIN,  TIMER_SEC(1));
#if !CONFIG_JIG_RBK_FRONT
    StartTimer( TIMER_ID_DEBUG, 100);
#endif
    StartTimer( TIMER_ID_FRONT, 20 );


#if CONFIG_JIG_GAS_SWITCH
    StartTimer( TIMER_ID_TEST_100MS, 100 );
#endif

    StartTimer( TIMER_ID_COMM_FRONT_RX_ERR, TIMER_SEC(10));

    StartTimer( TIMER_ID_CHECK_SYSTEM, TIMER_SEC(3));
}

static void InitFotaMemory(void)
{
#if CONFIG_FOTA_LIB 
    static bit fota_test;

    fota_test = 1;
    fota_test = 0;
#endif
}



int main( void )
{
#if 0
    /* hdwinit for FOTA */
    PIOR = 0x00U;
    R_CGC_Get_ResetSource();
    R_CGC_Create();
    R_PORT_Create();
    R_SAU0_Create();
    R_SAU1_Create();
    R_IICA0_Create();
    R_ADC_Create();
    R_TAU0_Create();
    R_TAU1_Create();
    R_WDT_Create();
    R_INTC_Create();
    IAWCTL = 0x00U;
    /* hdwinit for FOTA */
#else
    PRR0 = 0x7FU;    /* reset IICA, ADC, TAU and SAU module */
    PRR1 = 0xF3U;    /* reset DAC, SMS, COMP, ITL, REMC, CTSU module */
    PRR0 = 0x00U;    /* release IICA, ADC, TAU and SAU module */
    PRR1 = 0x00U;    /* release DAC, SMS, COMP, ITL, REMC, CTSU module */
    /* Set peripheral settings */
    R_Config_PORT_Create();
    R_SAU0_Create();
    R_SAU1_Create();
    R_TAU0_Create();
    R_TAU1_Create();
    R_Config_IICA0_Create();
    R_Config_ADC_Create();
    R_Config_INTC_Create();
    R_Config_WDT_Create();
#endif
    
    Delay_MS( 1400U );   // actually 500ms...

    RESET_WDT();

    InitSystem();
    InitStartTimers();
    InitFotaMemory();
    EI();

    /* RTC & EEPROM */
    InitRtc();
    RESET_WDT();
#if !CONFIG_JIG_RBK_FRONT
    InitEeprom();
#endif
    CheckFactroySetup();

    SetFotaReboot( UserReadEep( EEPROM_ADDR_FOTA_MCU ) );

    CheckSystem();
    if( GetFotaReboot() == FALSE )
    {
        Sound( SOUND_POWER_ON );
    }

#if CONFIG_JIG_RBK_FRONT
    StartEol( EOL_TYPE_FRONT );
    SetVersionDisp( 0 );
#endif

    if( GetSavingConfig() == TRUE )
    {
#if CONFIG_TEST_8585
        StartTimer( TIMER_ID_TEST_8585, TIMER_SEC(20) );
        StartTimer( TIMER_ID_TEST_8585_2, TIMER_SEC(20) );
#endif
#if CONFIG_TEST_8585_ICE
        StartTimer( TIMER_ID_TEST_8585_ICE, TIMER_SEC(30) );
#endif
    }

    WifiControlProcess(WIFI_TIME_SET);

    while(1)
    {
        RESET_WDT();

        ProcessEventHandler();

        /* COMM */
        RecvPacketHandler();
        SendPacketHandler();

        /* WIFI */
        WifiControlProcess(WIFI_TIME_WHILE);
    }

    //return 0;
}


