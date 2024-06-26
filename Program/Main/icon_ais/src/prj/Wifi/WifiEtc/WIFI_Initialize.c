
/*******************************************************************************
&Description :
    $)C<-9v?,0a ?O7a =C CJ1b 5%@LEM <-9v @|<[
  
&Histiry : ShinHM
    2020.12.24 update
    2021.10.28 retry 5$)CH8 A&GQ (<-9v@e>V =C 9f>n)
        - $)C>F@LD\8F=: V1002 / :tF.@N V1002 / 3k:m<<7N CJ55 / 0!7N CJ55 / GA7N CJ55 9]?5
    2021.12.02 retry $)CA$8. 9W ?O7a7NAw 5?@OGO0T C38. (A2010 :055H.@N)
*******************************************************************************/

/***** Header *****************************************************************/
#include "WIFI_Common.h"
#include "WIFI_DefUser.h"
#include "WIFI_Initialize.h"
#include "WIFI_Status.h"
#include "WIFI_UserInterface.h"


/***** Define *****************************************************************/

/***** Variable ***************************************************************/

/***** Constant ***************************************************************/

/***** Static Function ********************************************************/
static void IniServerConnect ( void );
static void IniRqstInfo ( void );
static void IniSendStart ( void );
static void IniCheckSkip ( void );
static void IniFinished ( void );
static U8 IsIniRetryOver ( U8 mCount );


/***** Table ******************************************************************/
typedef void (*WifiIniFun_T)(void);
typedef struct _wifi_ini_list_
{
    U8 Id;
    U32 Tx;
    WifiIniFun_T Func;
    U8 Step;
} WifiInitializeList_T;

//static void RunInitData ( WifiInitializeList_T *pList );
//static void RunInitAck ( WifiInitializeList_T *pList );
static void RunInitData ( const WifiInitializeList_T *pList );
static void RunInitAck ( const WifiInitializeList_T *pList );


/* Start Initialize Table */
static const WifiInitializeList_T WifiInitializeList[] =
{   /*  ID                          Tx Data                 Function            NextStep                WIFI $)CCJ1b=C@[ =CDv=: */
    {   INI_SEQ_NULL,               WIFI_TX_NULL,           NULL,               INI_SEQ_NULL            }, // <0> $)C4k1b

    {   INI_SEQ_SERVER_CONNECT,     WIFI_TX_NULL,           IniServerConnect,   INI_SEQ_RQST_INFO       }, // <1> $)C<-9v?,0a H.@N
    
    {   INI_SEQ_RQST_INFO,          WIFI_TX_RQST_INFO,      IniRqstInfo,        INI_SEQ_ACK_INFO        }, // $)C:N0!A$:8 ?dC;
    {   INI_SEQ_ACK_INFO,           WIFI_TX_NULL,           NULL,               INI_SEQ_SEND_STATUS     }, // ACK $)C4k1b
    
    {   INI_SEQ_SEND_STATUS,        WIFI_TX_SEND_FUNC,      IniSendStart,       INI_SEQ_ACK_STATUS      }, // $)C;sEB5%@LEM @|<[
    {   INI_SEQ_ACK_STATUS,         WIFI_TX_NULL,           NULL,               INI_SEQ_SEND_SENSOR     }, // ACK $)C4k1b
    
    {   INI_SEQ_SEND_SENSOR,        WIFI_TX_SEND_SENSOR,    NULL,               INI_SEQ_ACK_SENSOR      }, // $)C<><-5%@LEM @|<[
    {   INI_SEQ_ACK_SENSOR,         WIFI_TX_NULL,           NULL,               INI_SEQ_SEND_ERROR      }, // ACK $)C4k1b
    
    {   INI_SEQ_SEND_ERROR,         WIFI_TX_SEND_ERR,       IniCheckSkip,       INI_SEQ_ACK_ERROR       }, // $)C?!7/5%@LEM @|<[
    {   INI_SEQ_ACK_ERROR,          WIFI_TX_NULL,           NULL,               INI_SEQ_SEND_PART       }, // ACK $)C4k1b

    #ifdef USER_USE_ACCUMULATE
    {   INI_SEQ_SEND_PART,          WIFI_TX_SEND_PART,      NULL,               INI_SEQ_ACK_PART        }, // $)C:NGO5%@LEM @|<[
    {   INI_SEQ_ACK_PART,           WIFI_TX_NULL,           NULL,               INI_SEQ_SEND_ACCUMULATE }, // ACK $)C4k1b
    #else
    {   INI_SEQ_SEND_PART,          WIFI_TX_SEND_PART,      NULL,               INI_SEQ_ACK_PART        }, // $)C:NGO5%@LEM @|<[
    {   INI_SEQ_ACK_PART,           WIFI_TX_NULL,           NULL,               INI_SEQ_FINISHED        }, // ACK $)C4k1b
    #endif
    
    {   INI_SEQ_SEND_ACCUMULATE,    WIFI_TX_SEND_ACCUMULATE,NULL,               INI_SEQ_ACK_ACCUMULATE  }, // $)C4)@{5%@LEM @|<[
    {   INI_SEQ_ACK_ACCUMULATE,     WIFI_TX_NULL,           NULL,               INI_SEQ_FINISHED        }, // ACK $)C4k1b
    
    {   INI_SEQ_FINISHED,           WIFI_TX_NULL,           IniFinished,        INI_SEQ_NULL            }  // $)CCJ1b 5%@LEM @|<[ ?O7a
};

#define SZ_INI_LIST ( sizeof(WifiInitializeList) / sizeof(WifiInitializeList_T) )


/***** Global Function ********************************************************/
void WifiStartProcess ( void )
{
    const WifiInitializeList_T *pList = NULL;
    U8 i = 0U;
    U8 mInit = 0U;
    
    /* Initialze Operation */
    mInit = GetWifiIniStatus( STATUS_INI_SEQ );
    pList = WifiInitializeList;
    for ( i = 0 ; i < SZ_INI_LIST; i++ )
    {
        if ( mInit == (pList+i)->Id )
        {
            RunInitData( pList+i );
            break;
        }
    }
}

void WifiCheckInitializeAck ( void )
{
    const WifiInitializeList_T *pList = NULL;
    U8 i = 0U;
    U8 mInit = 0U;
    
    if ( GetWifiIniStatus( STATUS_INI_FINISH ) == SET )
    {
        return;
    }
    
    /* Check Init Ack & Run */
    mInit = GetWifiIniStatus( STATUS_INI_SEQ );
    pList = WifiInitializeList;    
    for ( i = 0 ; i < SZ_INI_LIST; i++ )
    {
        if ( mInit == (pList+i)->Id )
        {
            RunInitAck( pList+i );
            break;
        }
    }
}

void WifiCheckInitializeInfoAck ( void )
{
    if ( GetWifiIniStatus( STATUS_INI_SEQ ) == INI_SEQ_ACK_INFO )
    {
        SetWifiIniStatus( STATUS_INI_SEQ, (WifiInitializeList+INI_SEQ_ACK_INFO)->Step );
    }
}

/***** Local Function *********************************************************/
static void RunInitData ( const WifiInitializeList_T *pList )
{
    WifiIniFun_T pFun;
    U32 mTx = 0UL;
    U8 mStep = 0U;

    pFun = pList->Func;
    mTx = pList->Tx;
    mStep = pList->Step;
            
    if ( mTx != WIFI_TX_NULL )
    {
        SetWifiSendStatus( TX_DATA, mTx );
        SetWifiIniStatus( STATUS_INI_SEQ, mStep );
    }
        		
    if ( pFun != NULL )
    {
        pFun();
    }
}

static void RunInitAck ( const WifiInitializeList_T *pList )
{
    U32 mTx = 0UL;
    U8 mStep = 0U;
            
    mTx = pList->Tx;
    mStep = pList->Step;
            
    if ( mTx == WIFI_TX_NULL )
    {
        SetWifiIniStatus( STATUS_INI_SEQ, mStep );
    }
}

static void IniServerConnect ( void )
{
    if ( GetWifiServerStatus( STATUS_SVR_CONNECT ) != SET )
    { // Server Disconnect
        return;
    }
    
    if ( GetWifiIniStatus( STATUS_INI_INFO ) != SET )
    {
        SetWifiIniStatus( STATUS_INI_SEQ, (WifiInitializeList+INI_SEQ_SERVER_CONNECT)->Step );
    }
    else
    {
        SetWifiIniStatus( STATUS_INI_SEQ, INI_SEQ_SEND_STATUS );
    }
}

static void IniRqstInfo ( void )
{
    if ( IsIniRetryOver( SET ) == TRUE )
    {
        IniFinished();
    }
}

static void IniSendStart ( void )
{
    if ( IsIniRetryOver( START ) == TRUE )
    {
        IniFinished();
    }
}

static void IniCheckSkip ( void )
{
    if ( GetUserInterface( USER_SMART_SENSING_STATUS ) == TRUE )
    { // Running SmartSensing 
        IniFinished();
    }
}

static void IniFinished ( void )
{
    SetWifiIniStatus( STATUS_INI_FINISH, SET );
    SetWifiIniStatus( STATUS_INI_SEQ, INI_SEQ_NULL );
    IsIniRetryOver( CLEAR );
}

#define W_INIT_OVER_CNT 5  // Retry 5 Count
static U8 IsIniRetryOver ( U8 mCount )
{
    static U8 mInfoRetry = 0U;
    static U8 mSendRetry = 0U;
    
    if ( mCount == CLEAR )
    {
        mInfoRetry = CLEAR;
        mSendRetry = CLEAR;
    }
    else if ( mCount == SET )
    {
        mInfoRetry++;
    }
    else
    {
        mSendRetry++;
    }

    if ( ( mInfoRetry >= W_INIT_OVER_CNT ) || ( mSendRetry >= W_INIT_OVER_CNT ) )
    {
        return TRUE;
    }
    return FALSE;
}


