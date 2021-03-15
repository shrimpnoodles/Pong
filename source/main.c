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
#include <stdio.h>
#include <time.h>
#include <scheduler.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif


//unsigned char pattern = 0x80;
//unsigned char row = 0xFE;

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

unsigned char score;
unsigned char score2;

unsigned char row;
unsigned char userpaddle_pattern;

unsigned char enemyrow;
unsigned char enemypattern;

unsigned char userpaddle_pos;
unsigned char enemypos;


int enemyrand;

unsigned char temp_ballpattern;

unsigned char ballpattern;
unsigned char ballrow;

unsigned char button; //p2, reset... ~PINA

unsigned char p2paddle;
unsigned char p2row;
unsigned char p2on;

unsigned char intropat;
unsigned char introrow;

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
//	PORTC = userpaddle_pattern;
//	PORTD = row;
	return state;
}

//skeleton of enemy paddle
//

enum enemypaddle_States{start2, startwait, init2, waitclose, pos1, pos2, pos3, pos4, pos5, pos6, waitenemy };

int enemypaddle_Tick(int state){
	switch(state){
		case start2:
			if(!p2on){
				state = startwait;
			}
			break;
		case startwait:
			if(p2on){
				state = start2;
			}
			else{
				state = init2;
			}
			break;
		case init2:
			if(p2on){
				state = start2;
			}

			else if(ballpattern <= 0x04){
				state = waitclose;
			}
			break;
		case waitclose:
			if(p2on){
				state = start2;
			}

			else if(enemypos ==1){
				state = pos1;
			}
			else if(enemypos ==2){
				state = pos2;
			}
			else if(enemypos ==3){
				state = pos3;
			}
			else if(enemypos == 4){
				state = pos4;
			}
			else if(enemypos == 5){
				state = pos5;
			}
			else if(enemypos ==6){
				state = pos6;
			}
			break;
		case pos1:
			state = waitenemy;
			break;
		case pos2:
			state = waitenemy;
			break;
		case pos3:
			state = waitenemy;
			break;
		case pos4:
			state = waitenemy;
			break;
		case pos5:
			state = waitenemy;
			break;
		case pos6:
			state = waitenemy;
			break;
		case waitenemy:
			if(ballpattern >= 0x10){
				state = init2;
			}
			break;
		default:
			state = start2;
			break;
	}
	switch(state){
		case start2:
			enemyrow = 0xff;
			enemypattern = 0x00;
			break;
		case startwait:
			enemyrow = 0xc7;
			enemypattern = 0x01;
			enemypos =3;
			break;
		case init2:
			break;
		case pos1:
			if(ballrow <= 0xef){
				enemyrand = rand () % 2;

				if(enemyrand == 0){
					enemyrow = 0x1f;
					enemypos =1;
				}
				else if(enemyrand ==1){
					enemyrow = (enemyrow >>1)|0x80;
					enemypos =2;
				}
			}
			else if(ballrow > 0xef){
				enemyrand = rand () % 3;

				if(enemyrand ==0){
					enemyrow = (enemyrow >>1)|0x80;
					enemyrow = (enemyrow >>1)|0x80;
					enemyrow = (enemyrow >>1) |0x80;
					enemypos = 4;
				}
				else if(enemyrand ==1){
					enemyrow = (enemyrow >>1)|0x80;
					enemyrow = (enemyrow >>1)|0x80;
					enemyrow = (enemyrow >>1) |0x80;
					enemyrow = (enemyrow >>1)|0x80;
					enemypos = 5;
				}
				else if(enemyrand ==2){
					enemyrow = (enemyrow >>1)|0x80;
					enemyrow = (enemyrow >>1)|0x80;
					enemyrow = (enemyrow >>1) |0x80;
					enemyrow = (enemyrow >>1)|0x80;
					enemyrow = (enemyrow >>1) |0x80;
					enemypos =6;
				}			
			}
			break;
		case pos2:
			if(ballrow <= 0xef){
				enemyrand = rand() % 2;
				
				if(enemyrand == 0){
					enemyrow = 0x8f;
					enemypos =2;
				}
				else if(enemyrand ==1){
					enemyrow = (enemyrow <<1)|0x01;
					enemypos =1;
				}
				
			}
			else if(ballrow > 0xef){
				enemyrand = rand() % 3;

				if(enemyrand ==0){
					enemyrow = (enemyrow >>1)|0x80;
					enemyrow = (enemyrow >>1)|0x80;
					enemypos = 4;
				}
				else if(enemyrand ==1){
					enemyrow = (enemyrow >>1)|0x80;
					enemyrow = (enemyrow >>1)|0x80;
					enemyrow = (enemyrow >>1) |0x80;
					enemypos = 5;
				}
				else if(enemyrand ==2){
					enemyrow = (enemyrow >>1)|0x80;
					enemyrow = (enemyrow >>1)|0x80;
					enemyrow = (enemyrow >>1) |0x80;
					enemyrow = (enemyrow >>1) |0x80;
					enemypos=6;
				}
			}
			break;
		case pos3:
			if(ballrow <= 0xef){
				enemyrand = rand() %3;

				if(enemyrand ==0){
					enemyrow = (enemyrow <<1) | 0x01;
					enemypos = 2;
				}
				else if(enemyrand ==1){
					enemyrow = (enemyrow <<1) | 0x01;
					enemyrow = (enemyrow<<1) | 0x01;
					enemypos =1;
				}
				else if(enemyrand == 2){
					enemyrow = 0xc7;
					enemypos =3;
				}	
			}
			else if(ballrow > 0xef){
				enemyrand = rand() % 3;

				if(enemyrand ==0){
					enemyrow = (enemyrow >>1) |0x80;
					enemypos =4;
				}
				else if(enemyrand == 1){
					enemyrow = (enemyrow >>1) | 0x80;
					enemyrow = (enemyrow >> 1) | 0x80;
					enemypos =5;
				}
				else if(enemyrand ==2){
					enemyrow = (enemyrow >> 1) | 0x80;
					enemyrow = (enemyrow >> 1) | 0x80;
					enemyrow = (enemyrow >> 1) |0x80;
					enemypos = 6;
				}

			}
	
			
			break;
		case pos4:
			if(ballrow <= 0xef){
				enemyrand = rand() % 3;

				if(enemyrand ==0){
					enemyrow = (enemyrow <<1)|0x01;
					enemypos = 3;
				}
				else if(enemyrand == 1){
					enemyrow = (enemyrow <<1)|0x01;
					enemyrow = (enemyrow << 1) |0x01;
					enemypos =2;
				}
				else if(enemyrand ==2){
					enemyrow = (enemyrow << 1) |0x01;
					enemyrow = (enemyrow <<1)|0x01;
					enemyrow = (enemyrow <<1)|0x01;
					enemypos = 1;
				}

			}
			else if(ballrow > 0xef){
				enemyrand = rand() % 3;

				if(enemyrand ==0){
					enemyrow = 0xe3;
					enemypos =4;
				}
				else if(enemyrand == 1){
					enemyrow = (enemyrow >>1)|0x80;
					enemypos = 5;
				}
				else if(enemyrand ==2){
					enemyrow = (enemyrow >>1)|0x80;
					enemyrow = (enemyrow >>1)|0x80;
					enemypos =6;
				}
			}
			break;
		case pos5:
			if(ballrow <= 0xef){
				enemyrand = rand() % 3;

				if(enemyrand ==0){
					enemyrow = (enemyrow <<1)|0x01;
					enemyrow = (enemyrow <<1)|0x01;
					enemypos = 3;
				}
				else if(enemyrand ==1){
					enemyrow = (enemyrow <<1)|0x01;
					enemyrow = (enemyrow <<1)|0x01;
					enemyrow = (enemyrow <<1)|0x01;
					enemypos =2;
				}
				else if(enemyrand == 2){
					enemyrow = (enemyrow <<1)|0x01;
					enemyrow = (enemyrow <<1)|0x01;
					enemyrow = (enemyrow <<1)|0x01;
					enemypos = 1;
				}		
			}
			else if(ballrow > 0xef){
				enemyrand = rand() % 2;

				if(enemyrand ==0){
					enemyrow = 0xf1;
					enemypos =5;
				}
				else if(enemyrand ==1){
					enemyrow = (enemyrow >>1)|0x80;
					enemypos =6;
				}
			}
			break;
		case pos6:
			if(ballrow <= 0xef){
				enemyrand = rand() % 3;

				if(enemyrand ==0){	
					enemyrow = (enemyrow <<1)|0x01;
					enemyrow = (enemyrow <<1)|0x01;
					enemyrow = (enemyrow <<1)|0x01;
					enemypos = 3;
				}
				else if(enemyrand ==1){
					enemyrow = (enemyrow <<1)|0x01;
					enemyrow = (enemyrow <<1)|0x01;
					enemyrow = (enemyrow <<1)|0x01;
					enemyrow = (enemyrow <<1)|0x01;
					enemypos = 2;
				}
				else if(enemyrand ==2){
					enemyrow = (enemyrow <<1)|0x01;
					enemyrow = (enemyrow <<1)|0x01;
					enemyrow = (enemyrow <<1)|0x01;
					enemyrow = (enemyrow <<1)|0x01;
					enemyrow = (enemyrow <<1) |0x01;
					enemypos =1;
				}
			}
			else if(ballrow > 0xef){
				enemyrand = rand() % 2;

				if(enemyrand == 0){
					enemyrow = (enemyrow <<1)|0x01;
					enemypos =5;
				}
				else if(enemyrand ==1){
					enemyrow = 0xf8;
					enemypos =6;
				}
			}
			break;
		case waitenemy:
			break;
		default:
			break;
	}
		

//	PORTD = enemyrow;
//	PORTC = enemypattern;
	return state;
}


enum ballmovements_States{start3, init3, ballup, ballcenter, balldown, ballup2, ballcenter2, balldown2, nohit, nohit2};

int ballmovements_Tick(int state){
	switch(state){
	case start3:
		//need any starting condition?
		//change starting ball set up?
		state = init3;
		break;
	case init3:
		//need any starting condition?
		if(temp_ballpattern == 0x40){
			if(userpaddle_pos ==1 && ballrow == 0x7f){
				state = ballup;
			}
			else if(userpaddle_pos ==1 && ballrow == 0xbf){
				state = ballcenter;
			}
			else if(userpaddle_pos == 1 && ballrow == 0xdf){
				state = balldown;
			}

			else if(userpaddle_pos ==2 && ballrow == 0xbf){
				state = ballup;
			}
			else if(userpaddle_pos ==2 && ballrow == 0xdf){
				state = ballcenter;
			}
			else if(userpaddle_pos ==2 && ballrow == 0xef){
				state = balldown;
			}

			else if(userpaddle_pos ==3 && ballrow == 0xdf){
				state = ballup;
			}
			else if(userpaddle_pos == 3 && ballrow == 0xef){
				state = ballcenter;
			}
			else if(userpaddle_pos ==3 && ballrow == 0xf7){
				state = balldown;
			}

			else if(userpaddle_pos ==4 && ballrow == 0xef){
				state = ballup;
			}
			else if(userpaddle_pos ==4 && ballrow == 0xf7){
				state = ballcenter;
			}
			else if(userpaddle_pos == 4 && ballrow == 0xfb){
				state = balldown;
			}

			else if(userpaddle_pos == 5 && ballrow == 0xf7){
				state = ballup;
			}
			else if(userpaddle_pos == 5 && ballrow == 0xfb){
				state = ballcenter;
			}
			else if(userpaddle_pos == 5 && ballrow == 0xfd){
				state = balldown;
			}
			
			else if(userpaddle_pos == 6 && ballrow == 0xfb){
				state = ballup;
			}
			else if(userpaddle_pos == 6 && ballrow == 0xfd){
				state = ballcenter;
			}
			else if(userpaddle_pos == 6 && ballrow == 0xfe){
				state = balldown;
			}
			else {
				state = nohit;
			}
		}

		else if(temp_ballpattern == 0x02){
			if(enemypos ==1 && ballrow == 0x7f){
				state = ballup2;
			}
			else if(enemypos ==1 && ballrow == 0xbf){
				state = ballcenter2;
			}
			else if(enemypos == 1 && ballrow == 0xdf){
				state = balldown2;
			}

			else if(enemypos ==2 && ballrow == 0xbf){
				state = ballup2;
			}
			else if(enemypos ==2 && ballrow == 0xdf){
				state = ballcenter2;
			}
			else if(enemypos ==2 && ballrow == 0xef){
				state = balldown2;
			}

			else if(enemypos ==3 && ballrow == 0xdf){
				state = ballup2;
			}
			else if(enemypos == 3 && ballrow == 0xef){
				state = ballcenter2;
			}
			else if(enemypos ==3 && ballrow == 0xf7){
				state = balldown2;
			}

			else if(enemypos ==4 && ballrow == 0xef){
				state = ballup2;
			}
			else if(enemypos ==4 && ballrow == 0xf7){
				state = ballcenter2;
			}
			else if(enemypos == 4 && ballrow == 0xfb){
				state = balldown2;
			}

			else if(enemypos == 5 && ballrow == 0xf7){
				state = ballup2;
			}
			else if(enemypos == 5 && ballrow == 0xfb){
				state = ballcenter2;
			}
			else if(enemypos == 5 && ballrow == 0xfd){
				state = balldown2;
			}
			
			else if(enemypos == 6 && ballrow == 0xfb){
				state = ballup2;
			}
			else if(enemypos == 6 && ballrow == 0xfd){
				state = ballcenter2;
			}
			else if(enemypos == 6 && ballrow == 0xfe){
				state = balldown2;
			}
			else {
				state = nohit2;
			}
		}
		break;
	case ballup:
		if(ballrow == 0xfe && ballpattern != 0x02){
			state = balldown;
		}
		else if(ballpattern == 0x02){
			state = init3;
		}
		break;
	case balldown:
		if(ballrow == 0x7f && ballpattern != 0x02){
			state = ballup;
		}
		else if(ballpattern == 0x02){
			state = init3;
		}
		break;
	case ballcenter:
		if(ballpattern == 0x02){
			state = init3;
		}
		break;
	case ballup2:
		if(ballrow == 0xfe && ballpattern != 0x40){
			state = balldown2;
		}
		else if(ballpattern == 0x40){
			state = init3;
		}
		break;
	case balldown2:
		if(ballrow == 0x7f&& ballpattern != 0x40){
			state = ballup2;
		}
		else if(ballpattern == 0x40){
			state = init3;
		}
		break;
	case ballcenter2:
		if(ballpattern == 0x40){
			state = init3;
		}
		break;
	case nohit:
		state = start3;
		break;
	case nohit2:
		state = start3;
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
		//	ballpattern = 0x02;
		//	ballrow = 0xef;
			//ball is starting in middle at default paddle
			//position on enemy side
			
			ballpattern = 0x40;
		//	ballrow = 0x7f;
			ballrow = 0xef;
			temp_ballpattern = ballpattern;		
			break;
		case init3:
			temp_ballpattern = ballpattern;
			//need any more conditions?
			break;
		case ballup:
		//	if(temp_ballpattern == 0x40){
				ballpattern = ballpattern >> 1;
				ballrow = (ballrow >> 1) | 0x80;
		//	}
		//	else if(temp_ballpattern == 0x02){
		//		ballpattern = ballpattern << 1;
		//	       ballrow = (ballrow >> 1) | 0x80;
		//	}
	 		//any more conditions?
			break;
		case balldown:
		//	if(temp_ballpattern == 0x40){
				ballpattern = ballpattern >> 1;
				ballrow = (ballrow << 1) | 0x01;
		//	}
		//	else if(temp_ballpattern == 0x02){
		//		ballpattern = ballpattern << 1;
		//		ballrow = (ballrow << 1) | 0x01;
		//	}
			//any more conditions?
			break;
		case ballcenter:
		//	if(temp_ballpattern == 0x40){
				ballpattern = ballpattern >>1;
		//	}
		//	else if(temp_ballpattern == 0x02){
		//		ballpattern = ballpattern << 1;
		//	}
			//any more conditions?
			break;
		case ballup2:
			ballpattern = ballpattern <<1;
			ballrow = (ballrow >> 1) | 0x80;
			break;
		case balldown2:
			ballpattern = ballpattern <<1;
			ballrow = (ballrow << 1) | 0x01;
			break;
		case ballcenter2:
			ballpattern = ballpattern <<1;
			break;
		case nohit:
			score++;
		/*	if(score ==1){
				PORTB |= 0x80;
			}
			else if(score ==2){
				PORTB |= 0xc0;
			}
			else if(score ==3){
				PORTB |= 0xe0;
			}
			else if(score >=4){
				PORTB |= 0x00;
				score = 0;
			}*/	
			//keep track of score, remember to install LEDs
			break;
		case nohit2:
			score2++;
			break;
		default:
		break;
		}
//	return state;
	//remember to transmit ballpattern and ballrow data to LED matrix
//	PORTD = ballrow;
//	PORTC = ballpattern;
	return state;
}

enum player2_States{startp2, initp2, nomovep2, upp2, downp2, p2off};

int player2_Tick(int state){
//	Set_A2D_Pin(0x01);
	switch(state){
		case startp2:
			if(button == 0x02){
				state = initp2;
			}
			break;
		case initp2:
			if(button == 0x00){
				state = nomovep2;
			}
			break;
		case nomovep2:
			if(button == 0x04){
				state = upp2;
			}
			else if(button == 0x08){
				state = downp2;
			}
			else if(button == 0x02){
				state =p2off;
			}
			break;
		case upp2:
			if(button == 0x08){
				state = downp2;
			}
			else{
				state = nomovep2;
			}
			break;
		case downp2:
			if(button == 0x04){
				state = upp2;
			}
			else{
				state = nomovep2;
			}
			break;
		case p2off:
			if(button == 0x00){
				state = startp2;
			}
			break;
		default:
			state = startp2;
			break;
		}
	switch(state){
		case startp2:
			break;
		case initp2:
			//default user paddle position
			p2on=1;
			p2paddle= 0x01;
			p2row = 0xc7;
			enemypos =3;
			break;
		case nomovep2:
			break;
		case upp2:
			if(p2row == 0xf8){
				p2row = 0xf1;
				enemypos = 5;
			}
			else if(p2row == 0xf1){
				p2row = 0xe3;
				enemypos = 4;
			}
			else if(p2row == 0xe3){
				p2row = 0xc7;
				enemypos = 3;
			}
			else if(p2row == 0xc7){
				p2row = 0x8f;
				enemypos = 2;
			}
			else if(p2row == 0x8f){
				p2row = 0x1f;
				enemypos = 1;
			}
			break;
		case downp2:
			if(p2row == 0x1f){
				p2row = 0x8f;
				enemypos = 2;
			}
			else if(p2row == 0x8f){
				p2row = 0xc7;
				enemypos = 3;
			}
			else if(p2row == 0xc7){
				p2row =0xe3;
				enemypos = 4;
			}
			else if(p2row == 0xe3){
				p2row = 0xf1;
				enemypos=5;
			}
			else if(p2row == 0xf1){
				p2row = 0xf8;
				enemypos = 6;
			}
			break;
		case p2off:
			p2on =0;
			p2paddle = 0x00;
			p2row = 0xff;
			break;
		default:
			break;
	}
//	PORTC = p2paddle;
//	PORTD = p2row;
	return state;
}

enum introled_States { intro2, intro3, intro4};

int introled_Tick(int state){
	switch(state){
		case intro2:
			state = intro3;
			break;
		case intro3:
			state = intro4;
			break;
		case intro4:
			state = intro2;
			break;
		default:
			state = intro2;
			break;
	}
	switch(state){
		case intro2:
			intropat = 0x66;
			introrow = 0x9f;
			break;
		case intro3:
			intropat = 0x3c;
			introrow = 0xfb;
			break;
		case intro4:
			intropat = 0x18;
			introrow = 0xfd;
			break;
		default:
			break;
		}
	return state;
}


unsigned char introcount;
enum outputled_States{start5, intro, userout, ballout, enemyout, p2out};

int outputled_Tick(int state){
	switch(state){
		case start5:
			if(button == 0x00 || score >=3|| score2 >=3){
			state = intro;
			}
			break;
		case intro:
			if(introcount ==0){
				state = userout;
			}

			break;
		case userout:
			if(button == 0x01 || score >= 3|| score2 >=3){
				state = start5;
			}
			else{
			state = ballout;
			}
			break;
		case ballout:
			if(!p2on){
			state = enemyout;
			}
			else if(p2on){
			state = p2out;
			}
			else if(button == 0x01 || score >=3 || score2 >= 3){
				state = start5;
			}
		break;
		case enemyout:
			if(button == 0x01){
				state = start5;
			}
			else{
			state = userout;
			}
			break;
		case p2out:
			if(button == 0x01 || score >=3 || score2 >=3){
				state = start5;
			}
			else{
			state = userout;
			}
			break;	
		default:
			state = start5;
			break;
	}
	switch(state){
		case start5:
			score =0;
			score2 =0;

			row = 0xc7;
			userpaddle_pos =3;

			enemypos =3;
			enemyrow = 0xc7;

			p2row = 0xc7;
			p2on = 0;

			ballpattern = 0x80;
			ballrow = 0xef;

			introcount = 7000;
			break;
		case intro:
			PORTC = intropat;
			PORTD = introrow;
			introcount--;
			break;
		case userout:
			PORTC = userpaddle_pattern;
			PORTD = row;
			break;
		case ballout:
			PORTC = ballpattern;
			PORTD = ballrow;
			break;
		case enemyout:
			PORTC = enemypattern;
			PORTD = enemyrow;
			break;
		case p2out:
			PORTC = p2paddle;
			PORTD = p2row;
			break;
		default:
			break;
	}
	return state;
}


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0X00; PORTA = 0XFF;
	DDRB = 0x00; PORTB = 0xFF;
	DDRC = 0XFF; PORTC = 0X00;
	DDRD = 0XFF; PORTD = 0X00;
	
	button = 0x00;
	
	
	ADC_init();

//	srand((unsigned) time(&t));
	srand(time(NULL));



	static task task1, task2, task3, task4, task5, task6;
	task *tasks[] = {&task1, &task2, &task3, &task4, &task5, &task6};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	task1.state = start;
	task1.period = 15;
	task1.elapsedTime = task1.period;
	task1.TickFct = &userpaddle_Tick;

	task2.state = start;
	task2.period = 15;
	task2.elapsedTime = task2.period;
	task2.TickFct = &enemypaddle_Tick;

	task3.state = start;
	task3.period = 15;
	task3.elapsedTime = task3.period;
	task3.TickFct = &ballmovements_Tick;

	task4.state = start;
	task4.period = 15;
	task4.elapsedTime = task4.period;
	task4.TickFct = &player2_Tick;

	task5.state = start;
	task5.period = 1;
	task5.elapsedTime = task5.period;
	task5.TickFct = &introled_Tick;

	task6.state = start;
	task6.period =1;
	task6.elapsedTime = task6.period;
	task6.TickFct = &outputled_Tick;

	


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
    	button = ~PINB;

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
