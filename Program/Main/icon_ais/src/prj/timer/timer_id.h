#ifndef __TIMER_ID_H__
#define __TIMER_ID_H__

/* TIMER - MS */
enum
{
    TIMER_ID_COMM_EOL_RX,
    TIMER_ID_COMM_EOL_TX,

    TIMER_ID_COMM_DEBUG_RX,
    TIMER_ID_COMM_DEBUG_TX,

    TIMER_ID_COMM_FRONT_RX,
    TIMER_ID_COMM_FRONT_TX,
    TIMER_ID_COMM_FRONT_RX_ERR,

    TIMER_ID_COMM_WIFI_RX,
    TIMER_ID_COMM_WIFI_TX,

    TIMER_ID_COMM_COMP_RX,
    TIMER_ID_COMM_COMP_TX,
    TIMER_ID_COMM_COMP_RX_ERR,

    TIMER_ID_1MS,
    TIMER_ID_10MS,
    TIMER_ID_100MS,
    TIMER_ID_1SEC,
    TIMER_ID_10SEC,
    TIMER_ID_40SEC,
    TIMER_ID_1MIN,
    TIMER_ID_DEBUG,
    TIMER_ID_FRONT,
    TIMER_ID_COMP,

    TIMER_ID_DRAIN_PUMP_24H,
    TIMER_ID_CHECK_SYSTEM,

    TIMER_ID_TEST_8585,
    TIMER_ID_TEST_8585_2,
    TIMER_ID_TEST_8585_ICE,
    TIMER_ID_TEST_100MS,

    MAX_TIMER_ID_NUM
};

#endif /* __TIMER_ID_H__ */