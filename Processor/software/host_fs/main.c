#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system.h"
#include "sys/alt_stdio.h"
#include <ctype.h>
#include <altera_up_sd_card_avalon_interface.h>
//#include "images.h"

#define BYTES_PER_TRANSFER (4)
#define BYTES_PER_IMAGE (784)
#define IMAGE_BASE_ADDRESS 0x8041020 //Base address for 32 image register
#define FPGA_CONTROL_LINE_ADDRESS 0x8041000 //Input from FPGA
#define NIOS_CONTROL_LINE_ADDRESS 0x8041010 //Output to FPGA

#define BUSY (0x0000)
#define READY (0x0001)
#define CONTINUE (0x0002)
#define DONE (0x0003)

void printImage(unsigned char image[BYTES_PER_IMAGE], int size) {
    alt_printf("Printing: \n");
    for(int i = 0; i < size; i++) {
        int value = (int)image[i];
        char buffer[5]; //String won't be more than 3 characters
        alt_printf("%s ", itoa(value, buffer, 10));
    }
    alt_printf("\n");
}

void transferImage(unsigned char image[BYTES_PER_IMAGE]) {
	volatile unsigned char* fpgaControl = FPGA_CONTROL_LINE_ADDRESS;
	unsigned char* niosControl = NIOS_CONTROL_LINE_ADDRESS;
	unsigned int* imageRegister = IMAGE_BASE_ADDRESS;

	for(int i = 0; i < BYTES_PER_IMAGE; i += BYTES_PER_TRANSFER) {
		*niosControl = CONTINUE; //Continue signal
		while(*fpgaControl != READY) {} //Wait until the fpga is ready
		unsigned int* pointer = &image[i]; // Point to the first of a group of four bytes
		*imageRegister = *pointer;
		*niosControl = READY; //Start reading signal
		while(*fpgaControl != CONTINUE) {} //Wait until we start the read
	}
	*niosControl = DONE;
}

void sdTest() {

}


int main()
{
	Altera_UP_Flash_Memory_IP_Core_Avalon_Interface interface;
	sdTest();
	alt_printf("First Starting...\n");
	FILE* fp_ascii = NULL;
	fp_ascii = fopen ("/mnt/host/test.txt", "r");
	if (fp_ascii == NULL)
	{
	 alt_printf ("Cannot open file for reading\n");
	 exit(1);
	}
	char* line;
	size_t lineSize;
	ssize_t lineLength;
	unsigned char image[BYTES_PER_IMAGE];
	int pixelPos = 0;
	int imageCount = 0;
	while((lineLength = __getline(&line, &lineSize, fp_ascii)) != -1) {
		char *pch;
		pch = strtok(line, " ");
		image[pixelPos++] = (unsigned char)atoi(pch);
		while (pch != NULL && pixelPos < BYTES_PER_IMAGE) {
			pch = strtok(NULL, " ");
			if(pch != NULL) {
				image[pixelPos++] = (unsigned char) atoi(pch);
			}
		}
		if(pixelPos != BYTES_PER_IMAGE) {
			return 21; //Have it return an error
		}
		//printImage(image, BYTES_PER_IMAGE);
		transferImage(image);
		pixelPos = 0;
		imageCount++;
	}
	alt_printf("Number of images: %d", imageCount);
	fclose(fp_ascii);
	alt_printf("Done!");
	while(1){}

}
