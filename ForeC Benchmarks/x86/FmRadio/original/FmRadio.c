/* A simple approach to FMradio.
 *
 * The code was addapted from Marco Cornero Aug'06 
 * for Acotes project. This version ensures to have
 * the same result that the stream one for plain C.
 * 
 * Author: David Rodenas-Pico, BSC
 * Creation Date: March 2007
 */


#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


#undef M_PI
static const double M_PI = 3.14159265358979323846;

static const int KSMPS = 1024;
static const int FM_MAX = 5;
static const int FM_LIMIT = (65536/2) - 1;
static const int GAIN_LMR = 2000;


/* *********************************************** complex operations   */

typedef struct {
	float real;
	float imag;
} complex;

void complex_conj(const complex* s, complex* d) {
	(d)->real = (s)->real;
	(d)->imag = -1 * (s)->imag;
}

float complex_arg(const complex *x) {
	return atan2((x)->imag, (x)->real);
}

void complex_mul(const complex* a, const complex* b, complex* d) {
	(d)->real = (a)->real * (b)->real - ((a)->imag * (b)->imag);
	(d)->imag = (a)->real * (b)->imag + (a)->imag * (b)->real;
}


/* *********************************************** auxiliar functions   */

short dac_cast_trunc_and_normalize_to_short(float f) {
	short x;
	f = (f / FM_MAX)*FM_LIMIT;
	x = f;
	return x;
}

/* *********************************************** fm_quad_demod        */

typedef struct {
	float history[2];
} fm_quad_demod_filter;

void fm_quad_demod_init(fm_quad_demod_filter* filter) {
	filter->history[0]= 0;
	filter->history[1]= 0;
}

void fm_quad_demod(fm_quad_demod_filter* filter, const float i1, const float i2, float* result) {
	complex x_N;
	complex x_N_1;
	complex x_N_1_conj;
	complex y_N;
	float demod;
	const float d_gain = 0.5;
	
	// y(n) = angle(x(n)*conj(x(n-1))
	
	/* read two complex data */
	x_N.real = i2;
	x_N.imag = i1;
	
	x_N_1.real = filter->history[1];
	x_N_1.imag = filter->history[0];
	
	/* compute */
	complex_conj(& x_N_1, & x_N_1_conj);
	complex_mul(& x_N_1_conj, & x_N, & y_N);
	
	demod = d_gain * complex_arg(& y_N); 
	
	filter->history[0]= i1;
	filter->history[1]= i2;
	
	*result = demod;
}



/* *********************************************** multiply_square      */

void multiply_square(const float i1, const float i2, float* result) {
	*result= GAIN_LMR * i1 * i2 * i2;
}


/* *********************************************** ntaps_filter         */

typedef struct ntaps_filter_conf {
	double* coeff;
	int decimation;
	int taps;
	float*  history;
	int next;
} ntaps_filter_conf;

static const int WIN_HAMMING = 0;
static const int WIN_HANNING = 1;
static const int WIN_BLACKMAN = 2;

void compute_window(const unsigned int ntaps, double *taps, const unsigned int type) {
	int n;
	const int M = ntaps - 1;    // filter order
	
	if (type == WIN_HAMMING) {
		for (n = 0; n < ntaps; n++) {
			taps[n] = 0.54 - 0.46 * cos((2 * M_PI * n) / M);
		}
	} else if (type == WIN_HANNING) {
		for (n = 0; n < ntaps; n++) {
			taps[n] = 0.5 - 0.5 * cos((2 * M_PI * n) / M);
		}
	} else if(type == WIN_BLACKMAN) {
		for (n = 0; n < ntaps; n++) {
			taps[n] = 0.42 - 0.50 * cos((2*M_PI * n) / (M-1)) - 0.08 * cos((4*M_PI * n) / (M-1));
		}
	}
}

int compute_ntaps (const float sampling_freq, const float transition_width, const int window_type) {
	/* Mormalized transition width */
	const float delta_f = transition_width / sampling_freq;
	
	const float width_factor[3] = {3.3, 3.1, 5.5};
	
	/* compute number of taps required for given transition width */
	int ntaps = (int) (width_factor[window_type]/ delta_f + 0.5);
	
	if ((ntaps & 1) == 0) {
		ntaps++;
	}
	
	return ntaps;
}

void ntaps_filter_ffd_init(ntaps_filter_conf *conf, const double cutoff_freq, const double transition_band,
						   double gain, const int decimation, const double sampling_rate, const int window_type) {
	/* Taken from the GNU software radio .. */
	
	int n;
	int i;
	
	const unsigned int ntaps = compute_ntaps(sampling_rate, transition_band, window_type);
	double* w = malloc(ntaps * sizeof(double));
	const int M = (ntaps - 1) / 2;	
	const double fwT0 = 2 * M_PI * cutoff_freq / sampling_rate;

	conf->coeff = malloc(ntaps * sizeof(double));
	conf->taps = ntaps;
	conf->decimation = decimation;
	
	compute_window(ntaps, w, window_type);
	
	for (n = 0; n < ntaps; n++) {
		conf->coeff[n] = 0.0;
	}
	
	for (n = -M; n <= M; n++) {
		if (n == 0) {
			conf->coeff[n + M] = fwT0 / M_PI * w[n + M];
		} else {
			conf->coeff[n + M] =  sin (n * fwT0) / (n * M_PI) * w[n + M];
		}
	}
	
	double fmax = conf->coeff[0 + M];
	
	for (n = 1; n <= M; n++) {
		fmax += 2 * conf->coeff[n + M];
	}
	
	gain /= fmax; // normalize
	
	for (i = 0; i < ntaps; i++) {
		conf->coeff[i] *= gain;
	}
	
	// init history
	conf->next= 0;
	conf->history = (float*)malloc(sizeof(float) * conf->taps);
	for (n= 0; n < conf->taps; n++) {
		conf->history[n]= 0;
    }
	
	free(w);
}


void ntaps_filter_ffd(ntaps_filter_conf* conf, const int input_size, const float input[], float* result) {
	assert(input_size == conf->decimation);
	
	int i;
	for (i = 0; i < conf->decimation; i++) {
		conf->history[conf->next]= input[i];
		conf->next = (conf->next + 1) % conf->taps;
    }
	
	float sum = 0.0;
	for (i = 0; i < conf->taps; i++) {
		sum = sum + conf->history[(conf->next + i) % conf->taps] * conf->coeff[conf->taps - i - 1];
    }
	
	*result = sum;
}


/* *********************************************** stereo_sum           */

void stereo_sum(const float data_spm, const float data_smm, float* left, float* right) {
	*left = (data_spm + data_smm);
	*right= (data_spm - data_smm);
}


/* *********************************************** subctract            */

void subctract(const float i1, const float i2, float* result) {
	*result= i1 - i2;
}


/* *********************************************** program options      */

int read_opts(FILE** input_file, FILE** output_file, FILE** text_file) {	
	*input_file = fopen("input.dat", "r");
	if (*input_file == NULL) {
		printf("Cannot open input file input.dat\n");
		return 1;
	}
	
	*output_file = fopen("FmRadio.raw", "w");
	if (*output_file == NULL) {
		printf("Cannot open output file FmRadio.raw\n");
		return 1;
	}
	
	*text_file = fopen("FmRadio.txt", "w");
	if (*text_file == NULL) {
		printf("Cannot open output file FmRadio.txt\n");
		return 1;
	}
	
	return 0;
}



/* *********************************************** main                 */

int main(int argc, char* argv[]) {
	struct timeval startTime, endTime;
	gettimeofday(&startTime, 0);
	//- - - -
	
	const int final_audio_frequency = 64 * KSMPS;
	const float input_rate = 512 * KSMPS;
	const float inout_ratio = ((int) input_rate)/final_audio_frequency;
	
	FILE* input_file;
	FILE* output_file;
	FILE* text_file;
	
	if (read_opts(&input_file, &output_file, &text_file) == 1) {
		return 1;
    }
	
	ntaps_filter_conf lp_11_conf, lp_12_conf;
	ntaps_filter_conf lp_21_conf, lp_22_conf;
	ntaps_filter_conf lp_2_conf, lp_3_conf;
	
	ntaps_filter_ffd_init(&lp_11_conf, 53000, 4000, 1,   1,           input_rate, WIN_HANNING);
	ntaps_filter_ffd_init(&lp_12_conf, 23000, 4000, 1,   1,           input_rate, WIN_HANNING);
	ntaps_filter_ffd_init(&lp_21_conf, 21000, 2000, 1,   1,           input_rate, WIN_HANNING);
	ntaps_filter_ffd_init(&lp_22_conf, 17000, 2000, 1,   1,           input_rate, WIN_HANNING);
	ntaps_filter_ffd_init(&lp_2_conf,  15000, 4000, 0.5, inout_ratio, input_rate, WIN_HANNING);
	ntaps_filter_ffd_init(&lp_3_conf,  15000, 4000, 1.0, inout_ratio, input_rate, WIN_HANNING);
	
	fm_quad_demod_filter fm_qd_conf;
	fm_quad_demod_init(&fm_qd_conf);
	
	float read_buffer[16];
	float fm_qd_buffer[8];
	float ffd_buffer[8];
	float fm_qd_value=0.0;
	float band_11=0.0;
	float band_12=0.0;
	float band_21=0.0;
	float band_22=0.0;
	float band_2= 0.0;
	float band_3= 0.0;
	float resume_1=0.0;
	float resume_2=0.0;
	float output1 = 0.0;
	float output2 = 0.0;
	float ffd_value=0.0;
	short output_short[2];
	
	while (fread(read_buffer, sizeof(float), 16, input_file) == 16) {
		int i;
		for (i = 0; i < 8; i++) {
			fm_quad_demod(&fm_qd_conf, read_buffer[i*2], read_buffer[i*2 + 1], &fm_qd_value);
			fm_qd_buffer[i]= fm_qd_value;
			
			ntaps_filter_ffd(&lp_11_conf, 1, &fm_qd_value, &band_11);
			ntaps_filter_ffd(&lp_12_conf, 1, &fm_qd_value, &band_12);
			ntaps_filter_ffd(&lp_21_conf, 1, &fm_qd_value, &band_21);
			ntaps_filter_ffd(&lp_22_conf, 1, &fm_qd_value, &band_22);
			
			subctract(band_11, band_12, &resume_1);
			subctract(band_21, band_22, &resume_2);
			
			multiply_square(resume_1, resume_2, &ffd_value);
			
			ffd_buffer[i]= ffd_value;
		}
		
		ntaps_filter_ffd(&lp_2_conf, 8, fm_qd_buffer, &band_2);
		ntaps_filter_ffd(&lp_3_conf, 8, ffd_buffer, &band_3);
		stereo_sum(band_2, band_3, &output1, &output2);
		
		output_short[0]= dac_cast_trunc_and_normalize_to_short(output1);
		output_short[1]= dac_cast_trunc_and_normalize_to_short(output2);
		fwrite(output_short, sizeof(short), 2, output_file);
		fprintf(text_file, "%-10.5f %-10.5f\n", output1, output2);
	}
	
	fclose(input_file);
	fclose(output_file);
	fclose(text_file);
	
	//- - - -
	gettimeofday(&endTime,0);
	long seconds = endTime.tv_sec - startTime.tv_sec;
	int microseconds = endTime.tv_usec - startTime.tv_usec;
	if (microseconds < 0) {
		microseconds += 1000000;
		seconds--;
	}
	
	printf("Runtime: %ld.%.6d\r\n", seconds, microseconds);
	
	return 0;
}

