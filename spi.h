/**********************************************************************
University of Texas - Pan American
Eder Torres
Oct. 18, 2014
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
#ifndef SPI_H
#define SPI_H

#include <xc.h>
#define _XTAL_FREQ 64000000

#define SS1 PORTAbits.RA5
#define SCLK1 PORTCbits.RC3
#define SDI1 PORTCbits.RC4
#define SDO1 PORTCbits.RC5

#define SS1_DIR TRISAbits.RA5
#define SCLK1_DIR TRISCbits.RC3
#define SDI1_DIR TRISCbits.RC4
#define SDO1_DIR TRISCbits.RC5

void spi_slave_init(void);
void spi_data(uint8_t tx_data);


#endif 

