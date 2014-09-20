/********************************************************************
	created:	2014/08/01
	created:	1:8:2014   21:31
	filename: 	E:\Dropbox\Electronics\Code\AVR\Headers\myDHT11.h
	file path:	E:\Dropbox\Electronics\Code\AVR\Headers
	file base:	myDHT11
	file ext:	h
	author:		Jeff Goodwill
	
	purpose:	This header file is used to retrieve the temperature and humidity data from the DHT11 sensor.
				Only one function is needed.  A 8-bit array of 2 bytes is inputed into the function and the function uses pointers to change the data. Array index 0 is temperature 1 is humidity
				
				Currently set for PB0
				
	Updates: 9/13/2014 - Removed unnecessary global variables
*********************************************************************/

//Includes
#include <util/delay.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

//Function Declarations
uint8_t myDHT11_retrieve(uint8_t*);

//Functions
uint8_t myDHT11_retrieve(uint8_t* returnData)
{
	//Disable interrupts
	cli();
	
	//Variable Declarations
	uint8_t bytes[5]; //5 bytes to hold sensor data
	uint8_t byte_count, bit_count;
	uint8_t byte_result;
	uint8_t	i;
	uint8_t sum;
	
	//SETUP PORTS
	DDRB|=(1<<PB0); //output
	PORTB|=(1<<PB0); //high
	_delay_ms(100);  

	//SEND REQUEST
	PORTB&=~(1<<PB0); //low
	_delay_ms(18);
	
	PORTB|=(1<<PB0); //high
	_delay_us(39);
	
	//Change port to input.  The 40us delay was split into 2 delays to avoid an issue 
	//when the port value would be changed at the end of he 40us.
	DDRB&=~(1<<PB0); //input
	_delay_us(1);
		
	//RESPONSE
	//This will work by reading the input 1/2 through the length of a level change to see if the pin is high or low.
		
	//Wait for 0.5*54us=27us. input should be low.
	_delay_us(27);
	if((PINB&(1<<PB0)))
	{
		//Send error if input isn't low
		send_uart_string("Error: myDHT11_retrieve 1",1);
		return 1;
	}
	
	//wait for 0.5*54us+0.5*80us=67, input should be high
	_delay_us(67);
	if(!(PINB & (1<<PB0)))
	{
		send_uart_string("Error: myDHT11_retrieve 2",1);
		return 2;
	}
		
	//Wait for request to be finished.
	_delay_us(80);
	
	
	//DATA READING
	for(byte_count=0;byte_count<5;byte_count++)
	{
		byte_result=0;
		
		for (bit_count=0;bit_count<8;bit_count++)
		{
			//Wait until pin goes high
			
			while(!(PINB & (1<<PB0)));
			
			//Wait 35ms, if pin is low -> bit=0 if pin high bit=1
			_delay_us(35);
	
			if(PINB&(1<<PB0))
			{
				//Device send MSB first
				byte_result= byte_result | (1<<(7-bit_count));
			}	
			
			//Wait until pin goes low
			while(PINB&(1<<PB0));	
		}
		bytes[byte_count]=byte_result;
	}
			
	//Check Check Sum
	sum=0;	
	for(i=0;i<4;i++)
		sum=sum+bytes[i];
		
	if(sum!=bytes[4])
	{	
		//Check sum failed
		send_uart_string("Error: myDHT11_retrieve 3",1);
		return 3;
	}
	

	
	returnData[0]=bytes[2];
	returnData[1]=bytes[0];
	
	
	
	/*	Debugging info
	send_uart_string("Start",1);
	for(i=0;i<5;i++)
	send_uart_uint8(bytes[i]);
	
	send_uart_uint8(returnData[0]);
	send_uart_uint8(returnData[1]);
	send_uart_string("Done",1);*/
	
	//Enable interrupts
	sei();
	return 0;
}
