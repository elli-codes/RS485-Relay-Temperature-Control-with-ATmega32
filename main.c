#include <mega32.h>
#include <stdio.h>
#include <string.h>
#include <delay.h>

char data[32]; 
char buf[40];               // larger buffer
volatile unsigned char i = 0;
volatile int flag = 0;        // set by ISR
float v;

// forward
void rs485_send_string(const char *s);

/* RX ISR */
interrupt [USART_RXC] void usart_rx_isr(void)
{
    char c = UDR;
    
    if (c == '\r' || c == '\n') {
        data[i] = '\0';   // End string

        if (strstr(data, "relay1 on")) {
            flag = 1;
        }
        else if (strstr(data, "relay1 off")) {
            flag = 2;
        }
        else if (strstr(data, "relay2 on")) {
            flag = 3;
        }
        else if (strstr(data, "relay2 off")) {
            flag = 4;
        }
        else if (strstr(data, "temp?")) {
            flag = 5;
        }
        
        
        i = 0;   // reset buffer for next command
    } 
    else {
        if (i < 17) data[i++] = c;
    }
}

unsigned int adc_data;

// Voltage Reference: AVCC
#define ADC_VREF_TYPE ((0<<REFS1) | (1<<REFS0) | (0<<ADLAR))

interrupt [ADC_INT] void adc_isr(void)
{
    adc_data = ADCW;
}

unsigned int read_adc(unsigned char adc_input)
{
    ADMUX = adc_input | ADC_VREF_TYPE;
    delay_us(10);
    ADCSRA |= (1<<ADSC);           // Start conversion
    while ((ADCSRA & (1<<ADIF))==0); 
    ADCSRA |= (1<<ADIF);           // Clear flag
    return ADCW;
}


void main(void)
{
    DDRB = 0xFF;   // PB2 used as DE
    PORTB = 0x00;

    // USART init 9600 @ 8MHz, 8N1
    UCSRB = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);
    UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
    UBRRH = 0;
    UBRRL = 51;   // 9600 @ 8MHz

    // ADC init
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);

    #asm("sei")
    rs485_send_string("RS485 console\r\n");
    rs485_send_string("Type command:");

    while (1) {
        switch (flag) {
            case 1:
                PORTB |= (1<<0);   // Relay1 ON
                rs485_send_string("relay1 is ON\r\n");
                rs485_send_string("Command :");
                flag = 0;
                break;

            case 2:
                PORTB &= ~(1<<0);  // Relay1 OFF
                rs485_send_string("relay1 is OFF\r\n");
                rs485_send_string("Command :");
                flag = 0;
                break;

            case 3:
                PORTB |= (1<<1);   // Relay2 ON
                rs485_send_string("relay2 is ON\r\n");
                rs485_send_string("Command :");
                flag = 0;
                break;

            case 4:
                PORTB &= ~(1<<1);  // Relay2 OFF
                rs485_send_string("relay2 is OFF\r\n");
                rs485_send_string("Command :");
                flag = 0;
                break;

            case 5: {
                v = (float)read_adc(1) * 500.0 / 1023.0;
                
                sprintf(buf, "Temp= %.2f C\r\n", v);  // don't use printf()
                rs485_send_string(buf);
                rs485_send_string("Command :");
                flag = 0;
                break;
            }

            
              
        }
    }
}

/* Safe RS485 blocking send: clear TXC before send, cli() while DE is high */
void rs485_send_string(const char *s)
{
    unsigned int j;

    // Make critical section (prevent interrupts while DE is high)
    #asm("cli")

    // 1) enable DE (driver)
    PORTB |= (1<<2);
    delay_ms(1);                // give transceiver time to enable

    // 2) clear TXC so we can wait for this new transmission
    UCSRA |= (1<<TXC);         // write 1 to clear TXC

    // 3) send bytes
    for (j = 0; s[j]; ++j) {
        while (!(UCSRA & (1<<UDRE))); // wait until UDR ready
        UDR = s[j];
    }

    // 4) wait until this frame fully transmitted
    while (!(UCSRA & (1<<TXC)));
    UCSRA |= (1<<TXC);         // clear TXC (optional)

    // 5) small extra hold to be sure transceiver finished (increase if needed)
    delay_ms(2);

    // 6) disable DE -> back to receive
    PORTB &= ~(1<<2);

    // end critical section
    #asm("sei")
}
