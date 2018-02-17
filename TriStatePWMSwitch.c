/*
 * File:   RCindicator.c
 * Author: neil
 *
 * Created on January 2, 2017, 2:30 PM
 */


#include <xc.h>

#pragma config FOSC=INTOSC, WDTE=ON, MCLRE=OFF, CP=OFF, BOREN=OFF
#pragma config config WRT=OFF, PLLEN=OFF, STVREN=ON, BORV=LO, LVP=OFF


//Definitions
#define _XTAL_FREQ 	16000000
#define PWM_In		PORTAbits.RA4
#define SW0		LATAbits.LATA0
#define SW1		LATAbits.LATA1
#define SW2	        LATAbits.LATA2
#define PulseCENTER	24000


// Global Variables
volatile unsigned int PulseValue;	//this will contain our counter value
struct
{
    volatile unsigned PulseFound :1;	// flag used to indicate that pulse value has been grabbed

}flags;

void interrupt Timer1_Gate_ISR()
{
	PulseValue = TMR1;	//read timer value
    	flags.PulseFound=1;	//set flag for main routine to use
	TMR1=0;			//clear timer value
    	PIR1bits.TMR1GIF=0;	//clear the interrupt flag
	T1GCONbits.T1GGO=1;	//set the T1GGO/Done bit so it starts looking for pulse again
}




void main() {
    unsigned int Timer1Count;	//this will contain the Timer1 count value
    // set up oscillator control register
    OSCCONbits.SPLLEN=0;	//PLL is disabled
    OSCCONbits.IRCF=0X0F;	//set OSCCCON IRCF bits to select OSC frequency=16MHz
    OSCCONbits.SCS=0x02;	//sset the SCS bits to select internal oscillator block
    //OSCON should be 0x7AH now.
    WDTCONbits.WDTPS=0x05;      //watchdog prescale every 32ms as pulse gap is 20ms
 

	//set up I/O pins
    ANSELAbits.ANSELA=0;
    ADCON0bits.ADON=0;
    DACCON0bits.DACEN=0;
    TRISA=0x10;
    T1CONbits.TMR1CS=0x01;
    T1CONbits.T1CKPS0=0;
    T1CONbits.T1CKPS1=0;
    T1CONbits.nT1SYNC=1;
    T1CONbits.TMR1ON=1;

    T1GCONbits.TMR1GE=1;
    T1GCONbits.T1GPOL=1;
    T1GCONbits.T1GTM=0;
    T1GCONbits.T1GSPM=1;
    T1GCONbits.T1GSS=0x00;

    TMR1=0;
    PIR1bits.TMR1GIF=0;
    INTCONbits.PEIE=1;
    INTCONbits.GIE=1;
    PIE1bits.TMR1GIE=1;
    T1GCONbits.T1GGO=1;

    flags.PulseFound=0;
    PulseValue=0;

    do{ // main loop
	if (flags.PulseFound==1)
	{

	di();
     	Timer1Count = PulseValue;
	CLRWDT();
    	ei();

	if (Timer1Count < (PulseCENTER - 2000)){
		SW0=1;
		SW1=1;
		SW2=0;
		//__delay_ms(200);
		//SW1=0;	
		//__delay_ms(400);
	}
	else if (Timer1Count > (PulseCENTER + 2000)) {
                SW0=0;	
		SW1=1;
		SW2=1;
		//__delay_ms(200);
		//SW2=0;
		//__delay_ms(400);
        }
	else { // Pulse is at center value
	        SW0=0;
		SW1=0;
		SW2=1;
        }

	flags.PulseFound=0; //clear flag

    }
    else
	{
	NOP(); //or do stuff if pulse not found
    	__delay_ms(1);
        }
    }while (1);
     

}
