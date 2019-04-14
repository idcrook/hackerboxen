/*
 * File:   blink.c
 * Author: dpc
 *
 * Created on April 1, 2019, 7:09 PM
 */

// PIC16F628A Configuration Bit Settings

// 'C' source line config statements

// CONFIG
/* #pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSC oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN) */
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator: High-speed crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is digital input, MCLR internally tied to VDD)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB4/PGM pin has digital I/O function, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Data memory code protection off)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.
/* #define _XTAL_FREQ 4000000 */
#define _XTAL_FREQ 16000000


#include <xc.h>

void main(void) {
    TRISA = 0b00000000;
    while (1)
    {   
        PORTAbits.RA3 = 1;
        __delay_ms(1000);
        PORTAbits.RA3 = 0;
        __delay_ms(1000);
    } 
}
