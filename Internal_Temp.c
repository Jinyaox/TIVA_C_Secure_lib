#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
//#include "driverlib/timer.h"
//#include "driverlib/systick.h"
#include "driverlib/adc.h"

#define ADC_SEQUENCER_LENGTH 1

//initializes the adc
void initADC(void){
  //enable the adc0 peripherial.
  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
  //set the speed to 1msps.
  //SysCtlADCSpeedSet(SYSCTL_ADCSPEED_1MSPS);
  //set the auto avergage to 64.
  ADCHardwareOversampleConfigure(ADC0_BASE, 64);
  //before setting up I must disable the sequence 3.
  ADCSequenceDisable(ADC0_BASE, 3);
  //set the sequence to use (adc0 sequence 3).
  ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
  //set up the sequence step.
  //set up the last step and start an interrupt when the conversion it's over.
  ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);
  //enable the sequence again!
  ADCSequenceEnable(ADC0_BASE, 3);
}

void initUART(void)
{
    // Initialize the UART.
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);    
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    UARTStdioConfig(0, 115200,  16000000);
}

int main(void){
  
  //Variable to store the data from the sequencer.
  unsigned long ulADC0Value[ADC_SEQUENCER_LENGTH];
  unsigned long ulTemp_ValueC=0;
  //Set the syjstem clock to run at 40MHz.
  SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

  initADC();
  initUART();
  
  while(1)
  {
    //clear the interrupt flag
    ADCIntClear(ADC0_BASE, 3);
    //trigger the adc conversion process.
    ADCProcessorTrigger(ADC0_BASE, 3);
    //wait for the interrupt flag to get set!
    while(!ADCIntStatus(ADC0_BASE, 3, false))
    {
    }
    //get the actual data samples from adc0 sequencer 3!
    ADCSequenceDataGet(ADC0_BASE, 3, ulADC0Value);
        //convert the value!
    //  TEMP = 147.5 - ((75 * (VREFP - VREFN) * ADCVALUE) / 4096)
    //  3.3V *10 * 75 = 2475 //13.3.6 of the TM4C123GH6PM datasheet
    ulTemp_ValueC = (1475 -((2475 * ulADC0Value[0])) / 4096)/10;
    UARTprintf("Temperature = %3d*C \n\r", ulTemp_ValueC);

    SysCtlDelay(SysCtlClockGet() / 12);
  }
}