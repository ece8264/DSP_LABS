#include <stdio.h>
#include <c6x.h>
#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>
#include "dsk6713.h"
#include "dsk6713_aic23.h"
#include "C6xdsk.h"

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
short w2,w3,w4,w5,w6; //variables for seperate Q15 multiplications
int argmax1,argmax2,data,j,k,subsample; 
int max1=0;
int max2=0;
int i=0;
int n=0;
short x[3];
short Q1[3];
short Xk[8];
short copikN[8]={27906,26797,25596,24295,19056,16528,25889,25889}; //coefficients for 8 different frequencies 
char tone[4][4]={{'1','2','3','A'},{'4','5','6','B'},{'7','8','9','C'},{'*','0','#','D'}}; //dual tone buttons

void max_Xk(void){     //used for finding the 2 bigest  of the 8 Xk^2   values 
	for(k=0;k<4;k++){
		if(Xk[k]>max1){
			max1=Xk[k];
			 argmax1=k;} //indexes used for printing the button (from tone array) which corresponds to the 2 frequencies
	}
	for(j=0;j<4;j++){   
		if((Xk[j+4]>max2))
			max2=Xk[j+4];
			argmax2=j;}
}





interrupt void serial_port_rcv_isr(void);
void init_hw_interrupts(void);

void main()
{

	DSK6713_init();		// Initialize the board support library, must be called first
	hCodec = DSK6713_AIC23_openCodec(0, &config);	// open codec and get handle
	// set codec sampling frequency 48kHz
	DSK6713_AIC23_setFreq(hCodec, DSK6713_AIC23_FREQ_48KHZ);
	*(unsigned volatile int *)McBSP1_RCR = 0x00A0;
	*(unsigned volatile int *)McBSP1_XCR = 0x00A0;
//	comm_intr();
	init_hw_interrupts();
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
	if(subsample%6==0){   //Fs=48kHz adding this condition is equivelant with using Fs' = Fs/6 = 8kHz (subsampling) 
	
	data = input_leftright_sample();
	// "data" contains both audio channels

	// ---------------------- Useful code goes here ----------------------

	
	x[n%3]=(short)data; //only one audio chanel is taken as input for processing
	if(i%6==0 || i%7==0) 
		w2=(short)(((int)copikN[i%8]*(int)Q1[(n-1+2)%3])>>15);
	else												//the 6 first values of coefficients are divided by 2 before they are converted in 2's complement 
		w2=(short)(((int)copikN[i%8]*(int)Q1[(n-1+2)%3])>>14);//so we have to shift them 14 bits instead of 15
                                             
	w3= Q1[(n-2+2)%3];                      
	Q1[(n+2)%3] = x[n%3] + w2 - w3;			//repeat these calculations 205 times 
	n++;									
	if(((n%205)==0) ){                      //when we reach n=205, Xk^2 value is calculated 
	w4=(short)(((int)Q1[(n+2)%3]*(int)Q1[(n+2)%3])>>15);
	w5=(short)((((int)Q1[(n-1+2)%3])*(int)Q1[(n-1+2)%3])>>15);
	if(i%6==0 || i%7==0)
	w6=(short)(((int)copikN[i%8]*((int)Q1[(n+2)%3]))>>15);
	else
	w6=(short)(((int)copikN[i%8]*((int)Q1[(n+2)%3]))>>14);

	w6=(short)(((int)w6*(int)Q1[(n-1+2)%3])>>15);
	Xk[i%8] = (w4 + w5 - w6);
	i++;        //each time we calculate 1 of the 8 values of Xk^2 i is increased
	Q1[0]=0;  //clear Q1 buffer for the next 205 calculations
	Q1[1]=0;
	Q1[2]=0;
	/*if((i%8)==0 ){    //every time i is a product of 8  print the selected button 
		max_Xk;
		printf("Tone : %c \n",tone[argmax1][argmax2]);
		}}*/

	// -------------------------------------------------------------------

	// process "data", or pass another variable to change the output
    //	output_leftright_sample(data);

}subsample++;}
