#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>

/////////////////////////////////////////////////////// START OF HEADER FILES ////////////////////////////////////////////////////////////////////////
 
//TIMER.H
 
// Permission to copy is granted provided that this header remains intact.
// This software is provided with no warranties.
 
////////////////////////////////////////////////////////////////////////////////
 
#ifndef TIMER_H
#define TIMER_H
 
volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.
 
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1ms
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks
 
// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}
 
void TimerOn() {
    TCCR1B  = 0x0B;
    OCR1A   = 125;
    TIMSK1  = 0x02;
    TCNT1 = 0;
    _avr_timer_cntcurr = _avr_timer_M;
    SREG = SREG | 0x80;
}
 
void TimerOff() {
    TCCR1B  = 0x00; // bit3bit2bit1bit0=0000: timer off
}
 
void TimerISR() {
    TimerFlag = 1;
}
 
ISR(TIMER1_COMPA_vect)
{
    _avr_timer_cntcurr--;           // Count down to 0 rather than up to TOP
    if (_avr_timer_cntcurr == 0) {  // results in a more efficient compare
        TimerISR();                 // Call the ISR that the user uses
        _avr_timer_cntcurr = _avr_timer_M;
    }
}
 
#endif //TIMER_H
 
//SCHEDULER.H
 
// Permission to copy is granted provided that this header remains intact.
// This software is provided with no warranties.
 
////////////////////////////////////////////////////////////////////////////////
 
#ifndef SCHEDULER_H
#define SCHEDULER_H
 
////////////////////////////////////////////////////////////////////////////////
//Functionality - finds the greatest common divisor of two values
//Parameter: Two long int's to find their GCD
//Returns: GCD else 0
unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
    unsigned long int c;
    while(1){
        c = a % b;
        if( c == 0 ) { return b; }
        a = b;
        b = c;
    }
    return 0;
}
////////////////////////////////////////////////////////////////////////////////
//Struct for Tasks represent a running process in our simple real-time operating system
typedef struct _task{
    // Tasks should have members that include: state, period,
    //a measurement of elapsed time, and a function pointer.
    signed   char state;        //Task's current state
    unsigned long period;       //Task period
    unsigned long elapsedTime;  //Time elapsed since last task tick
    int (*TickFct)(int);        //Task tick function
} task;
 
#endif //SCHEDULER_H
 
 
// NOKIA 5110 .C AND .H FILES FROM: https://github.com/LittleBuster/avr-nokia5110
 
/* Nokia 5110 LCD AVR Library
 *
 * Copyright (C) 2015 Sergey Denisov.
 * Written by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version 3
 * of the Licence, or (at your option) any later version.
 *
 * Original library written by SkewPL, http://skew.tk
 */
 
#ifndef __NOKIA_5110_H__
#define __NOKIA_5110_H__
 
/*
 * LCD's port
 */
#define PORT_LCD PORTB
#define DDR_LCD DDRB
 
/*
 * LCD's pins
 */
#define LCD_SCE 0
#define LCD_RST 1
#define LCD_DC 2
#define LCD_DIN 3
#define LCD_CLK 4
 
#define LCD_CONTRAST 0x40
 
/*
 * Must be called once before any other function, initializes display
 */
void nokia_lcd_init(void);
 
/*
 * Clear screen
 */
void nokia_lcd_clear(void);
 
/**
 * Power of display
 * @lcd: lcd nokia struct
 * @on: 1 - on; 0 - off;
 */
void nokia_lcd_power(uint8_t on);
 
/**
 * Set single pixel
 * @x: horizontal pozition
 * @y: vertical position
 * @value: show/hide pixel
 */
void nokia_lcd_set_pixel(uint8_t x, uint8_t y, uint8_t value);
 
/**
 * Draw single char with 1-6 scale
 * @code: char code
 * @scale: size of char
 */
void nokia_lcd_write_char(char code, uint8_t scale);
 
/**
 * Draw string. Example: writeString("abc",3);
 * @str: sending string
 * @scale: size of text
 */
void nokia_lcd_write_string(const char *str, uint8_t scale);
 
/**
 * Set cursor position
 * @x: horizontal position
 * @y: vertical position
 */
void nokia_lcd_set_cursor(uint8_t x, uint8_t y);
 
/*
 * Render screen to display
 */
void nokia_lcd_render(void);
 
 
#endif
 
const uint8_t CHARSET[][5] PROGMEM = {
    { 0x00, 0x00, 0x00, 0x00, 0x00 }, // 20 space
    { 0x00, 0x00, 0x5f, 0x00, 0x00 }, // 21 !
    { 0x00, 0x07, 0x00, 0x07, 0x00 }, // 22 "
    { 0x14, 0x7f, 0x14, 0x7f, 0x14 }, // 23 #
    { 0x24, 0x2a, 0x7f, 0x2a, 0x12 }, // 24 $
    { 0x23, 0x13, 0x08, 0x64, 0x62 }, // 25 %
    { 0x36, 0x49, 0x55, 0x22, 0x50 }, // 26 &
    { 0x00, 0x05, 0x03, 0x00, 0x00 }, // 27 '
    { 0x00, 0x1c, 0x22, 0x41, 0x00 }, // 28 (
    { 0x00, 0x41, 0x22, 0x1c, 0x00 }, // 29 )
    { 0x14, 0x08, 0x3e, 0x08, 0x14 }, // 2a *
    { 0x08, 0x08, 0x3e, 0x08, 0x08 }, // 2b +
    { 0x00, 0x50, 0x30, 0x00, 0x00 }, // 2c ,
    { 0x08, 0x08, 0x08, 0x08, 0x08 }, // 2d -
    { 0x00, 0x60, 0x60, 0x00, 0x00 }, // 2e .
    { 0x20, 0x10, 0x08, 0x04, 0x02 }, // 2f /
    { 0x3e, 0x51, 0x49, 0x45, 0x3e }, // 30 0
    { 0x00, 0x42, 0x7f, 0x40, 0x00 }, // 31 1
    { 0x42, 0x61, 0x51, 0x49, 0x46 }, // 32 2
    { 0x21, 0x41, 0x45, 0x4b, 0x31 }, // 33 3
    { 0x18, 0x14, 0x12, 0x7f, 0x10 }, // 34 4
    { 0x27, 0x45, 0x45, 0x45, 0x39 }, // 35 5
    { 0x3c, 0x4a, 0x49, 0x49, 0x30 }, // 36 6
    { 0x01, 0x71, 0x09, 0x05, 0x03 }, // 37 7
    { 0x36, 0x49, 0x49, 0x49, 0x36 }, // 38 8
    { 0x06, 0x49, 0x49, 0x29, 0x1e }, // 39 9
    { 0x00, 0x36, 0x36, 0x00, 0x00 }, // 3a :
    { 0x00, 0x56, 0x36, 0x00, 0x00 }, // 3b ;
    { 0x08, 0x14, 0x22, 0x41, 0x00 }, // 3c <
    { 0x14, 0x14, 0x14, 0x14, 0x14 }, // 3d =
    { 0x00, 0x41, 0x22, 0x14, 0x08 }, // 3e >
    { 0x02, 0x01, 0x51, 0x09, 0x06 }, // 3f ?
    { 0x32, 0x49, 0x79, 0x41, 0x3e }, // 40 @
    { 0x7e, 0x11, 0x11, 0x11, 0x7e }, // 41 A
    { 0x7f, 0x49, 0x49, 0x49, 0x36 }, // 42 B
    { 0x3e, 0x41, 0x41, 0x41, 0x22 }, // 43 C
    { 0x7f, 0x41, 0x41, 0x22, 0x1c }, // 44 D
    { 0x7f, 0x49, 0x49, 0x49, 0x41 }, // 45 E
    { 0x7f, 0x09, 0x09, 0x09, 0x01 }, // 46 F
    { 0x3e, 0x41, 0x49, 0x49, 0x7a }, // 47 G
    { 0x7f, 0x08, 0x08, 0x08, 0x7f }, // 48 H
    { 0x00, 0x41, 0x7f, 0x41, 0x00 }, // 49 I
    { 0x20, 0x40, 0x41, 0x3f, 0x01 }, // 4a J
    { 0x7f, 0x08, 0x14, 0x22, 0x41 }, // 4b K
    { 0x7f, 0x40, 0x40, 0x40, 0x40 }, // 4c L
    { 0x7f, 0x02, 0x0c, 0x02, 0x7f }, // 4d M
    { 0x7f, 0x04, 0x08, 0x10, 0x7f }, // 4e N
    { 0x3e, 0x41, 0x41, 0x41, 0x3e }, // 4f O
    { 0x7f, 0x09, 0x09, 0x09, 0x06 }, // 50 P
    { 0x3e, 0x41, 0x51, 0x21, 0x5e }, // 51 Q
    { 0x7f, 0x09, 0x19, 0x29, 0x46 }, // 52 R
    { 0x46, 0x49, 0x49, 0x49, 0x31 }, // 53 S
    { 0x01, 0x01, 0x7f, 0x01, 0x01 }, // 54 T
    { 0x3f, 0x40, 0x40, 0x40, 0x3f }, // 55 U
    { 0x1f, 0x20, 0x40, 0x20, 0x1f }, // 56 V
    { 0x3f, 0x40, 0x38, 0x40, 0x3f }, // 57 W
    { 0x63, 0x14, 0x08, 0x14, 0x63 }, // 58 X
    { 0x07, 0x08, 0x70, 0x08, 0x07 }, // 59 Y
    { 0x61, 0x51, 0x49, 0x45, 0x43 }, // 5a Z
    { 0x00, 0x7f, 0x41, 0x41, 0x00 }, // 5b [
    { 0x02, 0x04, 0x08, 0x10, 0x20 }, // 5c backslash
    { 0x00, 0x41, 0x41, 0x7f, 0x00 }, // 5d ]
    { 0x04, 0x02, 0x01, 0x02, 0x04 }, // 5e ^
    { 0x40, 0x40, 0x40, 0x40, 0x40 }, // 5f _
    { 0x00, 0x01, 0x02, 0x04, 0x00 }, // 60 `
    { 0x20, 0x54, 0x54, 0x54, 0x78 }, // 61 a
    { 0x7f, 0x48, 0x44, 0x44, 0x38 }, // 62 b
    { 0x38, 0x44, 0x44, 0x44, 0x20 }, // 63 c
    { 0x38, 0x44, 0x44, 0x48, 0x7f }, // 64 d
    { 0x38, 0x54, 0x54, 0x54, 0x18 }, // 65 e
    { 0x08, 0x7e, 0x09, 0x01, 0x02 }, // 66 f
    { 0x0c, 0x52, 0x52, 0x52, 0x3e }, // 67 g
    { 0x7f, 0x08, 0x04, 0x04, 0x78 }, // 68 h
    { 0x00, 0x44, 0x7d, 0x40, 0x00 }, // 69 i
    { 0x20, 0x40, 0x44, 0x3d, 0x00 }, // 6a j
    { 0x7f, 0x10, 0x28, 0x44, 0x00 }, // 6b k
    { 0x00, 0x41, 0x7f, 0x40, 0x00 }, // 6c l
    { 0x7c, 0x04, 0x18, 0x04, 0x78 }, // 6d m
    { 0x7c, 0x08, 0x04, 0x04, 0x78 }, // 6e n
    { 0x38, 0x44, 0x44, 0x44, 0x38 }, // 6f o
    { 0x7c, 0x14, 0x14, 0x14, 0x08 }, // 70 p
    { 0x08, 0x14, 0x14, 0x18, 0x7c }, // 71 q
    { 0x7c, 0x08, 0x04, 0x04, 0x08 }, // 72 r
    { 0x48, 0x54, 0x54, 0x54, 0x20 }, // 73 s
    { 0x04, 0x3f, 0x44, 0x40, 0x20 }, // 74 t
    { 0x3c, 0x40, 0x40, 0x20, 0x7c }, // 75 u
    { 0x1c, 0x20, 0x40, 0x20, 0x1c }, // 76 v
    { 0x3c, 0x40, 0x30, 0x40, 0x3c }, // 77 w
    { 0x44, 0x28, 0x10, 0x28, 0x44 }, // 78 x
    { 0x0c, 0x50, 0x50, 0x50, 0x3c }, // 79 y
    { 0x44, 0x64, 0x54, 0x4c, 0x44 }, // 7a z
    { 0x00, 0x08, 0x36, 0x41, 0x00 }, // 7b {
    { 0x00, 0x00, 0x7f, 0x00, 0x00 }, // 7c |
    { 0x00, 0x41, 0x36, 0x08, 0x00 }, // 7d }
    { 0x10, 0x08, 0x08, 0x10, 0x08 }, // 7e ~
    { 0x00, 0x00, 0x00, 0x00, 0x00 } // 7f
};
 
static struct {
    /* screen byte massive */
    uint8_t screen[504];
 
    /* cursor position */
    uint8_t cursor_x;
    uint8_t cursor_y;
 
} nokia_lcd = {
    .cursor_x = 0,
    .cursor_y = 0
};
 
/**
 * Sending data to LCD
 * @bytes: data
 * @is_data: transfer mode: 1 - data; 0 - command;
 */
static void write(uint8_t bytes, uint8_t is_data)
{
    register uint8_t i;
    /* Enable controller */
    PORT_LCD &= ~(1 << LCD_SCE);
 
    /* We are sending data */
    if (is_data)
        PORT_LCD |= (1 << LCD_DC);
    /* We are sending commands */
    else
        PORT_LCD &= ~(1 << LCD_DC);
 
    /* Send bytes */
    for (i = 0; i < 8; i++) {
        /* Set data pin to byte state */
        if ((bytes >> (7-i)) & 0x01)
            PORT_LCD |= (1 << LCD_DIN);
        else
            PORT_LCD &= ~(1 << LCD_DIN);
 
        /* Blink clock */
        PORT_LCD |= (1 << LCD_CLK);
        PORT_LCD &= ~(1 << LCD_CLK);
    }
 
    /* Disable controller */
    PORT_LCD |= (1 << LCD_SCE);
}
 
static void write_cmd(uint8_t cmd)
{
    write(cmd, 0);
}
 
static void write_data(uint8_t data)
{
    write(data, 1);
}
 
/*
 * Public functions
 */
 
void nokia_lcd_init(void)
{
    register unsigned i;
    /* Set pins as output */
    DDR_LCD |= (1 << LCD_SCE);
    DDR_LCD |= (1 << LCD_RST);
    DDR_LCD |= (1 << LCD_DC);
    DDR_LCD |= (1 << LCD_DIN);
    DDR_LCD |= (1 << LCD_CLK);
 
    /* Reset display */
    PORT_LCD |= (1 << LCD_RST);
    PORT_LCD |= (1 << LCD_SCE);
    _delay_ms(10);
    PORT_LCD &= ~(1 << LCD_RST);
    _delay_ms(70);
    PORT_LCD |= (1 << LCD_RST);
 
    /*
     * Initialize display
     */
    /* Enable controller */
    PORT_LCD &= ~(1 << LCD_SCE);
    /* -LCD Extended Commands mode- */
    write_cmd(0x21);
    /* LCD bias mode 1:48 */
    write_cmd(0x13);
    /* Set temperature coefficient */
    write_cmd(0x06);
    /* Default VOP (3.06 + 66 * 0.06 = 7V) */
    write_cmd(0xC2);
    /* Standard Commands mode, powered down */
    write_cmd(0x20);
    /* LCD in normal mode */
    write_cmd(0x09);
 
    /* Clear LCD RAM */
    write_cmd(0x80);
    write_cmd(LCD_CONTRAST);
    for (i = 0; i < 504; i++)
        write_data(0x00);
 
    /* Activate LCD */
    write_cmd(0x08);
    write_cmd(0x0C);
}
 
void nokia_lcd_clear(void)
{
    register unsigned i;
    /* Set column and row to 0 */
    write_cmd(0x80);
    write_cmd(0x40);
    /*Cursor too */
    nokia_lcd.cursor_x = 0;
    nokia_lcd.cursor_y = 0;
    /* Clear everything (504 bytes = 84cols * 48 rows / 8 bits) */
    for(i = 0;i < 504; i++)
        nokia_lcd.screen[i] = 0x00;
}
 
void nokia_lcd_power(uint8_t on)
{
    write_cmd(on ? 0x20 : 0x24);
}
 
void nokia_lcd_set_pixel(uint8_t x, uint8_t y, uint8_t value)
{
    uint8_t *byte = &nokia_lcd.screen[y/8*84+x];
    if (value)
        *byte |= (1 << (y % 8));
    else
        *byte &= ~(1 << (y %8 ));
}
 
void nokia_lcd_write_char(char code, uint8_t scale)
{
    register uint8_t x, y;
 
    for (x = 0; x < 5*scale; x++)
        for (y = 0; y < 7*scale; y++)
            if (pgm_read_byte(&CHARSET[code-32][x/scale]) & (1 << y/scale))
                nokia_lcd_set_pixel(nokia_lcd.cursor_x + x, nokia_lcd.cursor_y + y, 1);
            else
                nokia_lcd_set_pixel(nokia_lcd.cursor_x + x, nokia_lcd.cursor_y + y, 0);
 
    nokia_lcd.cursor_x += 5*scale + 1;
    if (nokia_lcd.cursor_x >= 84) {
        nokia_lcd.cursor_x = 0;
        nokia_lcd.cursor_y += 7*scale + 1;
    }
    if (nokia_lcd.cursor_y >= 48) {
        nokia_lcd.cursor_x = 0;
        nokia_lcd.cursor_y = 0;
    }
}
 
void nokia_lcd_write_string(const char *str, uint8_t scale)
{
    while(*str)
        nokia_lcd_write_char(*str++, scale);
}
 
void nokia_lcd_set_cursor(uint8_t x, uint8_t y)
{
    nokia_lcd.cursor_x = x;
    nokia_lcd.cursor_y = y;
}
 
void nokia_lcd_render(void)
{
    register unsigned i;
    /* Set column and row to 0 */
    write_cmd(0x80);
    write_cmd(0x40);
 
    /* Write screen to display */
    for (i = 0; i < 504; i++)
        write_data(nokia_lcd.screen[i]);
}
 
//END 5100.C
 
// ADC_init() and ADCNum(unsigned char read_input) were from http://maxembedded.com/2011/06/the-adc-of-the-avr/
 
void ADC_init()
{
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}
 
unsigned char ADCNum(unsigned char read_input)
{
    read_input = read_input & 0x07;
    ADMUX = (ADMUX & 0xf8) | read_input;
    ADCSRA = ADCSRA | (1<<ADSC);
    while( (1<<ADSC) & (ADCSRA));
    return (ADC);
}
/////////////////////////////////////////////////////// END OF HEADER FILES //////////////////////////////////////////////////////////////////////////
