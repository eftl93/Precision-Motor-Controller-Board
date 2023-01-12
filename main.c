


#include <xc.h>                  
#include "lm629.h"
#include "spi.h"
#include "main.h"
#define _XTAL_FREQ 64000000       //Use 64MHz as FOSC (16MHz Crystal with 4X PLL)

////////////////////////////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
//%///////////////////////////Start Main Loop////////////////////////////////%//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
////////////////////////////////////////////////////////////////////////////////
extern volatile uint8_t spi_read_data;
uint8_t signal_distribution_packet[6];
struct spi_package
{
    uint8_t lx_joystick;
    uint8_t ly_joystick;
    uint8_t rx_joystick;
    uint8_t ry_joystick;
};

struct spi_package classic_ctrl;

void main(void)
{
    unsigned char received_data;
    unsigned char dummy_data;
    received_data = 0x00;
    dummy_data = 0x55;
    RCON &= 0x7F;   //RCONbits.IPEN = 0, Disable priority level on interrupts
    INTCON=0;       //disable global interrupt and peripheral interrupts
    ANSELA=0;       //disable analog output on port A
    ANSELB=0;       //disable analog output on port B
    ANSELC=0;       //disable analog output on port C
    ANSELD=0;       //disable analog output on port D
    ANSELE=0;       //disable analog output on port E
    SSP1CON1=0x00;  //disable serial peripherals 1
    SSP2CON1=0x00;  //disable serial peripherals 2
    CM1CON0=0b00000000; //disable comparator 1
    CM2CON0=0b00000000; //disable comparator 2
    TRISA=0xFF;     //initialize as input for safety
    TRISD=0xFF;     //initialize as input for safety
    TRISB=0xFF;     //initialize as input for safety
    TRISC=0xFF;     //initialize as input for safety
    CTMUCONH=0x00;  //disables charge time measurement unit
    SRCON0=0x00;    //disable SR-latch
    VREFCON0=0x00;  //disables fixed voltage reference
    VREFCON1=0x00;  //disables the DDC
    HLVDCON=0x00;   //disables the high/low voltage detect module  
    __delay_ms(100);
    LM629_init();       //initializes the four LM629 (precision Motion Controllers)
    __delay_ms(100);
    spi_slave_init();   //initializes the SSP as an SPI slave device
    spi_data(dummy_data); //load the SPI buffer with a dummy
    all_off(); //initiate motors off
    signal_distribution_packet[1] = 16;
    signal_distribution_packet[2] = 16;
    signal_distribution_packet[3] = 16;
    signal_distribution_packet[4] = 16;
    
    //Every time this MCU receives a character via SPI, the ISR will read the buffer.
    //In the Super Loop (infinite loop, the value of the received character is read and the 
    //correct function is called, each function is meant to send commands to the
    //LM629s in order to generate the correct PWM signals which are fed to H-Bridges
    //to start mobilizing the robot
    
    while(1)
    {
        classic_ctrl.lx_joystick = signal_distribution_packet[1];
        classic_ctrl.ly_joystick = signal_distribution_packet[2];
        classic_ctrl.rx_joystick = signal_distribution_packet[3];
        classic_ctrl.ry_joystick = signal_distribution_packet[4];
        signal_distribution_packet[5] = '\n';
        
        set_absolute_velocity(1,classic_ctrl.ly_joystick);
        set_absolute_velocity(3,classic_ctrl.ry_joystick);
        //set_absolute_velocity(1,31);
        //set_absolute_velocity(3,31);


    } 
}
////////////////////////////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
//%/////////////////////////////End Main Loop////////////////////////////////%//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
////////////////////////////////////////////////////////////////////////////////

