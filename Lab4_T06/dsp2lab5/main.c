#include <stdio.h>
#include <c6x.h>
#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>
#include "dsk6713.h"
#include "dsk6713_aic23.h"
#include "C6xdsk.h"
#include "math.h"
#define pi 3.14159265358979323846

static DSK6713_AIC23_CodecHandle hCodec;							// Codec handle
static DSK6713_AIC23_Config config = { \
		0x0017,  /* 0 DSK6713_AIC23_LEFTINVOL  Left line input channel volume */ \
		0x0017,  /* 1 DSK6713_AIC23_RIGHTINVOL Right line input channel volume */\
		0x01f9,  /* 2 DSK6713_AIC23_LEFTHPVOL  Left channel headphone volume */  \
		0x01f9,  /* 3 DSK6713_AIC23_RIGHTHPVOL Right channel headphone volume */ \
		0x0011,  /* 4 DSK6713_AIC23_ANAPATH    Analog audio path control */      \
		0x0000,  /* 5 DSK6713_AIC23_DIGPATH    Digital audio path control */     \
		0x0000,  /* 6 DSK6713_AIC23_POWERDOWN  Power down control */             \
		0x0043,  /* 7 DSK6713_AIC23_DIGIF      Digital audio interface format */ \
		0x0001,  /* 8 DSK6713_AIC23_SAMPLERATE Sample rate control */            \
		0x0001   /* 9 DSK6713_AIC23_DIGACT     Digital interface activation */   \
	};  // Codec configuration with default settings
int fft_enable,k,count_in;
short buffer_fft_input[128];
short w[64];
short sum_Xk_square[64];
short real,imag;
short index[8];
short buffer_in[64];
short input;
int i;
short ham64[64];
interrupt void serial_port_rcv_isr(void);
void init_hw_interrupts(void);


void fft_coef(void){
	int i;
	short w[64];
	for(i=0;i<32;i++){
		w[i*2] = 32767 * (-cos( i*2*pi/64));
		w[i*2+1] = 32767 * (-sin( i*2*pi/64));
		}
}

void main()
{
	int input;
	DSK6713_init();		// Initialize the board support library, must be called first
	hCodec = DSK6713_AIC23_openCodec(0, &config);	// open codec and get handle
	// set codec sampling frequency 48kHz
	DSK6713_AIC23_setFreq(hCodec, DSK6713_AIC23_FREQ_8KHZ);
	*(unsigned volatile int *)McBSP1_RCR = 0x00A0;
	*(unsigned volatile int *)McBSP1_XCR = 0x00A0;
//	comm_intr();
	fft_coef();
	bitrev_index(buffer_fft_input, index,64);
	init_hw_interrupts();
	input = input_leftright_sample();
	output_leftright_sample(input);
	while(1){
		if(fft_enable==1)
		{
			DSP_radix2 (64, buffer_fft_input, w);

			DSP_bitrev_cplx ((int *)buffer_fft_input, index, 64);
			for(k=0;k<64;k++){
				real=((int)buffer_fft_input[2*k]*(int)buffer_fft_input[2*k])>>15;
				imag=((int)buffer_fft_input[2*k+1]*(int)buffer_fft_input[2*k+1])>>15;
				sum_Xk_square[k]=real+imag;
			}
		}

	};  // wait for interrupts

}

// interrupt service routine
void init_hw_interrupts(void)
{

	IRQ_globalDisable();			// Globally disables interrupts
	IRQ_nmiEnable();				// Enables the NMI interrupt

	// Maps an event to a physical interrupt
	// We can set the interrupt number wherever we like in vectors.asm ( this is the interrupt number )
	// Interrupt number is the second argument in IRQ_map define
	// The first argument must be set to a physical event and here we use the mcbsp1
	// and we want the receive interrupt so we have RINT1
	IRQ_map(IRQ_EVT_RINT1, 11);
	IRQ_enable(IRQ_EVT_RINT1);		// Enables the event
	IRQ_globalEnable();				// Globally enables interrupts

}


// interrupt service routine
interrupt void serial_port_rcv_isr()
{
	input = input_leftright_sample();

	if(count_in<64){
	fft_enable=0;
	buffer_in[count_in]=(((int)input*(int)ham64[count_in])>>15);		//oso einai count_in<64 gemise to buffer_in
	count_in++;}

	if(count_in==64)
	{   fft_enable=1;
		for(i=0;i<64;i++){

			buffer_fft_input[i*2]=buffer_in[i];//antegrapse moy ta dedomena ston buffer gia th radix
			buffer_fft_input[i*2+1]=0;

	}
		count_in=0;//molis teleiwseis me thn adigrafh count_in=0 gia na peraseis
		}

	
	output_leftright_sample(input);
}
