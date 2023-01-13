/*
 * File:   lm629.c
 * Author: eder0
 *
 * Created on June 10, 2022, 3:37 PM
 */


#include <xc.h>
#include "lm629.h"


//look up table
//This table is generated with Octave-7.2
//the script is located in "lookup_table_calculator.m" file
uint32_t lm629_velocity(uint8_t analog_value)
{
    uint32_t velocity;
    static const uint32_t lookup_table[32] = 
    {
 166072, 155693, 145313, 134934, 124554, 114175, 103795, 93416,
 83036, 72657, 62277, 51898, 41518, 31139, 20759, 10380,
 0, 11071, 22143, 33214, 44286, 55357, 66429, 77500,
 88572, 99643, 110715, 121786, 132858, 143929, 155001, 166072
    };
    
    velocity = lookup_table[analog_value];
    return velocity;
}
/***********************************************************************
Definition of Functions
***********************************************************************/

//Read the status register and check the value of bit 1 (busy_bit), if it equals
//to 1, keep polling the register until it becomes 0. 
void check_busy()
{
	uint8_t x;
	x=read_status();
	while (x & busy_bit)
		{
			x=read_status();
		}
}

//This function will return the status register, it returns a byte
uint8_t read_status()
{
	uint8_t status;
	DATABUS_DIR(0xFF); //Set the data bus mcu pins as inputs
	LM629_PS=0;        //To Read status register PS pin must be LOW
	_delay(3);        //PS setup time: T8 delay = min 30 ns
	LM629_RD=0;        //RD pin strobed to 0, status will remain valid while RD==0
	_delay(5);        //data will come valid at a max of 180ns 
	status=DATABUS(0xFF,0); //reading the status by reading the DATABUS
	LM629_RD=1;        //return RD to default RD=1
	_delay(3);       //RD setup time: T9 delay = 30 ns
	LM629_PS=1;        //return PS to default PS=1
    _delay(3);

return status;
}

// reads 16 bits of data from the LM629
// it returns a 16 bit number. 
//chip_select() must be called before if more than one LM629 is being used
uint16_t read_data()
{
	uint8_t left_value;
	uint8_t right_value;
	uint16_t data;
	DATABUS_DIR(0xFF); //set the MCU bus as an input to read values
	LM629_PS=1;        //PS must equal 1 to read data
	_delay(5);         //T10 delay
	LM629_RD=0;        //in order to read data, the RD pin must be strobed
	_delay(5);        //a delay is needed for data to become valid (180ns Max)
	left_value=DATABUS(0xFF,0); //save the MSB of the data
	LM629_RD=1;        //strobe needed for second byte. 
	_delay(5);         //T17 Read recovery time, Min=120ns
	LM629_RD=0;        //Strobing RD pin to read the LSB of the two-byte word 
	_delay(5);
	right_value=DATABUS(0xFF,0); //save the LSB of the data
	LM629_RD=1;        //Latches second byte and return to default RD=1
	_delay(5);
	LM629_PS=1;        //make sure RD still on default value of PS=1
	data=left_value;
	data=data<<8;
	data=data+right_value;	//joining the bytes to form a 16 bit word
    check_busy();    
	return data;
}

//writes 16 bits of data to the LM629
//chip_select() must be called before if more than one LM629 is being used
void write_data(uint8_t MSB, uint8_t LSB)
{
    check_busy();
	DATABUS_DIR(0); //Sets the MCU pins as outputs
	LM629_PS=1;     //To write data the PS pin must be set HIGH
	_delay(5);
	LM629_WR=0;     //To write data the WR pin must be strobe
	_delay(5);
	DATABUS(0,MSB); //Write MSB to the Databus pins
	_delay(5);
	LM629_WR=1;     //Latches first byte
	_delay(5);
	LM629_WR=0;     //needs to strobe for second byte
	_delay(5);
	DATABUS(0,LSB); //Write LSB to the databus pins
	_delay(5);
	LM629_WR=1;     //Latches second byte and returns to default value of WR=1
	_delay(5);
	LM629_PS=1;     //make sure PS still on default value of PS=1
    _delay(5);
    check_busy();
}

//writes command to the LM629
//chip_select() must be called before if more than one LM629 is being used
void write_command(uint8_t command)
{
    check_busy();
	DATABUS_DIR(0); //set the MCU pin as outputs
	LM629_PS=0;     //to write commands the PS pin must be set LOW
	_delay(5);
	LM629_WR=0;     //WR must be strobe to write into the bus
	_delay(5);
	DATABUS(0,command); //Puts command on the bus
	_delay(5);
	LM629_WR=1;     //latches the command into the LM629
	_delay(5);
	LM629_PS=1;     //return PS to default PS=1
	_delay(5);
    check_busy();
}

void LM629_init()
{
	uint8_t x;
	x=0;
    LM629_RD_DIR = 0;
    LM629_PS_DIR = 0;
    LM629_WR_DIR = 0;
    LM629_RST_DIR= 0;
    LM629_0_CS_DIR = 0;
    LM629_1_CS_DIR = 0;
    LM629_2_CS_DIR = 0;
    LM629_3_CS_DIR = 0;
	LM629_PS=1;
	LM629_RD=1;
	LM629_WR=1;
	LM629_RST=1;
	chip_select(0);
beginning:
	__delay_us(10);
	LM629_RST=0;
	__delay_us(12);
	LM629_RST=1;
	__delay_ms(2);
	x=read_status();
	if(!(x==0xC4 || x==0x84))
		{
		goto beginning;
		}
	else
		{
			check_busy();
			write_command(RSTI);
        	check_busy();
			write_data(0x00,0x00);
			check_busy();
			x=read_status();
            if(!(x==0xC0 || x==0x80))
				{	
				goto beginning;
				}
		}

	chip_select(1);
beginning1:
	__delay_us(10);
	LM629_RST=0;
	__delay_ms(2);
	LM629_RST=1;
	__delay_ms(2);
	x=read_status();
	if(!(x==0xC4 || x==0x84))
		{
		goto beginning1;
		}
	else
		{
			check_busy();
			write_command(0x1D);
        	check_busy();
			write_data(0x00,0x00);
			check_busy();
			x=read_status();
            if(!(x==0xC0 || x==0x80))
				{	
				goto beginning1;
				}
		}

    chip_select(2);
beginning2:
	__delay_us(10);
	LM629_RST=0;
	__delay_us(12);
	LM629_RST=1;
	__delay_ms(2);
	x=read_status();
	if(!(x==0xC4 || x==0x84))
		{
		goto beginning2;
		}
	else
		{
			check_busy();
			write_command(0x1D);
        	check_busy();
			write_data(0x00,0x00);
			check_busy();
			x=read_status();
            if(!(x==0xC0 || x==0x80))
				{	
				goto beginning2;
				}
		}
    
	chip_select(3);
beginning3:
	__delay_us(10);
	LM629_RST=0;
	__delay_ms(1);
	LM629_RST=1;
	__delay_ms(2);
	x=read_status();
	if(!(x==0xC4 || x==0x84))
		{
		goto beginning3;
		}
	else
		{
			check_busy();
			write_command(0x1D);
        	check_busy();
			write_data(0x00,0x00);
			check_busy();
			x=read_status();
            if(!(x==0xC0 || x==0x80))
				{	
				goto beginning3;
				}
		}
chip_select(0);
filter_module();
chip_select(1);
filter_module();
chip_select(2);
filter_module();
chip_select(3);
filter_module();
set_absolute_acceleration(0,0x00000250); //set this acceleration through trial and error in such a way it had a comfortable reaction
set_absolute_acceleration(1,0x00000250);
set_absolute_acceleration(2,0x00000250);
set_absolute_acceleration(3,0x00000250);
}

//Since the same bus is used for both reading and writing
//the direction of the bus must be changed to input or output from the mcu
//point of view before writing or reading data to the bus
//if arg is 0x00, the direction will be OUT
//if arg is 0xFF, the direction will be IN
void DATABUS_DIR(uint8_t dir)
{
    if(dir==0)
    {
        TRISC&=0b00111111;
        TRISD&=0b00001111;
        TRISB&=0b11111100;
    }
    else if(dir==0xFF)
    {
        TRISC|=0b11000000;
        TRISD|=0b11110000;
        TRISB|=0b00000011;
    }
}

//either read or write data to the pins connected to the DATABUS in the correct order (connected to the lm629)
//if dirl is 0x00, byte0 must be passed to the DATABUS in the correct order
//if dirl is 0xFF, The values in the DATABUS must be returned in the correct order
uint8_t DATABUS(uint8_t dir1, uint8_t byte0)
{
uint8_t x;
if(dir1==0)
{ 
    D0 = ((byte0 & 0x01) >> 0);
    D1 = ((byte0 & 0x02) >> 1);
    D2 = ((byte0 & 0x04) >> 2);
    D3 = ((byte0 & 0x08) >> 3);
    D4 = ((byte0 & 0x10) >> 4);
    D5 = ((byte0 & 0x20) >> 5);
    D6 = ((byte0 & 0x40) >> 6);
    D7 = ((byte0 & 0x80) >> 7);
    x=0;
}

else if (dir1==0xFF)
{
         
    x |= D7;
    x = (x<<1) | D6;
    x = (x<<1) | D5;
    x = (x<<1) | D4;
    x = (x<<1) | D3;
    x = (x<<1) | D2;
    x = (x<<1) | D1;
    x = (x<<1) | D0; 
}
return x;
}

void chip_select(uint8_t chip)
{
	if(chip==0)
	{	
		LM629_0_CS=0;
		LM629_1_CS=1;
		LM629_2_CS=1;
		LM629_3_CS=1;
	}
	else if(chip==1)
	{
		LM629_0_CS=1;
		LM629_1_CS=0;
		LM629_2_CS=1;
		LM629_3_CS=1;
	}
	else if(chip==2)
	{
		LM629_0_CS=1;
		LM629_1_CS=1;
		LM629_2_CS=0;
		LM629_3_CS=1;
	}
	else if(chip==3)
	{
		LM629_0_CS=1;
		LM629_1_CS=1;
		LM629_2_CS=1;
		LM629_3_CS=0;
	}
	else 
	{
		LM629_0_CS=1;
		LM629_1_CS=1;
		LM629_2_CS=1;
		LM629_3_CS=1;	
	}		

	__delay_us(1);
}

void motor_off()
{
    write_command(LTRJ);
    write_data(0x01,0x00);
    write_command(STT);
}

void motor_break()
{
    write_command(LTRJ);
    write_data(0x02,0x00);
    write_command(STT);
}

void all_break()
{
    chip_select(0);
    motor_break();
    chip_select(1);
    motor_break();
    chip_select(2);
    motor_break();
    chip_select(3);
    motor_break();
}

void all_off()
{
    chip_select(0);
    motor_off();
    chip_select(1);
    motor_off();
    chip_select(2);
    motor_off();
    chip_select(3);
    motor_off();
}

void filter_module()
{
    write_command(LFIL);
    write_data(0x00,0x0F); //set all coefficients kp, ki, kd, il
    write_data(0x01,0x0F); //kp
    write_data(0x00,0x04); //ki
    write_data(0x7F,0xFF); //kd
    write_data(0x00,0xFF); //il
    write_command(UDF);
}

void simple_absolute_position()
{
    write_command(LTRJ);
    write_data(0x00,0x24);
    write_data(0x00,0x00);
    write_data(0x00,0x02);
    write_data(0x00,0x00);
    write_data(0x34,0x6E);
    write_data(0x00,0x00);
    write_data(0x1F,0x40);
    write_command(STT);
}


void simple_relative_position()
{
    write_command(LTRJ);
    write_data(0x00,0x2B); //All parameter will be loaded Vel & Acc
    write_data(0x00,0x00); //Acc. High
    write_data(0x00,0x40); //Acc. Low
    write_data(0x00,0x05); //Vel. High
    write_data(0x75,0x3F); //Vel. Low
    write_data(0x00,0x01); //Pos. High (0xff,0xFE)
    write_data(0x00,0x40); //Pos. Low
    write_command(STT);
}

void set_absolute_acceleration(uint8_t motor, uint32_t acceleration_value)
{
    uint8_t trajectory_control_MSB = 0x00; 
    uint8_t trajectory_control_LSB = 0x20; //acceleration will be loaded and is not relative
    uint8_t high_word_MSB;
    uint8_t high_word_LSB;
    uint8_t low_word_MSB;
    uint8_t low_word_LSB;
    high_word_MSB = ((acceleration_value & 0xFF000000) >> 24 ); 
    high_word_LSB = ((acceleration_value & 0x00FF0000) >> 16 );
    low_word_MSB = ((acceleration_value & 0x0000FF00) >> 8 );
    low_word_LSB = ((acceleration_value & 0x000000FF) >> 0 );
    
    chip_select(motor);
    write_command(LTRJ);
    write_data(trajectory_control_MSB,trajectory_control_LSB);
    write_data(high_word_MSB, high_word_LSB);
    write_data(low_word_MSB, low_word_LSB);
    write_command(STT);
    chip_select(5);
}

void set_absolute_velocity(uint8_t motor, uint8_t analog_in)
{
    uint8_t trajectory_control_MSB = 0x18; //direction is forward by default, velocity mode
    uint8_t trajectory_control_LSB = 0x08; //velocity will be loaded and is not relative
    uint32_t velocity_value;
    uint8_t high_word_MSB;
    uint8_t high_word_LSB;
    uint8_t low_word_MSB;
    uint8_t low_word_LSB;
    
    
    switch(motor)
    {
            case ((2)|(3)): //this is the left motor, it needs to spin opposite direction as the right motor
            {
                if(analog_in <= 15)
                {
                    trajectory_control_MSB &= 0x0F;
                }
                else
                {
                    trajectory_control_MSB |= 0x10;
                }
                break;
            }
            case((0)|(1)): //this is the right motor, it needs to spin opposite direction as the left motor
            {
                if(analog_in > 15)
                {
                    trajectory_control_MSB &= 0x0F;
                }
                else
                {
                    trajectory_control_MSB |= 0x10;
                }
                break;                
            }
    }

    velocity_value = lm629_velocity(analog_in);
    high_word_MSB = ((velocity_value & 0xFF000000) >> 24 ); 
    high_word_LSB = ((velocity_value & 0x00FF0000) >> 16 );
    low_word_MSB = ((velocity_value & 0x0000FF00) >> 8 );
    low_word_LSB = ((velocity_value & 0x000000FF) >> 0 );
    
    chip_select(motor);
    write_command(LTRJ);
    write_data(trajectory_control_MSB, trajectory_control_LSB); 
    write_data(high_word_MSB, high_word_LSB);
    write_data(low_word_MSB, low_word_LSB);
    write_command(STT);
    chip_select(5);
     
}
/***********************************************************************
End of definition of functions
***********************************************************************/