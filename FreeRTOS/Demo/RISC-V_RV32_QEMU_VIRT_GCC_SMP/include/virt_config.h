/*
 * Default configuration helpers for the QEMU RV32 virt FreeRTOS port.
 */

#ifndef VIRT_CONFIG_H
#define VIRT_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef configCLINT_BASE_ADDRESS
    #define configCLINT_BASE_ADDRESS    0x02000000UL
#endif

#ifndef configCLINT_CLOCK_HZ
    #define configCLINT_CLOCK_HZ        10000000UL
#endif

#ifndef configCLINT_MTIMER_STRIDE
    #define configCLINT_MTIMER_STRIDE   8UL
#endif

#ifndef configCLINT_MSIP_STRIDE
    #define configCLINT_MSIP_STRIDE     4UL
#endif

#ifdef __cplusplus
}
#endif

#endif /* VIRT_CONFIG_H */
