/*
 * File:   spi.c
 * Author: Eder Torres
 *
 * These functions are used to initialize the SPI module as a slave device with interrupts enabled
 * spi_data() can load the buffer with a value. this function is not needed in this project
 * The ISR checks the serial peripheral interrupt flag to check if the interrupt was trigger by the spi module and loads the buffer with a dummy value
 */
#include <xc.h>
#include "spi.h"
volatile uint8_t spi_read_data;
volatile uint8_t recording_on;
volatile uint8_t *spi_str_interrupt;
extern uint8_t signal_distribution_packet[];

void spi_slave_init()
{
    SSP1STATbits.SMP = 0;  //must be cleared for slave mode
    SSP1STATbits.CKE = 1; //Trasnmit occurs on transition from active to idle clock state
    SSP1CON1bits.CKP = 0;
    SSP1CON1bits.SSPM = 0x04; //SPI Slave mode, csk pin, and ss pin are enabled
    SSP1CON3 = 0b00010000;  //SSP1BUF update every time that a new data byte is shifted in ignoring the BF bit
    ADCON0 = 0x00;          //disable adc0 circuitry
    ADCON1 = 0x00;          //disable adc1 circuitry
    SS1_DIR = 1;            //set pin for~ss1 as an input 
    SCLK1_DIR = 1;          //set pin for sclk as an input
    SDO1_DIR = 0;           //set pin for sdo as an output
    SDI1_DIR = 1;           //set pin for sdi as an input
    SSP1IE=1;               //enable ssp interrupt
    PEIE=1;                 //enable peripheral interrupt
    GIE=1;                  //enable global interrupt
    SSP1CON1bits.SSPEN = 1; //SPI enabled
}

void spi_data(uint8_t tx_data)
{
    SSP1BUF=tx_data;
}

//Interrupt Service routing, it services interrupts coming from the SPI peripheral

void __interrupt() SPI_ISR(void)
{
    if(PIR1bits.SSP1IF)
    {
        spi_read_data=SSP1BUF;  //Save content in a global variable
        if(spi_read_data == 'z') //'z' character marks the start of a valid packaged coming
        {
            recording_on = 1;    //we want to save the coming string so we set a flag to do it. 
        }
        else if(spi_read_data == 'y') //if the character received is 'y', it means that the packages is finished
        {
            recording_on = 0;
        }
        else
        {
            recording_on = recording_on;
        }
        
        switch(recording_on) 
        {
            case(0):
                spi_str_interrupt = &signal_distribution_packet; //if the recording is "off", point "spi_str_interrupt" to "signal_distribution_packet[0]" address
                break;
            case(1):
                *spi_str_interrupt++ = spi_read_data;           //if the recording is "on", save the byte received over SPI to the address pointed and increase the address to the next element of the array
                break;
            default:
                *spi_str_interrupt = *spi_str_interrupt;       
                break;
        } 
        PIR1bits.SSP1IF=0;                                     //clear the flag to 
        SSP1BUF = 0x55;         //fill the buffer with dummy data
    }
}

/***********************************************************************
End of definition of functions
***********************************************************************/
