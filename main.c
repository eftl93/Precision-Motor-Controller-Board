/*
 * File:   main.c
 * Author: Eder Torres
 *
 * This file initializes the MCU into a safe state
 * it creates a struct for data received through SPI.
 * 
 *  
 */


#include <xc.h>                  
#include "lm629.h"
#include "spi.h"
#include "main.h"
#define _XTAL_FREQ 64000000       //Use 64MHz as FOSC (16MHz Crystal with 4X PLL)

//Global variable declarations
extern volatile uint8_t spi_read_data;
uint8_t signal_distribution_packet[5]; //determines the size of the packet to be received
                                       //[0] will store the store the character "z"


struct spi_package //the elements of the struct can be changed to whatever kind of data is expected to be received
{
    uint8_t lx_joystick;
    uint8_t ly_joystick;
    uint8_t rx_joystick;
    uint8_t ry_joystick;
};

struct spi_package controller; //This code was tested using a controller with two joysticks


void main(void)
{
    uint8_t received_data;
    uint8_t dummy_data;
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
        controller.lx_joystick = signal_distribution_packet[1];
        controller.ly_joystick = signal_distribution_packet[2];
        controller.rx_joystick = signal_distribution_packet[3];
        controller.ry_joystick = signal_distribution_packet[4];
        
        //the values received through spi from each joystick have a value from 0 to 31
        //in "lm629.c" file, there is a look up table that converts this value into a speed with units of samples/period (quadrature_encoder_samples * Fclock_lm629 )
        //and direction of the motor. 0-15, gives a negative direction value to motor 2 and 3 and if its from 16-31 it gives a positive direction to motor 2 and 3 (it's the opposite for motor 0 and 1)
        //if the joystick value is 16, the speed will be 0. If the joystick value is 0 or 31, the speed of the motor will be the maximum written on the look up table in the
        //"lm629.c" file
        set_absolute_velocity(1,controller.ly_joystick);
        set_absolute_velocity(3,controller.ry_joystick);
        //set_absolute_velocity(0,31);
        //set_absolute_velocity(2,31);


    } 
}


