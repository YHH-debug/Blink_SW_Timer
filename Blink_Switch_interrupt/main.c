#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "driverlib/interrupt.h"
//#include "Unlock_PF0/unlock_PF0.h"
uint8_t SW1_Flag,SW2_Flag;
void SW1_ISR(void);
void SW2_ISR(void);

//------------------------------------------------------
void SW1_ISR(void)
    {
    uint32_t val = 0;
    val = GPIOIntStatus(GPIO_PORTF_BASE,true);
    if((val & 0x00000001))
        SW2_Flag = 1;
    else
        SW1_Flag = 1;
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_4);//Clear Flag Interrupt on PF4
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_0);
    val = GPIOIntStatus(GPIO_PORTF_BASE,true);
    }
void SW2_ISR(void)
    {
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_INT_PIN_0);//Clear Flag Interrupt onf PF0
    SW2_Flag=1;//Set Flag to indicate SW2 is pressed
    }

//-----------------------------------------------------

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1| GPIO_PIN_2| GPIO_PIN_3,0);
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x1;
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4, GPIO_FALLING_EDGE);

    //GPIOIntRegister(GPIO_PORTF_BASE,&SW2_ISR);//Register for interrupt on PF0
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0);

    GPIOIntRegister(GPIO_PORTF_BASE,&SW1_ISR);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_4);//Register for interrupt for PF4

    IntEnable(INT_GPIOF);
    IntMasterEnable();
while(1)
    {
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2,0);
    if(SW1_Flag)// if SW1 is pressed, clear Flag, turn LED RED on.
    {
        SW1_Flag=0;
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1,0x02);
        SysCtlDelay(4000000);
    }
    if(SW2_Flag)// if SW2 is pressed, clear Flag, turn LED BLUE on.
    {
        SW2_Flag=0;
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2,0x04);
        SysCtlDelay(4000000);
    }
    }
}
