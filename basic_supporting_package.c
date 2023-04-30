//the following bsp contains function safe c code for embedded programming
// it also help for setup the watchdog timer and additional tivaware library
#include <string.h>
#include <stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_watchdog.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/watchdog.h"
#include "tm4c123gh6pm.h"


//func1: watchdog timer setup for TIVA C tm4c123gh6pm
void WatchdogIntHandler(void)
{
    //the function can be used or modified to accomodate the setup function
    WatchdogIntClear(WATCHDOG0_BASE);
}


void watchdog_setup(uint32_t timeout){

    SysCtlPeripheralEnable(SYSCTL_PERIPH_WDOG0);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_WDOG0)){}

    if(WatchdogLockState(WATCHDOG0_BASE)==true){
        WatchdogUnlock(WATCHDOG0_BASE);
    }

    WatchdogReloadSet(WATCHDOG0_BASE,timeout);

    WatchdogResetEnable(WATCHDOG0_BASE);
    WatchdogIntRegister(WATCHDOG0_BASE,WatchdogIntHandler);

    IntEnable(INT_WATCHDOG);
    WatchdogIntEnable(WATCHDOG0_BASE);
    WatchdogEnable(WATCHDOG0_BASE);
}


//fun2: strcmp
int safe_strcmp(const char *s1, const char *s2, int len)
{
    //always put the comp under test string in s1
    //the default, non-overflowing string is at s2
    
    //Additional Argument Len (only compare up to len size, if len=-1, works as regular strcmp)

    //return value {0,-1,-2,>1}:
    //0 == valid
    //-1 == Null string
    // -2 == s1 greater then s2 (SUT is too big)
    // >1 == invalid comparison

    int      m = 0;
    volatile int i = 0;
    volatile int j = 0;
    volatile int k = 0;

    if (s1 == NULL || s2 == NULL)
        return -1; //-1 means null

    while (1) {
        m |= s1[i]^s2[j];

        if (s1[i] == '\0')
            break;
        i++;

        if (s2[j] != '\0')
            j++;
        if (s2[j] == '\0')
            k++;
        
        if(i==len){
            return -2; //m exceeds the length thus fail
        }
    }

    return m; // 0 means ok, >1 means bad
}