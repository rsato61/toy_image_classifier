#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>

#define BYTES_PER_IMAGE (784)
#define KNOWN_SIZE (10000)
#define UNKNOWN_SIZE (5000)

#define RESET (0x0000)
#define KNOWN (0x0001)
#define KNOWN_INC_DIFF (0x0002) // Send a byte of a known image and increment the difference calculation counter
#define INC_DIFF (0x0003) //Just increment the difference counter
#define UNKNOWN (0x0004)
#define WRITE (0x0005)
#define RESET_DISTANCE (0x0006)

const int DATA_REG_PINS[] = {2,3,4,17,27,22,10,9};
const int PI_READ_STATE_PINS[] = {14, 15, 24};
const int PI_READ_CLOCK = 18;
const int TOGGLE_WRITE = 23;

unsigned int size = 0;

// UPDATE THIS VALUE TO CHANGE THE NUMBER K OF
// NEAREST NEIGHBORS IS USED TO CLASSIFY AN IMAGE
#define K   50

typedef struct image {
    unsigned int distance;
    char label;
    struct image* next;
    struct image* prev;
} image;

image list[K];

/*
 * Prepares the array for use.
 */
void initializeArray();

/*
 * Inserts the image with the given distance and label if it
 * is one of the K smallest neighbors seen so far.
 */
void insertData(unsigned int distance, unsigned char label);

/*
 * Returns the distance and label data of the K nearest
 * images in the parameters. Assumes distances and labels
 * are of proper size.
 */
void getData(unsigned int* distances, unsigned char* labels);

/*
 * Returns the current number of images being stored.
 */
unsigned int arraySize();

/*
 * Looks at the K (as defined in comparisonArray.h) nearest
 * neighbors and assigns a label to the unknown image by voting
 * with the labels of the nearest neighbors.
 * Returns the label as a char and a measure of confidence
 * in the parameter.
 * Confidence is calculated as a percent from the number of
 * neighbors with the winning label over K.
 */
unsigned char voteAndClassify(unsigned int* confidence);

void pressToContinue() {
	printf("Press Any Key To Continue\n");
	getchar();
}

void printImage(unsigned char image[BYTES_PER_IMAGE], int size) {
    for(int i = 0; i < size; i++) {
        int value = (int)image[i];
        printf("%d ", value);
    }
    printf("\n");
}

void printClassifications(unsigned char classification[KNOWN_SIZE]) {
    for(int i = 0; i < KNOWN_SIZE; i++) {
        printf("%d ", classification[i]);
    }
    printf("\n");
}

void writeByte(unsigned char pixel) {
	for(int i = 0; i < 8; i++) {
		if(pixel & (1 << i)) {
			digitalWrite(DATA_REG_PINS[i], HIGH);
		} else {
			digitalWrite(DATA_REG_PINS[i], LOW);
		}
	}
}

unsigned char readByte() {
	//printf("Reading byte: ");
	unsigned char total = 0;
	for(int i = 0; i < 8; i++) {
		int current = digitalRead(DATA_REG_PINS[i]);
		//printf("%d", current);
		if(current == HIGH) {
			total += pow(2, i);
		}
	}
	//printf("\n");
	return total;
}

void writeReadState(unsigned char state) {
	//printf("Writing: ");
	for(int i = 0; i < 3; i++) {
		if(state & (1 << i)) {
			digitalWrite(PI_READ_STATE_PINS[i], HIGH);
			//printf("1");
		} else {
			digitalWrite(PI_READ_STATE_PINS[i], LOW);
			//printf("0");
		}
	}
	//printf(" to read state.\n");
}

void cycle() {
	digitalWrite(PI_READ_CLOCK, HIGH);
	digitalWrite(PI_READ_CLOCK, LOW);
}

unsigned int readDistance() {
	//First change the ouput pins to inputs
	for(int i = 0; i < 8; i++) {
		pinMode(DATA_REG_PINS[i], INPUT);
	}
	digitalWrite(TOGGLE_WRITE, HIGH); //Tell FPGA to output over the dataRegisters
	writeReadState(WRITE); //Switch to the WRITE state to allow the write counter to increment
	//Read four bytes
	unsigned char distance[3];
	for(int i = 0; i < 4; i++) {
		distance[i] = readByte();
		
		cycle();
	}
	digitalWrite(TOGGLE_WRITE, LOW);
	unsigned int* output = (unsigned int*)distance; //cast to int
	for(int i = 0; i < 8; i++) {
		pinMode(DATA_REG_PINS[i], OUTPUT);
		digitalWrite(DATA_REG_PINS[i], LOW);
	}
	writeReadState(RESET_DISTANCE); //Reset the internal counters
	cycle();
	return *output;
}

void transferUnknownImage(unsigned char image[BYTES_PER_IMAGE]) {
	writeReadState(UNKNOWN);
	for(int i = 0; i < BYTES_PER_IMAGE; i++) {
		writeByte(image[i]); //Write to the register
		cycle();
	}
	writeReadState(RESET); //Reset the internal counter
	cycle();
}

void transferKnownImage(unsigned char image[BYTES_PER_IMAGE]) {
	int i = 0;
	writeReadState(KNOWN); //Write first byte so the difference calculator has it ready
	writeByte(image[i++]); 
	cycle();
	writeReadState(KNOWN_INC_DIFF); //From now on, also increment the counter
	for(; i < BYTES_PER_IMAGE; i++) {
		//pressToContinue();
		writeByte(image[i]); //Write to the register
		cycle();
	}
	writeReadState(INC_DIFF); //Esnure we calculate the difference for the last byte without modifying the known image
	cycle();
	writeReadState(RESET); //Reset the internal counter
	cycle();
}

void getKnowns(unsigned char knowns[KNOWN_SIZE][BYTES_PER_IMAGE]) {
	FILE* imagesFile = NULL;
	imagesFile = fopen ("./imageData.txt", "r");
	if (!imagesFile) {
		printf ("Cannot open file for reading\n");
		exit(1);
	}
	char* line = NULL;
	size_t lineSize;
	ssize_t lineLength;
	int pixelPos = 0;
	int imageCount = 0;
	printf("Loading Images to Raspberry Pi...\n");
	clock_t begin = clock();
	while((lineLength = getline(&line, &lineSize, imagesFile)) != -1) {
		char *pch;
		pch = strtok(line, " ");
		knowns[imageCount][pixelPos++] = (unsigned char)atoi(pch);
		while (pch != NULL && pixelPos < BYTES_PER_IMAGE) {
			pch = strtok(NULL, " ");
			if(pch != NULL) {
				knowns[imageCount][pixelPos++] = (unsigned char) atoi(pch);
			}
		}
		pixelPos = 0;
		imageCount++;
	}
	clock_t end = clock();
	double timeTaken = (double)(end-begin) / CLOCKS_PER_SEC;
	printf("Loaded %d known images in %f seconds\n", imageCount, timeTaken);
	fclose(imagesFile);
}

void getUnknowns(unsigned char unknowns[UNKNOWN_SIZE][BYTES_PER_IMAGE]) {
	FILE* imagesFile = NULL;
	imagesFile = fopen ("./testImageData.txt", "r");
	if (!imagesFile) {
		printf ("Cannot open file for reading\n");
		exit(1);
	}
	char* line = NULL;
	size_t lineSize;
	ssize_t lineLength;
	int pixelPos = 0;
	int imageCount = 0;
	printf("Loading Images to Raspberry Pi...\n");
	clock_t begin = clock();
	while((lineLength = getline(&line, &lineSize, imagesFile)) != -1) {
		char *pch;
		pch = strtok(line, " ");
		unknowns[imageCount][pixelPos++] = (unsigned char)atoi(pch);
		while (pch != NULL && pixelPos < BYTES_PER_IMAGE) {
			pch = strtok(NULL, " ");
			if(pch != NULL) {
				unknowns[imageCount][pixelPos++] = (unsigned char) atoi(pch);
			}
		}
		pixelPos = 0;
		imageCount++;
	}
	clock_t end = clock();
	double timeTaken = (double)(end-begin) / CLOCKS_PER_SEC;
	printf("Loaded %d test images in %f seconds\n", imageCount, timeTaken);
	fclose(imagesFile);
}

void getKnownClassifications(unsigned char classifications[KNOWN_SIZE]) {
	FILE* file = NULL;
	file = fopen ("./imageLabels.txt", "r");
	if (!file) {
		printf ("Cannot open file for reading\n");
		exit(1);
	}
	char* line = NULL;
	size_t lineSize;
	ssize_t lineLength;
	int index = 0;
	printf("Loading Classifications to Raspberry Pi...\n");
	clock_t begin = clock();
	while((lineLength = getline(&line, &lineSize, file)) != -1) {
		classifications[index] = (unsigned char)atoi(line);
		index++;
	}
	clock_t end = clock();
	double timeTaken = (double)(end-begin) / CLOCKS_PER_SEC;
	printf("Loaded %d classifications in %f seconds\n", index, timeTaken);
	fclose(file);
}

void getUnknownClassifications(unsigned char classifications[UNKNOWN_SIZE]) {
	FILE* file = NULL;
	file = fopen ("./testImageLabels.txt", "r");
	if (!file) {
		printf ("Cannot open file for reading\n");
		exit(1);
	}
	char* line = NULL;
	size_t lineSize;
	ssize_t lineLength;
	int index = 0;
	printf("Loading Classifications to Raspberry Pi...\n");
	clock_t begin = clock();
	while((lineLength = getline(&line, &lineSize, file)) != -1) {
		classifications[index] = (unsigned char)atoi(line);
		index++;
	}
	clock_t end = clock();
	double timeTaken = (double)(end-begin) / CLOCKS_PER_SEC;
	printf("Loaded %d classifications in %f seconds\n", index, timeTaken);
	fclose(file);
}

void tests() {
	//Test get classifications
	printf("Test get classification\n");
	pressToContinue();
	unsigned char classifications[KNOWN_SIZE];
	getKnownClassifications(classifications);
	printClassifications(classifications);
	
	printf("Test getKnowns\n");
	pressToContinue();
	unsigned char knowns[KNOWN_SIZE][BYTES_PER_IMAGE];
	getKnowns(knowns);

	//Test writeByte
	printf("Test writeByte\n");
	pressToContinue();
	writeByte(0);
	pressToContinue();
	writeByte(1);
	pressToContinue();
	writeByte(255);
	pressToContinue();
}

void testDistance() {
	unsigned char (*knowns)[BYTES_PER_IMAGE] = malloc(sizeof *knowns * KNOWN_SIZE);
	getKnowns(knowns);
	unsigned char* classifications = (unsigned char*) malloc(sizeof(unsigned char) * KNOWN_SIZE);
	getKnownClassifications(classifications);
	unsigned char (*unknowns)[BYTES_PER_IMAGE] = malloc(sizeof *unknowns * UNKNOWN_SIZE);
	getUnknowns(unknowns);
	
	printf("Ready to start algorithm...\n");
	pressToContinue();
	writeReadState(RESET);
	cycle();
	writeReadState(RESET_DISTANCE);
	cycle();
	transferUnknownImage(knowns[500]); //Placeholder for unknown image
	transferKnownImage(knowns[500]);
	unsigned int distance = readDistance();
	printf("500: %d\n",distance);
	free(knowns);
	free(unknowns);
	free(classifications);
}

void runSingleUnknown() {
	unsigned char (*knowns)[BYTES_PER_IMAGE] = malloc(sizeof *knowns * KNOWN_SIZE);
	getKnowns(knowns);
	unsigned char* classifications = (unsigned char*) malloc(sizeof(unsigned char) * KNOWN_SIZE);
	getKnownClassifications(classifications);
	unsigned char (*unknowns)[BYTES_PER_IMAGE] = malloc(sizeof *unknowns * UNKNOWN_SIZE);
	getUnknowns(unknowns);
	
	printf("Ready to start algorithm...\n");
	pressToContinue();
	writeReadState(RESET);
	cycle();
	writeReadState(RESET_DISTANCE);
	cycle();
	transferUnknownImage(knowns[500]); //Placeholder for unknown image
	initializeArray();
	for(int i = 0; i < KNOWN_SIZE; i++) {
		transferKnownImage(knowns[i]); //Transfer the ith known image
		unsigned int distance = readDistance();
		insertData(distance, classifications[i] + '0');
	}
	unsigned int confidence = 0;
	unsigned char label = voteAndClassify(&confidence);
	printf("Closest Match: %c with confidence of %d\n", label, confidence);
	printf("Correct label: %d\n", (int)classifications[500]);
	unsigned int distances[K];
	unsigned char labels[K];
	getData(distances, labels);
	
	for(int i = 0; i < K; i++) {
		printf("%d Distance: %d Label %c\n", i, distances[i], labels[i]);
	}
	free(knowns);
	free(unknowns);
	free(classifications);
}

void runManyUnknowns() {
	unsigned char (*knowns)[BYTES_PER_IMAGE] = malloc(sizeof *knowns * KNOWN_SIZE);
	getKnowns(knowns);
	unsigned char* knownClassifications = (unsigned char*) malloc(sizeof(unsigned char) * KNOWN_SIZE);
	getKnownClassifications(knownClassifications);
	unsigned char (*unknowns)[BYTES_PER_IMAGE] = malloc(sizeof *unknowns * UNKNOWN_SIZE);
	getUnknowns(unknowns);
	unsigned char* unknownClassifications = (unsigned char*) malloc(sizeof(unsigned char) * UNKNOWN_SIZE);
	getUnknownClassifications(unknownClassifications);
	
	printf("Ready to start algorithm...\n");
	pressToContinue();
	int N = 10; //Number of unknowns to run for comparison
	int numCorrect = 0;
	for(int i = 0; i < N; i++) {
		printf("\nRunning of unknown #%d\n", i);
		writeReadState(RESET);
		cycle();
		writeReadState(RESET_DISTANCE);
		cycle();
		transferUnknownImage(unknowns[i]); //Placeholder for unknown image
		initializeArray();
		for(int i = 0; i < KNOWN_SIZE; i++) {
			transferKnownImage(knowns[i]); //Transfer the ith known image
			unsigned int distance = readDistance();
			insertData(distance, knownClassifications[i] + '0');
		}
		unsigned int confidence = 0;
		unsigned char labelGuess = voteAndClassify(&confidence);
		unsigned char correctLabel = '0' + unknownClassifications[i];
		printf("Closest Match: %c with confidence of %d%%\n", labelGuess, confidence);
		printf("Correct label: %c\n", correctLabel);
		if(labelGuess == correctLabel) {
			numCorrect++;
			printf("Correct!\n");
		} else {
			printf("Incorrect!\n");
		}
	}
	printf("Accuracy: %f%%\n", 100.0f * ((double)numCorrect/(double)N));
	free(knowns);
	free(unknowns);
	free(knownClassifications);
	free(unknownClassifications);
	
}

int main(void) {
	wiringPiSetupGpio();
	//Image transfer output bit pins
	for(int i = 0; i < 8; i++) {
		pinMode(DATA_REG_PINS[i], OUTPUT);
		digitalWrite(DATA_REG_PINS[i], LOW);
	}

	//Pi to FPGA line
	for(int i = 0; i < 3; i++) {
		pinMode(PI_READ_STATE_PINS[i], OUTPUT);
		digitalWrite(PI_READ_STATE_PINS[i], LOW);
	}
	
	pinMode(PI_READ_CLOCK, OUTPUT);
	digitalWrite(PI_READ_CLOCK, LOW);

	pinMode(TOGGLE_WRITE, OUTPUT);
	digitalWrite(TOGGLE_WRITE, LOW);
	
	//testDistance();
	runManyUnknowns(); // Run the program  
	
	return 0;
}

void initializeArray() {
    for (int q = 0; q < size; q++) {
        list[q].distance = 0;
        list[q].label = '0';
    }
    size = 0;
}


void insertData(unsigned int distance, unsigned char label) {
    if (size == 0) {
        list[0].distance = distance;
        list[0].label = label;
        size = size + 1;
        return;
    }

    unsigned int index = 0;

    while (index < size && list[index].distance <= distance) {
        index = index + 1;
    }

    // distance is the largest, only insert if size < K
    if (index == size) {
        if (size < K) {
            list[size].distance = distance;
            list[size].label = label;
            size = size + 1;
        }

    // distance isn't the largest, and index holds the index of the value
    // that should be after the parameter values in list, so insert it in the list
    // and shift distances up to size K if needed
    } else {
        unsigned int currDistance = distance;
        unsigned char currLabel = label;

        unsigned int tempDistance = list[index].distance;
        unsigned char tempLabel = list[index].label;

        // Replace value at index and increment index
        list[index].distance = currDistance;
        list[index].label = currLabel;
        index = index + 1;

        // Bubble until size or size - 1 if size == K
        while (index < size) {
            // copy the current values
            currDistance = list[index].distance;
            currLabel = list[index].label;

            // put the temp values here
            list[index].distance = tempDistance;
            list[index].label = tempLabel;

            // set the current values to the temp values
            tempDistance = currDistance;
            tempLabel = currLabel;

            // increment distance
            index = index + 1;
        }

        // If there's still space in the list, increment
        // size and set the stored temp values there
        if (size < K) {
            list[index].distance = tempDistance;
            list[index].label = tempLabel;
            size = size + 1;
        }

    }

}

void getData(unsigned int* distances, unsigned char* labels) {
    for (int i = 0; i < size; i++) {
        distances[i] = list[i].distance;
        labels[i] = list[i].label;
    }
}

unsigned int arraySize() {
    return size;
}

unsigned char voteAndClassify(unsigned int* confidence) {
    unsigned int minD[K];
    unsigned char minL[K];

    // Get the min K images
    getData(minD, minL);

    // Each index corresponds to the number
    // of nearest neighbors with that classification,
    // so index 0 holds the number of nearest neighbors
    // with a label of '0', and so on.
    unsigned int classificationBuckets[10];

    // Reset and populate the bucket array with counts
    for (int i = 0; i < 10; i++) {
        classificationBuckets[i] = 0;
    }
    for (int q = 0; q < K; q++) {
        classificationBuckets[(int) (minL[q] - '0')] += 1;
    }

    // Looks for the bucket with the most number of
    // neighbors. If there are equal numbers, the earliest
    // encountered one is taken. If they are all equal, the
    // label of the closest neighbors is taken.
    unsigned char minBucket = minL[0];
    unsigned int minBucketCount = classificationBuckets[(int) (minL[0] - '0')];

    for (int i = 1; i < K; i++) {
        // Grab the count for the next smallest distance
        unsigned int currCount = classificationBuckets[(int) (minL[i] - '0')];

        // Update the label if the neighbor count is greater for that label
        if (currCount > minBucketCount) {
            minBucket = minL[i];
            minBucketCount = currCount;
        }
    }

    // Confidence is number of neighbors with the given label
    // over the number K of neighbors looked at.
    *confidence = (int) ((minBucketCount * 100.0) / K);

    return minBucket;
}
