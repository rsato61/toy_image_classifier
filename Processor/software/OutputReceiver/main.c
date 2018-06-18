#include "sys/alt_stdio.h"
#include "system.h"

volatile unsigned char* READ_STATE = READ_DIFF_PIO_BASE;
volatile unsigned char* CLASSIFICATION_REGISTER = CLASSIFICATION_PIO_BASE;
volatile unsigned int* DISTANCE_REGISTER = DISTANCE_PIO_BASE;
unsigned char* DONE_READ = DONE_READ_PIO_BASE;

#define LOW (0x0000)
#define HIGH 	(0x0001)

/*
 * Toggles the DONE_READ output to let the Pi know we have read the registers
 */
void notifyDoneRead() {
	char current = *DONE_READ;
	if(current == HIGH) {
		*DONE_READ = LOW;
	} else {
		*DONE_READ = HIGH;
	}
}

int main()
{
	char old = *READ_STATE;
	while(1) {
		char newest;
		while((newest = *READ_STATE) == old) {} //Wait until the bit changes
		old = newest; //Record change
		alt_printf("Class: %d", (unsigned int)*CLASSIFICATION_REGISTER);
		alt_printf("Difference: %d", (unsigned int)*DISTANCE_REGISTER);
	}


	return 0;
}
