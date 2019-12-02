#include <stdio.h>
#include <c6x.h>
#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>
#include "dsk6713.h"
#include "dsk6713_aic23.h"
#include "C6xdsk.h"
#include <math.h>
#include "sine8000_table.h"
#include "sin480_table.h"
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

interrupt void serial_port_rcv_isr(void);
void init_hw_interrupts(void);

int data;
int n=0;
short bufferin[512],sin8000[8000],bufferout1[512],y,bufferout[512];
int w1[3],w2[3];
int x1,x2,X1,X2,W1,W2,W3,W4,W5,W6,W7,W8,W9,W10,W11;

short G1=420;
short G2=31;

short C1=43532;
short C2=15443;
short C3=16384;
short C4=56698;
short C5=49941;
short C6=7197;
short C7=43630;



//METABLHTES GIA TO FLOAT
float bufferout[512];
short bufferin[512];
float x[5];
float y[5];
short sine8000[8000];


void main()
{

	//DSK6713_init();		// Initialize the board support library, must be called first
	//hCodec = DSK6713_AIC23_openCodec(0, &config);	// open codec and get handle
	// set codec sampling frequency 48kHz
	//DSK6713_AIC23_setFreq(hCodec, DSK6713_AIC23_FREQ_8KHZ);
	//*(unsigned volatile int *)McBSP1_RCR = 0x00A0;
	//*(unsigned volatile int *)McBSP1_XCR = 0x00A0;
//	comm_intr();
	//init_hw_interrupts();*/
	// ---------------------- Useful code goes here ----------------------
	// bufferIN[n%512]=(short)data;




	while(n<512){

					bufferin[n]=sin8000[n];
					//1o sistima
					x1=sin8000[n];
					X1=(((int)x1)<<1);//eisodos sto 1o systhma

					W2=(((int)C1*w1[(n+1)%3])>>13);

					W3=(((int)C2*w1[(n)%3])>>13);


					w1[(n+3)%3]=X1-W2-W3;//endiamesh metavliti 1ou sistimatos


					W4=(((int)C3*w1[(n+2)%3])>>14);

					W5=(((int)C4*w1[(n+1)%3])>>14);

					W6=(((int)C3*w1[(n)%3])>>14);


				    x2=(((int)G1*(W4+W5+W6))>>15);//exodos 1ou systhmatos

				  //2o systhma

				    X2=((x2)<<1); //eisodos sto 2o systhma (exodos 1ou)

					W7=(((int)C5*w2[(n+1)%3])>>13);

				    W8=(((int)C6*w2[(n)%3])>>13);

				    w2[(n+3)%3]=(X2-W7-W8);//endiamesh exodos 2ou filtroy


				    W9=(((int)C3*w2[(n+2)%3])>>14);

				    W10=(((int)C7*w2[(n+1)%3])>>14);

				    W11=(((int)C3*w2[(n)%3])>>14);

				    y=(short)(((int)G2*(W9+W10+W11))>>15);//exodos

				    bufferout[n]=y;

				    n++;

//fixed point ylopoihsh
/*
	bufferin[n]=sine8000[(80*n)%8000]+sine8000[(500*n)%8000]+sine8000[(1200*n)%8000]+sine8000[(3500*n)%8000]+sine8000[(1450*n)%8000]+sine8000[(2500*n)%8000]+sine8000[(760*n)%8000];
	x[(n+4)%5]=sine8000[(80*n)%8000]+sine8000[(500*n)%8000]+sine8000[(1200*n)%8000]+sine8000[(3500*n)%8000]+sine8000[(1450*n)%8000]+sine8000[(2500*n)%8000]+sine8000[(760*n)%8000];
	y[(n+4)%5]=0.000012530*(x[(n+4)%5]-1.876*x[(n+3)%5]+2.721*x[(n+2)%5]-1.876*x[(n+1)%5]+0.9999*x[n%5])+2.295*y[(n+3)%5]-2.66*y[(n+2)%5]+1.487*y[(n+1)%5]-0.4141*y[(n)%5];
	bufferout[n]=y[(n+4)%5];
	n++;

*/


}
	while(1);
}












































































































































































	    // -------------------------------------------------------------------


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

	data = input_leftright_sample();
	// "data" contains both audio channels

	// ---------------------- Useful code goes here ----------------------


	// -------------------------------------------------------------------

	// process "data", or pass another variable to change the output
	output_leftright_sample(data);

}
