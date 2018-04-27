/*
 * Copyright (c) Ben J. Helmcke 2017. All rights reserved.
 *
 * As a reference for the communication protocol, the EA-200 manual (http://support.casio.com/storage/en/manual/pdf/EN/004/EA200_TechnicalReference_EN.pdf) was used.
 */


#include "Casio38K.h"
 
Casio38K::Casio38K(const int n) {
	uart_port = NULL;
	switch (n) {
		#if defined(HAVE_HWSERIAL0)
		case 0: {
			 uart_port = &Serial;
			 break;
		}
		#endif
		#if defined(HAVE_HWSERIAL1)
		case 1: {
			uart_port = &Serial1;
			 break;
		}
		#endif
		#if defined(HAVE_HWSERIAL2)
		case 2: {
			uart_port = &Serial1;
			 break;
		}
		#endif
		#if defined(HAVE_HWSERIAL3)
		case 3: {
			uart_port = &Serial1;
			 break;
		}
		#endif
	}
}

void Casio38K::begin() {
	if (uart_port == NULL) return;
	uart_port->begin(38400, SERIAL_8N2);
}

bool Casio38K::available() {
	if (uart_port == NULL) return 0;
	return uart_port->available() > 0;
}

int Casio38K::recieve38K(long * x, bool end) {
	if (uart_port == NULL) return -1;
	// Read the 0x15:
	while (!uart_port->available());
	uart_port->read();
	// Response with CodeA(0x13 for OK):
	uart_port->print((char)0x13);
	// recieve the header:
	for (int i = 0; i < 10; i++) {
		while (!uart_port->available());
		uart_port->read();
	}
	// Read the packet-size:
	while (!uart_port->available());
	int packet_size = uart_port->read() << 8;
	while (!uart_port->available());
	packet_size = (0xff00 & packet_size) | uart_port->read();
	// read the rest of the header:
	for (int i = 0; i < 3; i++) {
		while (!uart_port->available());
		uart_port->read();
	}
	// Respond with CodeB(0x06 for OK):
	uart_port->print((char)0x06);
	// Read the data:
	while (!uart_port->available());
	uart_port->read(); // read the ':'
	for (int i = 0; i < packet_size; i++) {
		while (!uart_port->available());
		_str_buff0[i] = uart_port->read();
	}
	while (!uart_port->available());
	uart_port->read(); // read the checksum
	_str_buff0[packet_size] = '\0';
	// Get the value from the string:
	*x = atol(_str_buff0);
	// Respond with CodeB(0x06 for OK):
	uart_port->print((char)0x06);
	if (end) {
		// Read the 0x00:
		while (!uart_port->available());
		uart_port->read();
	}
	return 0;
}

int Casio38K::send38K(long x) {
	if (uart_port == NULL) return -1;
	// Read 0x15 (start code):
	uart_port->read();
	// Reply with 0x13 for OK:
	uart_port->print((char)0x13);
	// Read the request header:
	for (int i = 0; i < 15; i++) {
		while (!uart_port->available());
		uart_port->read();
	}
	// Generate the conent-packet-string:
	int packet_size = sprintf(_str_buff0, ":%ld", x) - 1;
	// Generate the header:
	_packet_buff[0] = (int) ':'; // start symbol
	_packet_buff[1] = (int) 'N'; // N
	_packet_buff[2] = (int) 'A'; // ASCII
	_packet_buff[3] = (int) 'V'; // Variable
	_packet_buff[4] = 0x00; // line
	_packet_buff[5] = 0x01; // line
	_packet_buff[6] = 0x00; // offset
	_packet_buff[7] = 0x00; // offset
	_packet_buff[8] = 0x00; // offset
	_packet_buff[9] = 0x01; // offset
	_packet_buff[10] = 0xff00 & packet_size; // packet_size;
	_packet_buff[11] = 0x00ff & packet_size; // packet_size;
	_packet_buff[12] = 0xff; // 0xff;
	_packet_buff[13] = (int) 'A'; // 'A' for all
	// calc checksum:
	int checksum0 = 0;
	for (int i = 1; i < 14; i++) {
		checksum0 += _packet_buff[i];
	}
	checksum0 = ~checksum0;
	checksum0++;
	// Set the checksum:
	_packet_buff[14] = checksum0;
	// Send the packet:
	for (int i = 0; i < 15; i++) {
		uart_port->print((char)_packet_buff[i]);
	}
	/*uart_port->print(":NAV");
	uart_port->print((char)0x00); // line
	uart_port->print((char)0x01); // line
	uart_port->print((char)0x00); // offset
	uart_port->print((char)0x00); // offset
	uart_port->print((char)0x00); // offset
	uart_port->print((char)0x01); // offset
	uart_port->print((char)(0xff00 & packet_size)); // packet-size
	uart_port->print((char)(0xff & packet_size)); // packet_size;
	uart_port->print((char)0xff); // 0xff
	uart_port->print('A'); // A for All
	uart_port->print((char)(217 - packet_size)); // checksum*/

	int rd;
	while(!uart_port->available()); // wait for response
	if ((rd = uart_port->read() != 0x06)) { // read the response code:
		return -1;
	}
 	// Send the data-packet-string:
	uart_port->print(_str_buff0);
	// Calculate the checksum:
	int checksum1 = 0;
	{
		int i = 1;
		while (_str_buff0[i] != '\0')
			checksum1 += (int) _str_buff0[i++];
		checksum1 = ~checksum1;
		checksum1++;
	}
	// Send the checksum:
	uart_port->print((char)checksum1);
	// Read the response:
	while (!uart_port->available());
	if (uart_port->read() != 0x06) {
		return -1;
	}
	// Read 0x00:
	while (!uart_port->available());
	uart_port->read();
	return 0;
}
