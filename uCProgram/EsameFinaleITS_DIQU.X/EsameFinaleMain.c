/*
 * File:   EsameFinaleMain.c
 * Author: Davide Scapolan
 *
 * Created on 9 luglio 2022, 13.04
 */


/*
================================================================================
 CONFIG
================================================================================ 
*/
#pragma config FOSC = HS // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN = ON // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = ON // Low-Voltage (Single-Supply) In-Circuit Serial 
//Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming 
//enabled)
#pragma config CPD = OFF // Data EEPROM Memory Code Protection bit (Data EEPROM 
//code protection off)
#pragma config WRT = OFF // Flash Program Memory Write Enable bits (Write 
//protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF // Flash Program Memory Code Protection bit (Code 
//protection off)

#define _XTAL_FREQ 8000000

#include <xc.h>

void initPic(void);

unsigned char timeCount; //contatore per interrupt

void main(void) {
    return;
}

/*
================================================================================
 INIT
================================================================================
*/
//INIT Pic generico, richiama tutti gli altri init del caso
void initPic(){
    //imposto tutto come uscita
    TRISA= 0x00;
    TRISB= 0x00;
    TRISC= 0x00;
    TRISD= 0x00;
    TRISE= 0x00;
    //inizializzo le variabili
    timeCount= 0;
    //inizializzo il timer
    setTimer0();
}
//INIT per Interrupt
void setTimer0(void){
    INTCON= 0xA0;
    OPTION_REG= 0x07;
    TMR0= 6;
}

/*
================================================================================
 INTERRUPT
================================================================================
*/
//interrupt ogni 32 ms
void __interrupt() ISR(){
    if(T0IF){
        //ricarico il timer
        TMR0= 6;
        
        timeCount++;
        //ogni 1 s circa faccio lampeggiare RB7 come segnale di funzionamento
        if(timeCount > 30){
            timeCount= 0;
            PORTB^= 0x80;
        }
        
        T0IF=0;
    }
}