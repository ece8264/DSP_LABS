#include <stdio.h>
#include <c6x.h>
#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>
#include "dsk6713.h"
#include "dsk6713_aic23.h"
#include "C6xdsk.h"
#include "math.h"
#include "sine8000_table.h"

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



interrupt void serial_port_rcv_isr(void);
void init_hw_interrupts(void);
short sine8000[8000];
float x[10],d[10],y[10],w[10],e[10];
float noise[10];
float bufferout[512];
float s;
float b=1.5;
float a=0.00001;
int n,i,k;
short bufferin[512];

void nlms(void){
	s=0; //CLEAR SUM S EVERY TIME n
	i=0;
	while(i<10){
	s=s+noise[((n-i)+10)%10]*noise[((n-i)+10)%10];//COMPUTE SUM S WITH INPUT DELAYED NOISE
	i++;}


	k=0;
	if(e[n%10]>0.001)
	while(k<10){

	w[(k)%10]=w[(k)%10] + (b/(a+s))*e[n%10]*noise[((n-k)+10)%10];
	k++;

	}
	n++;
};
// a(i%30) == a((30+i)%30)
void main()
{

	/*DSK6713_init();		// Initialize the board support library, must be called first
	hCodec = DSK6713_AIC23_openCodec(0, &config);	// open codec and get handle
	// set codec sampling frequency 48kHz
	DSK6713_AIC23_setFreq(hCodec, DSK6713_AIC23_FREQ_8KHZ);
	*(unsigned volatile int *)McBSP1_RCR = 0x00A0;
	*(unsigned volatile int *)McBSP1_XCR = 0x00A0;
//	comm_intr();
	init_hw_interrupts();*/
	while(n<512){
			bufferin[n]=sine8000[(n*500)%8000] + sine8000[(n*3900)%8000];
			x[n%10]=sine8000[(n*500)%8000] + sine8000[(n*3900)%8000];
			noise[n%10]=sine8000[((n*500)+2000)%8000];
			e[n%10]=fabsf( sine8000[(n*3900)%8000]-d[n%10]);

			nlms();



			y[n%10]=w[0]*noise[n%10]+w[1]*noise[(n-1+10)%10]+w[2]*noise[(n-2+10)%10]+w[3]*noise[(n-3+10)%10]+w[4]*noise[(n-4+10)%10]+w[5]*noise[(n-5+10)%10]+w[6]*noise[(n-6+10)%10]+w[7]*noise[(n-7+10)%10]+w[8]*noise[(n-8+10)%10]+w[9]*noise[(n-9+10)%10];//FIR FILTER
			d[n%10]=x[n%10]-y[n%10];

			bufferout[n]=d[n%10];

	}
	while(1);  // wait for interrupts

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



	// ---------------------- Useful code goes here ----------------------


	//ypologismos twn sydelestwn


	// -------------------------------------------------------------------



}
