/*	Author: lab
 *  Partner(s) Name: none
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <timer.h>
#include <stdlib.h>
#include <scheduler.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char pattern = 0x80;
unsigned char row = 0xFE;

void Set_A2D_Pin(unsigned char pinNum) {
	ADMUX = (pinNum <= 0x07) ? pinNum : ADMUX;
	static unsigned char i = 0;
	for (i = 0; i < 15; i++) { asm("nop"); }
}

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	//ADEN: setting this bit enables analog-to-digital conversion
	//ADSC: setting this bit starts the first conversion.
	//ADATE: setting this bit enables auto-triggering. Since we are in
			// Free Running Mode, a new conversion will trigger whenever the previous conversion completes
}

unsigned char row;
unsigned char userpaddle_pattern;

unsigned char enemyrow;
unsigned char enemypattern;

unsigned char userpaddle_pos;
unsigned char enemypos;

unsigned char enemyrand;

unsigned char temp_ballpattern;

unsigned char ballpattern;
unsigned char ballrow;

unsigned char button; //p2, reset... ~PINA

unsigned char p2paddle;
unsigned char p2row;
unsigned char p2on;

enum userpaddle_States{start1, init1, nomove1, up1, down1};

int userpaddle_Tick(int state){
	Set_A2D_Pin(0x01);
	switch(state){
		case start1:
			state = init1;
			break;
		case init1:
			state = nomove1;
			break;
		case nomove1:
			if(ADC > 512 +100){
				state = up1;
			}
			else if(ADC < 512-100){
				state = down1;
			}
			break;
		case up1:
			if(ADC < 512-100){
				state = down1;
			}
			else{
				state = nomove1;
			}
			break;
		case down1:
			if(ADC > 512 +100){
				state = up1;
			}
			else{
				state = nomove1;
			}
			break;
		default:
			state = start1;
			break;
		}
	switch(state){
		case start1:
			break;
		case init1:
			//default user paddle position
			userpaddle_pattern= 0x80;
			row = 0xc7;
			break;
		case nomove1:
			break;
		case up1:
			if(row == 0xf8){
				row = 0xf1;
				userpaddle_pos = 5;
			}
			else if(row == 0xf1){
				row = 0xe3;
				userpaddle_pos = 4;
			}
			else if(row == 0xe3){
				row = 0xc7;
				userpaddle_pos = 3;
			}
			else if(row == 0xc7){
				row = 0x8f;
				userpaddle_pos = 2;
			}
			else if(row == 0x8f){
				row = 0x1f;
				userpaddle_pos = 1;
			}
			break;
		case down1:
			if(row == 0x1f){
				row = 0x8f;
				userpaddle_pos = 2;
			}
			else if(row == 0x8f){
				row = 0xc7;
				userpaddle_pos = 3;
			}
			else if(row == 0xc7){
				row =0xe3;
				userpaddle_pos = 4;
			}
			else if(row == 0xe3){
				row = 0xf1;
				userpaddle_pos=5;
			}
			else if(row == 0xf1){
				row = 0xf8;
				userpaddle_pos = 6;
			}
			break;
		default:
			break;
	}
	PORTC = userpaddle_pattern;
	PORTD = row;
	return state;
}

//skeleton of enemy paddle
//
enum enemypaddle_States{start2, init2, nomove2, up2, down2};

int enemypaddle_Tick(int state){
	switch(state){
		case start2:
			if(!p2on){
				state = init2;
			}
			break;
		case init2:
			state = nomove2;
			break;
		case nomove2:
			if(ballrow <= 0xe3 && ballpattern == 0x08){
				state = down2;
			}
			else if(ballrow > 0xe3 && ballpattern == 0x08){
				state =up2;
			}

			break;
		case up2:
			state = nomove2;
			break;
		case down2:
			state = nomove2;
			break;
		default:
			state = start2;
			break;
	}
	switch(state){
		case start2:
			break;
		case init2:
			enemyrow = 0xc7;
			enemypattern = 0x01;
			enemypos =3;
			break;
		case nomove2:
			break;
		case up2:
			enemyrand = (rand() % (4-1+1))+1;

			if(enemyrand ==1){
				//do not move
			}
			else if(enemyrand == 2){//1 up
				if(enemyrow == 0xf8){
					enemyrow = 0xf1;
					enemypos = 5;
				}
				else if(enemyrow == 0xf1){
					enemyrow = 0xe3;
					enemypos = 4;
				}
				else if(enemyrow == 0xe3){
					enemyrow = 0xc7;
					enemypos =3;
				}
				else if(enemyrow == 0xc7){
					enemyrow = 0x8f;
					enemypos = 2;
				}
				else if(enemyrow == 0x8f){
					enemyrow = 0x1f;
					enemypos =1;
				}
			}
			else if(enemyrand == 3){
				if(enemyrow == 0xf8){//2 up
					enemyrow = 0xe3;
					enemypos = 4;
				}
				else if(enemyrow == 0xf1){
					enemyrow = 0xc7;
					enemypos =3;
				}
				else if(enemyrow == 0xe3){
					enemyrow = 0x8f;
					enemypos = 2;
				}
				else if(enemyrow == 0xc7){
					enemyrow = 0xf1;
					enemypos =1;
				}
				else if(enemyrow == 0x8f){
					enemyrow = 0xf1;
					enemypos =1;
				}
			}
			else if(enemyrand == 4){ //1 down
				if(enemyrow == 0x1f){
					enemyrow = 0x8f;
					enemypos =2;
				}
				else if(enemyrow == 0x8f){
					enemyrow = 0xc7;
					enemypos =3;
				}
				else if(enemyrow == 0xc7){
					enemyrow = 0xe3;
					enemypos = 4;
				}
				else if(enemyrow == 0xe3){
					enemyrow = 0xf1;
					enemypos = 5;
				}
				else if(enemyrow == 0xf1){
					enemyrow = 0xf8;
					enemypos = 6;
				}
			}
			break;
		case down2:
			enemyrand = (rand() % (4-1+1))+1;

			if(enemyrand ==1){
				//do not move
			}
			else if(enemyrand == 2){//1 up
				if(enemyrow == 0xf8){
					enemyrow = 0xf1;
					enemypos = 5;
				}
				else if(enemyrow == 0xf1){
					enemyrow = 0xe3;
					enemypos = 4;
				}
				else if(enemyrow == 0xe3){
					enemyrow = 0xc7;
					enemypos =3;
				}
				else if(enemyrow == 0xc7){
					enemyrow = 0x8f;
					enemypos = 2;
				}
				else if(enemyrow == 0x8f){
					enemyrow = 0x1f;
					enemypos =1;
				}
			}
			else if(enemyrand == 3){
				if(enemyrow == 0xf1){//2 down
					enemyrow = 0xc7;
					enemypos = 3;
				}
				else if(enemyrow == 0x8f){
					enemyrow = 0xe3;
					enemypos = 4;
				}
				else if(enemyrow == 0xc7){
					enemyrow = 0xf1;
					enemypos = 5;
				}
				else if(enemyrow == 0xe3){
					enemyrow = 0xf8;
					enemypos = 6;
				}
				else if(enemyrow ==0xf1){
					enemyrow = 0xf8;
					enemypos = 6;
				}
			}
			else if(enemyrand == 4){ //1 down
				if(enemyrow == 0x1f){
					enemyrow = 0x8f;
					enemypos =2;
				}
				else if(enemyrow == 0x8f){
					enemyrow = 0xc7;
					enemypos =3;
				}
				else if(enemyrow == 0xc7){
					enemyrow = 0xe3;
					enemypos = 4;
				}
				else if(enemyrow == 0xe3){
					enemyrow = 0xf1;
					enemypos = 5;
				}
				else if(enemyrow == 0xf1){
					enemyrow = 0xf8;
					enemypos = 6;
				}
			}
			break;
		default:
			break;
	}
	PORTD = enemyrow;
	PORTC = enemypattern;
	return state;
}


enum ballmovements_States{start3, init3, ballup, ballcenter, balldown, nohit};

int ballmovements_Tick(int state){
	switch(state){
	case start3:
		//need any starting condition?
		state = init3;
		break;
	case init3:
		//need any starting condition?
		if(ballpattern == 0x40 && ballrow == 0x7f && userpaddle_pos == 1){
			state = balldown;
		}
		else if(ballpattern == 0x40 && ballrow == 0xbf && userpaddle_pos ==1){
			state = ballcenter;
		}
		else if(ballpattern == 0x40 && ballrow == 0xdf && userpaddle_pos ==1){
			state = balldown;
		}
		else if(ballpattern == 0x40 && ballrow == 0xbf && userpaddle_pos == 2){
			state = balldown;
		}
		else if(ballpattern == 0x40 && ballrow == 0xdf && userpaddle_pos ==2){
			state = ballcenter;
		}
		else if(ballpattern == 0x40 && ballrow == 0xef && userpaddle_pos ==2){
			state = balldown;
		}
		else if(ballpattern == 0x40 && ballrow == 0xdf && userpaddle_pos == 3){
			state = balldown;
		}
		else if(ballpattern == 0x40 && ballrow == 0xef && userpaddle_pos ==3){
			state = ballcenter;
		}
		else if(ballpattern == 0x40 && ballrow == 0xf7 && userpaddle_pos ==3){
			state = balldown;
		}
		else if(ballpattern == 0x40 && ballrow == 0xef && userpaddle_pos == 4){
			state = balldown;
		}
		else if(ballpattern == 0x40 && ballrow == 0xf7 && userpaddle_pos ==4){
			state = ballcenter;
		}
		else if(ballpattern == 0x40 && ballrow == 0xfb && userpaddle_pos ==4){
			state = balldown;
		}
		else if(ballpattern == 0x40 && ballrow == 0xf7 && userpaddle_pos == 5){
			state = balldown;
		}
		else if(ballpattern == 0x40 && ballrow == 0xfb && userpaddle_pos ==5){
			state = ballcenter;
		}
		else if(ballpattern == 0x40 && ballrow == 0xfd && userpaddle_pos ==5){
			state = balldown;
		}
		else if(ballpattern == 0x40 && ballrow == 0xfb && userpaddle_pos == 6){
			state = balldown;
		}
		else if(ballpattern == 0x40 && ballrow == 0xfd && userpaddle_pos ==6){
			state = ballcenter;
		}
		else if(ballpattern == 0x40 && ballrow == 0xfe && userpaddle_pos ==6){
			state = balldown;
		}
		else if(ballpattern == 0x02 && ballrow == 0x7f && enemypos == 1){
			state = balldown;
		}
		else if(ballpattern == 0x02 && ballrow == 0xbf && enemypos ==1){
			state = ballcenter;
		}
		else if(ballpattern == 0x02 && ballrow == 0xdf && enemypos ==1){
			state = balldown;
		}
		else if(ballpattern == 0x02 && ballrow == 0xbf && enemypos == 2){
			state = balldown;
		}
		else if(ballpattern == 0x02 && ballrow == 0xdf && enemypos ==2){
			state = ballcenter;
		}
		else if(ballpattern == 0x02 && ballrow == 0xef && enemypos ==2){
			state = balldown;
		}
		else if(ballpattern == 0x02 && ballrow == 0xdf && enemypos == 3){
			state = balldown;
		}
		else if(ballpattern == 0x02 && ballrow == 0xef && enemypos ==3){
			state = ballcenter;
		}
		else if(ballpattern == 0x02 && ballrow == 0xf7 && enemypos ==3){
			state = balldown;
		}
		else if(ballpattern == 0x02 && ballrow == 0xef && enemypos == 4){
			state = balldown;
		}
		else if(ballpattern == 0x02 && ballrow == 0xf7 && enemypos ==4){
			state = ballcenter;
		}
		else if(ballpattern == 0x02 && ballrow == 0xfb && enemypos ==4){
			state = balldown;
		}
		else if(ballpattern == 0x02 && ballrow == 0xf7 && enemypos == 5){
			state = balldown;
		}
		else if(ballpattern == 0x02 && ballrow == 0xfb && enemypos ==5){
			state = ballcenter;
		}
		else if(ballpattern == 0x02 && ballrow == 0xfd && enemypos ==5){
			state = balldown;
		}
		else if(ballpattern == 0x02 && ballrow == 0xfb && enemypos == 6){
			state = balldown;
		}
		else if(ballpattern == 0x02 && ballrow == 0xfd && enemypos ==6){
			state = ballcenter;
		}
		else if(ballpattern == 0x02 && ballrow == 0xfe && enemypos ==6){
			state = balldown;
		}
		break;
	case ballup:
		if(ballrow == 0x7f && ballpattern!=0x02){
			state = balldown;
		}
		else if(ballrow == 0x7f && ballpattern != 0x40){
			state = balldown;
		}
		else if(temp_ballpattern == 0x02 && ballpattern == 0x40){
			state = init3;
		}
		else if(temp_ballpattern == 0x40 && ballpattern == 0x02){
			state = init3;
		}
		break;
	case balldown:
		if(ballrow == 0xfe && ballpattern != 0x02){
			state = ballup;
		}
		else if(ballrow == 0xfe && ballpattern != 0x40){
			state = ballup;
		}
		else if (temp_ballpattern == 0x02 && ballpattern == 0x40){
			state = init3;
		}
		else if(temp_ballpattern == 0x40 && ballpattern == 0x02){
			state = init3;
		}

		break;
	case ballcenter:
		if(temp_ballpattern == 0x02 && ballpattern == 0x40){
			state = init3;
		}
		else if(temp_ballpattern == 0x40 && ballpattern == 0x02){
			state = init3;
		}
		break;
	default:
		state = start3;
		break;
	}
	switch(state){
		case start3:
			//need any condition?
			//
			//initial starting condition:
			ballpattern = 0x02;
			ballrow = 0xef;
			//ball is starting in middle at default paddle
			//position on enemy side
			break;
		case init3:
			temp_ballpattern = ballpattern;
			//need any more conditions?
			break;
		case ballup:
			if(temp_ballpattern == 0x40){
				ballpattern = ballpattern >> 1;
				ballrow = (ballrow >> 1) | 0x80;
			}
			else if(temp_ballpattern == 0x02){
				ballpattern = ballpattern << 1;
			       ballrow = (ballrow >> 1) | 0x80;
			}
	 		//any more conditions?
			break;
		case balldown:
			if(temp_ballpattern == 0x40){
				ballpattern = ballpattern >> 1;
				ballrow = (ballrow << 1) | 0x01;
			}
			else if(temp_ballpattern == 0x02){
				ballpattern = ballpattern << 1;
				ballrow = (ballrow >> 1) | 0x01;
			}
			//any more conditions?
			break;
		case ballcenter:
			if(temp_ballpattern == 0x40){
				ballpattern = ballpattern >>1;
			}
			else if(temp_ballpattern == 0x02){
				ballpattern = ballpattern << 1;
			}
			//any more conditions?
			break;
		default:
		break;
		}
//	return state;
	//remember to transmit ballpattern and ballrow data to LED matrix
	PORTD = ballrow;
	PORTC = ballpattern;
	return state;
}	

enum player2_States{waitStart, waitrelease, p2active, p2up, p2down, p2up_wait, p2down_wait, p2off};

int player2_Tick(int state){
	switch(state){
		case waitStart:
			if(button == 0x10){
				state = waitrelease;
			}
			break;
		case waitrelease:
			if(button == 0x00){
				state = p2active;
			}
			break;
		case p2active:
			if(button == 0x04){
				state = p2up;
			}
			else if(button == 0x08){
				state = p2down;
			}
			else if(button == 0x10){
				state = p2off;
			}
			break;
		case p2up:
			state = p2up_wait;
			break;
		case p2down:
			state = p2down_wait;
			break;
		case p2up_wait:
			if(button == 0x00){
				state = p2active;
			}
			break;
		case p2down_wait:
			if(button == 0x00){
				state = p2active;
			}
			break;
		case p2off:
			if(button == 0x00){
				state = waitStart;
			}
			break;
		}
	switch(state){
		case waitStart:
			p2on = 0;
			break;
		case waitrelease:
			p2on =1;
			p2paddle = 0x01;
			p2row = 0xc7;
			//default paddle pos
			break;
		case p2active:
			break;
		case p2up:
			if(p2row == 0xf8){
				p2row = 0xf1;
			}
			else if(p2row == 0xf1){
				p2row = 0xe3;
			}
			else if(p2row == 0xe3){
				p2row = 0xc7;
			}
			else if(p2row == 0xc7){
				p2row = 0x8f;
			}
			else if(p2row == 0x8f){
				p2row = 0x1f;
			}
			break;
		case p2down:
			if(p2row == 0x1f){
				p2row = 0x8f;
			}
			else if(p2row == 0x8f){
				p2row = 0xc7;
			}
			else if(p2row == 0xc7){
				p2row = 0xe3;
			}
			else if(p2row == 0xe3){
				p2row = 0xf1;
			}
			else if(p2row == 0xf1){
				p2row = 0xf8;
			}
			break;
		case p2up_wait:
			break;
		case p2down_wait:
			break;
		case p2off:
			p2on =0;
			p2row = 0xff;
			p2paddle = 0x00;
			break;
		default:
			break;
	}
//	return state;
	//dont forget to transmit data to LED matrix
	PORTD = p2row;
	PORTC = p2paddle;
	return state;
}

int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0X00; PORTA = 0XFF;
	DDRC = 0XFF; PORTC = 0X00;
	DDRD = 0XFF; PORTD = 0X00;

	button = ~PINA;
	
	ADC_init();

	srand(time(0));




	static task task1, task2, task3, task4;
	task *tasks[] = {&task1, &task2, &task3, &task4};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	task1.state = start;
	task1.period = 1;
	task1.elapsedTime = task1.period;
	task1.TickFct = &userpaddle_Tick;

	task2.state = start;
	task2.period = 1;
	task2.elapsedTime = task2.period;
	task2.TickFct = &enemypaddle_Tick;

	task3.state = start;
	task3.period = 1;
	task3.elapsedTime = task3.period;
	task3.TickFct = &ballmovements_Tick;

	task4.state = start;
	task4.period = 1;
	task4.elapsedTime = task4.period;
	task4.TickFct = &player2_Tick;



	unsigned long int findGCD(unsigned long int a, unsigned long int b){
		unsigned long int c;
		while(1){
			c = a%b;
			if (c==0){
				return b;
			}
			a = b;
			b = c;
		}
		return 0;
	}
	unsigned short i;
	unsigned long GCD = tasks[0]->period;
	for(i=1; i< numTasks; i++){
		GCD = findGCD(GCD, tasks[i]->period);
	}
	
	TimerSet(GCD);
	TimerOn();

    /* Insert your solution below */
    while (1) {
	for(i=0; i<numTasks; i++){
		if(tasks[i]->elapsedTime == tasks[i]->period){
			tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
			tasks[i]->elapsedTime = 0;
		}
		tasks[i]->elapsedTime += GCD;
	}

	while(!TimerFlag){}
	TimerFlag = 0;
    }
    return 1;
}