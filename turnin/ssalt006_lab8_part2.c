/*	Author: Skyler Saltos 
 *  Partner(s) Name:  
 *	Lab Section:
 *	Assignment: Lab #8  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *	This lab has three buttons that toggle sound, and scale up and scale down
 *	a set list of frequencies respectively. 
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *
 *
 *	https://youtu.be/HcGjVineGbY
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

//array of tunes to hold the values for assignment 
static double tunes[] = { 261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25 }; 

//variable to hold the currentTune to play to speaker
 double currentTune = 261.63; // begin at start of array
static char nextTune;
static  char prevTune;
static char sound;
static char i = 0;


enum SM2_States { SM2_Start,SM2_NonePushed, SM2_NextPressed, SM2_nextHold, SM2_prevPressed, SM2_prevHold } SM2_State;

void InputDeterminator() {

	//unsigned char A0 = PINA & 0x01;
	unsigned char A1 = PINA & 0x02;
	unsigned char A2 = PINA & 0X04;



   switch(SM2_State) { // Transitions
      case SM2_Start:
         SM2_State = SM2_NonePushed;
         break;
      case SM2_NonePushed:
         if (!A1 && A2) {
            SM2_State = SM2_NextPressed;
         }
         else if (A1 && !A2) {
            SM2_State = SM2_prevPressed;
         }

	 else if ( A1 && A2 ) {

		SM2_State = SM2_NonePushed;
	 }//none pushed 
         break;
      case SM2_NextPressed:
         if (!A1 && A2) {
            SM2_State = SM2_nextHold;
         }

	 else if ( A1 && A2 ){
		SM2_State = SM2_NonePushed;


	 }//button released so go to NonePush
         break;
      case SM2_nextHold:
         if (!A1 && A2) {
            SM2_State = SM2_nextHold;
         }
         else if (A1 && A2) {
            SM2_State = SM2_NonePushed;
         }
         break;
      case SM2_prevPressed:
         if (A1 && !A2) {
            SM2_State = SM2_prevHold;
         }

	 else if ( A1 && A2){

	SM2_State = SM2_NonePushed;
	 }
         break;
      case SM2_prevHold:
         if (A1 && A2) {
            SM2_State = SM2_NonePushed;
         }
         else if (A1 && !A2) {
            SM2_State = SM2_prevHold;
         }
         break;
      default:
         SM2_State = SM2_Start;
      } // Transitions

   switch(SM2_State) { // State actions
      case SM2_NonePushed:
         nextTune = 0;
         prevTune  = 0;
         //currently on tune[0] from array
         break;
      case SM2_NextPressed:
         nextTune = 1;
         break;
      case SM2_nextHold:
         nextTune = 0;
         break;
      case SM2_prevPressed:
         prevTune = 1;
         break;
      case SM2_prevHold:
         prevTune = 0;
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}

enum SM3_States { SM3_Start,SM3_PlayCurrent, SM3_PlayNext, SM3_PlayPrev } SM3_State;

void TuneTransition() {


	//unsigned char A0 = PINA & 0x01;
	//unsigned char A1 = PINA & 0x02;
	//unsigned char A2 = PINA & 0X04;

   switch(SM3_State) { // Transitions
      case SM3_Start:
         SM3_State = SM3_PlayCurrent;
         break;
      case SM3_PlayCurrent:
         if (nextTune && !prevTune) {
            SM3_State = SM3_PlayNext;
         }
         else if (prevTune && !nextTune ) {
            SM3_State = SM3_PlayPrev;
         }
         else if (!nextTune && !prevTune) {
            SM3_State = SM3_PlayCurrent;
         }
         break;
      case SM3_PlayNext:
         if (1) {
            SM3_State = SM3_PlayCurrent;
         }
         break;
      case SM3_PlayPrev:
         if (1) {
            SM3_State = SM3_PlayCurrent;
         }
         break;
      default:
         SM3_State = SM3_Start;
      } // Transitions

   switch(SM3_State) { // State actions
      case SM3_PlayCurrent:
         if ( sound ) {
         
         set_PWM(currentTune);
         }
         break;
      case SM3_PlayNext:
         if( i <=7){
         
         
         currentTune = tunes[i];
         ++i;

         }//setPWM with currentTune
         
         if ( sound ) {
         
         set_PWM(currentTune); 
         }
         break;
      case SM3_PlayPrev:
         
         if( i >=1){
         
		 --i;
         currentTune = tunes[i];
         

         }//setPWM with currentTune
         
         
         if ( sound ) {
         set_PWM(currentTune); 
         }
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}

enum SM4_States { SM4_Start,SM4_Initial, SM4_SoundOn, SM4_SoundOff, SM4_WaitOn, SM4_WaitOff } SM4_State;

void SoundCheck() {
	unsigned char A0 = PINA & 0x01;
	unsigned char A1 = PINA & 0x02;
	unsigned char A2 = PINA & 0X04;


   switch(SM4_State) { // Transitions
      case SM4_Start:
         SM4_State = SM4_Initial;
         break;
      case SM4_Initial:
         if (!A0) {
            SM4_State = SM4_SoundOn;
         }
         break;
      case SM4_SoundOn:
         if (!A0) {
            SM4_State = SM4_SoundOn;
         }
         else if (A0) {
            SM4_State = SM4_WaitOn;
         }
         break;
      case SM4_SoundOff:
         if (!A0) {
            SM4_State = SM4_SoundOff;
         }
         else if (A0) {
            SM4_State = SM4_WaitOff;
         }
         break;
      case SM4_WaitOn:
         if (A0) {
            SM4_State = SM4_WaitOn;
         }
         else if (!A0) {
            SM4_State = SM4_SoundOff;
         }
         break;
      case SM4_WaitOff:
         if (A0) {
            SM4_State = SM4_WaitOff;
         }
         else if (!A0) {
            SM4_State = SM4_SoundOn;
         }
         break;
      default:
         SM4_State = SM4_Start;
      } // Transitions

   switch(SM4_State) { // State actions
      case SM4_Initial:
         set_PWM(0.0);
         sound = 0;
         break;
      case SM4_SoundOn:
         set_PWM(currentTune);
         sound = 1;
         break;
      case SM4_SoundOff:
        set_PWM(0.0);
         sound = 0;
         break;
      case SM4_WaitOn:
         break;
      case SM4_WaitOff:
         break;
      default: // ADD default behaviour below
         break;
   } // State actions
}//end state machine that determines when to play sound







int main(void) {
    /* Insert DDR and PORT initializations */
DDRA = 0x00; 	PORTA = 0xFF; // portA as input 

DDRB = 0xFF;	PORTB = 0x00; //portB as ouput 

	SM2_State = SM2_Start;
	SM3_State = SM3_Start;
	SM4_State = SM4_Start;

	PWM_on(); // enable speaker
    /* Insert your solution below */
    while (1) {
	    	
		TuneTransition();
		SoundCheck();
		InputDeterminator();

    }
    return 1;
}
