
/*******************************************************************************
&Description :
  $)CA&G0?!<- ;g?kGO4B WIFI GT<v File
  
&Histiry : 
  ShinHM 2021.07.09 update
*******************************************************************************/


/***** Header *****************************************************************/
#include "WIFI_Common.h"
#include "WIFI_SetCommon.h"
#include "WIFI_DefUser.h"
#include "WIFI_Fota.h"
#include "WIFI_DataCheck.h"
#include "WIFI_RxProcess.h"
#include "WIFI_TxProtocol.h"
#include "WIFI_RxServer.h"
#include "WIFI_Status.h"
#include "WIFI_UartInterrupt.h"
#include "WIFI_UserInterface.h"
#include "WIFI_MonitorFixRAM.h"
#include "WIFI_Control.h"


/***** Define *****************************************************************/


/***** Variable ***************************************************************/
extern WifiRx_T WifiRx;

/***** Constant ***************************************************************/


/***** Static Function ********************************************************/
static void InitializeWifiAllValue ( void );
static void WifiDualApModeKey ( void );
static void WifiApModekey ( void );
static void WifiApBLEkey ( void );
static void WifiOnOffKey ( void );
static void WifiBleCertKey ( void );
static void WifiTestKey ( void );
static void WifiOnKey ( void );
static void WifiOffKey ( void );


/***** Table ******************************************************************/


/***** Global Function ********************************************************/
void WifiControlProcess ( E_WIFI_TIME_T mu8Time )
{
    if( mu8Time == WIFI_TIME_INI ) /* $)CCJ1bH- (EEPROM @L@|) */
    {
        InitializeWifiAllValue();
    }
    else if ( mu8Time == WIFI_TIME_SET ) /* $)CCJ1b<BFC (EEPROM @LHD While @L@|) */
    {
        WifiOnOffSetting();
    }
    else if ( mu8Time == WIFI_TIME_FACTORY ) /* $)C0x@e8p5e Ax@T =C eep 5%@LEM CJ1bH- */
    {
        WifiFactorySetting();
    }
    else if ( mu8Time == WIFI_TIME_WHILE )
    {
        WifiRxDataProcess();
        ReceiveWifiUart();
        WifiTimerControl();
    }
    else if ( mu8Time == WIFI_TIME_1MIN )
    {
        WifiDataCheckProcess(mu8Time);
        // TDEC_1min_WiFi();
    }
    else if ( mu8Time == WIFI_TIME_1SEC )
    {
        WifiDataCheckProcess(mu8Time);
        // TDEC_1s_WiFi();
    }
    else if ( mu8Time == WIFI_TIME_100MS )
    {
        // GetRamData();
        // TDEC_100ms_WiFi();
        WifiDataCheckProcess(mu8Time);
    }
}

void WifiKey ( E_WIFI_KEY_T mu8Key )
{
    if ( mu8Key == WIFI_KEY_AP )
    {
        WifiApModekey();
    }
    else if ( mu8Key == WIFI_KEY_BLE )
    {
        WifiApBLEkey();
    }
    else if ( mu8Key == WIFI_KEY_ON )
    {
        WifiOnKey();
    }
    else if ( mu8Key == WIFI_KEY_OFF )
    {
        WifiOffKey();
        SetWifiBuzzSound(WIFI_BUZZER_CANCEL);
    }
    else if ( mu8Key == WIFI_KEY_ONOFF )
    {
        WifiOnOffKey();
    }
    else if ( mu8Key == WIFI_KEY_TEST )
    {
        WifiTestKey();
    }
    else if ( mu8Key == WIFI_KEY_BLE_CERT )
    {
        WifiBleCertKey();
    }
    // else if ( mu8Key == WIFI_KEY_BLE_CERT_CANCEL )
    // {
    //     WifiBleCertCancelKey();
    // }
    else
    {
        
    }
}

void WifiSendData ( E_WIFI_DATA_T mu8Data )
{
    // if ( GetWifiServerStatus(STATUS_SVR_CONNECT) != SET )
    if (( GetWifiServerStatus(STATUS_SVR_CONNECT) != SET ) && ( GetWifiBLESendStatus(TX_BLE_WIFI) == WIFIDATA ))
    { /* $)C<-9v?,0a @L@|?!4B 5?@[ >HGT */
        return;
    }
    
    if ( mu8Data == WIFI_DATA_FUNCTION )
    {
        SetWifiSendStatus(TX_DATA,WIFI_TX_SEND_FUNC);
    }
    else if ( mu8Data == WIFI_DATA_SENSOR )
    {
        SetWifiSendStatus(TX_DATA,WIFI_TX_SEND_SENSOR);
    }
    else if ( mu8Data == WIFI_DATA_ERROR )
    {
        SetWifiSendStatus(TX_DATA,WIFI_TX_SEND_ERR);
    }
    else if ( mu8Data == WIFI_DATA_PART )
    {
        if ( GetUserInterface( USER_SMART_SENSING_STATUS ) != TRUE )
        {
            SetWifiSendStatus(TX_DATA,WIFI_TX_SEND_PART);
        }
    }
    else if ( mu8Data == WIFI_DATA_EXAMINE )
    {
        SetWifiSendStatus(TX_DATA,WIFI_TX_EXCEPT_FUNC);
        SetWifiSendStatus(TX_DATA,WIFI_TX_SEND_EXAMINE);
    }
    else if ( mu8Data == WIFI_DATA_INFO )
    {
        SetWifiSendStatus(TX_DATA,WIFI_TX_RQST_INFO_USER);
    }
}

void WifiSendDataControl ( E_WIFI_DATA_T mu8Data, U8 mu8IdData )
{
    if ( GetWifiServerStatus(STATUS_SVR_CONNECT) != SET )
    { 
        return;
    }
    
    if ( mu8Data == WIFI_DATA_FUNCTION_CONTROL )
    {
        SetWifiSendStatus(TX_DATA,WIFI_TX_SEND_FUNC_CONTROL);
        SetWifiSendStatusControl(mu8IdData);

    }
}

U8 GetWifiStatusValue ( E_WIFI_STATUS_T mType )
{
    U8 mValue = 0U;
    U8 mApStep = 0U;

    if ( mType == WIFI_STATUS_DISP ) /* WIFI Display */
    {
        mApStep = GetWifiIndicateStatus(STATUS_PARING);
        if ( GetWifiApStatus(STATUS_AP_WIFI) != CLEAR )
        {
            if ( mApStep == WIFI_AP_STEP_WAIT )
            {
                mValue = WIFI_DISP_AP_STEP0;
            }
            else if ( mApStep == WIFI_AP_STEP_START )
            {
                mValue = WIFI_DISP_AP_STEP1;
            }
            else if ( mApStep == WIFI_AP_STEP_MODEM )
            {
                mValue = WIFI_DISP_AP_STEP2;
            }
            else if ( mApStep == WIFI_AP_STEP_ROOTCA )
            {
                mValue = WIFI_DISP_AP_STEP3;
            }
            else
            {
                mValue = WIFI_DISP_AP_START;
            }
        }
        else if ( GetWifiApStatus(STATUS_AP_BLE) != CLEAR )
        {
            if ( mApStep == WIFI_AP_STEP_WAIT )
            {
                mValue = WIFI_DISP_BLE_STEP0;
            }
            else if ( mApStep == WIFI_AP_STEP_START )
            {
                mValue = WIFI_DISP_BLE_STEP1;
            }
            else if ( mApStep == WIFI_AP_STEP_MODEM )
            {
                mValue = WIFI_DISP_BLE_STEP2;
            }
            else if ( mApStep == WIFI_AP_STEP_ROOTCA )
            {
                mValue = WIFI_DISP_BLE_STEP3;
            }
            else
            {
                mValue = WIFI_DISP_BLE_START;
            }
        }
        else
        {
            mValue = GetWifiIndicateStatus(STATUS_DISPLAY);
        }
    }
    else if ( mType == WIFI_STATUS_POWER ) /* WIFI $)C@|?x */
    {
        mValue = GetWifiControlStatus(STATUS_CON_POWER);
    }
    else if ( mType == WIFI_STATUS_TEST ) /* $)CEW=:F.8p5e 0x@/1b?,0a H.@N (DISPLAY) */
    {
        mValue = GetWifiIndicateStatus(STATUS_FACTORY);
    }
    else if ( mType == WIFI_STATUS_ERROR ) /* WIFI Error */
    {
        mValue = GetWifiIndicateStatus(STATUS_ERROR);
    }
    else if ( mType == WIFI_STATUS_AP_FIRST )
    {
        mValue = GetWifiApStatus(STATUS_AP_FIRST);
    }
    else if ( mType == WIFI_STATUS_AP_MODE ) /* AP MODE $)CH.@N (DISPLAY) */
    {
        if ( GetWifiApStatus(STATUS_AP_WIFI) != CLEAR )
        {
            mValue = SET;
        }
    }
    else if ( mType == WIFI_STATUS_AP_BLE ) /* BLE AP MODE $)CH.@N (DISPLAY) */
    {
        if ( GetWifiApStatus(STATUS_AP_BLE) != CLEAR )
        {
            mValue = SET;
        }
    }
    else if ( mType == WIFI_STATUS_BLE_CERT ) /* BLE CERT MODE (0:Unauthenticated status, 1: Authenticateable Status, 2:Authentication completion status ) */
    {
        if (GetWifiApStatus(STATUS_AP_BLE) != CLEAR)  // BLE°¡ È°¼ºÈ­ µÇ¾î ÀÖ´Â °æ¿ì
        {
            if ( GetWifiApStatus(STATUS_AP_BLE_CERT) == SET )
            {
                mValue = SET;
            }
            else if (GetWifiApStatus(STATUS_AP_BLE_CERT) == DONE )
            {
                mValue = DONE;
            }
            else
            {
            }
        }
        else
        {
        }
    }
    else if ( mType == WIFI_STATUS_AP_STEP ) /* $)CFd>n85 =:E\ H.@N (DISPLAY) */
    {
        mValue = GetWifiIndicateStatus(STATUS_PARING);
    }  
    else if ( mType == WIFI_STATUS_AP_ERROR ) /* WIFI AP Mode Error */
    {
        if ( GetWifiIndicateStatus(STATUS_PARING) >= WIFI_AP_STEP_START )
        {
            mValue = GetWifiIndicateStatus(STATUS_AP_ERROR);
        }
    }
    
    return mValue;
}

U16 GetWifiRequestValue ( E_WIFI_RQST_T mId )
{
    U16 mValue = 0;
    mValue = GetWifiInfoData(mId);
    return mValue;
}

void InterruptUartWifiTx ( void )
{
    IntWifiUartTx();
}

void InterruptUartWifiRx ( U8 mu8RxData )
{
    IntWifiUartRx(mu8RxData);
}

void InterruptUartWifiError ( void )
{
    //U8 rx_data = 0;
    volatile U8 err_type;

    //rx_data = R_RXD;
    err_type = (U8)(SSR03 & 0x0007U);
    SIR03 = (U16)err_type;
}


/***** Local Function *********************************************************/
static void InitializeWifiAllValue ( void )
{
    InitializeWifiFota();
    InitializeWifiStatus();
    InitializeWifiRxProcess();
    InitializeWifiRxServer();
    InitializeWifiTxProtocol();
    InitializeWifiUart();
    InitializeWifiUserInterface();
}

static void WifiDualApModeKey ( void )
{
    static U8 mu8Dual = 0;
    /* BLE -> Off -> WiFi -> Off -> BLE... repeat */
    if ( GetWifiControlStatus(STATUS_CON_POWER) != ON )
    {
        if ( mu8Dual  == CLEAR )
        {
            mu8Dual = SET;
        }
        else
        {
            mu8Dual = CLEAR;
        }
    }

    if ( mu8Dual == SET )
    {
        WifiApModekey();
    }
    else
    {
        WifiApBLEkey();
    }

}

// WIFI AP MODE KEY
static void WifiApModekey ( void )
{
    // if ( ( GetWifiApStatus(STATUS_AP_WIFI) != CLEAR ) || GetWifiApStatus(STATUS_AP_BLE) != CLEAR )
    if (GetWifiApStatus(STATUS_AP_WIFI) != CLEAR )
    { 
        SetWifiApStatus(STATUS_AP_WIFI, CLEAR);
        SetWifiApStatus(STATUS_AP_BLE, CLEAR);
        WifiOffKey();
        SetWifiBuzzSound(WIFI_BUZZER_WiFi_OFF);
        return;
    }    
    else if (GetWifiApStatus(STATUS_AP_BLE) != CLEAR )
    {
        // HAL_SetWifiOnOff(RESET); //
        SetWifiBuzzSound(WIFI_BUZZER_ERROR);
        return;
    }

    if ( GetWifiControlStatus(STATUS_CON_POWER) != ON )
    {
        HAL_SetWifiOnOff(ON);
        SetWifiApStatus(STATUS_AP_WIFI,RESET);
    }
    else
    {
        if ( GetWifiServerStatus(STATUS_SVR_CONNECT) == SET )
        { // AP$)CA"<S =C <-9v?,0a5G>n @V@88i A>7a=EH# @|<[ HD AP Ax@T
            SetWifiSendStatus(TX_DATA,WIFI_TX_SEND_STOP);
            SetWifiApStatus(STATUS_AP_WIFI,WAIT);
        }
        else
        {
            SetWifiApStatus(STATUS_AP_WIFI,RESET);
        }
    }
    ClearRxStatus();
    SetWifiIndicateStatus(STATUS_AP_ERROR,WIFI_DISP_OFF);
    SetWifiBuzzSound(WIFI_BUZZER_AP_START);
    WifiRx.u8DisplayConnectCount = CLEAR;
}

static void WifiApBLEkey ( void )
{
    if(GetWifiApStatus(STATUS_AP_BLE_CERT) == SET) // Á¡À¯ÀÎÁõ °¡´É (Á¶ÀÛ ´ë±â) »óÅÂÀÎ °æ¿ì
    {
        WifiBleCertKey();
        return;
    }
    else if (GetWifiApStatus(STATUS_AP_BLE_CERT) == DONE) 
    {
        // return; // »èÁ¦
    }

    if (GetWifiApStatus(STATUS_AP_BLE) != CLEAR )
    { 
        if ( GetWifiServerStatus(STATUS_SVR_CONNECT) == SET )
        {
            SetWifiApStatus(STATUS_AP_BLE, CLEAR);
            SetWifiSendStatus(TX_ATCMD,WIFI_AT_BLE_OFF);
            SetWifiIndicateStatus(STATUS_PARING,WIFI_AP_STEP_CLEAR);
            SetWifiBuzzSound(WIFI_BUZZER_BLE_OFF);
        }
        else
        {
            SetWifiApStatus(STATUS_AP_BLE, CLEAR);
            WifiOffKey();
            SetWifiBuzzSound(WIFI_BUZZER_BLE_OFF);
        }
        return;
    }    
    else if ( GetWifiApStatus(STATUS_AP_WIFI) != CLEAR )
    {
        // HAL_SetWifiOnOff(RESET);
        SetWifiBuzzSound(WIFI_BUZZER_ERROR);
        return;
    }
    
    if ( GetWifiControlStatus(STATUS_CON_POWER) != ON )
    {
        HAL_SetWifiOnOff(ON);
    }
    else
    {
        //SetWifiSendStatus(TX_ATCMD,WIFI_AT_BLE_ADV);
    }
    SetWifiApStatus(STATUS_AP_BLE,WAIT);
    SetWifiBuzzSound(WIFI_BUZZER_AP_START);
    WifiRx.u8DisplayConnectCount = CLEAR;
}

// WIFI ON/OFF KEY
static void WifiOnOffKey ( void )
{
    if ( GetWifiControlStatus(STATUS_CON_POWER) != ON )
    {
        WifiOnKey();
    }
    else
    {
        WifiOffKey();
        SetWifiBuzzSound(WIFI_BUZZER_CANCEL);
    }
}
// BLE CERT KEY
static void WifiBleCertKey ( void )
{
    if ((GetWifiStatusValue(WIFI_STATUS_AP_BLE) == SET) && (GetWifiApStatus(STATUS_AP_BLE_CERT) == SET)) // BLEÈ°¼ºÈ­ && Á¡À¯ÀÎÁõ °¡´É»óÅÂ
    {
        // Send Authentication completed
        SetWifiCertID(WIFI_BLE_CERT_0003);
        SetWifiCertResult(RESULT_CERT_SUCCESS);
        SetWifiSendStatus(TX_DATA,WIFI_TX_SEND_BLE_CERT);
        
        SetWifiApStatus(STATUS_AP_BLE_CERT,DONE);
        SetWifiBuzzSound(WIFI_BUZZER_SELECT);
    }

}

// Test Key Operation
static void WifiTestKey ( void )
{
    HAL_SetWifiOnOff(ON);
    SetWifiBuzzSound(WIFI_BUZZER_SELECT);
}

// On Key Operation
static void WifiOnKey ( void )
{
    if ( GetWifiControlStatus(STATUS_CON_POWER) == ON )
    {
        SetWifiBuzzSound(WIFI_BUZZER_ERROR);
        return;
    }
    
    if ( ( GetUserInterface(USER_FACTORY_STATUS) == SET ) || ( GetWifiApStatus(STATUS_AP_FIRST) == SET ) )
    {
        HAL_SetWifiOnOff(ON);
        WifiWriteEep(EEPROM_ADDR_WIFI_POWER, ON);
        SetWifiBuzzSound(WIFI_BUZZER_SETUP);
    }
    else
    {
        SetWifiBuzzSound(WIFI_BUZZER_ERROR);
    }
    ClearRxStatus();
}

// Off Key Operation
static void WifiOffKey ( void )
{
    if ( GetWifiControlStatus(STATUS_CON_POWER) == OFF )
    {
        SetWifiBuzzSound(WIFI_BUZZER_ERROR);
        return;
    }
    
    if ( GetWifiServerStatus(STATUS_SVR_CONNECT) == SET )
    { // $)CA>7a=EH# <[=E @LHD @OA$=C0# Av3*8i A>7a5?@[
        SetWifiServerStatus(STATUS_SVR_END,SET);
        SetWifiSendStatus(TX_DATA,WIFI_TX_SEND_STOP);
        SetWifiIndicateStatus(STATUS_DISPLAY,WIFI_DISP_OFF);
    }
    else
    {
        HAL_SetWifiOnOff(OFF);
    }
    // SetWifiBuzzSound(WIFI_BUZZER_CANCEL);
    ClearRxStatus();
}

void WifiTimerControl(void)
{
}
