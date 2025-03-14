#include "bf2_fifo_stage4.h"

#define fifo_len_div2 15

void bf2_fifo_stage4(
						// inputs
						const float *data_i_in, 
						const float *data_q_in, 
						
						// outputs
						float *data_i_out, 
						float *data_q_out) {
	static int nsample= 0;

	int nsample_r;

	static float fifo_i_array[31]= { (float)0.0 };
	static float fifo_q_array[31]= { (float)0.0 };

	static float fifo_i_tmp;
	static float fifo_q_tmp;

	*data_i_out = fifo_i_tmp;
	*data_q_out = fifo_q_tmp;
  
	if (nsample < 1) {
		nsample_r = 1;
	} else {
		if (nsample > fifo_len_div2 - 2) {
			nsample_r = 0;
		} else {
			nsample_r = nsample + 1;
		}
	}
	
	fifo_i_tmp = fifo_i_array[nsample_r];
	fifo_q_tmp = fifo_q_array[nsample_r];

	fifo_i_array[nsample] = *data_i_in;
	fifo_q_array[nsample] = *data_q_in;

	if (nsample < fifo_len_div2 - 1) {
		nsample++;
	} else {
		nsample = 0;
	}
}
