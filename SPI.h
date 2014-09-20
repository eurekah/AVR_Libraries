 /********************************************************************
	created:	2014/07/18
	created:	18:7:2014   14:40
	filename: 	C:\AVR\Headers\SPI.h
	file path:	C:\AVR\Headers
	file base:	SPI
	file ext:	h
	author:		Jeff Goodwill
	
	2014/07/18
		-Implemented a simple SPI interface to communicate with an LCD display.  
		-Currently set to 4 Mhz 
		-send_spi works
		-read_spi is not tested	
*********************************************************************/


void init_spi(void);
void send_spi(uint8_t data);
uint8_t read_spi(void);

void init_spi()
{
	//Set MOSI and SCK pins as output(Pins PB5 and PB3)
	DDRB|=(1<<PORTB5)|(1<<PORTB3)|(1<<PORTB2);
	
	//Enable SPI Master, set clock rate(4 MHZ is default)
	SPCR|=(1<<SPE)|(1<<MSTR);
}

void send_spi(uint8_t data)
{
	//Put data in transfer buffer
	SPDR=data;

	//Wait for transfer to complete
	while(!(SPSR & (1<<SPIF)));
}

//Not tested
uint8_t read_spi(void)
{
	//Send dummy data
	SPDR=0x00;
	
	//Wait for transmission to complete
	while(!(SPSR & (1<<SPIF)));
	
	//Return input
	return SPDR;
}


