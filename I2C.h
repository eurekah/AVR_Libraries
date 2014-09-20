/********************************************************************
	Created: 7/5/2014 8:26:00 PM
	filename: 	E:\Dropbox\Electronics\Code\AVR\Headers\I2C.h
	file path:	E:\Dropbox\Electronics\Code\AVR\Headers
	file base:	I2C
	file ext:	h
	author:		Jeff Goodwill
	
	purpose: This header contains all the functions needed to communicate 
			 to an I2C device.
				
	Notes:  -Need to add the ability to read and interpret the I2C status register			
*********************************************************************/

//Definitions
#define READ 1
#define WRITE 0

//Function Prototypes
uint8_t i2c_init(void);
uint8_t i2c_start(void);
uint8_t i2c_write(uint8_t data);
uint8_t i2c_Read_Ack(void);
uint8_t i2c_Read_noAck(void);
uint8_t i2c_status(void);

uint8_t i2c_init(void)
{
	//Setup bit rate register for 100kHz
	TWBR=0x72;
	//Setup pre-scaler value to 1
	TWSR=0x00;	
	
	return 0;
}

uint8_t i2c_start(void)
{
	//Clear TWI Interrupt flag, send start condition, enable TWI.
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	//Wait for start condition to be sent.
	while( !(TWCR & (1<<TWINT)) );
	
	return 0;
}

uint8_t i2c_stop(void)
{
	//Send stop condition
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	
	return 0;
}

uint8_t i2c_write(uint8_t data)
{
	//Load data to be sent into TWI data register
	TWDR=data;
	//Send data
	TWCR=(1<<TWINT)|(1<<TWEN);
	//Wait for data to be transmitted
	while((TWCR&(1<<TWINT))==0);
	
	return 0;
}



//This has yet to be fully implemented! The status can be read, but
//I want to add the ability to return the actual error that is associated 
//with the status register value.
uint8_t i2c_status(void)
{
	uint8_t statusReg;
	
	statusReg=TWSR&0xF8;
	
	return statusReg;
}


uint8_t i2c_Read_Ack(void)
{
	//Setup i2c to receive data, send ack
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	//Wait for data to be received
	while ((TWCR & (1<<TWINT)) == 0);
	
	return TWDR;
}


uint8_t i2c_Read_noAck(void)
{
	//Setup i2c to receive data, send don't ack
	TWCR = (1<<TWINT)|(1<<TWEN);
	//Wait for data to be received
	while ((TWCR & (1<<TWINT)) == 0);
	
	return TWDR;
}