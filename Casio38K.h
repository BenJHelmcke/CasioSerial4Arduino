
/*
 * Copyright (c) Ben J. Helmcke 2017. All rights reserved.
 */

#include <HardwareSerial.h>


#ifndef Casio38K_h
#define Casio38K_h

class Casio38K
{
public:

	Casio38K(const int n);
	void begin();
	bool available();
	int send38K(long x);
	//int send38K(double x);
	int recieve38K(long * x, bool end); // TODO: Remove second parameter-> Add func for reading a single byte from the uart-port (end byte).

private:

	HardwareSerial * uart_port;
	char _str_buff0[256];
	int _packet_buff[15];

};

#endif
