# toy_image_classifier
Abstract
The project builds an image classifier that can read and classify handwritten numbers, zero through nine, with the DE1-SoC development board and a Raspberry Pi. To accomplish this, we used a Raspberry Pi and its GPIO pins to transfer 8-bit grayscale 28x28 images, provided from an online dataset, to an FPGA. The FPGA was used to calculate the Euclidean distances between known and unknown images in a parallel fashion. This measurement was used in the K nearest neighbors algorithm to determine a classification of the unknown, handwritten images. As a visual aid to the algorithm, the system displays images in the process of being classified as zero through nine on a monitor controlled by a VGA module developed for the system. The system is able to correctly classify a 28x28 numerical image with an accuracy of about 93% in able 12 seconds. The project demonstrates that the parallelizable nature of the FPGA is advantageous in problems like image classification where computations can be run independent of one another. In future work, the accuracy of the system can bn be improved, and the computation speed of the algorithm can be increased.
