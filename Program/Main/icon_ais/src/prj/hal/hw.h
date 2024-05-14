#ifndef __HW__H__
#define __HW__H__

#include "mcu.h"
#include "r_smc_entry.h"

/* HW
 *
 * TIMER12-13 : PWM 
 *  -. DRAIN PUMP OUTPUT
 *
 * TIMER0 
 *  -. BASE TIMER 1ms
 *
 *  TIMER1
 *  -. STEP MOTOR 1ms
 *
 *  TIMER2
 *  -. HEATER OUT ( TRIAC )
 *  -. 8.33ms ( 120hz @ 60hz )
 *
 * PIN INTERRUPT
 *  -. FLOW METER
 *  -. INTP11
 *
 */


/* Watchdog timer reset */
#define	RESET_WDT()					R_Config_WDT_Restart()

/* COMP POWER */
#define  P_COMP_ONOFF               4,5//P4.5
#define  TURN_ON_COMP()             do{ PIN_WRITE(P_COMP_ONOFF) = 1; }while(0)
#define  TURN_OFF_COMP()            do{ PIN_WRITE(P_COMP_ONOFF) = 0; }while(0)
#define  GET_STATUS_COMP()          PIN_READ(P_COMP_ONOFF)

/* ICE FEEDER CW */
#define  P_FEEDER_CW                4,4//P4.4
#define  TURN_ON_FEEDER_CW()        do{ PIN_WRITE(P_FEEDER_CW) = 1; }while(0)
#define  TURN_OFF_FEEDER_CW()       do{ PIN_WRITE(P_FEEDER_CW) = 0; }while(0)
#define  GET_STATUS_FEEDER_CW()     PIN_READ(P_FEEDER_CW)

/* ICE FEEDER CCW */
#define  P_FEEDER_CCW                4,3//P4.3
#define  TURN_ON_FEEDER_CCW()        do{ PIN_WRITE(P_FEEDER_CCW) = 1; }while(0)
#define  TURN_OFF_FEEDER_CCW()       do{ PIN_WRITE(P_FEEDER_CCW) = 0; }while(0)
#define  GET_STATUS_FEEDER_CCW()     PIN_READ( P_FEEDER_CCW )

/* ICE TARY CW */
#define  P_ICE_TRAY_CW                4,1//P4.1
#define  TURN_ON_ICE_TRAY_CW()        do{ PIN_WRITE(P_ICE_TRAY_CW) = 1; }while(0)
#define  TURN_OFF_ICE_TRAY_CW()       do{ PIN_WRITE(P_ICE_TRAY_CW) = 0; }while(0)
#define  GET_STATUS_ICE_TRAY_CW()     PIN_READ( P_ICE_TRAY_CW )

/* ICE TRAY CCW */
#define  P_ICE_TRAY_CCW                4,2//P4.2
#define  TURN_ON_ICE_TRAY_CCW()        do{ PIN_WRITE(P_ICE_TRAY_CCW) = 1; }while(0)
#define  TURN_OFF_ICE_TRAY_CCW()       do{ PIN_WRITE(P_ICE_TRAY_CCW) = 0; }while(0)
#define  GET_STATUS_ICE_TRAY_CCW()     PIN_READ( P_ICE_TRAY_CCW )

/* SMPS 12V ON/OFF */
#define  P_SMPS_12V                    4,7//P4.7
#define  TURN_ON_SMPS_12V()            do{ PIN_WRITE(P_SMPS_12V) = 0; }while(0)
#define  TURN_OFF_SMPS_12V()           do{ PIN_WRITE(P_SMPS_12V) = 1; }while(0)
#define  GET_STATUS_SMPS_12V()         PIN_READ( P_SMPS_12V )

/* DRAIN TANK HIGH LEVEL */
#define  P_DRAIN_TANK_HIGH             8,1//P8.1
#define  GET_DRAIN_TANK_HIGH()         PIN_READ( P_DRAIN_TANK_HIGH )

/* DRAIN TANK LOW LEVEL */
#define  P_DRAIN_TANK_LOW              8,0//P8.0
#define  GET_DRAIN_TANK_LOW()          PIN_READ( P_DRAIN_TANK_LOW )


/* STEP  MOTOR - GAS VALVE */
// A,B,/A,/B
#define  P_STEP_MOTOR_GAS_SW_1         7,0//P7.0     // A
#define  P_STEP_MOTOR_GAS_SW_2         0,5//P0.5     // B
#define  P_STEP_MOTOR_GAS_SW_3         7,1//P7.1     // /A
#define  P_STEP_MOTOR_GAS_SW_4         0,6//P0.6     // /B

#define  TURN_ON_STEP_MOTOR_GAS_SW_1()     do { PIN_WRITE(P_STEP_MOTOR_GAS_SW_1) = 1; }while(0)
#define  TURN_ON_STEP_MOTOR_GAS_SW_2()     do { PIN_WRITE(P_STEP_MOTOR_GAS_SW_2) = 1; }while(0)
#define  TURN_ON_STEP_MOTOR_GAS_SW_3()     do { PIN_WRITE(P_STEP_MOTOR_GAS_SW_3) = 1; }while(0)
#define  TURN_ON_STEP_MOTOR_GAS_SW_4()     do { PIN_WRITE(P_STEP_MOTOR_GAS_SW_4) = 1; }while(0)

#define  TURN_OFF_STEP_MOTOR_GAS_SW_1()     do { PIN_WRITE(P_STEP_MOTOR_GAS_SW_1) = 0; }while(0)
#define  TURN_OFF_STEP_MOTOR_GAS_SW_2()     do { PIN_WRITE(P_STEP_MOTOR_GAS_SW_2) = 0; }while(0)
#define  TURN_OFF_STEP_MOTOR_GAS_SW_3()     do { PIN_WRITE(P_STEP_MOTOR_GAS_SW_3) = 0; }while(0)
#define  TURN_OFF_STEP_MOTOR_GAS_SW_4()     do { PIN_WRITE(P_STEP_MOTOR_GAS_SW_4) = 0; }while(0)

#define  GET_STEP_MOTOR_GAS_SW_1()     PIN_READ(P_STEP_MOTOR_GAS_SW_1)
#define  GET_STEP_MOTOR_GAS_SW_2()     PIN_READ(P_STEP_MOTOR_GAS_SW_2)
#define  GET_STEP_MOTOR_GAS_SW_3()     PIN_READ(P_STEP_MOTOR_GAS_SW_3)
#define  GET_STEP_MOTOR_GAS_SW_4()     PIN_READ(P_STEP_MOTOR_GAS_SW_4)

/* STEP MOTOR - ICE DOOR  */
#define  P_STEP_MOTOR_ICE_DOOR_4       5,5//P5.5
#define  P_STEP_MOTOR_ICE_DOOR_3       5,6//P5.6
#define  P_STEP_MOTOR_ICE_DOOR_2       5,7//P5.7
#define  P_STEP_MOTOR_ICE_DOOR_1       1,7//P1.7

#define  TURN_ON_STEP_MOTOR_ICE_DOOR_1()     do { PIN_WRITE(P_STEP_MOTOR_ICE_DOOR_1) = 1; }while(0)
#define  TURN_ON_STEP_MOTOR_ICE_DOOR_2()     do { PIN_WRITE(P_STEP_MOTOR_ICE_DOOR_2) = 1; }while(0)
#define  TURN_ON_STEP_MOTOR_ICE_DOOR_3()     do { PIN_WRITE(P_STEP_MOTOR_ICE_DOOR_3) = 1; }while(0)
#define  TURN_ON_STEP_MOTOR_ICE_DOOR_4()     do { PIN_WRITE(P_STEP_MOTOR_ICE_DOOR_4) = 1; }while(0)

#define  TURN_OFF_STEP_MOTOR_ICE_DOOR_1()     do { PIN_WRITE(P_STEP_MOTOR_ICE_DOOR_1) = 0; }while(0)
#define  TURN_OFF_STEP_MOTOR_ICE_DOOR_2()     do { PIN_WRITE(P_STEP_MOTOR_ICE_DOOR_2) = 0; }while(0)
#define  TURN_OFF_STEP_MOTOR_ICE_DOOR_3()     do { PIN_WRITE(P_STEP_MOTOR_ICE_DOOR_3) = 0; }while(0)
#define  TURN_OFF_STEP_MOTOR_ICE_DOOR_4()     do { PIN_WRITE(P_STEP_MOTOR_ICE_DOOR_4) = 0; }while(0)

#define  GET_STEP_MOTOR_ICE_DOOR_4()   PIN_READ(P_STEP_MOTOR_ICE_DOOR_4)
#define  GET_STEP_MOTOR_ICE_DOOR_3()   PIN_READ(P_STEP_MOTOR_ICE_DOOR_3)
#define  GET_STEP_MOTOR_ICE_DOOR_2()   PIN_READ(P_STEP_MOTOR_ICE_DOOR_2)
#define  GET_STEP_MOTOR_ICE_DOOR_1()   PIN_READ(P_STEP_MOTOR_ICE_DOOR_1)

/* STEP MOTOR - FLOW_VALVE ( HOT WATER OUT )  */
#define  P_STEP_MOTOR_FLOW_VALVE_4      14,2//P14.2
#define  P_STEP_MOTOR_FLOW_VALVE_3      14,1//P14.1
#define  P_STEP_MOTOR_FLOW_VALVE_2      14,0//P14.0
#define  P_STEP_MOTOR_FLOW_VALVE_1      4,6//P4.6

#define  TURN_ON_STEP_MOTOR_FLOW_VALVE_1()     do { PIN_WRITE(P_STEP_MOTOR_FLOW_VALVE_1) = 1; }while(0)
#define  TURN_ON_STEP_MOTOR_FLOW_VALVE_2()     do { PIN_WRITE(P_STEP_MOTOR_FLOW_VALVE_2) = 1; }while(0)
#define  TURN_ON_STEP_MOTOR_FLOW_VALVE_3()     do { PIN_WRITE(P_STEP_MOTOR_FLOW_VALVE_3) = 1; }while(0)
#define  TURN_ON_STEP_MOTOR_FLOW_VALVE_4()     do { PIN_WRITE(P_STEP_MOTOR_FLOW_VALVE_4) = 1; }while(0)

#define  TURN_OFF_STEP_MOTOR_FLOW_VALVE_1()     do { PIN_WRITE(P_STEP_MOTOR_FLOW_VALVE_1) = 0; }while(0)
#define  TURN_OFF_STEP_MOTOR_FLOW_VALVE_2()     do { PIN_WRITE(P_STEP_MOTOR_FLOW_VALVE_2) = 0; }while(0)
#define  TURN_OFF_STEP_MOTOR_FLOW_VALVE_3()     do { PIN_WRITE(P_STEP_MOTOR_FLOW_VALVE_3) = 0; }while(0)
#define  TURN_OFF_STEP_MOTOR_FLOW_VALVE_4()     do { PIN_WRITE(P_STEP_MOTOR_FLOW_VALVE_4) = 0; }while(0)

#define  GET_STEP_MOTOR_FLOW_VALVE_4()  PIN_READ(P_STEP_MOTOR_FLOW_VALVE_4)
#define  GET_STEP_MOTOR_FLOW_VALVE_3()  PIN_READ(P_STEP_MOTOR_FLOW_VALVE_3)
#define  GET_STEP_MOTOR_FLOW_VALVE_2()  PIN_READ(P_STEP_MOTOR_FLOW_VALVE_2)
#define  GET_STEP_MOTOR_FLOW_VALVE_1()  PIN_READ(P_STEP_MOTOR_FLOW_VALVE_1)

/* DRAIN PUMP PWM OUTPUT */
#define  P_DRAIN_PUMP                  6,7//P6.7
#define  TURN_ON_DRAIN_PUMP()          do{ PIN_WRITE(P_DRAIN_PUMP) = 1; }while(0)
#define  TURN_OFF_DRAIN_PUMP()         do{ PIN_WRITE(P_DRAIN_PUMP) = 0; }while(0)

/* FILTER DETECT */
#define  P_FILTER_DETECT               6,6//P6.6
#define  GET_STATUS_FILTER_DETECT()    PIN_READ( P_FILTER_DETECT )


/* SWING BAR */
#define  P_SWING_BAR                   5,0//P5.0
#define  TURN_ON_SWING_BAR()           do{ PIN_WRITE(P_SWING_BAR) = 1; }while(0)
#define  TURN_OFF_SWING_BAR()          do{ PIN_WRITE(P_SWING_BAR) = 0; }while(0)
#define  GET_STATUS_SWING_BAR()        PIN_READ( P_SWING_BAR )

/* UV_ICE_TRAY ON/OFF (1)*/
#define  P_UV_ONOFF_ICE_TRAY           1,5//P1.5
#define  TURN_ON_UV_ICE_TRAY()         do{ PIN_WRITE(P_UV_ONOFF_ICE_TRAY) = 1; }while(0)
#define  TURN_OFF_UV_ICE_TRAY()        do{ PIN_WRITE(P_UV_ONOFF_ICE_TRAY) = 0; }while(0)
#define  GET_STATUS_UV_ICE_TRAY()      PIN_READ( P_UV_ONOFF_ICE_TRAY )

/* UV_ICE TANK ON/OFF (2)*/
#define  P_UV_ONOFF_ICE_TANK            1,6//P1.6
#define  TURN_ON_UV_ICE_TANK()          do{ PIN_WRITE(P_UV_ONOFF_ICE_TANK) = 1; }while(0)
#define  TURN_OFF_UV_ICE_TANK()         do{ PIN_WRITE(P_UV_ONOFF_ICE_TANK) = 0; }while(0)
#define  GET_STATUS_UV_ICE_TANK()       PIN_READ( P_UV_ONOFF_ICE_TANK )


/* WIFI */
#define P_WIFI_ONOFF                            0,4//P0.4
#define TURN_ON_WIFI_MODULE()                  do{ PIN_WRITE(P_WIFI_ONOFF) = 1; }while(0)
#define TURN_OFF_WIFI_MODULE()                 do{ PIN_WRITE(P_WIFI_ONOFF) = 0; }while(0)

#define P_WIFI_RESET                            0,1//P0.1
#define TURN_ON_RESET_WIFI_MODULE()            do{ PIN_WRITE(P_WIFI_RESET) = 1; }while(0)
#define TURN_OFF_RESET_WIFI_MODULE()           do{ PIN_WRITE(P_WIFI_RESET) = 0; }while(0)


/* ICE TRAY */
#define  P_MICRO_SW_ICE_TRAY_2          11,1//P11.1
#define  GET_MICRO_SW_ICE_TRAY_2()      PIN_READ( P_MICRO_SW_ICE_TRAY_2 )

/* ICE TRAY */
#define  P_MICRO_SW_ICE_TRAY_1          14,6//P14.6
#define  GET_MICRO_SW_ICE_TRAY_1()      PIN_READ( P_MICRO_SW_ICE_TRAY_1 )

/* ICE TANK COVER REED_1 */
#define  P_REED_SW_TANK_OPEN            1,0//P1.0
#define  GET_REED_SW_TANK_OPEN()        PIN_READ( P_REED_SW_TANK_OPEN )

/* ICE TANK COVER REED_2 */
#define  P_REED_SW_TANK_OPEN_2          10,1//P10.1
#define  GET_REED_SW_TANK_OPEN_2()      PIN_READ( P_REED_SW_TANK_OPEN_2 )

#if 0
/* COLD TANK OVERFLOW */
#define  P_COLD_TANK_LOW                P7.5
#define  GET_COLD_TANK_LOW()            ( P_COLD_TANK_LOW )

/* COLD TANK HIGH */
#define  P_COLD_TANK_HIGH               P7.5
#define  GET_COLD_TANK_HIGH()           ( P_COLD_TANK_HIGH )

/* COLD TANK OVERFLOW */
#define  P_COLD_TANK_OVERFLOW           P10.1
#define  GET_COLD_TANK_OVERFLOW()       ( P_COLD_TANK_OVERFLOW )
#endif

/* ICE - ICE FULL */
#define  P_ICE_INFRARED                0,0//P0.0
#define  TURN_ON_ICE_INFRARED()        do{ PIN_WRITE(P_ICE_INFRARED) = 1; }while(0)
#define  TURN_OFF_ICE_INFRARED()       do{ PIN_WRITE(P_ICE_INFRARED) = 0; }while(0)
#define  GET_STATUS_ICE_INFRARED()     PIN_READ( P_ICE_INFRARED )


/* LEAK ON/OFF */
#define  P_LEAK_ONOFF                13,0//P13.0
#define  TURN_ON_LEAK_ONOFF()        do{ PIN_WRITE(P_LEAK_ONOFF) = 0; }while(0)
#define  TURN_OFF_LEAK_ONOFF()       do{ PIN_WRITE(P_LEAK_ONOFF) = 1; }while(0)
#define  GET_STATUS_LEAK_ONOFF()     PIN_READ( P_LEAK_ONOFF )


/* Condensor DC FAN MOTOR - 1 */
#define  P_FAN_MOTOR_1                      5,1//P5.1
#define  TURN_ON_FAN_MOTOR_1()              do{ PIN_WRITE(P_FAN_MOTOR_1) = 1; }while(0)
#define  TURN_OFF_FAN_MOTOR_1()             do{ PIN_WRITE(P_FAN_MOTOR_1) = 0; }while(0)
#define  GET_STATUS_FAN_MOTOR_1_ONOFF()     PIN_READ(P_FAN_MOTOR_1)


/* COLD TEMP ADC ON/OFF - ROOM TEMPERTURE  */
#define  P_ADC_COLD_ONOFF           6,2//P6.2
#define  TURN_ON_ADC_COLD()         do{ PIN_WRITE(P_ADC_COLD_ONOFF) = 1; }while(0)
#define  TURN_OFF_ADC_COLD()        do{ PIN_WRITE(P_ADC_COLD_ONOFF) = 0; }while(0)


/* COLD TEMP-2 ADC ON/OFF - ROOM TEMPERTURE  */
#define  P_ADC_COLD_2_ONOFF         7,6//P7.6
#define  TURN_ON_ADC_COLD_2()       do{ PIN_WRITE(P_ADC_COLD_2_ONOFF) = 1; }while(0)
#define  TURN_OFF_ADC_COLD_2()      do{ PIN_WRITE(P_ADC_COLD_2_ONOFF) = 0; }while(0)


/* ROOM ADC ON/OFF - ROOM TEMPERTURE  */
#define  P_ADC_ROOM_ONOFF           6,3//P6.3
#define  TURN_ON_ADC_ROOM()         do{ PIN_WRITE(P_ADC_ROOM_ONOFF) = 1; }while(0)
#define  TURN_OFF_ADC_ROOM()        do{ PIN_WRITE(P_ADC_ROOM_ONOFF) = 0; }while(0)

/* CODY SERVICE S/W */
#define  P_SERVICE_SW               14,5//P14.5
#define  GET_STATUS_SERVICE()       PIN_READ( P_SERVICE_SW )

/* EOL COMM LINE */
#define  PM_EOL_RX                  PM14.3
#define  P_EOL_RX                   14,3//P14.3

#define  PM_EOL_TX                  PM14.4
#define  P_EOL_TX                   14,4//P14.4

///* BLDC COMP COMM LINE */
//#define  PM_COMP_RX                  PM1.1
//#define  P_COMP_RX                   P1.1
//
//#define  PM_COMP_TX                  PM1.2
//#define  P_COMP_TX                   P1.2


#define P_FLOW_SENSOR               7,7//P7.7
#define  GET_FLOW_SENSOR()          PIN_READ( P_FLOW_SENSOR )


// RELAY
#define P_HEATER_RELAY               7,3//P7.3
#define TURN_ON_HEATER_RELAY()       do{ PIN_WRITE(P_HEATER_RELAY) = 1; }while(0)
#define TURN_OFF_HEATER_RELAY()      do{ PIN_WRITE(P_HEATER_RELAY)= 0; }while(0)
#define GET_STATUS_HEATER_RELAY()    PIN_READ(P_HEATER_RELAY)

// TRIAC
#define P_HEATER_TRIAC                7,2//P7.2
#define TURN_ON_HEATER_TRIAC()        do{ PIN_WRITE(P_HEATER_TRIAC) = 1; }while(0)
#define TURN_OFF_HEATER_TRIAC()       do{ PIN_WRITE(P_HEATER_TRIAC) = 0; }while(0)
#define GET_STATUS_HEATER_TRIAC()     PIN_READ(P_HEATER_TRIAC)

#define P_HEATER_INPUT                13,7 //P13.7
#define GET_STATUS_HEATER_INPUT()     PIN_READ(P_HEATER_INPUT )


/* VALVE - OUT POWER - DC 24V */
// ROOM OUT, HOT OUT, COLD OUT, COLD DRAIN
#define  P_VALVE_POWER_OUT                  7,4//P7.4
#define  OPEN_VALVE_POWER_OUT()             do{ PIN_WRITE(P_VALVE_POWER_OUT) = 1; }while(0)
#define  CLOSE_VALVE_POWER_OUT()            do{ PIN_WRITE(P_VALVE_POWER_OUT) = 0; }while(0)
#define  GET_STATUS_VALVE_POWER_OUT()       PIN_READ( P_VALVE_POWER_OUT )


/* VALVE - NOS */
#define  P_VALVE_NOS                        5,4//P5.4
#define  OPEN_VALVE_NOS()                   do{ PIN_WRITE(P_VALVE_NOS) = 0; }while(0)
#define  CLOSE_VALVE_NOS()                  do{ PIN_WRITE(P_VALVE_NOS) = 1; }while(0)
#define  GET_STATUS_VALVE_NOS()             PIN_READ( P_VALVE_NOS )


/* VALVE - ROOM OUT */
#define  P_VALVE_ROOM_OUT                   8,3//P8.3
#define  OPEN_VALVE_ROOM_OUT()              do{ PIN_WRITE(P_VALVE_ROOM_OUT) = 1; }while(0)
#define  CLOSE_VALVE_ROOM_OUT()             do{ PIN_WRITE(P_VALVE_ROOM_OUT) = 0; }while(0)
#define  GET_STATUS_VALVE_ROOM_OUT()        PIN_READ( P_VALVE_ROOM_OUT )

/* VALVE - ICE TRAY */
#define  P_VALVE_ICE_TRAY_IN                8,6//P8.6
#define  OPEN_VALVE_ICE_TRAY_IN()           do{ PIN_WRITE(P_VALVE_ICE_TRAY_IN) = 1; }while(0)
#define  CLOSE_VALVE_ICE_TRAY_IN()          do{ PIN_WRITE(P_VALVE_ICE_TRAY_IN) = 0; }while(0)
#define  GET_STATUS_VALVE_ICE_TRAY_IN()     PIN_READ( P_VALVE_ICE_TRAY_IN )


/* VALVE - COLD OUT */
#define  P_VALVE_COLD_OUT                   10,2//P10.2
#define  OPEN_VALVE_COLD_OUT()              do{ PIN_WRITE(P_VALVE_COLD_OUT) = 1; }while(0)
#define  CLOSE_VALVE_COLD_OUT()             do{ PIN_WRITE(P_VALVE_COLD_OUT) = 0; }while(0)
#define  GET_STATUS_VALVE_COLD_OUT()        PIN_READ( P_VALVE_COLD_OUT )

/* VALVE - COLD IN */
#define  P_VALVE_COLD_IN                    8,2//P8.2
#define  OPEN_VALVE_COLD_IN()               do{ PIN_WRITE(P_VALVE_COLD_IN) = 1; }while(0)
#define  CLOSE_VALVE_COLD_IN()              do{ PIN_WRITE(P_VALVE_COLD_IN) = 0; }while(0)
#define  GET_STATUS_VALVE_COLD_IN()         PIN_READ( P_VALVE_COLD_IN )

/* VALVE - COLD AIR ( NOS ) */
#define P_VALVE_COLD_AIR                    8,7//P8.7
#define OPEN_VALVE_COLD_AIR()               do{ PIN_WRITE(P_VALVE_COLD_AIR) = 0; }while(0)
#define CLOSE_VALVE_COLD_AIR()              do{ PIN_WRITE(P_VALVE_COLD_AIR) = 1; }while(0)
#define GET_STATUS_VALVE_COLD_AIR()         PIN_READ( P_VALVE_COLD_AIR )

#define P_VALVE_COLD_DRAIN                  3,0//P3.0
#define OPEN_VALVE_COLD_DRAIN()             do{ PIN_WRITE(P_VALVE_COLD_DRAIN) = 1; }while(0)
#define CLOSE_VALVE_COLD_DRAIN()            do{ PIN_WRITE(P_VALVE_COLD_DRAIN) = 0; }while(0)
#define GET_STATUS_VALVE_COLD_DRAIN()       PIN_READ( P_VALVE_COLD_DRAIN )

/* VALVE - COLD FLUSHING (FEED) */
#define  P_VALVE_COLD_FLUSH                 5,2//P5.2
#define  OPEN_VALVE_COLD_FLUSH()            do{ PIN_WRITE(P_VALVE_COLD_FLUSH) = 1; }while(0)
#define  CLOSE_VALVE_COLD_FLUSH()           do{ PIN_WRITE(P_VALVE_COLD_FLUSH) = 0; }while(0)
#define  GET_STATUS_VALVE_COLD_FLUSH()      PIN_READ(P_VALVE_COLD_FLUSH)

/* VALVE - HOT IN */
#define  P_VALVE_HOT_IN                     5,3//P5.3
#define  OPEN_VALVE_HOT_IN()                do{ PIN_WRITE(P_VALVE_HOT_IN) = 1; }while(0)
#define  CLOSE_VALVE_HOT_IN()               do{ PIN_WRITE(P_VALVE_HOT_IN) = 0; }while(0)
#define  GET_STATUS_VALVE_HOT_IN()          PIN_READ( P_VALVE_HOT_IN )

#define P_VALVE_HOT_DRAIN                   8,4//P8.4
#define OPEN_VALVE_HOT_DRAIN()              do{ PIN_WRITE(P_VALVE_HOT_DRAIN) = 0; }while(0)
#define CLOSE_VALVE_HOT_DRAIN()             do{ PIN_WRITE(P_VALVE_HOT_DRAIN) = 1; }while(0)
#define  GET_STATUS_VALVE_HOT_DRAIN()       PIN_READ( P_VALVE_HOT_DRAIN )

#define P_VALVE_HOT_OUT                     8,5//P8.5
#define OPEN_VALVE_HOT_OUT()                do{ PIN_WRITE(P_VALVE_HOT_OUT) = 1; }while(0)
#define CLOSE_VALVE_HOT_OUT()               do{ PIN_WRITE(P_VALVE_HOT_OUT) = 0; }while(0)
#define  GET_STATUS_VALVE_HOT_OUT()         PIN_READ( P_VALVE_HOT_OUT )


#endif /* __HW__H__ */

