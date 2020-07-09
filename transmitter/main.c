#define __AVR_ATtiny13A__
#define F_CPU 9600000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define TXOne(ms) PORTB |= (1<<PORTB0); _delay_ms(ms);
#define TXZero(ms) PORTB &= ~(1<<PORTB0); _delay_ms(ms);

uint8_t TXFlag=0;

void configuration();

int main()
{
    configuration();
    while(1)
    {
        if (TXFlag)
        {
            TXOne(16);
            TXZero(16);
            for (uint8_t i=0; i<3; i++)
            {
                TXOne(8);
                TXZero(8);
            }
            _delay_ms(300);
            TXFlag=0;
        }
        else
        {
            cli();
            sleep_enable();
            sei();
            sleep_cpu();
        }
        
    }
}

void configuration()
{
    cli();
    PORTB = (1<<PORTB3); // enable pull-up
    //Enable pin change interrupt on PB3
    GIMSK = (1<<PCIE);
    PCMSK = (1<<PCINT3);

    DDRB=(1<<PIN0);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    /*
    //Timer in CTC Mode
    TIMSK0=(1<<OCIE0A);
    TCCR0A=(1<<WGM01)|(0<<WGM00);
    //No prescaler freq=9.6MHz --- 10us interval
    TCCR0B=(1<<CS00);
    TCNT0=0;
    OCR0A=96;
    */
    sei();
}

ISR(PCINT0_vect)
{
    TXFlag=1;
    sleep_disable();
}
