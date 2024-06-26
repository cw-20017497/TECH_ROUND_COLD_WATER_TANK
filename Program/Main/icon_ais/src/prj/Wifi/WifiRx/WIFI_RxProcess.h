
#ifndef _WIFI_RxProcess_H_
#define _WIFI_RxProcess_H_

typedef struct _wifirx_
{
    U8  u8State;        // Rx분석 (0:Rx가능 / 1:분석중)
    U8	u8Wait;         // 대기중인 데이터 (0:없음 / 1: 있음)
    U8  u8CheckModem;   // 공유기 첫 연결확인
    U8  u8CheckServer;  // 서버 첫 연결확인
    U8  u8ConnectCheck; // Uart 수신확인
    U16 u16ConnectCheckCount;  // 모듈과의 연결확인 (5분카운트)
    U8  u8DisplayConnectCount;
} WifiRx_T;
typedef U8 (*WifiRxAtFun_T)(void);
typedef struct _wifi_rxat_list_
{
    const I8 *Id;
    U8  Disp;
    U8  Err;
    U8  ApWifi;
    U8  ApBle;
    U8  ApStep;
    U8  Connect;
    U8  Sound;
    WifiRxAtFun_T Func;
} WifiRxAtList_T;
/*******************************************************************************
FUNCTION NAME: InitializeWifiProcessRx

DESCRIPTION: Initialize Rx Value

PARAMETERS: X

RETURN VALUE: X

NOTES: $)CCJ1b 1H8 ( Control )
*******************************************************************************/
void InitializeWifiRxProcess ( void );

/*******************************************************************************
FUNCTION NAME: GetWifiUartRxData

DESCRIPTION: Get uart Rx Data

PARAMETERS: mai8UartData

RETURN VALUE: X

NOTES: Int $)CH#Cb ( UartInterrupt )
*******************************************************************************/
void GetWifiUartRxData ( I8* mai8UartData );

/*******************************************************************************
FUNCTION NAME: WifiRxDataProcess

DESCRIPTION: Rx Data Process

PARAMETERS: X

RETURN VALUE: X

NOTES: While ( Control )
*******************************************************************************/
void WifiRxDataProcess ( void );

/*******************************************************************************
FUNCTION NAME: WifiRxAckTimer

DESCRIPTION: Module Connect Check

PARAMETERS: X

RETURN VALUE: X

NOTES: 100ms ( DataCheck )
*******************************************************************************/
// void WifiRxAckTimer ( void );

void ClearRxStatus ( void );

#endif

