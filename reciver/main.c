#define __AVR_ATtiny13A__
#define F_CPU 9600000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

uint8_t playSongFlag = 0;
uint8_t rxSequencyFlag = 0;
uint16_t msCounter = 0;
uint16_t prevMsCounter = 0;
uint8_t toneCounter = 0;

uint16_t buffer = 0;

typedef struct
{
    uint16_t length;
    uint8_t tone;
} SONG;
#define NOTE(x,t) OCR0A=x; _delay_ms(t);
#define PAUSE(t) DDRB&=~(1<<PINB0); _delay_ms(t); DDRB|=(1<<PINB0);

void configuration();
void checkSequency();

int main()
{
    configuration();
    playSongFlag = 0;
    while(1)
    {
        if (rxSequencyFlag)
        {
            checkSequency();
        }
        rxSequencyFlag = 0;
        //sleep
        if(!playSongFlag && !rxSequencyFlag)
        {
            DDRB &= ~(1<<PINB0);
            cli();
            sleep_enable();
            sei();
            sleep_cpu();
        }
        if(playSongFlag && !rxSequencyFlag)
        {
            DDRB=(1<<PIN0);
            //-------------- SONG ------------------
            for (uint8_t i=0; i<6; i++)
            {
                NOTE(25,50);
                PAUSE(50);
            }
            NOTE(40, 300);
            NOTE(100, 300);
            NOTE(35, 250);
            NOTE(95, 250);
            NOTE(30, 200);
            NOTE(90, 200);
            NOTE(25, 150);
            NOTE(85, 150);
            NOTE(22, 125);
            NOTE(82, 125);
            NOTE(20, 100);
            NOTE(80, 100);
            NOTE(18, 75);
            NOTE(78, 75);
            NOTE(16, 50);
            NOTE(76, 50);
            NOTE(14, 40);
            NOTE(74, 40);
            NOTE(12, 30);
            NOTE(72, 30);
            NOTE(11, 20);
            NOTE(71, 20);
            for (uint8_t i=1; i<120;i++)
            {
                NOTE(i,10)
            }
            playSongFlag=0;
        }
        
    }
}

void configuration()
{
    cli();
    //Enable pin change interrupt on PB3
    GIMSK = (1<<PCIE);
    PCMSK = (1<<PCINT3);

    DDRB=(1<<PIN0);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    //Timer in CTC Mode, toggle OC0A on compare match
    //TIMSK0=(1<<OCIE0B);
    TCCR0A=(1<<WGM01)|(0<<WGM00)|(1<<COM0A0);
    //Prescaler 1024 freq=9.6MHz/1024=9.375kHz
    TCCR0B=(1<<CS02)|(1<<CS00);
    TCNT0=0;
    OCR0A=10; //For buzzer
    DDRB &= ~(1<<PINB0);
    sei();
}

void checkSequency()
{
    cli();
    uint8_t bitCounter = 0;
    for (uint8_t i=0; i<16; i++)
    {
        if (PINB & (1<<PORTB3)) bitCounter++;
        _delay_ms(1);
    }
    if (bitCounter<14) 
    {
        sei();
        return;
    }
    bitCounter = 0;
    for (uint8_t i=0; i<16; i++)
    {
        if (~PINB & (1<<PORTB3)) bitCounter++;
        _delay_ms(1);
    }
    if (bitCounter<14)
    {
        sei();
        return;
    }
    /*
    for (uint8_t k=0; k<3; k++)
    {
        bitCounter = 0;
        for (uint8_t i=0; i<6; i++)
        {
            if (PINB & (1<<PORTB3)) bitCounter++;
            _delay_ms(1);
        }
        if (bitCounter<3)
        {
            sei();
            return;
        }
        bitCounter = 0;
        for (uint8_t i=0; i<7; i++)
        {
            if (~PINB & (1<<PORTB3)) bitCounter++;
            _delay_ms(1);
        }
        if (bitCounter<3)
        {
            sei();
            return;
        }
    }
    */
    rxSequencyFlag=0;
    playSongFlag=1;
    sei();
    return;
    bitCounter = 0;
    for (uint8_t i=0; i<7; i++)
    {
        if (PINB & (1<<PORTB3)) bitCounter++;
        _delay_ms(1);
    }
    if (bitCounter<4)
    {
        sei();
        return;
    }
    rxSequencyFlag=0;
    playSongFlag=1;
    sei();
    return;
    bitCounter = 0;
    for (uint8_t i=0; i<7; i++)
    {
        if (~PINB & (1<<PORTB3)) bitCounter++;
        _delay_ms(1);
    }
    if (bitCounter<2)
    {
        sei();
        return;
    }
    rxSequencyFlag=0;
    playSongFlag=1;
    sei();
    return;
}

ISR(PCINT0_vect)
{
    rxSequencyFlag=1;
    sleep_disable();
}
