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

// <editor-fold defaultstate="collapsed" desc="SPECIFICHE PROTOCOLLO SERIALE">

#define BAUDRATE 9600
#define BUFMAX 3 //lunghezza del messaggio e quindi del buffer da specifiche
#define RXADDRESS 0 //posizione dell'inidirizzo destinatario da specifiche
#define MYADDRESS 0x01 //indirizzo scheda
#define BROADCASTADDRESS 0xff //indirizzo broadcast

// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="COMANDI LCD">

//Clear Display
#define L_CLR 0X01

//Character Entry Mode
#define L_EM 0x06 //Increment - Shift on (The pos increment but the text not)

//Display On/Off & Cursor
#define L_OFF 0X08 //Display Off
#define L_ON 0X0C //Display On (Prev 0X0F)
//#define L_CUR 0X0F //Display On - Underline On - Underline Blink On
#define L_CUR 0X0E //Display On - Underline On - Underline Blink Off
#define L_NCR 0x0C //Display On - Underline Off - Underline Blink Off

//Function Set
#define L_CFG 0X38 //Full 8bit input - 2 Line mode - 5*7 Dot format

//Set Display Address Position
#define L_L1_C1 0X80 //Cursor on first col of first line
#define L_L2_C1 0XC0 //Cursor on first col of second line
#define L_L1_C3 0x82
#define L_L2_C3 0xC2

//Pin Setup
#define L_RS 0X04 //LCD Register Selector (Command or Character)
#define L_EN 0X02 //LCD Enable

// </editor-fold>

#include <xc.h>

void initPic(void);
void ISR_Init(void);
void UART_Init(long int);
void clearBuff(char *, unsigned char, unsigned char *);
void decode(void);
void LCD_Init(void);
void LCD_Send(char, char);
void LCD_Write(char[]);

unsigned char timeCount; //contatore per interrupt
unsigned char dataReceived[BUFMAX]; //array di lunghezza fissa da specifiche
unsigned char indexReceived; //indice di inserimento del byte nel buffer
char isReceived; //flag di ricezione completa

void main(void) {
    initPic();
    while(1){
        if(isReceived){
            //tratto il dato
            decode();
            //ripulisco il buffer
            clearBuff(dataReceived, BUFMAX, &indexReceived);
            isReceived= 0;
        }
        //stampo il dato/reagisco al comando
    }
    return;
}

// <editor-fold defaultstate="collapsed" desc="INIT SECTION">

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
    ISR_Init();
    UART_Init(BAUDRATE);
    LCD_Init();
}

//INIT per Interrupt TMR0
void ISR_Init(void){
    //abilito GIE (0x80) e TMR0IE (0x20)
    INTCON= 0xA0;
    //carico il prescaler al massimo
    OPTION_REG= 0x07;
    //precarico il TMR0
    TMR0= 6;
}

//INIT per UART e Interrupt UART
void UART_Init(long int baudRate){
    //imposto come uscita tx
    TRISC&= ~0x40;
    //imposto come ingressi rx
    TRISC|= 0x80;
    
    //abilito TXEN (20) e BRGH [trasmissione alta velocità] (04)
    TXSTA|= 0x24;
    //abilito SPEN (80) e OPEN (10)
    RCSTA|= 0x90;
    //calcolo della frequenza di trasmissione
    SPBRG= (char) (_XTAL_FREQ/(unsigned long int) (64UL * (unsigned long int)baudRate))-1;
    
    //abilito GIE e PIE per interrupt su uart
    INTCON|= 0x80;
    INTCON|= 0x40;
    //abilito interrupt su rx
    PIE1|= 0x20;
    //pulisco buffer e azzero index
    clearBuff(dataReceived, BUFMAX, &indexReceived);
    isReceived= 0;
}

//INIT per LCD
void LCD_Init()
{
    //imposto come uscita RS (0X04) ed EN (0x02) del display 
    TRISE &= ~0x06;
    //imposto come uscita tutta portD
    TRISD &= 0x00;
    
    PORTE &= ~L_RS;
    PORTE &= ~L_EN;
    __delay_ms(20);
    PORTE |= L_EN;
    LCD_Send(L_CFG, 1);
    __delay_ms(5);
    LCD_Send(L_CFG, 1);
    __delay_ms(1);
    LCD_Send(L_CFG, 1);
    LCD_Send(L_OFF, 1);
    LCD_Send(L_ON, 1);
    LCD_Send(L_CLR, 1);
    LCD_Send(L_CUR, 1);
    LCD_Send(L_L1_C1, 1);
}

// </editor-fold>

//==============================================================================

// <editor-fold defaultstate="collapsed" desc="INTERRUPT SECTION">

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
    if(RCIF){
        //quando riceve un byte in seriale salva nell'array e aumenta l'indice
        dataReceived[indexReceived++]= RCREG;
        if(indexReceived == BUFMAX){
            //se l'array è pieno alza il flag di ricezione 
            isReceived= 1;
        }
        RCIF= 0;
    }
}

// </editor-fold>

//==============================================================================

// <editor-fold defaultstate="collapsed" desc="UART SECTION">

//ripulisce il buffer del messaggio
void clearBuff(char *buf, unsigned char dim, unsigned char *index){
    for (unsigned char i= 0; i < dim; i++){
        buf[i]= 0;
    }
    *index= 0;
}

//decodifica il messaggio
void decode(){
    if(dataReceived[RXADDRESS] == MYADDRESS || dataReceived[RXADDRESS] == BROADCASTADDRESS){
        
    }
}

// </editor-fold>

//==============================================================================

// <editor-fold defaultstate="collapsed" desc="DISPLAY SECTION">

//mand i singoli caratteri all'LCD
void LCD_Send(char data, char mode)
{
    //portare a 1 ( PORT |= byte ) 
    //portare a 0 ( PORT &= ~byte )
    PORTE |= L_EN;
    PORTD = data;   
    (mode) ? (PORTE = PORTE & ~L_RS) : (PORTE |= L_RS); //(true) ? (se vero)CMD : (se falso)DATA
    __delay_ms(3);
    PORTE &= ~L_EN;
    __delay_ms(3);
    PORTE |= L_EN;
}

//passa ogni carattere della stringa all'LCD_Send
void LCD_Write(char phrase[])
{
    //if(phrase[j] == '\0')
    for(int j = 0; j < 32; j++)
    {
        if(phrase[j] == '\0'){break;};
        if (j == 16){LCD_Send(L_L2_C1, 1);}
        if (j == 32){LCD_Send(L_L1_C1, 1);}
        LCD_Send(phrase[j], 0);
    }
}

// </editor-fold>