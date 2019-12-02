/*****************************************************************************
* FILENAME
*   Delays and echo.c
*
* DESCRIPTION
*   Delays and Echo using TMS320C6713 DSK Board and AIC23 audio codec.
*   Based on sampling rate of 48000 samples per second. Delay up to 4 seconds.  */
/*********************************************************************************/

#include <stdio.h>
#include "bargraph.h"
#include "stereo.h"
#include "switches.h"
#include "dsk6713.h"
#include "dsk6713_aic23.h"
#include "stdlib.h"
#include "math.h"
#include "c6713dsk.h"

#define N (48000 * 4)

// Codec configuration settings
DSK6713_AIC23_Config config = { \
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
};

unsigned int input32; /* a 32 bit input sample*/
unsigned int oldinput32; /* 32 bit old value from buffer*/
far unsigned int delay_array[N]; /* Buffer for maximum delay of 4 seconds */
int flagin=0;
int flagout=0;
short leftin;
short rightin;
unsigned output32;/*output with splited MSB and LSB*/
short leftout;
short rightout;
int count=0;
 unsigned int previous_delay;
/*
 Uses DIP switches to control the delay time between 0 ms and
 4 seconds. 48000 samples represent 1 second.
*/

unsigned int get_delay_time(void){
	unsigned int delay;
	delay=user_switches_read();   /*διαβασε την τιμη 0-7 και επεστρεψε την*/
	return delay;

}

/*
Take oldest sample from the array and replace with the newest
Uses a circular buffer because a straight buffer would be too slow.
*/

unsigned int delayed_input(unsigned int input,unsigned int delay){

	if(count<delay*24000){				/*24000 γιατι το delay κυμαινεται απο 0-7 αρα απο 0-3.5 sec καθυστερηση*/
		delay_array[count]=input;		/*fill in the buffer until count=N*/
		count++;
		return 0;
	}
	else{
		delay_array[count%(delay*24000) ]=input;	/*return to the first cell every time count>delay */
		count++;
		return delay_array[count%(delay*24000)+1];
	}
}


/*
Fill delay array with zeroes to prevent noise / clicks.
*/

void delay_array_clear(far unsigned int buffer[N]){
	int i=0;
	while(i<=N){
		buffer[i]=0;
		i++;
	}
}

/*
Show status on Stdout window.
*/

void switch_status_display(unsigned int delay){

if(previous_delay!=delay){							/*αν αλλαξουμε τα switces εκτυπωνει την κατασταση τους*/
	printf("switches_statous : %u\n",delay);
	previous_delay=delay;
}
}



int main(void)
{
	DSK6713_AIC23_CodecHandle hCodec;

	// Initialize BSL
	DSK6713_init();

	//Start codec
	hCodec = DSK6713_AIC23_openCodec(0, &config);

	// Set  frequency to 48KHz
	DSK6713_AIC23_setFreq(hCodec, DSK6713_AIC23_FREQ_48KHZ);
	 * (unsigned volatile int *)McBSP0_RCR  = 0x000000A0;
	 * (unsigned volatile int *)McBSP0_XCR	= 0x000000A0;

//------------------------------------------------------------------------------------------
//                            Enter the appropriate code here
//------------------------------------------------------------------------------------------
    delay_array_clear( delay_array);
    previous_delay=get_delay_time();/*αρχικοποιω την προηγούμενη κατάσταση για την stdout*/
    
	while(1){
	flagin=0;
	while((flagin==0)){
		flagin=DSK6713_AIC23_read(hCodec,&input32);
	}
	switch_status_display(get_delay_time());/*εκτυπωσε την κατασταση των switches */ 
	oldinput32=delayed_input(input32,(get_delay_time()));/*pare thn palia timh apo buffer kai xwrise th se 2 kanalia , me to 2o orisma kathorizw to delay toy buffer*/
	rightin=input32;						/*omoia me thn nea timh*/

	leftin=input32>>16;
	bargraph(5*stereo_to_mono(leftin,rightin)); /*apeikonish ths edashs toy hxoy mesw twn led*/
	rightout=stereo_to_mono(leftin,rightin); /*sto dexi h nea*/
	rightin=oldinput32;
	leftin=oldinput32>>16;
	leftout=stereo_to_mono(leftin,rightin);/*sto aristero h palia timh*/
	output32=(Uint32)leftout;
	output32=output32<<16;
	output32= ((Uint32)output32 |(0x00FFFF &(Uint32)rightout));/*exodos me palia kai nea timh sto kathe kanali*/




	flagout=0;
	while(flagout==0){
		flagout = DSK6713_AIC23_write(hCodec,output32>>16);
	}
}



//------------------------------------------------------------------------------------------
	return (0);
}
