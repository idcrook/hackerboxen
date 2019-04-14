/*
 * File:   gps_clock.c
 * Author: dpc
 *
 * Created on April 11, 2019, 1:23 PM
 */

// X8C global option has to be set to C Standard C90

// INCLUDES
#include <xc.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator: High-speed crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is digital input, MCLR internally tied to VDD)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB4/PGM pin has digital I/O function, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Data memory code protection off)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// DEFINITIONS
#define _XTAL_FREQ 16000000     // refenrence freq for the compiler
#define Rs PORTAbits.RA0        // Rs is connected to RA0
#define En PORTAbits.RA1        // En is connected to RA1

#define lcd_port PORTB          // lcd port b
//#define lcd_port TRISB          // lcd port b
#define LCD_LINE_1 0x00           // line one begins here
#define LCD_LINE_2 0x40           // line two begins here
#define LCD_LINE_3 0x14           // line three begins here
#define LCD_LINE_4 0x54           // line four begins here

// GLOBAL VARIABLES
unsigned char ch, i;
unsigned char gpstime[7];
unsigned char gpsdate[7];
unsigned char buffer[7];
unsigned char lat[10];
unsigned char lon[11];
//unsigned char gps_header[]="GPRMC,";
unsigned char gps_header[]="GNRMC,";
volatile bool getchar_active = false;

// FUNCTION PROTOTYPE
int lcd_strobe(void);
void lcd_write(unsigned char dat);
void lcd_cmd(unsigned char cmd);
void lcd_data(unsigned char dat);
void lcd_init(void);
void lcd_writestr(const unsigned char *c);
void lcd_setcursor(unsigned char loc);
void lcd_line1(void);
void lcd_line2(void);
void uart_init(void);
unsigned char getch(void);
//char getch();
void interrupt tc_int(void);
void notvaliddate(void);
void notvalidtime(void);

//// FUNCTION PROTOTYPE
//int lcd_strobe(void);
//void lcd_write(unsigned char dat);
//void lcd_cmd(unsigned char cmd);
//void lcd_data(unsigned char dat);
//void lcd_init(void);
//void lcd_writestr(const unsigned char *c);
//void lcd_setcursor(unsigned char loc);
//void lcd_line1(void);
//void lcd_line2(void);

// function to pulse the En pin high
int lcd_strobe(void)
{
    En = 1;
    __delay_us(1);
    En = 0;
    return 0;
}

// function to write to the lcd port
void lcd_write(unsigned char dat)
{
    lcd_port &= 0x0f;           // get current port and clear upper bits
    lcd_port |= (dat & 0xf0);   // combine upper and lower, leave lower
    lcd_strobe();

    lcd_port &= 0x0f;
    lcd_port |= ((dat << 4) & (0xf0));  // combine upper and lower
    lcd_strobe();
    __delay_ms(2);                      // little delay for lcd to process
}

// function to set lcd in command mode
void lcd_cmd(unsigned char cmd)
{
    Rs = 0;             // rs low for cmd mode
    lcd_write(cmd);
}

// functionto set lcd in data mode
void lcd_data (unsigned char dat)
{
    Rs = 1;             // rs high for data mode
    lcd_write(dat);
}

// function to initialize the lcd
void lcd_init(void)
{
    lcd_cmd(0x30);
    __delay_ms(50);

    lcd_cmd(0x20);  // 4-bit
    __delay_ms(10);

    lcd_cmd(0x28); //N = 1 (2 line display) F = 0 (5x8 characters)
    lcd_cmd(0x08); //Display on/off control D=0, C=0, B=0
    //lcd_cmd(0x0e);  // display on, cursor on, blink off
    lcd_cmd(0x01); //Clear Display
    lcd_cmd(0x06); //Entry mode set - I/D = 1 (increment cursor) & S = 0 (no shift)
    lcd_cmd(0x0C); //Display on/off control. D = 1, C and B = 0. (Cursor and blink, last two bits)
    //lcd_cmd(0x80);  // address ddram with 0 offset 80h
}

// function to write a strng to the lcd
void lcd_writestr(const unsigned char *c)
{
    while (*c != '\0')
    {
        lcd_data(*c);
        c++;
    }
}

// function to place the cursor on the lcd
void lcd_setcursor(unsigned char loc)
{
    lcd_cmd(loc | 0x80);
}

// function to move cursor to 1st place on line 1
void lcd_line1(void)
{
    lcd_setcursor(LCD_LINE_1);
}

// function to move cursor to 1st place on line 2
void lcd_line2(void)
{
    lcd_setcursor(LCD_LINE_2);
}

// function to move cursor to 1st place on line 3
void lcd_line3(void)
{
    lcd_setcursor(LCD_LINE_3);
}

// function to move cursor to 1st place on line 4
void lcd_line4(void)
{
    lcd_setcursor(LCD_LINE_4);
}

// function to initialize the uart port
void uart_init(void)
{
    TXSTAbits.BRGH = 0; // high baud selection bit, 1=high, 0=low
    TXSTAbits.SYNC = 0; // USART mode selection bit, 1=sync mode, 0=async mode
    TXSTAbits.TX9 = 0;  // 9-bit selection bit, 1=9-bit transmission, 0=8-bit trans
    RCSTAbits.CREN = 1; // continuous receive enable bit, 1=Enable continuous receive.
    /*
     * 16MHz
     * 16000000 / 9600 = 1666.6666
     * 1666.6666 / 64 = 26.0416
     * 26.0416 - 1 = 25.0416
     * 25.0416 = 25
     *
     * 16000000 / 4800 = 3333.3333
     * 3333.3333 / 64 = 52.0833
     * 52.0833 - 1 = 51.0833
     * 51.0833 = 51
     */
    SPBRG = 25; // 9600-n-8-1

    PIE1bits.RCIE = 1;  // USART Receive interrupt enable bit, 1=enable, 0=disable
    RCSTAbits.SPEN = 1; // serial port enable bit, 1=serial port enable, 0=disable
    TXSTAbits.TXEN = 1; // transmit enable bit, 1=Transmit enable, 0=disable
    return;
}

// function to read a char from the uart
unsigned char getch(void)
{
    getchar_active = true;
    while (getchar_active)
        continue;
    return RCREG;
}

// interrupt function, gets called on each received char
void interrupt tc_int(void)
{
    if (RCIF && RCIF)
    {
        getchar_active = false;
        RCREG;
    }
    return;
}

// function to display invalid time
void notvalidtime(void)
{
    lcd_line1();
    for (i=0;i<6;i++)
    {
        gpstime[i] == ' '; // empty array
    }
    lcd_writestr("  No valid time     ");
    __delay_ms(2000);
}

// function to display invalid date
void notvaliddate(void)
{
    for (i=0;i<6;i++)
    {
        gpsdate[i] == ' '; // empty array
    }
    lcd_line2();
    lcd_writestr("  No valid date     ");
    __delay_ms(2000);
}



void main(void)
{
    CMCON = 0x07;
    TRISA = 0b00000000;
    TRISB = 0b00000110;
    PORTA = 0b00000000;
    PORTB = 0b00000000;

    lcd_strobe();
    lcd_init();

    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

    lcd_strobe();
    lcd_init();
    uart_init();

    lcd_writestr("    PIC 16F628A  ");
    lcd_line2();
    lcd_writestr("     GPS CLOCK   ");
    __delay_ms(2000);
    lcd_line3();
    lcd_writestr(" so   very     much ");
    lcd_line4();
    lcd_writestr("hack   chill    wow!");

    while (1)
      {
        ch = getch();       // read the uartport, assign value to ch
        if (ch == '$')      // check if we have a $-sign, then we know it is the start of a sentence
          {
            for (i=0;i<6;i++) // read the 6 first chars after the $
              {
                ch = getch(); // read one char
                buffer[i] = ch; // put it in buffer array
              }
            if (strcmp(buffer, gps_header)==0) // check if buffer is equal to our header
              {
                for (i=0;i<6;i++) // reading gps time
                  {
                    ch = getch();
                    gpstime[i] = ch;
                  }
                for (i=0;i<5;i++)   // reading bytes before date
                  {
                    ch = getch();   // just going through the chars
                  }
                for (i=0;i<9;i++)   // reading latitude
                  {
                    ch = getch();
                    lat[i] = ch;
                  }
                for (i=0;i<3;i++)   // reading 3 bytes between lat & lon
                  {
                    ch = getch();   // just going through the chars
                  }
                for (i=0;i<11;i++)  // reading longitude
                  {
                    ch = getch();
                    lon[i] = ch;
                  }
                for (i=0;i<12;i++)  // reading 16 bytes between lon & date
                  {
                    ch = getch();   // just going through the chars
                  }
                for (i=0;i<6;i++)   // reading gps date
                  {
                    ch = getch();
                    gpsdate[i] = ch;
                  }
                lcd_line1();
                if (gpstime[0] == ',') // Check if time is valid
                  {
                    notvalidtime();
                    lcd_line1();
                  }
                lcd_writestr("      ");
                lcd_data(gpstime[0]);
                lcd_data(gpstime[1]);
                lcd_writestr(":");
                lcd_data(gpstime[2]);
                lcd_data(gpstime[3]);
                lcd_writestr(":");
                lcd_data(gpstime[4]);
                lcd_data(gpstime[5]);
                lcd_writestr(" (UTC) ");
                lcd_line2();
                if (gpsdate[0] == ',') // check if date is valid
                  {
                    notvaliddate();
                    lcd_line2();
                  }
                lcd_writestr("      ");
                if (false) {
                    lcd_data(gpsdate[0]);
                    lcd_data(gpsdate[1]);
                    lcd_writestr("/");
                    lcd_data(gpsdate[2]);
                    lcd_data(gpsdate[3]);
                } else {
                    if (gpsdate[2] == '0') {
                        lcd_data(' ');
                    } else {
                        lcd_data(gpsdate[2]);
                    }
                    lcd_data(gpsdate[3]);
                    lcd_writestr("/");
                    lcd_data(gpsdate[0]);
                    lcd_data(gpsdate[1]);
                }
                lcd_writestr("/");
                lcd_data(gpsdate[4]);
                lcd_data(gpsdate[5]);
                lcd_writestr("      ");
              } // end if strcmp
          } // end if $
      } // end while

}
