/********************************************************************
	created:	2014/08/04
	created:	4:8:2014   16:56
	filename: 	E:\Dropbox\Electronics\Code\AVR\Headers\BMP_180.h
	file path:	E:\Dropbox\Electronics\Code\AVR\Headers
	file base:	BMP_180
	file ext:	h
	author:		Jeff Goodwill
	
				I2C library is required.
	
	purpose:	Used to interface with BMP180 sensor.  
				
	Notes:  uread and sread are redundant.  Next update I will change so 
			only one exists that returns the data as a char, then the programmer
			will need to indicate if the data is actually signed or unsigned.
*********************************************************************/


#define BMP180_ADDRESS 0xEE
#include <math.h>

//Declarations
uint8_t bmp180_start(void);
uint16_t bmp180_uread(uint8_t);
int16_t bmp180_sread(uint8_t);
int16_t bmp180_getTemperature(void);
int32_t bmp180_getData(char);

//Calibration variables
int16_t AC1, AC2,AC3,B1,B2,MB,MC,MD;
uint16_t AC4,AC5,AC6;

/************************************************************************	
 *	Function: bmp180_start
 *  Purpose: Read the calibration data from the sensor.  Store data in registers
 *	Input: N/A
 *	Output:	Returns 0 for success
************************************************************************/
uint8_t bmp180_start(void)
{
	//Read the calibration coefficients
	
	AC1=0;
	AC1=bmp180_sread(0xAA);
	
	AC2=0;
	AC2=bmp180_sread(0xAC);
	
	AC3=0;
	AC3=bmp180_sread(0xAE);
	
	AC4=0;
	AC4=bmp180_uread(0xB0);
	
	AC5=0;
	AC5=bmp180_uread(0xB2);
	
	AC6=0;
	AC6=bmp180_uread(0xB4);
	
	B1=0;
	B1=bmp180_sread(0xB6);
	
	B2=0;
	B2=bmp180_sread(0xB8);
	
	MB=0;
	MB=bmp180_sread(0xBA);
	
	MC=0;
	MC=bmp180_sread(0xBC);
	
	MD=0;
	MD=bmp180_sread(0xBE);
	
	
	 /* Debug Output
		send_uart_string("Calibration",1);
		send_uart_int16(AC1);
		send_uart_int16(AC2);
		send_uart_int16(AC3);
		
		send_uart_uint16(AC4);
		send_uart_uint16(AC5);
		send_uart_uint16(AC6);
		
		send_uart_int16(B1);
		send_uart_int16(B2);
		send_uart_int16(MB);
		send_uart_int16(MC);
		send_uart_int16(MD);	*/
	

	return 0;
}

/************************************************************************	
 *	Function: bmp180_uread
 *  Purpose: read 2 bytes of unsigned data starting from the provided register
 *	Input: First register to read, value isMSB
 *	Output:	Returns 16bit value read
************************************************************************/
uint16_t bmp180_uread(uint8_t value)
{
	uint8_t temp_data[2];
	uint16_t total;
	
	temp_data[0]=0;
	temp_data[1]=0;
	total=0;
	
	i2c_start();
	i2c_write(BMP180_ADDRESS);
	i2c_write(value);
	
	i2c_start();
	i2c_write(BMP180_ADDRESS+1);
	
	temp_data[0]=i2c_Read_Ack();
	temp_data[1]=i2c_Read_noAck();
	
	i2c_stop();
	
	total=(temp_data[0]<<8)|(temp_data[1]);
	
	/* Debug Output
	send_uart_uint8(temp_data[0]);
	send_uart_uint8(temp_data[1]);
	send_uart_uint16(total);*/
	
	return total;
}

/************************************************************************	
 *	Function: bmp180_uread
 *  Purpose: read 2 bytes of signed data starting from the provided register
 *	Input: First register to read, value isMSB
 *	Output:	Returns 16bit value read
************************************************************************/
int16_t bmp180_sread(uint8_t value)
{
	uint8_t temp_data[2];
	uint16_t total;

	temp_data[0]=0;
	temp_data[1]=0;
	total=0;
	
	i2c_start();
	i2c_write(BMP180_ADDRESS);
	i2c_write(value);
	
	i2c_start();
	i2c_write(BMP180_ADDRESS+1);
	
	temp_data[0]=i2c_Read_Ack();
	temp_data[1]=i2c_Read_noAck();
	
	i2c_stop();
	
	total=(temp_data[0]<<8)|(temp_data[1]);
	
	/* Debug Output
	send_uart_uint8(temp_data[0]);
	send_uart_uint8(temp_data[1]);
	send_uart_uint16(total);*/
	
	return total;
}

/************************************************************************	
 *	Function: bmp180_getData
 *  Purpose: Initiate conversion, read, and return temperature or pressure data.
 *	Input: Select if temperature (choice='T') or pressure(choice='P') data should be returned
 *	Output:	32-bit data containing temperature or pressure data.
************************************************************************/
int32_t bmp180_getData(char choice)
{	
	int32_t presssure_kpa;
	uint8_t tdata[2];
	tdata[0]=tdata[1]=0;
	uint8_t pdata[2];
	pdata[0]=pdata[1]=0;

	uint16_t press;
	int16_t temp,Temperature, kPa;
	double B4, B7;
	double X1,X2,X3,B5,B6,B3,p;
	
	
/////Get temperature data
	i2c_start();
	i2c_write(BMP180_ADDRESS);
	i2c_write(0xF4);
	i2c_write(0x2E);
	i2c_stop();
	_delay_ms(5);
	
	i2c_start();
	i2c_write(BMP180_ADDRESS);
	i2c_write(0xF6);
	i2c_start();
	i2c_write(BMP180_ADDRESS+1);
	tdata[0]=i2c_Read_Ack();
	tdata[1]=i2c_Read_noAck();
	i2c_stop();
	temp=((tdata[0]<<8)|(tdata[1]));
	
/////Calculate temperature
	X1=((double)(temp-AC6)*AC5)/32768;
	X2=(double)MC*2048/(X1+(double)MD);
	B5=X1+X2;
	Temperature=(int16_t)((B5+8)/16);
	/*send_uart_string("Temperature",1);
	send_uart_int16(T);*/

/////Get pressure data
	i2c_start();
	i2c_write(BMP180_ADDRESS);
	i2c_write(0xF4);
	i2c_write(0x34);
	i2c_stop();
	_delay_ms(5);
	i2c_start();
	i2c_write(BMP180_ADDRESS);
	i2c_write(0xF6);
	i2c_start();
	i2c_write(BMP180_ADDRESS+1);
	pdata[0]=i2c_Read_Ack();
	pdata[1]=i2c_Read_noAck();
	i2c_stop();
	press=0;
	press=((pdata[0]<<8)|(pdata[1]));
	
//////Calculate pressure values
	B6=B5-4000;
	X1=(B2*(B6*B6/4096))/2048;
	X2=(double)AC2*B6/2048;
	X3=X1+X2;
	B3=((((double)AC1*4+X3))+2)/4;
	X1=(double)AC3*B6/8192;	
	X2=((double)B1*(B6*B6/4096))/65536;	
	X3=((X1+X2)+2)/4;		
	B4=((double)AC4*((X3+32768)/32768));
	B7=((double)press-B3)*50000;
	if((int32_t)B7<0x80000000)
	{
		p=B7*2/B4;		
	}
	else
	{
		p=(B7/B4)*2;
	}
	X1=(p/256)*(p/256);
	X1=(X1*3038)/65536;		
	X2=(-7357*p)/65536;	
	p=p+(X1+X2+3791)/16;
	presssure_kpa=(int32_t)p;

	/*send_uart_string("Pressure",1);
	send_uart_int32(presssure_kpa);*/
	
	if (choice=='T')
	{
		return Temperature;
	} 
	else if(choice=='P')
	{
		return presssure_kpa;
	}
	else
	{
		return 0;
	}
	
	
	return 0;
}

