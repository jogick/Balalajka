#define F_CPU 9600000UL // �������� ������� �����������
#define outA 3 // ���� �� ���� �������
#define outB 4 // ������ �����
#define LED PB1 // ����� ��� ���������� ������������� ������� ��������
#define controlInp PB0 // ���� �� ��������-������� ����
#define BUSE PB2 // ����� �� �����

#define tick_to_sleep 3*1800 // ����� ����� ������� ��������� ����� = (3 * ����� � ��������)
#define out_timer 1000 // ��������� ������� ��� ��������� �������

#include <avr/io.h>
#include <avr/interrupt.h>

unsigned int out_count; // ������ ��� ��������� ���������������� �������
unsigned char tick; // ����� �������
unsigned int time_to_sleep; // ������ ��� ���
unsigned char state; // ������� ��������� �������

int main (){
	DDRB = 0b00011110;
	PORTB = 0b00000001;

	TIMSK0 = (1<<OCIE0A);

	TCCR0A = (1<<WGM01);
	TCCR0B = 1<<CS01 | 1<<CS00;
	OCR0A = 0x02;

	MCUCR = 1 << SM1;

	sei();
	while (1){
		if (out_count == out_timer) {
			state = ~(state) & 0x01;
			out_count = 0;
			time_to_sleep++;
		}
		if (state) {
			PORTB &= ~(1 << outA);
			PORTB |= (1 << outB);
		} else {
			PORTB |= (1 << outA);
			PORTB &= ~(1 << outB);

		}


		if (time_to_sleep == tick_to_sleep){
			DDRB = 0x00;
			PORTB = 0x00;
			MCUCR |= 1 << SE;
			asm("SLEEP");
		}
	}
}


ISR (TIM0_COMPA_vect) {
	out_count++;
	tick = ~(tick) & 0x01;
	
	if ((!(PINB & 0x01)) & (out_count > 200)){
		PORTB |= 1 << LED;
		if (tick == 0) {
			PORTB |= (1<<BUSE);
		} else {
			PORTB &= ~(1<<BUSE);
		}
		time_to_sleep = 0;
	} else {
		PORTB &= ~(1 << LED);
		PORTB &= ~(1 << BUSE);
	}
}