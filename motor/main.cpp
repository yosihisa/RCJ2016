/*
 * モータードライバ.cpp
 *
 * Created: 2015/11/07 16:41:13
 * Author : yosihisa
 
 *方向HIで逆転
 
 */ 

#include <avr/io.h>

int main(void)
{
	DDRB =0b00011111;
	DDRD =0b01000000;
	DDRA =0b00000000;
	PORTD=0b00111100;
	PORTA=0b00000011;
    while (1) 
    {
		//モーターA
		if((PIND & (1<<PD4)) ==  (1<<PD4)){
			if((PIND & (1<<PD5)) ==  (1<<PD5)){
				PORTB&= ~(1<<PB3);
				PORTB|=  (1<<PB4);
			}else{
				PORTB&= ~(1<<PB4);
				PORTB|=  (1<<PB3);
			}
		}else{
			PORTB&=0b11100111;
		}
		
		//モーターB
		if((PIND & (1<<PD2)) ==  (1<<PD2)){
			if((PIND & (1<<PD3)) ==  (1<<PD3)){
				PORTB&= ~(1<<PB0);
				PORTD|=  (1<<PD6);
				}else{
				PORTD&= ~(1<<PD6);
				PORTB|=  (1<<PB0);
			}
			}else{
				PORTB&=0b11111110;
				PORTD&=0b10111111;
		}
		
		//モーターC
		if((PINA & (1<<PA0)) ==  (1<<PA0)){
			if((PINA & (1<<PA1)) ==  (1<<PA1)){
				PORTB&= ~(1<<PB2);
				PORTB|=  (1<<PB1);
				}else{
				PORTB&= ~(1<<PB1);
				PORTB|=  (1<<PB2);
			}
			}else{
				PORTB&=0b11111001;
		}
    }
}

