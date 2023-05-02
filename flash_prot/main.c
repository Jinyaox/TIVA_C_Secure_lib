#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/flash.h"

/**
 * main.c
 */
int main(void)
{
    FlashProtectSet(0x10000,1);
    int x;
    x=FlashErase(0x10000);

    while(1){
        x=1;
    }
	return 0;
}
