/*	Author: Skyler Saltos 
 *  Partner(s) Name:  
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *	This lab has three buttons which each play a different tune
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *
 *
 *	https://youtu.be/kkjpuVBLDU0
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif




void set_PWM(double frequency) {
    static double current_frequency;

    if (frequency != current_frequency) {
        if(!frequency)
            TCCR3B &= 0x08;
        else
            TCCR3B |= 0x03;

        if(frequency < 0.954)
            OCR3A = 0xFFFF;
        else if (frequency > 31250)
            OCR3A = 0x0000;
        else
            OCR3A = (short) (8000000 / (128 * frequency)) - 1;

        TCNT3 = 0;
        current_frequency = frequency;
    }
}//set frequency of PWM for speaker to play sound 

void PWM_on() {
    TCCR3A = (1 << COM3A0);
    TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
    set_PWM(0);
}//turns PWM on

void PWM_off() {
    TCCR3A = 0x00;
    TCCR3B = 0x00;
}//turns PWM_off






enum SM1_States {SM1_Start, SM1_Wait, SM1_PlayC4, SM1_PlayD4, SM1_PlayE4 } SM1_State;

void ThreeTonePlay() {


	unsigned int A0 = PINA & 0x01;
	unsigned int A1 = PINA & 0x02;
	unsigned int A2 = PINA & 0x04;
	static double C4 = 261.63;
	static double D4 = 293.66;
	static double E4 = 329.63;

   switch(SM1_State) { // Transitions
      case SM1_Start:
         SM1_State = SM1_Wait;
         break;

         case SM1_Wait:
         if (A0 && A1 && A2) {
            SM1_State = SM1_Wait;
         }
         else if (!A0 && A1 && A2) {
            SM1_State = SM1_PlayC4;
         }
         else if (A0 && !A1 && A2) {
            SM1_State = SM1_PlayD4;
         }
         else if (A0 && A1 && !A2) {
            SM1_State = SM1_PlayE4;
         }
         break;

      case SM1_PlayC4:
         if (!A0 && A1 && A2) {
            SM1_State = SM1_PlayC4;
         }
         else {
            SM1_State = SM1_Wait;
         }
         break;

      case SM1_PlayD4:
         if (A0 && !A1 && A2) {
            SM1_State = SM1_PlayD4;
         }
         else {
            SM1_State = SM1_Wait;
         }
         break;


      case SM1_PlayE4:
         if (A0 && A1 && !A2) {
            SM1_State = SM1_PlayE4;
         }
         else {
            SM1_State = SM1_Wait;
         }
         break;

      default:
         SM1_State = SM1_Start;
   } // Transitions

   switch(SM1_State) { // State actions
      case SM1_Wait:
        // PORTB = 0x00;
	set_PWM(0.0); //no frequenct to play
         break;
      case SM1_PlayC4:
        // PORTB = 0x40;
	set_PWM(C4); // play C4 tune
         break;
      case SM1_PlayD4:
         //PORTB = 0x40;
	 set_PWM(D4); // play D4 tune 
         break;
      case SM1_PlayE4:
         //PORTB = 0x40;
	 set_PWM(E4); // play E4 tune 
         break;
      default: // ADD default behaviour below
      break;
   } // State actions

}





int main(void) {
    /* Insert DDR and PORT initializations */
DDRA = 0x00; 	PORTA = 0xFF; // portA as input 

DDRB = 0xFF;	PORTB = 0x00; //portB as ouput 


	SM1_State = SM1_Start;

	PWM_on(); // enable speaker
    /* Insert your solution below */
    while (1) {
	ThreeTonePlay();
//	PWM_off(); //disable speaker functionality


    }
    return 1;
}
