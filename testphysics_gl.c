#include "uart.h"
#include "printf.h"

void main(void) {
	uart_init();
	uart_putstring("in test program...");
}
