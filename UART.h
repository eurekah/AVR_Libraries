/*

 UART_TEST.h
 Created: 6/18/2014 8:45:02 PM
 Author: Jeff Goodwill
  
Header Notes:  
	-BAUD_RATE and F_CPU must be defined in the main c file.  

6/18/2014
	Added:
		-Added ability to choose baud rate in main.c instead of fixed 9600
		-Added int 8 and int 16 transfer capabilities
	To Do:
		-Fix Baud Rate selection.  9600 works, 14400 doesn't.
		-Add string transmission 

7/10/2014
	Added:
		-send_uart_string function
	To Do:
		-Fix baud rate selection
7/30/2014
	Added:
		-Function descriptions
		-New functions planned to be implemented (Empty for now)
	To Do:
		-Add ability to change baud rate.
		-Implement the planned functions.
		-Test receive function.

7/31/2014:
	-Added binary output and unsigned 8-16 bit functions.
	
8/6/2014
	-Added 32-bit output functions.
	To Do: -Add ability to send float data.
			-Add read_uart functions
			-Add return values to indicate successful/failed transmissions
*/

//Defines
#define BAUD_PRESCALLER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

//Includes
#include <stdlib.h>								//Contains the itoa function
#include <avr/io.h>

//Declarations
void init_uart(void);
void send_uart(unsigned char);
void send_uart_int8(int8_t);
void send_uart_uint8(uint8_t);
void send_uart_int16(int16_t);
void send_uart_uint16(uint16_t);
void send_uart_binary8(uint8_t);
void send_uart_binary16(uint16_t);
void send_uart_string(char*, int);
int8_t receive_uart(void);

//Variable Declarations
char string[12];
uint8_t counter=0;

//Functions
/************************************************************************	
 *	Function:init_uart
 *  Purpose:Setup all the registers needed to begin uart communication.
 *	Input:baud rate (future addition)
 *	Output:
 *	Modified: July 30, 2014
************************************************************************/
void init_uart(void)
{
	//Set the BAUD rate.
	UBRR0H=(uint8_t)(BAUD_PRESCALLER>>8);
	UBRR0L=(uint8_t)(BAUD_PRESCALLER);
	
	//Setup registers for UART
	UCSR0A=(0<<U2X0);											//Asynchronous, normal mode(8-bit pre-scaler)
	UCSR0B=(0<<UCSZ02)|(1<<TXEN0)| (1<<RXEN0)|(1<<RXCIE0);		//Enable transmission and receiving, set data size, enable rx int
	UCSR0C=(!(3<<UMSEL00)|(0<<USBS0)|(3<<UCSZ00)|!(3<<UPM00));  //Asynchronous mode, 1 stop bit, no parity, 8bit data	
}

/************************************************************************	
 *	Function:send_uart
 *  Purpose: put one byte in the transmit buffer
 *	Input: Byte to be transmitted
 *	Output:
 *	Modified: July 30, 2014
************************************************************************/
void send_uart(unsigned char data)
{
	while(!(UCSR0A&(1<<UDRE0)));  //wait until TX buffer is empty
	UDR0=data;
}


/************************************************************************	
 *	Function: send_uart_int16
 *  Purpose: converts an integer before calling send_uart
 *	Input: signed int16
 *	Output:	
 *	Modified: July 31, 2014
************************************************************************/
void send_uart_int16(int16_t value)
{
	
	memset(string,0x00,12);
	
	itoa(value, string, 10);
	
	for (counter=0;counter<6;counter++)
	{
		if (!(string==0x00))
		{
			send_uart(string[counter]);
		}
	}
	//send new line and return to start
	send_uart(0x0A);
	send_uart(0x0D);
}

/************************************************************************	
 *	Function: send_uart_uint16
 *  Purpose: converts an integer before calling send_uart
 *	Input: unsigned int16
 *	Output:
 *	Modified: July 31, 2014	
************************************************************************/
void send_uart_uint16(uint16_t value)
{
	memset(string,0x00,12);
	
	counter=0;
	utoa(value, string, 10);
	
	for (counter=0;counter<7;counter++)
	{
		if (!(string==0x00))
		{
			send_uart(string[counter]);
		}
	}
	//send new line and return to start
	send_uart(0x0A);
	send_uart(0x0D);
}

/************************************************************************	
 *	Function: send_uart_int8
 *  Purpose: converts an integer before calling send_uart
 *	Input: signed int8
 *	Output:	
 *	Modified: July 31, 2014	
************************************************************************/
void send_uart_int8(int8_t value)
{
	itoa(value, string, 10);
	
	for (counter=0;counter<=3;counter++)
	{
		if (!(string==0x00))
		{
			send_uart(string[counter]);
		}
	}
	//send new line and return to start
	send_uart(0x0A);
	send_uart(0x0D);
}

/************************************************************************	
 *	Function: send_uart_uint8
 *  Purpose: converts an integer before calling send_uart
 *	Input: unsigned int8
 *	Output:	
 *	Modified: July 31, 2014	
************************************************************************/
void send_uart_uint8(uint8_t value)
{
	utoa(value, string, 10);
	
	for (counter=0;counter<=3;counter++)
	{
		if (!(string==0x00))
		{
			send_uart(string[counter]);
		}
	}
	//send new line and return to start
	send_uart(0x0A);
	send_uart(0x0D);
}

/************************************************************************	
 *	Function: send_uart_binary8
 *  Purpose: Will transmit over UART in a way that the binary will be displayed
 *	Input: int8
 *	Output:	
 *	Modified: July 31, 2014	
************************************************************************/
void send_uart_binary8(uint8_t value)
{
	int i =0;
	uint8_t mask=0x80;
	
	for(i=0;i<=7;i++)
	{
		if((mask&value)!=0)
		{
			send_uart(0x31); 
		}
		else if((mask&value)==0)
		{
			send_uart(0x30); 
		}
		else
		{
			//ERROR
		}
		mask=(mask>>1);
	}
	
	//send new line and return to start
	send_uart(0x0A);
	send_uart(0x0D);
}

/************************************************************************	
 *	Function: send_uart_binary16
 *  Purpose: Will transmit over UART in a way that the binary will be displayed
 *	Input: int16
 *	Output:	
 *	Modified: July 31, 2014	
************************************************************************/
void send_uart_binary16(uint16_t value)
{
		int i =0;
		uint16_t mask=0x8000;
		
		for(i=0;i<=15;i++)
		{
			if((mask&value)!=0)
			{
				send_uart(0x31);
			}
			else if((mask&value)==0)
			{
				send_uart(0x30);
			}
			else
			{
				//ERROR
			}
			mask=(mask>>1);
		}
		
	//send new line and return to start
	send_uart(0x0A);
	send_uart(0x0D);
}

/************************************************************************	
 *	Function:send_uart_string
 *  Purpose:Sends a string over UART
 *	Input:Character string to send, option to send linefeed command
 *	Output:
 *	Modified: July 30, 2014		
************************************************************************/
void send_uart_string(char* str,int LF)
{
	int i=0;
	while(str[i]!=0x00)
	{
		send_uart(str[i]);
		i++;
	}
	
	//If parameter is true send a line feed command after transferring a string
	if(LF==1)
	{
		//send new line and return to start
		send_uart(0x0A);
		send_uart(0x0D);
	}
}

/************************************************************************	
 *	Function: receive_uart
 *  Purpose: Retreive data from UART receive register
 *	Input:
 *	Output:	signed int8
 *	Modified: July 30, 2014	
************************************************************************/
int8_t receive_uart(void)
{
	while(!(UCSR0A&(1<<RXC0)));  //wait until RX buffer is full
	
	return UDR0;
}

/************************************************************************	
 *	Function: send_uart_int32
 *  Purpose: Sends a signed 32-bit integer over UART
 *	Input: signed 32-bit int to be sent
 *	Output:	
 *  Modified: August 6, 2014
************************************************************************/
void send_uart_int32(int32_t value)
{
	memset(string,0x00,12);
	
	counter=0;
	ltoa(value, string, 10);
	
	for (counter=0;counter<11;counter++)
	{
		if (!(string==0x00))
		{
			send_uart(string[counter]);
		}
	}
	//send new line and return to start
	send_uart(0x0A);
	send_uart(0x0D);
}

/************************************************************************	
 *	Function: send_uart_uint32
 *  Purpose: Sends a unsigned 32-bit integer over UART
 *	Input: unsigned 32-bit int to be sent
 *	Output:	
 *  Modified: August 6, 2014
************************************************************************/
void send_uart_uint32(uint32_t value)
{
	memset(string,0x00,12);
	
	counter=0;
	ltoa(value, string, 11);
	
	for (counter=0;counter<11;counter++)
	{
		if (!(string==0x00))
		{
			send_uart(string[counter]);
		}
	}
	//send new line and return to start
	send_uart(0x0A);
	send_uart(0x0D);
}