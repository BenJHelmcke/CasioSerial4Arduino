# CasioSerial4Arduino
Library for communicating with an Casio fx-9860GII over a 3Pin UART connector. 

# Getting started
Simply create a new folder called Casio38K inside your Arduino library folder, which should be located under Documents/Arduino/library. Then copy the Casio38K.cpp and Casio38K.h files from this repository into your new folder.
Now you can connect your arduino and connect your calculator to the arduino. After this, you can start using the library.

Here is a Arduino sketch for reading the values from the Calculator:
```C++
#include <Casio38K.h>

Casio38K casio_uart0(1/*rx and tx 1*/);

void setup() {
  Serial.begin(57600);
  casio_uart0.begin();
}

long x = 0;
void loop () {  
  if (casio_uart0.available()) {
    if (x == 99) {
      casio_uart0.recieve38K(&x, 1); // after this, we expect the calculator program to stop, when this happens, a single byte will be send. Therefore we have to read 1 extra byte, which is done by setting the second parameter to 1.
    } else {
      casio_uart0.recieve38K(&x, 0);
    }
    Serial.println(x);
  }
}
```

And here is a example for sending values to the Calculator:

```C++
#include <Casio38K.h>

Casio38K casio_uart0(1/*rx and tx 1*/);

void setup() {
  Serial.begin(57600);
  casio_uart0.begin();
}

void loop() {
  if (casio_uart0.available()) {
    int rd = casio_uart0.send38K(x);
  }
}
```
