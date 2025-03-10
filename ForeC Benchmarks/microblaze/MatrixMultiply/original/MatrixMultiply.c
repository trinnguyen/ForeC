// Xilinx Microblaze
#include "xparameters.h"

// Hardware counter connected via a direct FSL bus.
#include "counter_dfsl.h"
Counter counter;
#define FSL_COUNTER_ID				2


void multiply(void);

int main(int argc, char **args) {
	// Initialise and start counting the elapsed cycles for each reaction.
	counterInitialise(counter);
	counterStart(counter, FSL_COUNTER_ID);
	//--------------------------------------------------------------
	
	multiply();
	multiply();
	multiply();
	multiply();
	multiply();
	multiply();
	multiply();
	multiply();
	
	//--------------------------------------------------------------
	// Stop counting the elapsed cycles for the current reaction.
	counterStop(FSL_COUNTER_ID);
	counterRead(counter, FSL_COUNTER_ID);
	counterDifference(counter);
	counterMaximum(counter);
	counterMinimum(counter);
	counterAccumulate(counter);
	counterAverage(counter);
	xil_printf("Total time: %d cycles\r\n", counter.total);
	xil_printf("Program termination\r\n");
	// forec:scheduler:counter:end
	
	asm volatile (".long 0x80000001\r\n");
	return 0;
}

void multiply(void) {
	int SIZE = 5;
	
	int matrixA[SIZE][SIZE];
	int matrixB[SIZE][SIZE];
	int matrixC[SIZE][SIZE];

	int sum;
	sum = 0;
	sum += matrixA[0][0] * matrixB[0][0];
	sum += matrixA[0][1] * matrixB[1][0];
	sum += matrixA[0][2] * matrixB[2][0];
	sum += matrixA[0][3] * matrixB[3][0];
	sum += matrixA[0][4] * matrixB[4][0];
	matrixC[0][0] = sum;
	sum = 0;
	sum += matrixA[0][0] * matrixB[0][1];
	sum += matrixA[0][1] * matrixB[1][1];
	sum += matrixA[0][2] * matrixB[2][1];
	sum += matrixA[0][3] * matrixB[3][1];
	sum += matrixA[0][4] * matrixB[4][1];
	matrixC[0][1] = sum;
	sum = 0;
	sum += matrixA[0][0] * matrixB[0][2];
	sum += matrixA[0][1] * matrixB[1][2];
	sum += matrixA[0][2] * matrixB[2][2];
	sum += matrixA[0][3] * matrixB[3][2];
	sum += matrixA[0][4] * matrixB[4][2];
	matrixC[0][2] = sum;
	sum = 0;
	sum += matrixA[0][0] * matrixB[0][3];
	sum += matrixA[0][1] * matrixB[1][3];
	sum += matrixA[0][2] * matrixB[2][3];
	sum += matrixA[0][3] * matrixB[3][3];
	sum += matrixA[0][4] * matrixB[4][3];
	matrixC[0][3] = sum;
	sum = 0;
	sum += matrixA[0][0] * matrixB[0][4];
	sum += matrixA[0][1] * matrixB[1][4];
	sum += matrixA[0][2] * matrixB[2][4];
	sum += matrixA[0][3] * matrixB[3][4];
	sum += matrixA[0][4] * matrixB[4][4];
	matrixC[0][4] = sum;
	sum = 0;
	sum += matrixA[1][0] * matrixB[0][0];
	sum += matrixA[1][1] * matrixB[1][0];
	sum += matrixA[1][2] * matrixB[2][0];
	sum += matrixA[1][3] * matrixB[3][0];
	sum += matrixA[1][4] * matrixB[4][0];
	matrixC[1][0] = sum;
	sum = 0;
	sum += matrixA[1][0] * matrixB[0][1];
	sum += matrixA[1][1] * matrixB[1][1];
	sum += matrixA[1][2] * matrixB[2][1];
	sum += matrixA[1][3] * matrixB[3][1];
	sum += matrixA[1][4] * matrixB[4][1];
	matrixC[1][1] = sum;
	sum = 0;
	sum += matrixA[1][0] * matrixB[0][2];
	sum += matrixA[1][1] * matrixB[1][2];
	sum += matrixA[1][2] * matrixB[2][2];
	sum += matrixA[1][3] * matrixB[3][2];
	sum += matrixA[1][4] * matrixB[4][2];
	matrixC[1][2] = sum;
	sum = 0;
	sum += matrixA[1][0] * matrixB[0][3];
	sum += matrixA[1][1] * matrixB[1][3];
	sum += matrixA[1][2] * matrixB[2][3];
	sum += matrixA[1][3] * matrixB[3][3];
	sum += matrixA[1][4] * matrixB[4][3];
	matrixC[1][3] = sum;
	sum = 0;
	sum += matrixA[1][0] * matrixB[0][4];
	sum += matrixA[1][1] * matrixB[1][4];
	sum += matrixA[1][2] * matrixB[2][4];
	sum += matrixA[1][3] * matrixB[3][4];
	sum += matrixA[1][4] * matrixB[4][4];
	matrixC[1][4] = sum;
	sum = 0;
	sum += matrixA[2][0] * matrixB[0][0];
	sum += matrixA[2][1] * matrixB[1][0];
	sum += matrixA[2][2] * matrixB[2][0];
	sum += matrixA[2][3] * matrixB[3][0];
	sum += matrixA[2][4] * matrixB[4][0];
	matrixC[2][0] = sum;
	sum = 0;
	sum += matrixA[2][0] * matrixB[0][1];
	sum += matrixA[2][1] * matrixB[1][1];
	sum += matrixA[2][2] * matrixB[2][1];
	sum += matrixA[2][3] * matrixB[3][1];
	sum += matrixA[2][4] * matrixB[4][1];
	matrixC[2][1] = sum;
	sum = 0;
	sum += matrixA[2][0] * matrixB[0][2];
	sum += matrixA[2][1] * matrixB[1][2];
	sum += matrixA[2][2] * matrixB[2][2];
	sum += matrixA[2][3] * matrixB[3][2];
	sum += matrixA[2][4] * matrixB[4][2];
	matrixC[2][2] = sum;
	sum = 0;
	sum += matrixA[2][0] * matrixB[0][3];
	sum += matrixA[2][1] * matrixB[1][3];
	sum += matrixA[2][2] * matrixB[2][3];
	sum += matrixA[2][3] * matrixB[3][3];
	sum += matrixA[2][4] * matrixB[4][3];
	matrixC[2][3] = sum;
	sum = 0;
	sum += matrixA[2][0] * matrixB[0][4];
	sum += matrixA[2][1] * matrixB[1][4];
	sum += matrixA[2][2] * matrixB[2][4];
	sum += matrixA[2][3] * matrixB[3][4];
	sum += matrixA[2][4] * matrixB[4][4];
	matrixC[2][4] = sum;
	sum = 0;
	sum += matrixA[3][0] * matrixB[0][0];
	sum += matrixA[3][1] * matrixB[1][0];
	sum += matrixA[3][2] * matrixB[2][0];
	sum += matrixA[3][3] * matrixB[3][0];
	sum += matrixA[3][4] * matrixB[4][0];
	matrixC[3][0] = sum;
	sum = 0;
	sum += matrixA[3][0] * matrixB[0][1];
	sum += matrixA[3][1] * matrixB[1][1];
	sum += matrixA[3][2] * matrixB[2][1];
	sum += matrixA[3][3] * matrixB[3][1];
	sum += matrixA[3][4] * matrixB[4][1];
	matrixC[3][1] = sum;
	sum = 0;
	sum += matrixA[3][0] * matrixB[0][2];
	sum += matrixA[3][1] * matrixB[1][2];
	sum += matrixA[3][2] * matrixB[2][2];
	sum += matrixA[3][3] * matrixB[3][2];
	sum += matrixA[3][4] * matrixB[4][2];
	matrixC[3][2] = sum;
	sum = 0;
	sum += matrixA[3][0] * matrixB[0][3];
	sum += matrixA[3][1] * matrixB[1][3];
	sum += matrixA[3][2] * matrixB[2][3];
	sum += matrixA[3][3] * matrixB[3][3];
	sum += matrixA[3][4] * matrixB[4][3];
	matrixC[3][3] = sum;
	sum = 0;
	sum += matrixA[3][0] * matrixB[0][4];
	sum += matrixA[3][1] * matrixB[1][4];
	sum += matrixA[3][2] * matrixB[2][4];
	sum += matrixA[3][3] * matrixB[3][4];
	sum += matrixA[3][4] * matrixB[4][4];
	matrixC[3][4] = sum;
	sum = 0;
	sum += matrixA[4][0] * matrixB[0][0];
	sum += matrixA[4][1] * matrixB[1][0];
	sum += matrixA[4][2] * matrixB[2][0];
	sum += matrixA[4][3] * matrixB[3][0];
	sum += matrixA[4][4] * matrixB[4][0];
	matrixC[4][0] = sum;
	sum = 0;
	sum += matrixA[4][0] * matrixB[0][1];
	sum += matrixA[4][1] * matrixB[1][1];
	sum += matrixA[4][2] * matrixB[2][1];
	sum += matrixA[4][3] * matrixB[3][1];
	sum += matrixA[4][4] * matrixB[4][1];
	matrixC[4][1] = sum;
	sum = 0;
	sum += matrixA[4][0] * matrixB[0][2];
	sum += matrixA[4][1] * matrixB[1][2];
	sum += matrixA[4][2] * matrixB[2][2];
	sum += matrixA[4][3] * matrixB[3][2];
	sum += matrixA[4][4] * matrixB[4][2];
	matrixC[4][2] = sum;
	sum = 0;
	sum += matrixA[4][0] * matrixB[0][3];
	sum += matrixA[4][1] * matrixB[1][3];
	sum += matrixA[4][2] * matrixB[2][3];
	sum += matrixA[4][3] * matrixB[3][3];
	sum += matrixA[4][4] * matrixB[4][3];
	matrixC[4][3] = sum;
	sum = 0;
	sum += matrixA[4][0] * matrixB[0][4];
	sum += matrixA[4][1] * matrixB[1][4];
	sum += matrixA[4][2] * matrixB[2][4];
	sum += matrixA[4][3] * matrixB[3][4];
	sum += matrixA[4][4] * matrixB[4][4];
	matrixC[4][4] = sum;
}

