/**********************************************************************
University of Texas - Pan American
Eder Torres
Sept. 13, 2014
This header file will have data to facilitate the lm629 programming
**********************************************************************/


/**********************************************************************
Revisions
***********************************************************************
REV. 1: September 13, 2014: File created to have a cleaner main file
Command addresses were defined by their name. 
Pin Declaration has been started
************************************************************************
End of Revisions
***********************************************************************/




#ifndef LM629_H
#define LM629_H
#include <xc.h>
#define _XTAL_FREQ 64000000       //Use 64MHz as FOSC (16MHz Crystal with 4X PLL)
/***********************************************************************
Declaration
***********************************************************************/
//LM629 Command Addresses
#define LM629_RESET    0x00 // Reset LM629
#define DFH      0x02 // Define Home
#define SIP      0x03 // Set Index Position
#define LPEI     0x1B // Interrupt on Error
#define LPES     0x1A // Stop on Error
#define SBPA     0x20 // Set Breakpoint, Absolute
#define SBPR     0x21 // Set Breakpoint, Relative
#define MSKI     0x1C // Mask Interrupts
#define RSTI     0x1D // Reset Interrupts
#define LFIL     0x1E // Load Filter Parameters
#define UDF      0x04 // Update Filter
#define LTRJ     0x1F // Load Trajectory
#define STT      0x01 // Start Motion
#define RDSIGS   0x0C // Read Signals Register
#define RDIP     0x09 // Read Index Position
#define RDDP     0x08 // Read Desired Position
#define RDRP     0x0A // Real Real Position
#define RDDV     0x07 // Read Desired Velocity
#define RDRV     0x0B // Read Real Velocity
#define RDSUM    0x0D // Read Integration Sum
#define INPUT    0xFF // To Set a port as input
#define OUTPUT   0x00 // To Set a port as output
#define busy_bit    0x01//bit 0 of the status register is the busy bit is logic "high" no command write may take place

//Pins Declaration
#define LM629_0_CS  PORTDbits.RD3 //Chip Select 0
#define LM629_1_CS  PORTBbits.RB2 //Chip Select 1
#define LM629_2_CS  PORTAbits.RA0 //Chip Select 2
#define LM629_3_CS  PORTBbits.RB4 //Chip Select 3
#define LM629_0_CS_DIR TRISDbits.TRISD3
#define LM629_1_CS_DIR TRISBbits.TRISB2
#define LM629_2_CS_DIR TRISAbits.TRISA0
#define LM629_3_CS_DIR TRISBbits.TRISB4


#define LM629_RD    PORTDbits.RD2 //~Read
#define LM629_PS    PORTCbits.RC2 //~Port Select
#define LM629_WR    PORTDbits.RD1 //~Write (command byte is latched into the LM629 on the rising edge)
#define LM629_RST   PORTBbits.RB5
#define LM629_RD_DIR    TRISDbits.TRISD2
#define LM629_PS_DIR    TRISCbits.TRISC2 
#define LM629_WR_DIR    TRISDbits.TRISD1 
#define LM629_RST_DIR   TRISBbits.TRISB5


#define D0          PORTCbits.RC6
#define D1          PORTCbits.RC7
#define D2          PORTDbits.RD4
#define D3          PORTDbits.RD5
#define D4          PORTDbits.RD6
#define D5          PORTDbits.RD7
#define D6          PORTBbits.RB0
#define D7          PORTBbits.RB1

//Procedure Declaration
void LM629_init(void);
void DATABUS_DIR(unsigned char dir);     //Added in REV 5 for new pin assignment
unsigned char DATABUS(unsigned char dir1, unsigned char byte0);// Added in Rev 5 for new pin assigment
void LM629_velocity_start(const unsigned char chip, unsigned long vel);
void LM629_position_start(const unsigned char chip, const unsigned long vel, const long pos);
void check_busy(void);
unsigned char read_status(void);
unsigned int read_data(void);
void write_data(uint8_t MSB, uint8_t LSB);
void write_command(unsigned char command);
void chip_select(unsigned char chip);
void motor_break(void);
void all_break(void);
void motor_off(void);
void all_off(void);
void filter_module(void);
void simple_absolute_position(void);
void simple_relative_position(void);
void set_absolute_velocity(uint8_t motor, uint8_t analog_in);
uint32_t lm629_velocity(uint8_t analog_value);
void set_absolute_acceleration(uint8_t motor, uint32_t acceleration);
/***********************************************************************
End of Declaration
***********************************************************************/

#endif 